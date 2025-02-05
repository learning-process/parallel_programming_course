from pathlib import Path
from collections import defaultdict

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

print(directories)

columns = ''.join(['<th colspan=4 style="text-align: center;">' + task_type + '</th>' for task_type in task_types])
html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Task Directories</title>
    <link rel="stylesheet" type="text/css" href="static/main.css">
</head>
<body>
    <h1>Scoreboard</h1>
    <p>S - Solution, P - Performance, O - Overdue, C - Cheating</p>
    <table>
        <tr>
            <th colspan=4>Tasks</th>
            {columns}
            <th>Total</th>
        </tr>
        <tr>
            <th colspan=4></th>
            {''.join(['<th>S</th><th>P</th><th>O</th><th>C</th>' for _ in range(len(task_types))])}
            <th></th>
        </tr>
"""

for dir in directories:
    html_content += f"<tr><td colspan=4>{dir}</td>"
    total_count = 0
    for task_type in task_types:
        if directories[dir].get(task_type) == "done":
            html_content += '<td style="text-align: center;">1</td>'
            total_count += 1
        elif directories[dir].get(task_type) == "disabled":
            html_content += '<td style="text-align: center;background-color: lightblue;">1</td>'
            total_count += 1
        else:
            html_content += "<td>0</td>"
        html_content += '<td style="text-align: center;">0</td>'
        html_content += '<td style="text-align: center;">0</td>'
        html_content += '<td style="text-align: center;">0</td>'
    html_content += f'<td style="text-align: center;">{total_count}</td>'
    html_content += "</tr>"

html_content += """
    </table>
</body>
</html>
"""

output_file = Path('scoreboard/index.html')
with open(output_file, 'w') as file:
    file.write(html_content)

print(f"HTML page generated at {output_file}")
