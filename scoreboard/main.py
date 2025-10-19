from pathlib import Path
from collections import defaultdict, Counter
from datetime import datetime
import csv
import argparse
import subprocess
import yaml
import shutil
from jinja2 import Environment, FileSystemLoader
import logging
import sys

# Try ZoneInfo from stdlib, then from backports, else fall back to naive time
try:
    from zoneinfo import ZoneInfo  # type: ignore
except Exception:  # pragma: no cover - fallback for Python < 3.9
    try:
        from backports.zoneinfo import ZoneInfo  # type: ignore
    except Exception:  # Last resort: define a stub
        ZoneInfo = None  # type: ignore

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

task_types = ["all", "mpi", "omp", "seq", "stl", "tbb"]
# Threads table order: seq first, then omp, tbb, stl, all
task_types_threads = ["seq", "omp", "tbb", "stl", "all"]
task_types_processes = ["mpi", "seq"]

script_dir = Path(__file__).parent
tasks_dir = script_dir.parent / "tasks"
# Salt is derived from the repository root directory name (dynamic)
REPO_ROOT = script_dir.parent.resolve()
# Salt format: "learning_process/<repo_name>"
REPO_SALT = f"learning_process/{REPO_ROOT.name}"

# Ensure we can import assign_variant from scoreboard directory
if str(script_dir) not in sys.path:
    sys.path.insert(0, str(script_dir))
try:
    from assign_variant import assign_variant
except Exception:

    def assign_variant(
        surname: str,
        name: str,
        group: str,
        repo: str,
        patronymic: str = "",
        num_variants: int = 1,
    ) -> int:
        return 0


def _now_msk():
    """Return current datetime in MSK if tz support is available, else local time."""
    try:
        if ZoneInfo is not None:
            return datetime.now(ZoneInfo("Europe/Moscow"))
    except Exception:
        pass
    return datetime.now()


def _read_tasks_type(task_dir: Path) -> str | None:
    """Read tasks_type from settings.json in the task directory (if present)."""
    settings_path = task_dir / "settings.json"
    if settings_path.exists():
        try:
            import json

            with open(settings_path, "r") as f:
                data = json.load(f)
            return data.get("tasks_type")  # "threads" or "processes"
        except Exception as e:
            logger.warning("Failed to parse %s: %s", settings_path, e)
    return None


def discover_tasks(tasks_dir, task_types):
    """Discover tasks and their implementation status from the filesystem.

    Returns:
        directories: dict[task_name][task_type] -> status
        tasks_type_map: dict[task_name] -> "threads" | "processes" | None
    """
    directories = defaultdict(dict)
    tasks_type_map: dict[str, str | None] = {}

    if tasks_dir.exists() and tasks_dir.is_dir():
        for task_name_dir in tasks_dir.iterdir():
            if task_name_dir.is_dir() and task_name_dir.name not in ["common"]:
                task_name = task_name_dir.name
                # Save tasks_type from settings.json if present
                tasks_type_map[task_name] = _read_tasks_type(task_name_dir)
                for task_type in task_types:
                    task_type_dir = task_name_dir / task_type
                    if task_type_dir.exists() and task_type_dir.is_dir():
                        if task_name.endswith("_disabled"):
                            clean_task_name = task_name[: -len("_disabled")]
                            directories[clean_task_name][task_type] = "disabled"
                        else:
                            directories[task_name][task_type] = "done"

    return directories, tasks_type_map


directories, tasks_type_map = discover_tasks(tasks_dir, task_types)


def load_performance_data_threads(perf_stat_file_path: Path) -> dict:
    """Load threads performance ratios (T_x/T_seq) from CSV.
    Expected header: Task, SEQ, OMP, TBB, STL, ALL
    """
    perf_stats: dict[str, dict] = {}
    if perf_stat_file_path.exists():
        with open(perf_stat_file_path, "r", newline="") as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                task_name = row.get("Task")
                if not task_name:
                    continue
                perf_stats[task_name] = {
                    "seq": row.get("SEQ", "?"),
                    "omp": row.get("OMP", "?"),
                    "tbb": row.get("TBB", "?"),
                    "stl": row.get("STL", "?"),
                    "all": row.get("ALL", "?"),
                }
    else:
        logger.warning("Threads perf stats CSV not found at %s", perf_stat_file_path)
    return perf_stats


def load_performance_data_processes(perf_stat_file_path: Path) -> dict:
    """Load processes performance ratios (T_x/T_seq) from CSV.
    Expected header: Task, SEQ, MPI
    """
    perf_stats: dict[str, dict] = {}
    if perf_stat_file_path.exists():
        with open(perf_stat_file_path, "r", newline="") as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                task_name = row.get("Task")
                if not task_name:
                    continue
                perf_stats[task_name] = {
                    "seq": row.get("SEQ", "?"),
                    "mpi": row.get("MPI", "?"),
                }
    else:
        logger.warning("Processes perf stats CSV not found at %s", perf_stat_file_path)
    return perf_stats


def calculate_performance_metrics(perf_val, eff_num_proc, task_type):
    """Calculate acceleration and efficiency from performance value."""
    acceleration = "?"
    efficiency = "?"
    try:
        perf_float = float(perf_val)
        if perf_float > 0 and not (
            perf_float == float("inf") or perf_float != perf_float
        ):
            speedup = 1.0 / perf_float
            # For sequential code, acceleration and efficiency don't make sense
            # as it should be the baseline (speedup = 1.0 by definition)
            if task_type == "seq":
                acceleration = "1.00"  # Sequential is the baseline
                efficiency = "N/A"
            else:
                acceleration = f"{speedup:.2f}"
                efficiency = f"{speedup / eff_num_proc * 100:.2f}%"
    except (ValueError, TypeError):
        pass
    return acceleration, efficiency


def _find_max_solution(points_info, task_type: str) -> int:
    """Resolve max S for a given task type from points-info (threads list)."""
    threads_tasks = (points_info.get("threads", {}) or {}).get("tasks", [])
    for t in threads_tasks:
        if str(t.get("name")) == task_type:
            try:
                return int(t.get("S", 0))
            except Exception:
                return 0
    if task_type == "mpi":
        return 0
    return 0


