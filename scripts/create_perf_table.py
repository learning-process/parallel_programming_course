import argparse
import os
import re
import xlsxwriter
import csv

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

list_of_type_of_tasks = ["all", "mpi", "omp", "seq", "stl", "tbb"]

result_tables = {"pipeline": {}, "task_run": {}}
set_of_task_name = []

logs_file = open(logs_path, "r")
logs_lines = logs_file.readlines()
for line in logs_lines:
    # Handle both old format: tasks/task_type/task_name:perf_type:time
    # and new format: namespace_task_type_enabled:perf_type:time
    old_pattern = r"tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)"
    new_pattern = (
        r"(\w+_test_task_(threads|processes))_(\w+)_enabled:(\w*):(-*\d*\.\d*)"
    )

    old_result = re.findall(old_pattern, line)
    new_result = re.findall(new_pattern, line)

    if len(old_result):
        task_name = old_result[0][1]
        perf_type = old_result[0][2]
        set_of_task_name.append(task_name)
        result_tables[perf_type][task_name] = {}

        for ttype in list_of_type_of_tasks:
            result_tables[perf_type][task_name][ttype] = -1.0
    elif len(new_result):
        # Extract task name from namespace (e.g., "example_threads" from "nesterov_a_test_task_threads")
        full_task_name = new_result[0][0]
        task_category = new_result[0][1]  # "threads" or "processes"
        task_name = f"example_{task_category}"
        perf_type = new_result[0][3]

        if task_name not in set_of_task_name:
            set_of_task_name.append(task_name)

        if perf_type not in result_tables:
            result_tables[perf_type] = {}
        if task_name not in result_tables[perf_type]:
            result_tables[perf_type][task_name] = {}
            for ttype in list_of_type_of_tasks:
                result_tables[perf_type][task_name][ttype] = -1.0

for line in logs_lines:
    # Handle both old format: tasks/task_type/task_name:perf_type:time
    # and new format: namespace_task_type_enabled:perf_type:time
    old_pattern = r"tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)"
    new_pattern = (
        r"(\w+_test_task_(threads|processes))_(\w+)_enabled:(\w*):(-*\d*\.\d*)"
    )

    old_result = re.findall(old_pattern, line)
    new_result = re.findall(new_pattern, line)

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
        full_task_name = new_result[0][0]
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


for table_name in result_tables:
    workbook = xlsxwriter.Workbook(
        os.path.join(xlsx_path, table_name + "_perf_table.xlsx")
    )
    worksheet = workbook.add_worksheet()
    worksheet.set_column("A:Z", 23)
    right_bold_border = workbook.add_format({"bold": True, "right": 2, "bottom": 2})
    bottom_bold_border = workbook.add_format({"bold": True, "bottom": 2})
    cpu_num = os.environ.get("PPC_NUM_PROC")
    if cpu_num is None:
        raise EnvironmentError(
            "Required environment variable 'PPC_NUM_PROC' is not set."
        )
    cpu_num = int(cpu_num)
    worksheet.write(0, 0, "cpu_num = " + str(cpu_num), right_bold_border)

    it = 1
    for type_of_task in list_of_type_of_tasks:
        worksheet.write(
            0, it, "T_" + type_of_task + "(" + str(cpu_num) + ")", bottom_bold_border
        )
        it += 1
        worksheet.write(
            0,
            it,
            "S("
            + str(cpu_num)
            + ")"
            + " = "
            + "T_seq("
            + str(cpu_num)
            + ")"
            + " / "
            + "T_"
            + type_of_task
            + "("
            + str(cpu_num)
            + ")",
            bottom_bold_border,
        )
        it += 1
        worksheet.write(
            0,
            it,
            "Eff("
            + str(cpu_num)
            + ")"
            + " = "
            + "S("
            + str(cpu_num)
            + ")"
            + " / "
            + str(cpu_num),
            right_bold_border,
        )
        it += 1

    it = 1
    for task_name in list(set(set_of_task_name)):
        worksheet.write(
            it, 0, task_name, workbook.add_format({"bold": True, "right": 2})
        )
        it += 1

    it_i = 1
    it_j = 1
    right_border = workbook.add_format({"right": 2})
    for task_name in list(set(set_of_task_name)):
        for type_of_task in list_of_type_of_tasks:
            if task_name not in result_tables[table_name].keys():
                print(f"Warning! Task '{task_name}' is not found in results")
                worksheet.write(it_j, it_i, "Error!")
                it_i += 1
                worksheet.write(it_j, it_i, "Error!")
                it_i += 1
                worksheet.write(it_j, it_i, "Error!")
                it_i += 1
                continue
            par_time = result_tables[table_name][task_name][type_of_task]
            seq_time = result_tables[table_name][task_name]["seq"]
            if par_time == 0:
                speed_up = -1
            else:
                speed_up = seq_time / par_time
            efficiency = speed_up / cpu_num
            worksheet.write(it_j, it_i, par_time)
            it_i += 1
            worksheet.write(it_j, it_i, speed_up)
            it_i += 1
            worksheet.write(it_j, it_i, efficiency, right_border)
            it_i += 1
        it_i = 1
        it_j += 1
    workbook.close()
    # Dump CSV for performance times
    csv_file = os.path.join(xlsx_path, table_name + "_perf_table.csv")
    with open(csv_file, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        # Write header: Task, SEQ, OMP, TBB, STL, ALL
        writer.writerow(["Task", "SEQ", "OMP", "TBB", "STL", "ALL"])
        for task_name in sorted(result_tables[table_name].keys()):
            seq_time = result_tables[table_name][task_name]["seq"]
            row = [
                task_name,
                1.0 if seq_time != 0 else "?",
                (result_tables[table_name][task_name]["omp"] / seq_time)
                if seq_time != 0
                else "?",
                (result_tables[table_name][task_name]["tbb"] / seq_time)
                if seq_time != 0
                else "?",
                (result_tables[table_name][task_name]["stl"] / seq_time)
                if seq_time != 0
                else "?",
                (result_tables[table_name][task_name]["all"] / seq_time)
                if seq_time != 0
                else "?",
            ]
            writer.writerow(row)
