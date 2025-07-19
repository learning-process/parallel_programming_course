import csv
import numpy as np
from xlsxwriter.workbook import Workbook
from pathlib import Path


def get_project_path():
    script_path = Path(__file__).resolve()  # Absolute path of the script
    script_dir = script_path.parent  # Directory containing the script
    return script_dir.parent


def generate_group_table(_num_tasks, _num_students, _num_variants, _csv_file):
    if _num_tasks != len(_num_variants):
        raise Exception(
            f"Count of students: {_num_tasks} != count of list of variants: {len(_num_variants)}"
        )

    list_of_tasks = []
    str_of_print = ""
    str_of_headers = ""
    for i, num_v in zip(range(_num_tasks), _num_variants):
        list_of_variants = []
        shuffled_list_of_variants = []
        for j in range(int(_num_students / num_v) + 1):
            list_of_variants.append(np.arange(num_v) + 1)
        for variant in list_of_variants:
            np.random.shuffle(variant)
            shuffled_list_of_variants.append(variant)
        result_variants = np.concatenate(shuffled_list_of_variants)
        list_of_tasks.append(result_variants[:_num_students])
        str_of_print += "%d,"
        str_of_headers += "Task " + str(i + 1) + ","
    str_of_print = str_of_print[:-1]
    str_of_headers = str_of_headers[:-1]

    np.savetxt(
        _csv_file, np.dstack(list_of_tasks)[0], str_of_print, header=str_of_headers
    )

    workbook = Workbook(_csv_file[:-4] + ".xlsx")
    worksheet = workbook.add_worksheet()
    with open(_csv_file, "rt") as f:
        reader = csv.reader(f)
        for r, row in enumerate(reader):
            for c, col in enumerate(row):
                worksheet.write(r, c, col)
    workbook.close()


if __name__ == "__main__":
    # Define the number of tasks
    num_tasks = 3

    # List containing the number of students for each task
    list_students = [29, 10, 40]

    # List containing the number of variants (versions) for each task
    num_variants = [27, 2, 9]

    # Overall, `path_to_results` represents the file path leading to a csv's and xlsx's directory
    path_to_results = Path(get_project_path()) / "build" / "variants_results"
    path_to_results.mkdir(parents=True, exist_ok=True)

    for num_students, index in zip(list_students, range(len(list_students))):
        csv_path = path_to_results / f"variants_group_{index + 1}.csv"
        generate_group_table(num_tasks, num_students, num_variants, csv_path.as_posix())