def _find_report_max(points_info, task_type: str) -> int:
    """Resolve max Report (R) points for a given task type from points-info (threads).
    Returns 0 if not found.
    """
    threads_tasks = (points_info.get("threads", {}) or {}).get("tasks", [])
    for t in threads_tasks:
        if str(t.get("name")) == task_type:
            try:
                return int(t.get("R", 0))
            except Exception:
                return 0
    return 0


def _find_performance_max(points_info, task_type: str) -> int:
    """Resolve max Performance (A) points for a given task type (threads)."""
    threads_tasks = (points_info.get("threads", {}) or {}).get("tasks", [])
    for t in threads_tasks:
        if str(t.get("name")) == task_type:
            try:
                return int(t.get("A", 0))
            except Exception:
                return 0
    return 0


def _calc_perf_points_from_efficiency(efficiency_str: str, max_points: int) -> float:
    """Calculate Performance points as a real number (x.yy).

    Mapping (eff -> percent of max):
      >=50 -> 100; [45,50) -> 90; [42,45) -> 80; [40,42) -> 70; [37,40) -> 60;
      [35,37) -> 50; [32,35) -> 40; [30,32) -> 30; [27,30) -> 20; [25,27) -> 10; <25 -> 0
    Returns a float rounded to 2 decimals (no ceil).
    """
    if not isinstance(efficiency_str, str) or not efficiency_str.endswith("%"):
        return 0.0
    try:
        val = float(efficiency_str.rstrip("%"))
    except Exception:
        return 0.0
    perc = 0.0
    if val >= 50:
        perc = 1.0
    elif 45 <= val < 50:
        perc = 0.9
    elif 42 <= val < 45:
        perc = 0.8
    elif 40 <= val < 42:
        perc = 0.7
    elif 37 <= val < 40:
        perc = 0.6
    elif 35 <= val < 37:
        perc = 0.5
    elif 32 <= val < 35:
        perc = 0.4
    elif 30 <= val < 32:
        perc = 0.3
    elif 27 <= val < 30:
        perc = 0.2
    elif 25 <= val < 27:
        perc = 0.1
    else:
        perc = 0.0
    pts = max_points * perc if max_points > 0 else 0.0
    # round to 2 decimals (banker's rounding acceptable here)
    return round(pts, 2)


def _find_process_report_max(points_info, task_number: int) -> int:
    """Get max report (R) points for process task by ordinal (1..3).
    Looks up processes.tasks with names like 'mpi_task_1'.
    """
    proc = (points_info.get("processes", {}) or {}).get("tasks", [])
    key = f"mpi_task_{task_number}"
    for t in proc:
        if str(t.get("name")) == key:
            try:
                return int(t.get("R", 0))
            except Exception:
                return 0
    return 0


def _find_process_points(points_info, task_number: int) -> tuple[int, int, int, int]:
    """Return (S_mpi, S_seq, A_mpi, R) maxima for a given process task ordinal (1..3).
    Supports both mapping and list-of-maps (per user's YAML example).
    """
    proc_tasks = (points_info.get("processes", {}) or {}).get("tasks", [])
    key = f"mpi_task_{task_number}"
    for t in proc_tasks:
        if str(t.get("name")) == key:

            def _extract(obj, k):
                if isinstance(obj, dict):
                    return int(obj.get(k, 0))
                if isinstance(obj, list):
                    for it in obj:
                        if isinstance(it, dict) and k in it:
                            try:
                                return int(it.get(k, 0))
                            except Exception:
                                return 0
                return 0

            mpi_blk = t.get("mpi", {})
            seq_blk = t.get("seq", {})
            s_mpi = _extract(mpi_blk, "S")
            a_mpi = _extract(mpi_blk, "A")
            s_seq = _extract(seq_blk, "S")
            try:
                r = int(t.get("R", 0))
            except Exception:
                r = 0
            return s_mpi, s_seq, a_mpi, r
    return 0, 0, 0, 0


def _find_process_variants_max(points_info, task_number: int) -> int:
    proc_tasks = (points_info.get("processes", {}) or {}).get("tasks", [])
    key = f"mpi_task_{task_number}"
    for t in proc_tasks:
        if str(t.get("name")) == key:
            try:
                return int(t.get("variants_max", 1))
            except Exception:
                return 1
    return 1


def get_solution_points_and_style(task_type, status, cfg):
    """Get solution points and CSS style based on task type and status."""
    max_sol_points = _find_max_solution(cfg, task_type)
    sol_points = max_sol_points if status in ("done", "disabled") else 0
    solution_style = ""
    if status == "done":
        solution_style = "background-color: lightgreen;"
    elif status == "disabled":
        solution_style = "background-color: #6495ED;"
    return sol_points, solution_style


def check_plagiarism_and_calculate_penalty(
    dir, task_type, sol_points, plagiarism_cfg, cfg, semester: str | None
):
    """Check if task is plagiarized and calculate penalty points.

    Supports two config layouts:
      - legacy: { plagiarism: { seq: [...], omp: [...], ... } }
      - semesters: { threads: {plagiarism: {...}}, processes: {plagiarism: {...}} }
    """
    clean_dir = dir[: -len("_disabled")] if dir.endswith("_disabled") else dir

    # Resolve copying/plagiarism mapping based on layout
    plag_map = {}
    if isinstance(plagiarism_cfg, dict) and (
        "copying" in plagiarism_cfg or "plagiarism" in plagiarism_cfg
    ):
        plag_map = (
            plagiarism_cfg.get("copying")
            if "copying" in plagiarism_cfg
            else plagiarism_cfg.get("plagiarism", {})
        ) or {}
    elif (
        isinstance(plagiarism_cfg, dict)
        and semester
        and semester in plagiarism_cfg
        and isinstance(plagiarism_cfg[semester], dict)
    ):
        inner = plagiarism_cfg[semester]
        plag_map = (
            inner.get("copying") if "copying" in inner else inner.get("plagiarism", {})
        ) or {}

    flagged_list = set(plag_map.get(task_type, []) or [])
    is_cheated = dir in flagged_list or clean_dir in flagged_list
    plagiarism_points = 0
    if is_cheated:
        # Prefer new key 'copying', fallback to legacy 'plagiarism'
        try:
            plag_coeff = float(
                (cfg.get("copying", {}) or cfg.get("plagiarism", {})).get(
                    "coefficient", 0.0
                )
            )
        except Exception:
            plag_coeff = 0.0
        plagiarism_points = -plag_coeff * sol_points
    return is_cheated, plagiarism_points


