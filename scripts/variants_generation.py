import numpy as np
from sklearn.utils import shuffle
import csv
from xlsxwriter.workbook import Workbook

NUM_TASKS = 3
NUM_STUDENTS = 25
NUM_VARIANTS = 27
csvfile = 'variants.csv'

list_of_tasks = []
str_of_print = ""
str_of_headers = ""
for i in range(NUM_TASKS):
    list_of_variants = []
    shuffled_list_of_variants = []
    for j in range(int(NUM_STUDENTS / NUM_VARIANTS) + 1):
        list_of_variants.append(np.arange(NUM_VARIANTS) + 1)
    for variant in list_of_variants:
        shuffled_list_of_variants.append(shuffle(variant))
    result_variants = np.concatenate(shuffled_list_of_variants)
    list_of_tasks.append(result_variants[:NUM_STUDENTS])
    str_of_print += '%d,'
    str_of_headers += 'Task ' + str(i + 1) + ','
str_of_print = str_of_print[:-1]
str_of_headers = str_of_headers[:-1]

np.savetxt(csvfile, np.dstack(list_of_tasks)[0], str_of_print, header=str_of_headers)

workbook = Workbook(csvfile[:-4] + '.xlsx')
worksheet = workbook.add_worksheet()
with open(csvfile, 'rt', encoding='utf8') as f:
    reader = csv.reader(f)
    for r, row in enumerate(reader):
        for c, col in enumerate(row):
            worksheet.write(r, c, col)
workbook.close()