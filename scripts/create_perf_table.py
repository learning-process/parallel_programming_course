import argparse
import os
import re
import xlsxwriter
import csv

# -------------------------------
# Helpers and configuration
# -------------------------------

# Known task types (used to pre-initialize tables)
list_of_type_of_tasks = ["all", "mpi", "omp", "seq", "stl", "tbb"]

# Compile patterns once
OLD_PATTERN = re.compile(r"tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)")
NEW_PATTERN = re.compile(
    r"(\w+_test_task_(threads|processes))_(\w+)_enabled:(\w*):(-*\d*\.\d*)"
)
# Example formats:
#   example_threads_omp_enabled:task_run:0.4749
#   example_processes_2_mpi_enabled:pipeline:0.0507
SIMPLE_PATTERN = re.compile(
    r"(.+?)_(omp|seq|tbb|stl|all|mpi)_enabled:(task_run|pipeline):(-*\d*\.\d*)"
)


def _ensure_task_tables(result_tables: dict, perf_type: str, task_name: str) -> None:
    if perf_type not in result_tables:
        result_tables[perf_type] = {}
    if task_name not in result_tables[perf_type]:
        result_tables[perf_type][task_name] = {t: -1.0 for t in list_of_type_of_tasks}


def _infer_category(task_name: str) -> str:
    return "threads" if "threads" in task_name else "processes"


def _columns_for_category(category: str) -> list[str]:
    return (
        ["seq", "omp", "tbb", "stl", "all"] if category == "threads" else ["seq", "mpi"]
    )


def _write_excel_sheet(
    workbook,
    worksheet,
    cpu_num: int,
    tasks_list: list[str],
    cols: list[str],
    table: dict,
):
    worksheet.set_column("A:Z", 23)
    right_bold_border = workbook.add_format({"bold": True, "right": 2, "bottom": 2})
    bottom_bold_border = workbook.add_format({"bold": True, "bottom": 2})
    right_border = workbook.add_format({"right": 2})

    worksheet.write(0, 0, "cpu_num = " + str(cpu_num), right_bold_border)

    # Header (T_x, S, Eff) per column
    col = 1
    for ttype in cols:
        worksheet.write(0, col, f"T_{ttype}({cpu_num})", bottom_bold_border)
        col += 1
        worksheet.write(
            0,
            col,
            f"S({cpu_num}) = T_seq({cpu_num}) / T_{ttype}({cpu_num})",
            bottom_bold_border,
        )
        col += 1
        worksheet.write(
            0, col, f"Eff({cpu_num}) = S({cpu_num}) / {cpu_num}", right_bold_border
        )
        col += 1

    # Task rows
    row = 1
    for task_name in tasks_list:
        worksheet.write(
            row, 0, task_name, workbook.add_format({"bold": True, "right": 2})
        )
        row += 1

    # Values
    row = 1
    for task_name in tasks_list:
        col = 1
        for ttype in cols:
            if task_name not in table:
                # no data for task at all
                worksheet.write(row, col, "—")
                col += 1
                worksheet.write(row, col, "—")
                col += 1
                worksheet.write(row, col, "—", right_border)
                col += 1
                continue
            par_time = table[task_name].get(ttype, -1.0)
            seq_time = table[task_name].get("seq", -1.0)
            if par_time in (0.0, -1.0) or seq_time in (0.0, -1.0):
                speed_up = "—"
                efficiency = "—"
            else:
                speed_up = seq_time / par_time
                efficiency = speed_up / cpu_num
            worksheet.write(row, col, par_time if par_time != -1.0 else "?")
            col += 1
            worksheet.write(row, col, speed_up)
            col += 1
            worksheet.write(row, col, efficiency, right_border)
            col += 1
        row += 1