def calculate_deadline_penalty(dir, task_type, status, deadlines_cfg, tasks_dir):
    """Calculate deadline penalty points based on git commit timestamp."""
    deadline_points = 0
    deadline_str = deadlines_cfg.get(task_type)
    if status == "done" and deadline_str:
        try:
            deadline_dt = datetime.fromisoformat(deadline_str)
            git_cmd = [
                "git",
                "log",
                "-1",
                "--format=%ct",
                str(
                    tasks_dir
                    / (dir + ("_disabled" if status == "disabled" else ""))
                    / task_type
                ),
            ]
            result = subprocess.run(git_cmd, capture_output=True, text=True)
            if result.stdout.strip().isdigit():
                commit_dt = datetime.fromtimestamp(int(result.stdout.strip()))
                days_late = (commit_dt - deadline_dt).days
                if days_late > 0:
                    deadline_points = -days_late
        except Exception:
            pass
    return deadline_points


def load_configurations():
    """Load points-info (max points, deadlines, efficiency) and plagiarism lists."""
    points_info_path = Path(__file__).parent / "data" / "points-info.yml"
    assert points_info_path.exists(), f"Points info file not found: {points_info_path}"
    with open(points_info_path, "r") as f:
        points_info = yaml.safe_load(f)
    assert points_info, "Points info is empty"

    eff_num_proc = int(points_info.get("efficiency", {}).get("num_proc", 1))
    deadlines_cfg = points_info.get("deadlines", {})

    plagiarism_config_path = Path(__file__).parent / "data" / "copying.yml"
    with open(plagiarism_config_path, "r") as file:
        plagiarism_cfg = yaml.safe_load(file)
    assert plagiarism_cfg, "Plagiarism configuration is empty"

    return points_info, eff_num_proc, deadlines_cfg, plagiarism_cfg


def _build_rows_for_task_types(
    selected_task_types: list[str],
    dir_names: list[str],
    perf_stats: dict,
    cfg,
    eff_num_proc,
    deadlines_cfg,
):
    """Build rows for the given list of task directories and selected task types."""
    rows = []

    def _load_student_info_label(dir_name: str):
        import json

        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return None
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            s = data.get("student", {})
            last = s.get("last_name", "")
            first = s.get("first_name", "")
            middle = s.get("middle_name", "")
            parts = [p for p in [last, first, middle] if p]
            label = "<br/>".join(parts)
            return label if label else None
        except Exception:
            return None

    def _load_student_fields(dir_name: str):
        import json

        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return None
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            s = data.get("student", {})
            return (
                str(s.get("last_name", "")),
                str(s.get("first_name", "")),
                str(s.get("middle_name", "")),
                str(s.get("group_number", "")),
            )
        except Exception:
            return None

    for dir in sorted(dir_names):
        row_types = []
        total_count = 0
        for task_type in selected_task_types:
            status = directories[dir].get(task_type)
            sol_points, solution_style = get_solution_points_and_style(
                task_type, status, cfg
            )

            task_points = sol_points
            is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
                dir, task_type, sol_points, plagiarism_cfg, cfg, semester="threads"
            )
            task_points += plagiarism_points

            perf_val = perf_stats.get(dir, {}).get(task_type, "?")

            # Calculate acceleration and efficiency if performance data is available
            acceleration, efficiency = calculate_performance_metrics(
                perf_val, eff_num_proc, task_type
            )

            # Calculate deadline penalty points
            deadline_points = calculate_deadline_penalty(
                dir, task_type, status, deadlines_cfg, tasks_dir
            )

            # Report presence: award R only if report.md exists inside the task directory
            report_present = (tasks_dir / dir / "report.md").exists()
            report_points = _find_report_max(cfg, task_type) if report_present else 0

            # Performance points P for non-seq types, based on efficiency
            perf_max = _find_performance_max(cfg, task_type)
            if task_type != "seq":
                perf_points = _calc_perf_points_from_efficiency(efficiency, perf_max)
                perf_points_display = (
                    f"{perf_points:.2f}"
                    if isinstance(efficiency, str) and efficiency.endswith("%")
                    else "—"
                )
            else:
                perf_points = 0.0
                perf_points_display = "—"

            row_types.append(
                {
                    "solution_points": sol_points,
                    "solution_style": solution_style,
                    "perf": perf_val,
                    "acceleration": acceleration,
                    "efficiency": efficiency,
                    "perf_points": perf_points,
                    "perf_points_display": perf_points_display,
                    "deadline_points": deadline_points,
                    "plagiarised": is_cheated,
                    "plagiarism_points": plagiarism_points,
                    "report": report_points,
                }
            )
            # Total: include Solution + Performance + Report + Copying penalty (exclude Deadline)
            total_count += task_points + perf_points + report_points

        label_name = _load_student_info_label(dir) or dir
        # Generate variant for threads based on student info and variants_max
        threads_vmax = int((cfg.get("threads", {}) or {}).get("variants_max", 1))
        fields = _load_student_fields(dir)
        if fields:
            last, first, middle, group = fields
            try:
                v_idx = assign_variant(
                    last,
                    first,
                    group,
                    REPO_SALT,
                    patronymic=middle,
                    num_variants=threads_vmax,
                )
                variant = str(v_idx + 1)
            except Exception:
                variant = "?"
        else:
            variant = "?"
        rows.append(
            {
                "task": label_name,
                "variant": variant,
                "types": row_types,
                "total": total_count,
            }
        )
    return rows


