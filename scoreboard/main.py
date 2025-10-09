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

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

task_types = ["all", "mpi", "omp", "seq", "stl", "tbb"]
task_types_threads = ["all", "omp", "seq", "stl", "tbb"]
task_types_processes = ["mpi", "seq"]

script_dir = Path(__file__).parent
tasks_dir = script_dir.parent / "tasks"


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


def load_performance_data(perf_stat_file_path):
    """Load and parse performance statistics from CSV file."""

    perf_stats = dict()
    if perf_stat_file_path.exists():
        with open(perf_stat_file_path, "r", newline="") as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                task_name = row.get("Task")
                if task_name:
                    perf_stats[task_name] = {
                        "seq": row.get("SEQ", "?"),
                        "omp": row.get("OMP", "?"),
                        "tbb": row.get("TBB", "?"),
                        "stl": row.get("STL", "?"),
                        "all": row.get("ALL", "?"),
                        "mpi": "N/A",
                    }
    else:
        logger.warning("Performance stats CSV not found at %s", perf_stat_file_path)
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


def get_solution_points_and_style(task_type, status, cfg):
    """Get solution points and CSS style based on task type and status."""
    max_sol_points = int(cfg["scoreboard"]["task"][task_type]["solution"]["max"])
    sol_points = max_sol_points if status in ("done", "disabled") else 0
    solution_style = ""
    if status == "done":
        solution_style = "background-color: lightgreen;"
    elif status == "disabled":
        solution_style = "background-color: #6495ED;"
    return sol_points, solution_style


