from pathlib import Path
from collections import defaultdict
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

        rows.append({"task": dir, "types": row_types, "total": total_count})
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
    processes_rows = _build_rows_for_task_types(
        task_types_processes,
        processes_task_dirs,
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
    table_template = env.get_template("index.html.j2")
    threads_html = table_template.render(
        task_types=task_types_threads, rows=threads_rows
    )
    processes_html = table_template.render(
        task_types=task_types_processes, rows=processes_rows
    )

    with open(output_path / "threads.html", "w") as f:
        f.write(threads_html)
    with open(output_path / "processes.html", "w") as f:
        f.write(processes_html)

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
            ]
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