def main():
    """Main function to generate the scoreboard.

    Now generates three pages in the output dir:
      - index.html: simple menu linking to threads.html and processes.html
      - threads.html: scoreboard for thread-based tasks
      - processes.html: scoreboard for process-based tasks
    """
    cfg, eff_num_proc, deadlines_cfg, plagiarism_cfg_local = load_configurations()

    # Make plagiarism config available to rows builder
    global plagiarism_cfg
    plagiarism_cfg = plagiarism_cfg_local

    env = Environment(loader=FileSystemLoader(Path(__file__).parent / "templates"))

    # Load optional display deadlines from deadlines.yml and/or auto-compute evenly
    deadlines_display_threads: dict[str, str] | None = None
    deadlines_display_processes: dict[str, str] | None = None
    try:
        dl_file = script_dir / "data" / "deadlines.yml"
        if dl_file.exists():
            with open(dl_file, "r") as f:
                dl_cfg = yaml.safe_load(f) or {}
            deadlines_display_threads = dl_cfg.get("threads") or {}
            deadlines_display_processes = dl_cfg.get("processes") or {}
    except Exception:
        pass

    # Helper: compute evenly spaced dates for current semester (MSK)
    from datetime import date, timedelta
    import calendar

    def _abbr(day: date) -> str:
        return f"{day.day} {calendar.month_abbr[day.month]}"

    def _spring_bounds(today: date) -> tuple[date, date]:
        """Return [1 Feb .. 15 May] window for the appropriate year.
        If today is past 15 May, use next year's spring; otherwise this year's.
        """
        y = today.year
        start = date(y, 2, 1)
        end = date(y, 5, 15)
        if today > end:
            y += 1
            start = date(y, 2, 1)
            end = date(y, 5, 15)
        return start, end

    def _autumn_bounds(today: date) -> tuple[date, date]:
        """Return [15 Oct .. 14 Dec] window for the appropriate year.
        If today is past 14 Dec, use next year's autumn; otherwise this year's.
        """
        y = today.year
        start = date(y, 10, 15)
        end = date(y, 12, 14)
        if today > end:
            y += 1
            start = date(y, 10, 15)
            end = date(y, 12, 14)
        return start, end

    def _evenly_spaced_dates(n: int, start: date, end: date) -> list[date]:
        """
        Return n deadlines evenly spaced across the window (start..end],
        i.e., strictly after the start date, with the last at end.
        Positions are at fractions (i+1)/n of the total span.
        """
        if n <= 1:
            return [end]
        total = (end - start).days
        if total < 0:
            start, end = end, start
            total = -total
        res = []
        for i in range(n):
            off = int(round((i + 1) * total / n))
            if off <= 0:
                off = 1
            if off > total:
                off = total
            res.append(start + timedelta(days=off))
        return res

    def _compute_display_deadlines_threads(order: list[str]) -> dict[str, date]:
        # Threads = Spring semester (prefer MSK; fallback to local time)
        try:
            today = _now_msk().date()
        except Exception:
            today = datetime.now().date()
        s, e = _spring_bounds(today)
        ds = _evenly_spaced_dates(len(order), s, e)
        return {t: d for t, d in zip(order, ds)}

    def _compute_display_deadlines_processes(n_items: int) -> list[date]:
        # Processes = Autumn semester (prefer MSK; fallback to local time)
        try:
            today = _now_msk().date()
        except Exception:
            today = datetime.now().date()
        s, e = _autumn_bounds(today)
        ds = _evenly_spaced_dates(n_items, s, e)
        return ds

    # Locate perf CSVs from CI or local runs (threads and processes)
    candidates_threads = [
        script_dir.parent
        / "build"
        / "perf_stat_dir"
        / "threads_task_run_perf_table.csv",
        script_dir.parent / "perf_stat_dir" / "threads_task_run_perf_table.csv",
        # Fallback to old single-file name
        script_dir.parent / "build" / "perf_stat_dir" / "task_run_perf_table.csv",
        script_dir.parent / "perf_stat_dir" / "task_run_perf_table.csv",
    ]
    threads_csv = next(
        (p for p in candidates_threads if p.exists()), candidates_threads[0]
    )

    candidates_processes = [
        script_dir.parent
        / "build"
        / "perf_stat_dir"
        / "processes_task_run_perf_table.csv",
        script_dir.parent / "perf_stat_dir" / "processes_task_run_perf_table.csv",
    ]
    processes_csv = next(
        (p for p in candidates_processes if p.exists()), candidates_processes[0]
    )

    # Read and merge performance statistics CSVs (keys = CSV Task column)
    perf_stats_threads = load_performance_data_threads(threads_csv)
    perf_stats_processes = load_performance_data_processes(processes_csv)
    perf_stats_raw: dict[str, dict] = {}
    perf_stats_raw.update(perf_stats_threads)
    for k, v in perf_stats_processes.items():
        perf_stats_raw[k] = {**perf_stats_raw.get(k, {}), **v}

    # Partition tasks by tasks_type from settings.json
    threads_task_dirs = [
        name for name, ttype in tasks_type_map.items() if ttype == "threads"
    ]
    processes_task_dirs = [
        name for name, ttype in tasks_type_map.items() if ttype == "processes"
    ]

    # Fallback: if settings.json is missing, guess by directory name heuristic
    for name in directories.keys():
        if name not in tasks_type_map or tasks_type_map[name] is None:
            if "threads" in name:
                threads_task_dirs.append(name)
            elif "processes" in name:
                processes_task_dirs.append(name)

    # Resolve performance stats keys (from CSV Task names) to actual task directories
    import re as _re

    def _family_from_name(name: str) -> tuple[str, int]:
        # Infer family from CSV Task value, using only structural markers
        # threads -> ("threads", 0); processes[_N] -> ("processes", N|1)
        if "threads" in name:
            return "threads", 0
        if "processes" in name:
            m = _re.search(r"processes(?:_(\d+))?", name)
            if m:
                try:
                    idx = int(m.group(1)) if m.group(1) else 1
                except Exception:
                    idx = 1
            else:
                idx = 1
            return "processes", idx
        # Fallback: treat as threads family
        return "threads", 0

    def _family_from_dir(dir_name: str) -> tuple[str, int]:
        # Prefer explicit tasks_type from settings.json and task_number from info.json
        kind_guess = tasks_type_map.get(dir_name) or (
            "threads" if "threads" in dir_name else "processes"
        )
        idx = 0
        if kind_guess == "processes":
            # Lightweight reader to avoid dependency on later-scoped helpers
            try:
                import json as _json

                info_path = tasks_dir / dir_name / "info.json"
                if info_path.exists():
                    with open(info_path, "r") as _f:
                        data = _json.load(_f)
                    s = data.get("student", {}) if isinstance(data, dict) else {}
                    try:
                        idx = int(str(s.get("task_number", "0")))
                    except Exception:
                        idx = 0
            except Exception:
                idx = 0
        return kind_guess, idx

    # Build map family -> list of dir names in this repo
    family_to_dirs: dict[tuple[str, int], list[str]] = {}
    for d in sorted(directories.keys()):
        fam = _family_from_dir(d)
        family_to_dirs.setdefault(fam, []).append(d)

    # Aggregate perf by family (CSV keys may not match dir names)
    perf_by_family: dict[tuple[str, int], dict] = {}
    for key, vals in perf_stats_raw.items():
        fam = _family_from_name(key)
        perf_by_family[fam] = {**perf_by_family.get(fam, {}), **vals}

    # Project family perf onto actual directories (prefer exact one per family)
    perf_stats: dict[str, dict] = {}
    for fam, vals in perf_by_family.items():
        dirs_for_family = family_to_dirs.get(fam, [])
        if not dirs_for_family:
            continue
        # Assign same perf to all dirs in the family (usually one)
        for d in dirs_for_family:
            perf_stats[d] = vals.copy()

    # Build rows for each page
    threads_rows = _build_rows_for_task_types(
        task_types_threads,
        threads_task_dirs,
        perf_stats,
        cfg,
        eff_num_proc,
        deadlines_cfg,
    )
    # Processes page: build 3 tasks as columns for a single student
    import json

    def _load_student_info(dir_name: str):
        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return None
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            return data.get("student", {})
        except Exception as e:
            logger.warning("Failed to parse %s: %s", info_path, e)
            return None

    def _identity_key(student: dict) -> str:
        return "|".join(
            [
                str(student.get("first_name", "")),
                str(student.get("last_name", "")),
                str(student.get("middle_name", "")),
                str(student.get("group_number", "")),
            ]
        )

    def _build_cell(dir_name: str, ttype: str, perf_map: dict[str, dict]):
        status = directories[dir_name].get(ttype)
        sol_points, solution_style = get_solution_points_and_style(ttype, status, cfg)
        task_points = sol_points
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            dir_name, ttype, sol_points, plagiarism_cfg, cfg, semester="processes"
        )
        task_points += plagiarism_points
        perf_val = perf_map.get(dir_name, {}).get(ttype, "?")
        acceleration, efficiency = calculate_performance_metrics(
            perf_val, eff_num_proc, ttype
        )
        deadline_points = calculate_deadline_penalty(
            dir_name, ttype, status, deadlines_cfg, tasks_dir
        )
        return (
            {
                "solution_points": sol_points,
                "solution_style": solution_style,
                "perf": perf_val,
                "acceleration": acceleration,
                "efficiency": efficiency,
                "deadline_points": deadline_points,
                "plagiarised": is_cheated,
                "plagiarism_points": plagiarism_points,
            },
            task_points,
        )

    proc_infos = []
    for d in processes_task_dirs:
        s = _load_student_info(d)
        if s:
            proc_infos.append((d, s))

    # Choose target identity: prefer example_processes; otherwise most common
    target_identity = None
    if "example_processes" in processes_task_dirs:
        s0 = _load_student_info("example_processes")
        if s0:
            target_identity = _identity_key(s0)
    if not target_identity and proc_infos:
        cnt = Counter(_identity_key(s) for _, s in proc_infos)
        target_identity = cnt.most_common(1)[0][0]

    # Map task_number -> (dir_name, display_label)
    num_to_dir: dict[int, tuple[str, str]] = {}
    if target_identity:
        for d, s in proc_infos:
            if _identity_key(s) == target_identity:
                try:
                    tn = int(str(s.get("task_number", "0")))
                except Exception:
                    continue
                display = d
                num_to_dir[tn] = (d, display)

    expected_numbers = [1, 2, 3]
    proc_group_headers = []
    proc_top_headers = []
    proc_groups = []
    proc_r_values = []
    total_points_sum = 0
    for n in expected_numbers:
        entry = num_to_dir.get(n)
        if entry:
            d, display_label = entry
            # Top header shows task name (directory)
            proc_top_headers.append(f"task-{n}")
            # Second header row shows only mpi/seq
            proc_group_headers.append({"type": "mpi"})
            proc_group_headers.append({"type": "seq"})
            group_cells = []
            for ttype in ["mpi", "seq"]:
                cell, _ = _build_cell(d, ttype, perf_stats)
                group_cells.append(cell)
            # Override displayed points for processes: S under MPI/SEQ from points-info; A points under MPI only
            s_mpi, s_seq, a_mpi, r_max = _find_process_points(cfg, n)
            has_mpi = bool(directories[d].get("mpi"))
            has_seq = bool(directories[d].get("seq"))
            report_present = (tasks_dir / d / "report.md").exists()
            group_cells[0]["solution_points"] = s_mpi if has_mpi else 0
            group_cells[1]["solution_points"] = s_seq if has_seq else 0
            # Calculate Performance P for MPI based on efficiency and max a_mpi
            mpi_eff = group_cells[0].get("efficiency", "N/A")
            perf_points_mpi = (
                _calc_perf_points_from_efficiency(mpi_eff, a_mpi)
                if (has_mpi and has_seq)
                else 0
            )
            # Display '—' instead of 0 when metrics are absent (efficiency not a percent)
            if isinstance(mpi_eff, str) and mpi_eff.endswith("%"):
                perf_points_mpi_display = perf_points_mpi
            else:
                perf_points_mpi_display = "—"
            group_cells[0]["perf_points"] = perf_points_mpi
            group_cells[0]["perf_points_display"] = perf_points_mpi_display
            group_cells[1]["perf_points"] = 0
            # Recompute plagiarism penalty based on processes S maxima
            try:
                plag_coeff = float(
                    (cfg.get("copying", {}) or cfg.get("plagiarism", {})).get(
                        "coefficient", 0.0
                    )
                )
            except Exception:
                plag_coeff = 0.0
            p_mpi = (
                -plag_coeff * s_mpi
                if (has_mpi and group_cells[0].get("plagiarised"))
                else 0
            )
            p_seq = (
                -plag_coeff * s_seq
                if (has_seq and group_cells[1].get("plagiarised"))
                else 0
            )
            group_cells[0]["plagiarism_points"] = p_mpi
            group_cells[1]["plagiarism_points"] = p_seq
            proc_groups.extend(group_cells)
            # Sum points S + P + R + C (penalty negative) with gating
            s_inc = (s_mpi if has_mpi else 0) + (s_seq if has_seq else 0)
            p_inc = perf_points_mpi
            r_inc = r_max if report_present else 0
            total_points_sum += s_inc + p_inc + r_inc + p_mpi + p_seq
            proc_r_values.append(r_inc)
        else:
            proc_group_headers.append({"type": "mpi", "task_label": f"task_{n}"})
            proc_group_headers.append({"type": "seq", "task_label": f"task_{n}"})
            proc_top_headers.append(f"task-{n}")
            for _ in ["mpi", "seq"]:
                proc_groups.append(
                    {
                        "solution_points": "?",
                        "solution_style": "",
                        "perf": "?",
                        "acceleration": "?",
                        "efficiency": "?",
                        "deadline_points": "?",
                        "plagiarised": False,
                        "plagiarism_points": "?",
                    }
                )
            # Do not affect total; sum only existing tasks; report points 0
            proc_r_values.append(0)

    # Label for processes row: show Last, First, Middle on separate lines; no group number
    row_label = "processes"
    row_variant = "?"
    if target_identity:
        parts = target_identity.split("|")
        if len(parts) >= 3:
            first, last, middle = parts[0], parts[1], parts[2]
            name_parts = [p for p in [last, first, middle] if p]
            name = "<br/>".join(name_parts)
            row_label = name or row_label

    # Build three variants (one per task) based on student identity
    row_variant = "?"
    if target_identity:
        parts = target_identity.split("|")
        if len(parts) >= 4:
            first, last, middle, group = parts[0], parts[1], parts[2], parts[3]
            variants_render = []
            for n in expected_numbers:
                vmax = _find_process_variants_max(cfg, n)
                try:
                    v_idx = assign_variant(
                        surname=last,
                        name=first,
                        patronymic=middle,
                        group=group,
                        repo=f"{REPO_SALT}/processes/task-{n}",
                        num_variants=vmax,
                    )
                    variants_render.append(str(v_idx + 1))
                except Exception:
                    variants_render.append("?")
            row_variant = "<br/>".join(variants_render)
    processes_rows = [
        {
            "task": row_label,
            "variant": row_variant,
            "groups": proc_groups,
            "r_values": proc_r_values,
            "r_total": sum(proc_r_values),
            "total": total_points_sum,
        }
    ]

    # Rebuild threads rows with resolved perf stats
    threads_rows = _build_rows_for_task_types(
        task_types_threads,
        threads_task_dirs,
        perf_stats,
        cfg,
        eff_num_proc,
        deadlines_cfg,
    )

    parser = argparse.ArgumentParser(description="Generate HTML scoreboard.")
    parser.add_argument(
        "-o", "--output", type=str, required=True, help="Output directory path"
    )
    args = parser.parse_args()

    output_path = Path(args.output)
    output_path.mkdir(parents=True, exist_ok=True)

    # Render tables
    generated_msk = _now_msk().strftime("%Y-%m-%d %H:%M:%S")
    table_template = env.get_template("index.html.j2")
    threads_vmax = int((cfg.get("threads", {}) or {}).get("variants_max", 1))
    # Build display deadlines (use file values if present, fill missing with auto)
    threads_order = task_types_threads
    auto_threads_dl = _compute_display_deadlines_threads(threads_order)
    dl_threads_out = {}
    for t in threads_order:
        base_date = auto_threads_dl.get(t)
        # Default = 0 shift
        shift_days = 0
        label = None
        if deadlines_display_threads and t in deadlines_display_threads:
            val = deadlines_display_threads.get(t)
            if isinstance(val, int):
                shift_days = val
            else:
                # try int-like string, else treat as explicit label
                try:
                    shift_days = int(str(val).strip())
                except Exception:
                    label = str(val)
        if label is None and isinstance(base_date, date):
            vdate = base_date + timedelta(days=shift_days)
            dl_threads_out[t] = _abbr(vdate)
        else:
            dl_threads_out[t] = label or ""

    threads_html = table_template.render(
        task_types=task_types_threads,
        rows=threads_rows,
        generated_msk=generated_msk,
        repo_salt=REPO_SALT,
        threads_variants_max=threads_vmax,
        deadlines_threads=dl_threads_out,
    )
    # Use dedicated template for processes table layout
    processes_template = env.get_template("processes.html.j2")
    proc_vmaxes = [_find_process_variants_max(cfg, n) for n in expected_numbers]
    # Build display deadlines for processes in task order (1..3)
    auto_proc_dl = _compute_display_deadlines_processes(len(expected_numbers))
    proc_deadlines_list: list[str] = []
    for i, n in enumerate(expected_numbers):
        base_date = auto_proc_dl[i]
        shift_days = 0
        label = None
        if deadlines_display_processes:
            key = f"task_{n}"
            val = deadlines_display_processes.get(
                key
            ) or deadlines_display_processes.get(f"mpi_task_{n}")
            if val is not None:
                if isinstance(val, int):
                    shift_days = val
                else:
                    try:
                        shift_days = int(str(val).strip())
                    except Exception:
                        label = str(val)
        if label is None and isinstance(base_date, date):
            vdate = base_date + timedelta(days=shift_days)
            proc_deadlines_list.append(_abbr(vdate))
        else:
            proc_deadlines_list.append(label or "")

    processes_html = processes_template.render(
        top_task_names=proc_top_headers,
        group_headers=proc_group_headers,
        rows=processes_rows,
        generated_msk=generated_msk,
        repo_salt=REPO_SALT,
        processes_variants_max=proc_vmaxes,
        deadlines_processes=proc_deadlines_list,
    )

    with open(output_path / "threads.html", "w") as f:
        f.write(threads_html)
    with open(output_path / "processes.html", "w") as f:
        f.write(processes_html)

    # ——— Build per-group pages and group menus ————————————————————————
    def _load_group_number(dir_name: str):
        import json

        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return None
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            return data.get("student", {}).get("group_number")
        except Exception:
            return None

    def _slugify(text: str) -> str:
        return "".join(
            ch if ch.isalnum() or ch in ("-", "_") else "_" for ch in str(text)
        )

    # Collect groups
    threads_groups = sorted(
        set(filter(None, (_load_group_number(d) for d in threads_task_dirs)))
    )
    processes_groups = sorted(
        set(filter(None, (_load_group_number(d) for d in processes_task_dirs)))
    )

    # Threads: per-group pages
    threads_groups_menu = []
    for g in threads_groups:
        slug = _slugify(g)
        out_file = output_path / f"threads_{slug}.html"
        filtered_dirs = [d for d in threads_task_dirs if _load_group_number(d) == g]
        rows_g = _build_rows_for_task_types(
            task_types_threads,
            filtered_dirs,
            perf_stats,
            cfg,
            eff_num_proc,
            deadlines_cfg,
        )
        # Rebuild deadline labels for this page
        auto_threads_dl_g = _compute_display_deadlines_threads(threads_order)
        dl_threads_out_g = {}
        for t in threads_order:
            base_date = auto_threads_dl_g.get(t)
            shift_days = 0
            label = None
            if deadlines_display_threads and t in deadlines_display_threads:
                val = deadlines_display_threads.get(t)
                if isinstance(val, int):
                    shift_days = val
                else:
                    try:
                        shift_days = int(str(val).strip())
                    except Exception:
                        label = str(val)
            if label is None and isinstance(base_date, date):
                vdate = base_date + timedelta(days=shift_days)
                dl_threads_out_g[t] = _abbr(vdate)
            else:
                dl_threads_out_g[t] = label or ""

        html_g = table_template.render(
            task_types=task_types_threads,
            rows=rows_g,
            generated_msk=generated_msk,
            repo_salt=REPO_SALT,
            threads_variants_max=threads_vmax,
            deadlines_threads=dl_threads_out_g,
        )
        with open(out_file, "w") as f:
            f.write(html_g)
        threads_groups_menu.append({"href": out_file.name, "title": g})

    # Processes: per-group pages
    processes_groups_menu = []
    for g in processes_groups:
        slug = _slugify(g)
        out_file = output_path / f"processes_{slug}.html"
        filtered_dirs = [d for d in processes_task_dirs if _load_group_number(d) == g]

        # Reuse earlier logic but limited to filtered_dirs
        import json as _json

        def _load_student_info_group(dir_name: str):
            info_path = tasks_dir / dir_name / "info.json"
            if not info_path.exists():
                return None
            try:
                with open(info_path, "r") as f:
                    data = _json.load(f)
                return data.get("student", {})
            except Exception:
                return None

        def _id_key(stud: dict) -> str:
            return "|".join(
                [
                    str(stud.get("first_name", "")),
                    str(stud.get("last_name", "")),
                    str(stud.get("middle_name", "")),
                    str(stud.get("group_number", "")),
                ]
            )

        proc_infos_g = []
        for d in filtered_dirs:
            s = _load_student_info_group(d)
            if s:
                proc_infos_g.append((d, s))

        target_identity_g = None
        if "example_processes" in filtered_dirs:
            s0 = _load_student_info_group("example_processes")
            if s0 and s0.get("group_number") == g:
                target_identity_g = _id_key(s0)
        if not target_identity_g and proc_infos_g:
            cnt = Counter(_id_key(s) for _, s in proc_infos_g)
            target_identity_g = cnt.most_common(1)[0][0]

        num_to_dir_g: dict[int, tuple[str, str]] = {}
        if target_identity_g:
            for d, s in proc_infos_g:
                if _id_key(s) == target_identity_g:
                    try:
                        tn = int(str(s.get("task_number", "0")))
                    except Exception:
                        continue
                    num_to_dir_g[tn] = (d, d)

        proc_top_headers_g = []
        proc_group_headers_g = []
        proc_groups_g = []
        proc_r_values_g = []
        total_points_sum_g = 0
        for n in [1, 2, 3]:
            entry = num_to_dir_g.get(n)
            if entry:
                d, display_label = entry
                proc_top_headers_g.append(f"task-{n}")
                for ttype in ["mpi", "seq"]:
                    proc_group_headers_g.append({"type": ttype})
                    # build cell
                    status = directories[d].get(ttype)
                    sol_points, solution_style = get_solution_points_and_style(
                        ttype, status, cfg
                    )
                    task_points = sol_points
                    is_cheated, plagiarism_points = (
                        check_plagiarism_and_calculate_penalty(
                            d,
                            ttype,
                            sol_points,
                            plagiarism_cfg,
                            cfg,
                            semester="processes",
                        )
                    )
                    task_points += plagiarism_points
                    perf_val = perf_stats.get(d, {}).get(ttype, "?")
                    acceleration, efficiency = calculate_performance_metrics(
                        perf_val, eff_num_proc, ttype
                    )
                    deadline_points = calculate_deadline_penalty(
                        d, ttype, status, deadlines_cfg, tasks_dir
                    )
                    proc_groups_g.append(
                        {
                            "solution_points": sol_points,
                            "solution_style": solution_style,
                            "perf": perf_val,
                            "acceleration": acceleration,
                            "efficiency": efficiency,
                            "deadline_points": deadline_points,
                            "plagiarised": is_cheated,
                            "plagiarism_points": plagiarism_points,
                        }
                    )
                # Override displayed points to processes maxima and recompute P
                s_mpi_g, s_seq_g, a_max_g, r_max_g = _find_process_points(cfg, n)
                has_mpi_g = bool(directories[d].get("mpi"))
                has_seq_g = bool(directories[d].get("seq"))
                report_present_g = (tasks_dir / d / "report.md").exists()
                base_idx = len(proc_groups_g) - 2
                if base_idx >= 0:
                    proc_groups_g[base_idx]["solution_points"] = (
                        s_mpi_g if has_mpi_g else 0
                    )
                    proc_groups_g[base_idx + 1]["solution_points"] = (
                        s_seq_g if has_seq_g else 0
                    )
                    # Performance for MPI cell
                    mpi_eff_g = proc_groups_g[base_idx].get("efficiency", "N/A")
                    perf_points_mpi_g = (
                        _calc_perf_points_from_efficiency(mpi_eff_g, a_max_g)
                        if (has_mpi_g and has_seq_g)
                        else 0
                    )
                    if isinstance(mpi_eff_g, str) and mpi_eff_g.endswith("%"):
                        perf_points_mpi_display_g = perf_points_mpi_g
                    else:
                        perf_points_mpi_display_g = "—"
                    proc_groups_g[base_idx]["perf_points"] = perf_points_mpi_g
                    proc_groups_g[base_idx]["perf_points_display"] = (
                        perf_points_mpi_display_g
                    )
                    proc_groups_g[base_idx + 1]["perf_points"] = 0
                    try:
                        plag_coeff_g = float(
                            (cfg.get("copying", {}) or cfg.get("plagiarism", {})).get(
                                "coefficient", 0.0
                            )
                        )
                    except Exception:
                        plag_coeff_g = 0.0
                    p_mpi_g = (
                        -plag_coeff_g * s_mpi_g
                        if (has_mpi_g and proc_groups_g[base_idx].get("plagiarised"))
                        else 0
                    )
                    p_seq_g = (
                        -plag_coeff_g * s_seq_g
                        if (
                            has_seq_g and proc_groups_g[base_idx + 1].get("plagiarised")
                        )
                        else 0
                    )
                    proc_groups_g[base_idx]["plagiarism_points"] = p_mpi_g
                    proc_groups_g[base_idx + 1]["plagiarism_points"] = p_seq_g

                # Sum points by processes S + P + R (and C penalties)
                s_inc_g = (s_mpi_g if has_mpi_g else 0) + (s_seq_g if has_seq_g else 0)
                r_inc_g = r_max_g if report_present_g else 0
                total_points_sum_g += (
                    s_inc_g + perf_points_mpi_g + r_inc_g + p_mpi_g + p_seq_g
                )
                proc_r_values_g.append(r_inc_g)
            else:
                proc_top_headers_g.append(f"task-{n}")
                for ttype in ["mpi", "seq"]:
                    proc_group_headers_g.append({"type": ttype})
                    proc_groups_g.append(
                        {
                            "solution_points": "?",
                            "solution_style": "",
                            "perf": "?",
                            "acceleration": "?",
                            "efficiency": "?",
                            "deadline_points": "?",
                            "plagiarised": False,
                            "plagiarism_points": "?",
                        }
                    )
                # Missing task: do not affect total; sum only existing; report=0
                proc_r_values_g.append(0)

        # Row label for group page: name without group (three lines max)
        row_label_g = f"group {g}"
        if target_identity_g:
            parts = target_identity_g.split("|")
            if len(parts) >= 3:
                first, last, middle = parts[0], parts[1], parts[2]
                nm_parts = [p for p in [last, first, middle] if p]
                nm = "<br/>".join(nm_parts)
                row_label_g = nm or row_label_g

        # Build three variants (one per task) based on student identity
        row_variant_g = "?"
        if target_identity_g:
            parts = target_identity_g.split("|")
            if len(parts) >= 4:
                first, last, middle, group = parts[0], parts[1], parts[2], parts[3]
                vrender = []
                for n in [1, 2, 3]:
                    vmax = _find_process_variants_max(cfg, n)
                    try:
                        v_idx = assign_variant(
                            surname=last,
                            name=first,
                            patronymic=middle,
                            group=group,
                            repo=f"{REPO_SALT}/processes/task-{n}",
                            num_variants=vmax,
                        )
                        vrender.append(str(v_idx + 1))
                    except Exception:
                        vrender.append("?")
                row_variant_g = "<br/>".join(vrender)

        rows_g = [
            {
                "task": row_label_g,
                "variant": row_variant_g,
                "groups": proc_groups_g,
                "r_values": proc_r_values_g,
                "r_total": sum(proc_r_values_g),
                "total": total_points_sum_g,
            }
        ]

        proc_vmaxes_g = [_find_process_variants_max(cfg, n) for n in [1, 2, 3]]
        # Build display deadlines for processes group page
        auto_proc_dl_g = _compute_display_deadlines_processes(3)
        proc_deadlines_list_g: list[str] = []
        for i, n in enumerate([1, 2, 3]):
            base_date = auto_proc_dl_g[i]
            shift_days = 0
            label = None
            if deadlines_display_processes:
                key = f"task_{n}"
                val = deadlines_display_processes.get(
                    key
                ) or deadlines_display_processes.get(f"mpi_task_{n}")
                if val is not None:
                    if isinstance(val, int):
                        shift_days = val
                    else:
                        try:
                            shift_days = int(str(val).strip())
                        except Exception:
                            label = str(val)
            if label is None and isinstance(base_date, date):
                vdate = base_date + timedelta(days=shift_days)
                proc_deadlines_list_g.append(_abbr(vdate))
            else:
                proc_deadlines_list_g.append(label or "")

        html_g = processes_template.render(
            top_task_names=proc_top_headers_g,
            group_headers=proc_group_headers_g,
            rows=rows_g,
            generated_msk=generated_msk,
            repo_salt=REPO_SALT,
            processes_variants_max=proc_vmaxes_g,
            deadlines_processes=proc_deadlines_list_g,
        )
        with open(out_file, "w") as f:
            f.write(html_g)
        processes_groups_menu.append({"href": out_file.name, "title": g})

    # Render index menu page
    try:
        menu_template = env.get_template("menu_index.html.j2")
    except Exception:
        # Simple fallback menu if template missing
        menu_html_content = (
            '<html><head><title>Scoreboard</title><link rel="stylesheet" '
            'type="text/css" href="static/main.css"></head><body>'
            "<h1>Scoreboard</h1>"
            "<ul>"
            '<li><a href="threads.html">Threads Scoreboard</a></li>'
            '<li><a href="processes.html">Processes Scoreboard</a></li>'
            "</ul></body></html>"
        )
    else:
        menu_html_content = menu_template.render(
            pages=[
                {"href": "threads.html", "title": "Threads Scoreboard"},
                {"href": "processes.html", "title": "Processes Scoreboard"},
            ],
            groups_threads=threads_groups_menu,
            groups_processes=processes_groups_menu,
            generated_msk=generated_msk,
        )

    with open(output_path / "index.html", "w") as f:
        f.write(menu_html_content)

    # Copy static assets
    static_src = script_dir / "static"
    static_dst = output_path / "static"
    if static_src.exists():
        if static_dst.exists():
            shutil.rmtree(static_dst)
        shutil.copytree(static_src, static_dst)
        logger.info("Static directory copied to %s", static_dst)
    else:
        logger.warning("Static directory not found at %s", static_src)

    logger.info(
        "HTML pages generated at %s (index.html, threads.html, processes.html)",
        output_path,
    )


if __name__ == "__main__":
    main()