def check_plagiarism_and_calculate_penalty(
    dir, task_type, sol_points, plagiarism_cfg, cfg
):
    """Check if task is plagiarized and calculate penalty points."""
    clean_dir = dir[: -len("_disabled")] if dir.endswith("_disabled") else dir
    is_cheated = (
        dir in plagiarism_cfg["plagiarism"][task_type]
        or clean_dir in plagiarism_cfg["plagiarism"][task_type]
    )
    plagiarism_points = 0
    if is_cheated:
        plag_coeff = float(cfg["scoreboard"]["plagiarism"]["coefficient"])
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
    """Load all configuration files and return parsed data."""
    config_path = Path(__file__).parent / "data" / "threads-config.yml"
    assert config_path.exists(), f"Config file not found: {config_path}"
    with open(config_path, "r") as file:
        cfg = yaml.safe_load(file)
    assert cfg, "Configuration is empty"

    eff_num_proc = int(cfg["scoreboard"].get("efficiency", {}).get("num_proc", 1))
    deadlines_cfg = cfg["scoreboard"].get("deadlines", {})

    plagiarism_config_path = Path(__file__).parent / "data" / "plagiarism.yml"
    with open(plagiarism_config_path, "r") as file:
        plagiarism_cfg = yaml.safe_load(file)
    assert plagiarism_cfg, "Plagiarism configuration is empty"

    return cfg, eff_num_proc, deadlines_cfg, plagiarism_cfg


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

    def _load_variant(dir_name: str):
        import json
        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return "?"
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            return str(data.get("student", {}).get("variant_number", "?"))
        except Exception:
            return "?"

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
                dir, task_type, sol_points, plagiarism_cfg, cfg
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

            row_types.append(
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
            total_count += task_points

        label_name = _load_student_info_label(dir) or dir
        variant = _load_variant(dir)
        rows.append({
            "task": label_name,
            "variant": variant,
            "types": row_types,
            "total": total_count,
        })
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

    # Locate perf CSV from CI or local runs
    candidates = [
        script_dir.parent / "build" / "perf_stat_dir" / "task_run_perf_table.csv",
        script_dir.parent / "perf_stat_dir" / "task_run_perf_table.csv",
    ]
    perf_stat_file_path = next((p for p in candidates if p.exists()), candidates[0])

    # Read and parse performance statistics CSV
    perf_stats = load_performance_data(perf_stat_file_path)

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

    def _build_cell(dir_name: str, ttype: str):
        status = directories[dir_name].get(ttype)
        sol_points, solution_style = get_solution_points_and_style(ttype, status, cfg)
        task_points = sol_points
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            dir_name, ttype, sol_points, plagiarism_cfg, cfg
        )
        task_points += plagiarism_points
        perf_val = perf_stats.get(dir_name, {}).get(ttype, "?")
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
            for ttype in ["mpi", "seq"]:
                cell, pts = _build_cell(d, ttype)
                proc_groups.append(cell)
                total_points_sum += pts
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
            # Do not affect total; sum only existing tasks

    # Label for processes row: show Last, First, Middle on separate lines; no group number
    row_label = "processes"
    row_variant = "?"
    if target_identity:
        parts = target_identity.split("|")
        if len(parts) >= 4:
            first, last, middle, _group = parts[0], parts[1], parts[2], parts[3]
            name_parts = [p for p in [last, first, middle] if p]
            name = "<br/>".join(name_parts)
            row_label = name or row_label
    # Choose variant from the first available task (1..3)
    def _load_variant(dir_name: str):
        import json
        info_path = tasks_dir / dir_name / "info.json"
        if not info_path.exists():
            return "?"
        try:
            with open(info_path, "r") as f:
                data = json.load(f)
            return str(data.get("student", {}).get("variant_number", "?"))
        except Exception:
            return "?"
    for n in expected_numbers:
        ent = num_to_dir.get(n)
        if ent:
            row_variant = _load_variant(ent[0])
            break
    processes_rows = [
        {
            "task": row_label,
            "variant": row_variant,
            "groups": proc_groups,
            "total": total_points_sum,
        }
    ]

    parser = argparse.ArgumentParser(description="Generate HTML scoreboard.")
    parser.add_argument(
        "-o", "--output", type=str, required=True, help="Output directory path"
    )
    args = parser.parse_args()

    output_path = Path(args.output)
    output_path.mkdir(parents=True, exist_ok=True)

    # Render tables
    table_template = env.get_template("index.html.j2")
    threads_html = table_template.render(
        task_types=task_types_threads, rows=threads_rows
    )
    # Use dedicated template for processes table layout
    processes_template = env.get_template("processes.html.j2")
    processes_html = processes_template.render(
        top_task_names=proc_top_headers, group_headers=proc_group_headers, rows=processes_rows
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
        return "".join(ch if ch.isalnum() or ch in ("-", "_") else "_" for ch in str(text))

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
            task_types_threads, filtered_dirs, perf_stats, cfg, eff_num_proc, deadlines_cfg
        )
        html_g = table_template.render(task_types=task_types_threads, rows=rows_g)
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
                    is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
                        d, ttype, sol_points, plagiarism_cfg, cfg
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
                    total_points_sum_g += task_points
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
                # Missing task: do not affect total; sum only existing

        # Row label for group page: name without group (three lines max)
        row_label_g = f"group {g}"
        if target_identity_g:
            parts = target_identity_g.split("|")
            if len(parts) >= 4:
                first, last, middle, _group = parts[0], parts[1], parts[2], parts[3]
                nm_parts = [p for p in [last, first, middle] if p]
                nm = "<br/>".join(nm_parts)
                row_label_g = nm or row_label_g
        # Variant for group row
        def _load_variant_g(dir_name: str):
            import json
            info_path = tasks_dir / dir_name / "info.json"
            if not info_path.exists():
                return "?"
            try:
                with open(info_path, "r") as f:
                    data = json.load(f)
                return str(data.get("student", {}).get("variant_number", "?"))
            except Exception:
                return "?"
        row_variant_g = "?"
        for n in [1, 2, 3]:
            entry2 = num_to_dir_g.get(n)
            if entry2:
                row_variant_g = _load_variant_g(entry2[0])
                break

        rows_g = [
            {
                "task": row_label_g,
                "variant": row_variant_g,
                "groups": proc_groups_g,
                "total": total_points_sum_g,
            }
        ]

        html_g = processes_template.render(
            top_task_names=proc_top_headers_g,
            group_headers=proc_group_headers_g,
            rows=rows_g,
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
