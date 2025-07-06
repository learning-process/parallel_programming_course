#!/usr/bin/env python3
"""Script to create performance comparison tables from benchmark results."""

import argparse
import os
import re
import xlsxwriter

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input',
                    help='Input file path (logs of perf tests, .txt)',
                    required=True)
parser.add_argument('-o', '--output',
                    help='Output file path (path to .xlsx table)',
                    required=True)
args = parser.parse_args()
logs_path = os.path.abspath(args.input)
xlsx_path = os.path.abspath(args.output)

list_of_type_of_tasks = ["all", "mpi", "omp", "seq", "stl", "tbb"]

result_tables = {"pipeline": {}, "task_run": {}}
set_of_task_name = []

with open(logs_path, "r", encoding='utf-8') as logs_file:
    logs_lines = logs_file.readlines()

for line in logs_lines:
    PATTERN = r'tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)'
    result = re.findall(PATTERN, line)
    if len(result):
        task_name = result[0][1]
        perf_type = result[0][2]
        set_of_task_name.append(task_name)
        result_tables[perf_type][task_name] = {}

        for ttype in list_of_type_of_tasks:
            result_tables[perf_type][task_name][ttype] = -1.0

for line in logs_lines:
    PATTERN = r'tasks[\/|\\](\w*)[\/|\\](\w*):(\w*):(-*\d*\.\d*)'
    result = re.findall(PATTERN, line)
    if len(result):
        task_type = result[0][0]
        task_name = result[0][1]
        perf_type = result[0][2]
        perf_time = float(result[0][3])
        if perf_time < 0.1:
            MSG = (f"Performance time = {perf_time} < 0.1 second : "
                   f"for {task_type} - {task_name} - {perf_type} \n")
            raise ValueError(MSG)
        result_tables[perf_type][task_name][task_type] = perf_time


for table_name, table_data in result_tables.items():
    workbook = xlsxwriter.Workbook(os.path.join(xlsx_path, table_name + '_perf_table.xlsx'))
    worksheet = workbook.add_worksheet()
    worksheet.set_column('A:Z', 23)
    right_bold_border = workbook.add_format({'bold': True, 'right': 2, 'bottom': 2})
    right_border = workbook.add_format({'right': 2})
    bottom_border = workbook.add_format({'bottom': 2})
    bottom_bold_border = workbook.add_format({'bottom': 2, 'bold': True})
    cell_format = workbook.add_format({'align': 'center', 'valign': 'vcenter', 'bold': True})
    cell_result_format = workbook.add_format({'align': 'center', 'valign': 'vcenter'})

    IT = -1
    for name in sorted(set_of_task_name):
        IT += 1
        worksheet.merge_range(IT, 0, IT, 1, table_name + " : " + name, cell_format)
        for idx, ttype in enumerate(list_of_type_of_tasks):
            if idx < len(list_of_type_of_tasks) - 1:
                worksheet.write(IT, 2 + idx, ttype, cell_format)
            else:
                worksheet.write(IT, 2 + idx, ttype, right_bold_border)

    IT = -1
    for name in sorted(set_of_task_name):
        IT += 1

        IT_I = 2
        IT_J = 2
        seq_time = result_tables[table_name][name]["seq"]
        for ttype in list_of_type_of_tasks:
            res_time = result_tables[table_name][name][ttype]
            if res_time > 0.0:
                if seq_time > 0 and ttype != "seq":
                    time_str = "time = {:.6f}".format(res_time)
                    SPEED_UP = seq_time / res_time
                    speed_up_str = "speedup = {:.2f}".format(SPEED_UP)
                    cell_str = time_str + "\n" + speed_up_str
                else:
                    cell_str = "time = {:.6f}".format(res_time)
                if ttype == "tbb":
                    worksheet.write(IT, IT_I, cell_str, bottom_bold_border)
                else:
                    worksheet.write(IT, IT_I, cell_str, cell_result_format)
            else:
                if ttype == "tbb":
                    worksheet.write(IT, IT_I, "-", bottom_bold_border)
                else:
                    worksheet.write(IT, IT_I, "-", cell_result_format)
            IT_I += 1
        IT_I = 2

    workbook.close()