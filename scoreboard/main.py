from pathlib import Path
from collections import defaultdict
import argparse
import yaml
import csv

task_types = ['all', 'mpi', 'omp', 'seq', 'stl', 'tbb']

tasks_dir = Path('tasks')

directories = defaultdict(dict)

if tasks_dir.exists() and tasks_dir.is_dir():
    for task_name_dir in tasks_dir.iterdir():
        if task_name_dir.is_dir() and task_name_dir.name not in ['common']:
            task_name = task_name_dir.name
            for task_type in task_types:
                task_type_dir = task_name_dir / task_type
                if task_type_dir.exists() and task_type_dir.is_dir():
                    if task_name.endswith("_disabled"):
                        clean_task_name = task_name[:-len("_disabled")]
                        directories[clean_task_name][task_type] = "disabled"
                    else:
                        directories[task_name][task_type] = "done"

config_path = Path(__file__).parent / "data" / "threads-config.yml"
assert config_path.exists(), f"Config file not found: {config_path}"
with open(config_path, 'r') as file:
    cfg = yaml.safe_load(file)
assert cfg, "Configuration is empty"
plagiarism_config_path = Path(__file__).parent / "data" / "plagiarism.yml"
with open(plagiarism_config_path, 'r') as file:
    plagiarism_cfg = yaml.safe_load(file)
assert plagiarism_cfg, "Plagiarism configuration is empty"

columns = ''.join(['<th colspan=5 style="text-align: center;">' + task_type + '</th>' for task_type in task_types])
html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Task Directories</title>
    <link rel="stylesheet" type="text/css" href="static/main.css">
</head>
<body>
    <h1>Scoreboard</h1>
    <h3 style="color: red;">Note:</b> This is experimental and results are for reference only!</h3>
    <p>
        <b>(S)olution</b> - The correctness and completeness of the implemented solution.<br/>
        <b>(A)cceleration</b> - The process of speeding up software to improve performance.
        Speedup = T(seq) / T(parallel)<br/>
        <b>(E)fficiency</b> - Optimizing software speed-up by improving CPU utilization and resource management.
        Efficiency = Speedup / NumProcs * 100%<br/>
        <b>(D)eadline</b> - The timeliness of the submission in relation to the given deadline.<br/>
        <b>(P)lagiarism</b> - The originality of the work, ensuring no copied content from external sources.<br/>
    </p>
    <table>
        <tr>
            <th rowspan=2>Tasks</th>
            {columns}
            <th rowspan=2>Total</th>
        </tr>
        <tr>
            {''.join([
                f'<th style="text-align: center;">{letter}</th>'
                for _ in range(len(task_types))
                for letter in ('S', 'A', 'E', 'D', 'P')
            ])}
        </tr>
"""


perf_stat_file_path = Path(__file__).parent.parent / "build" / "perf_stat_dir" / "task_run_perf_table.csv"

# Read and parse performance statistics CSV
perf_stats = dict()
if perf_stat_file_path.exists():
    with open(perf_stat_file_path, 'r', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            perf_stats[row['Task']] = {
                "seq": row['SEQ'],
                "omp": row['OMP'],
                "tbb": row['TBB'],
                "stl": row['STL'],
                "all": row['ALL'],
                "mpi": "N/A",
            }
else:
    print(f"Warning: Performance stats CSV not found at {perf_stat_file_path}")


for dir in sorted(directories.keys()):
    html_content += f"<tr><td>{dir}</td>"
    total_count = 0
    for task_type in task_types:
        max_sol_points = int(cfg["scoreboard"]["task"][task_type]["solution"]["max"])
        task_count = 0
        if directories[dir].get(task_type) == "done":
            html_content += f'<td style="text-align: center;background-color: lightgreen;">{max_sol_points}</td>'
            task_count += max_sol_points
        elif directories[dir].get(task_type) == "disabled":
            html_content += f'<td style="text-align: center;background-color: #6495ED;">{max_sol_points}</td>'
            task_count += max_sol_points
        else:
            html_content += '<td style="text-align: center;">0</td>'
        html_content += '<td style="text-align: center;background-color: lavender;">'
        if dir in perf_stats.keys():
            html_content += perf_stats[dir][task_type]
        else:
            html_content += '?'
        html_content += '</td>'
        html_content += '<td style="text-align: center;background-color: lavender;">0</td>'
        html_content += '<td style="text-align: center;">0</td>'
        is_cheated = \
            dir in plagiarism_cfg["plagiarism"][task_type] or \
            dir[:-len("_disabled")] in plagiarism_cfg["plagiarism"][task_type]
        if is_cheated:
            plag_coeff = float(cfg["scoreboard"]["plagiarism"]["coefficient"])
            plagiarism_points = -plag_coeff * task_count
            task_count += plagiarism_points
            html_content += f'<td style="text-align: center; background-color: pink;">{plagiarism_points}</td>'
        else:
            html_content += '<td style="text-align: center;">0</td>'
        total_count += task_count
    html_content += f'<td style="text-align: center;">{total_count}</td>'
    html_content += "</tr>"

html_content += """
    </table>
</body>
</html>
"""

parser = argparse.ArgumentParser(description='Generate HTML scoreboard.')
parser.add_argument('-o', '--output', type=str, required=True, help='Output file path')
args = parser.parse_args()

output_file = Path(args.output) / "index.html"
with open(output_file, 'w') as file:
    file.write(html_content)

print(f"HTML page generated at {output_file}")
