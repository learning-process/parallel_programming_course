from pathlib import Path
from collections import defaultdict
from datetime import datetime
import argparse
import subprocess
import yaml
import csv
from jinja2 import Environment, FileSystemLoader
import logging

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)

task_types = ["all", "mpi", "omp", "seq", "stl", "tbb"]

script_dir = Path(__file__).parent
tasks_dir = script_dir.parent / "tasks"

directories = defaultdict(dict)

if tasks_dir.exists() and tasks_dir.is_dir():
    for task_name_dir in tasks_dir.iterdir():
        if task_name_dir.is_dir() and task_name_dir.name not in ["common"]:
            task_name = task_name_dir.name
            for task_type in task_types:
                task_type_dir = task_name_dir / task_type
                if task_type_dir.exists() and task_type_dir.is_dir():
                    if task_name.endswith("_disabled"):
                        clean_task_name = task_name[: -len("_disabled")]
                        directories[clean_task_name][task_type] = "disabled"
                    else:
                        directories[task_name][task_type] = "done"

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

env = Environment(loader=FileSystemLoader(Path(__file__).parent / "templates"))


perf_stat_file_path = (
    script_dir.parent / "build" / "perf_stat_dir" / "task_run_perf_table.csv"
)

# Read and parse performance statistics CSV
perf_stats = dict()
if perf_stat_file_path.exists():
    with open(perf_stat_file_path, "r", newline="") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            perf_stats[row["Task"]] = {
                "seq": row["SEQ"],
                "omp": row["OMP"],
                "tbb": row["TBB"],
                "stl": row["STL"],
                "all": row["ALL"],
                "mpi": "N/A",
            }
else:
    logger.warning("Performance stats CSV not found at %s", perf_stat_file_path)

rows = []
for dir in sorted(directories.keys()):
    row_types = []
    total_count = 0
    for task_type in task_types:
        max_sol_points = int(cfg["scoreboard"]["task"][task_type]["solution"]["max"])
        status = directories[dir].get(task_type)
        sol_points = max_sol_points if status in ("done", "disabled") else 0
        solution_style = ""
        if status == "done":
            solution_style = "background-color: lightgreen;"
        elif status == "disabled":
            solution_style = "background-color: #6495ED;"

        task_points = sol_points
        is_cheated = (
            dir in plagiarism_cfg["plagiarism"][task_type]
            or dir.rstrip("_disabled") in plagiarism_cfg["plagiarism"][task_type]
        )
        plagiarism_points = 0
        if is_cheated:
            plag_coeff = float(cfg["scoreboard"]["plagiarism"]["coefficient"])
            plagiarism_points = -plag_coeff * task_points
            task_points += plagiarism_points

        perf_val = perf_stats.get(dir, {}).get(task_type, "?")

        # Calculate acceleration and efficiency if performance data is available
        acceleration = "?"
        efficiency = "?"
        try:
            perf_float = float(perf_val)
            if perf_float > 0:
                speedup = 1.0 / perf_float
                acceleration = f"{speedup:.2f}"
                efficiency = f"{speedup / eff_num_proc * 100:.2f}"
        except (ValueError, TypeError):
            pass

        # Calculate deadline penalty points
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

template = env.get_template("index.html.j2")
html_content = template.render(task_types=task_types, rows=rows)

parser = argparse.ArgumentParser(description="Generate HTML scoreboard.")
parser.add_argument("-o", "--output", type=str, required=True, help="Output file path")
args = parser.parse_args()

output_file = Path(args.output) / "index.html"
with open(output_file, "w") as file:
    file.write(html_content)

logger.info("HTML page generated at %s", output_file)
