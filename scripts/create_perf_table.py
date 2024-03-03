import argparse
import os
import re
import xlsxwriter
import multiprocessing

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input', help='Input file path (logs of perf tests, .txt)', required=True)
parser.add_argument('-o', '--output', help='Output file path (path to .xlsx table)', required=True)
args = parser.parse_args()
logs_path = os.path.abspath(args.input)
xlsx_path = os.path.abspath(args.output)

list_of_type_of_tasks = ["mpi", "omp", "seq", "stl", "tbb"]

result_tables = {"pipeline": {}, "task_run": {}}
set_of_task_name = []

logs_file = open(logs_path, "r")
logs_lines = logs_file.readlines()
for line in logs_lines:
    pattern = r'tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)'
    result = re.findall(pattern, line)
    if len(result):
        task_name = result[0][1]
        perf_type = result[0][2]
        set_of_task_name.append(task_name)
        result_tables[perf_type][task_name] = {}

        for ttype in list_of_type_of_tasks:
            result_tables[perf_type][task_name][ttype] = -1.0

for line in logs_lines:
    pattern = r'tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)'
    result = re.findall(pattern, line)
    if len(result):
        task_type = result[0][0]
        task_name = result[0][1]
        perf_type = result[0][2]
        perf_time = float(result[0][3])
        result_tables[perf_type][task_name][task_type] = perf_time


for table_name in result_tables:
    workbook = xlsxwriter.Workbook(os.path.join(xlsx_path, table_name + '_perf_table.xlsx'))
    worksheet = workbook.add_worksheet()
    worksheet.set_column('A:Z', 23)
    right_bold_border = workbook.add_format({'bold': True, 'right': 2, 'bottom': 2})
    bottom_bold_border = workbook.add_format({'bold': True, 'bottom': 2})
    cpu_num = multiprocessing.cpu_count()
    worksheet.write(0, 0, "cpu_num = " + str(cpu_num), right_bold_border)

    it = 1
    for type_of_task in list_of_type_of_tasks:
        worksheet.write(0, it, "T_" + type_of_task + "(" + str(cpu_num) + ")", bottom_bold_border)
        it += 1
        worksheet.write(0, it, "S(" + str(cpu_num) + ")" + " = " +
                        "T_seq(" + str(cpu_num) + ")" + " / " +
                        "T_" + type_of_task + "(" + str(cpu_num) + ")", bottom_bold_border)
        it += 1
        worksheet.write(0, it, "Eff(" + str(cpu_num) + ")" + " = " +
                        "S(" + str(cpu_num) + ")" + " / " + str(cpu_num), right_bold_border)
        it += 1

    it = 1
    for task_name in list(set(set_of_task_name)):
        worksheet.write(it, 0, task_name, workbook.add_format({'bold': True, 'right': 2}))
        it += 1

    it_i = 1
    it_j = 1
    right_border = workbook.add_format({'right': 2})
    for task_name in list(set(set_of_task_name)):
        for type_of_task in list_of_type_of_tasks:
            par_time = result_tables[table_name][task_name][type_of_task]
            seq_time = result_tables[table_name][task_name]["seq"]
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
