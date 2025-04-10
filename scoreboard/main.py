from pathlib import Path
from collections import defaultdict
import argparse
import yaml

task_types = ['all', 'mpi', 'omp', 'seq', 'stl', 'tbb']

tasks_dir = Path('tasks')

directories = defaultdict(dict)

for task_type in task_types:
    task_type_dir = tasks_dir / task_type
    if task_type_dir.exists() and task_type_dir.is_dir():
        for task_name in (d.name for d in task_type_dir.iterdir() if d.is_dir()):
            if task_name.endswith("_disabled"):
                task_name = task_name[:-len("_disabled")]
                directories[task_name][task_type] = "disabled"
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
        html_content += '<td style="text-align: center;background-color: lavender;">0</td>'
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
