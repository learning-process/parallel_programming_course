import csv
import numpy as np
import sklearn.utils as skl_utils
from xlsxwriter.workbook import Workbook

NUM_TASKS = 3
NUM_STUDENTS = 29
NUM_VARIANTS = 27
csv_file = 'variants.csv'

list_of_tasks = []
str_of_print = ""
str_of_headers = ""
for i in range(NUM_TASKS):
    list_of_variants = []
    shuffled_list_of_variants = []
    for j in range(int(NUM_STUDENTS / NUM_VARIANTS) + 1):
        list_of_variants.append(np.arange(NUM_VARIANTS) + 1)
    for variant in list_of_variants:
        shuffled_list_of_variants.append(skl_utils.shuffle(variant))
    result_variants = np.concatenate(shuffled_list_of_variants)
    list_of_tasks.append(result_variants[:NUM_STUDENTS])
    str_of_print += '%d,'
    str_of_headers += 'Task ' + str(i + 1) + ','
str_of_print = str_of_print[:-1]
str_of_headers = str_of_headers[:-1]

np.savetxt(csv_file, np.dstack(list_of_tasks)[0], str_of_print, header=str_of_headers)

workbook = Workbook(csv_file[:-4] + '.xlsx')
worksheet = workbook.add_worksheet()
with open(csv_file, 'rt') as f:
    reader = csv.reader(f)
    for r, row in enumerate(reader):
        for c, col in enumerate(row):
            worksheet.write(r, c, col)
workbook.close()