def _write_csv(path: str, header: list[str], tasks_list: list[str], table: dict):
    with open(path, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(header)
        for task_name in tasks_list:
            seq_time = table.get(task_name, {}).get("seq", -1.0)
            if seq_time in (0.0, -1.0):
                writer.writerow([task_name] + ["?" for _ in header[1:]])
                continue
            row = [task_name, 1.0]
            # Remaining headers correspond to columns starting from 2
            for col_name in header[2:]:
                val = table[task_name].get(col_name.lower(), -1.0)
                row.append(val / seq_time if val != -1.0 else "?")
            writer.writerow(row)


parser = argparse.ArgumentParser()
parser.add_argument(
    "-i", "--input", help="Input file path (logs of perf tests, .txt)", required=True
)
parser.add_argument(
    "-o", "--output", help="Output file path (path to .xlsx table)", required=True
)
args = parser.parse_args()
logs_path = os.path.abspath(args.input)
xlsx_path = os.path.abspath(args.output)

# For each perf_type (pipeline/task_run) store times per task
result_tables = {"pipeline": {}, "task_run": {}}
# Map task name -> category (threads|processes)
task_categories = {}
# Track tasks per category to split output
tasks_by_category = {"threads": set(), "processes": set()}

with open(logs_path, "r") as logs_file:
    logs_lines = logs_file.readlines()
for line in logs_lines:
    # Handle both old format: tasks/task_type/task_name:perf_type:time
    # and new format: namespace_task_type_enabled:perf_type:time
    old_result = OLD_PATTERN.findall(line)
    new_result = NEW_PATTERN.findall(line)
    simple_result = SIMPLE_PATTERN.findall(line)

    if len(old_result):
        task_name = old_result[0][1]
        perf_type = old_result[0][2]
        # legacy: track task in threads category by default
        _ensure_task_tables(result_tables, perf_type, task_name)
        # Unknown category in legacy format; default to threads
        task_categories[task_name] = "threads"
        tasks_by_category["threads"].add(task_name)
    elif len(new_result):
        # Extract task name from namespace (e.g., "example_threads" from "nesterov_a_test_task_threads")
        full_task_name = new_result[0][0]
        task_category = new_result[0][1]  # "threads" or "processes"
        task_name = f"example_{task_category}"
        perf_type = new_result[0][3]

        # no set tracking needed; category mapping below

        _ensure_task_tables(result_tables, perf_type, task_name)
        task_categories[task_name] = task_category
        tasks_by_category[task_category].add(task_name)
    elif len(simple_result):
        # Extract task name in the current format (prefix already includes category suffix)
        task_name = simple_result[0][0]
        # Infer category by substring
        task_category = "threads" if "threads" in task_name else "processes"
        perf_type = simple_result[0][2]

        # no set tracking needed; category mapping below

        _ensure_task_tables(result_tables, perf_type, task_name)
        task_categories[task_name] = task_category
        tasks_by_category[task_category].add(task_name)

for line in logs_lines:
    # Handle both old format: tasks/task_type/task_name:perf_type:time
    # and new format: namespace_task_type_enabled:perf_type:time
    old_result = OLD_PATTERN.findall(line)
    new_result = NEW_PATTERN.findall(line)
    simple_result = SIMPLE_PATTERN.findall(line)

    if len(old_result):
        task_type = old_result[0][0]
        task_name = old_result[0][1]
        perf_type = old_result[0][2]
        perf_time = float(old_result[0][3])
        if perf_time < 0.001:
            msg = f"Performance time = {perf_time} < 0.001 second : for {task_type} - {task_name} - {perf_type} \n"
            raise Exception(msg)
        result_tables[perf_type][task_name][task_type] = perf_time
    elif len(new_result):
        # Extract task details from namespace format
        task_category = new_result[0][1]  # "threads" or "processes"
        task_type = new_result[0][2]  # "all", "omp", "seq", etc.
        perf_type = new_result[0][3]
        perf_time = float(new_result[0][4])
        task_name = f"example_{task_category}"

        if perf_time < 0.001:
            msg = f"Performance time = {perf_time} < 0.001 second : for {task_type} - {task_name} - {perf_type} \n"
            raise Exception(msg)

        if task_name in result_tables[perf_type]:
            result_tables[perf_type][task_name][task_type] = perf_time
        task_categories[task_name] = task_category
        tasks_by_category[task_category].add(task_name)
    elif len(simple_result):
        # Extract details from the simplified pattern (current logs)
        task_name = simple_result[0][0]
        # Infer category by substring present in task_name
        task_category = "threads" if "threads" in task_name else "processes"
        task_type = simple_result[0][1]
        perf_type = simple_result[0][2]
        perf_time = float(simple_result[0][3])

        if perf_time < 0.001:
            msg = f"Performance time = {perf_time} < 0.001 second : for {task_type} - {task_name} - {perf_type} \n"
            raise Exception(msg)

        if perf_type not in result_tables:
            result_tables[perf_type] = {}
        if task_name not in result_tables[perf_type]:
            result_tables[perf_type][task_name] = {}
            for ttype in list_of_type_of_tasks:
                result_tables[perf_type][task_name][ttype] = -1.0
        result_tables[perf_type][task_name][task_type] = perf_time
        task_categories[task_name] = task_category
        tasks_by_category[task_category].add(task_name)


for table_name, table_data in result_tables.items():
    # Prepare two workbooks/CSVs: threads and processes
    for category in ["threads", "processes"]:
        tasks_list = sorted(tasks_by_category[category])
        if not tasks_list:
            continue

        # Use appropriate env var per category
        if category == "threads":
            cpu_num_env = os.environ.get("PPC_NUM_THREADS")
            if cpu_num_env is None:
                raise EnvironmentError(
                    "Required environment variable 'PPC_NUM_THREADS' is not set."
                )
        else:
            cpu_num_env = os.environ.get("PPC_NUM_PROC")
            if cpu_num_env is None:
                raise EnvironmentError(
                    "Required environment variable 'PPC_NUM_PROC' is not set."
                )
        cpu_num = int(cpu_num_env)
        cols = _columns_for_category(category)

        # Excel
        wb_path = os.path.join(
            xlsx_path, f"{category}_" + table_name + "_perf_table.xlsx"
        )
        workbook = xlsxwriter.Workbook(wb_path)
        worksheet = workbook.add_worksheet()
        _write_excel_sheet(workbook, worksheet, cpu_num, tasks_list, cols, table_data)
        workbook.close()

        # CSV
        header = ["Task", "SEQ"] + [c.upper() for c in cols[1:]]
        csv_path = os.path.join(
            xlsx_path, f"{category}_" + table_name + "_perf_table.csv"
        )
        _write_csv(csv_path, header, tasks_list, table_data)
