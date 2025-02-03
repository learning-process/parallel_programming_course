from pathlib import Path
from collections import defaultdict

task_types = ['all', 'mpi', 'omp', 'seq', 'stl', 'tbb']

tasks_dir = Path('tasks')

directories = defaultdict(dict)

for task_type in task_types:
    task_type_dir = tasks_dir / task_type
    if task_type_dir.exists() and task_type_dir.is_dir():
        for task_name in (d.name for d in task_type_dir.iterdir() if d.is_dir()):
            directories[task_name][task_type] = True

print(directories)

columns = ''.join(['<th>' + task_type + '</th>' for task_type in task_types])
html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Task Directories</title>
    <link rel="stylesheet" type="text/css" href="static/main.css">
</head>
<body>
    <h1>Scoreboard</h1>
    <table>
        <tr>
            <th>Tasks</th>
            {columns}
        </tr>
"""

for dir in directories:
    html_content += f"<tr><td>{dir}</td>"
    for task_type in task_types:
        if directories[dir].get(task_type):
            html_content += "<td>✔</td>"
        else:
            html_content += "<td>✘</td>"
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
