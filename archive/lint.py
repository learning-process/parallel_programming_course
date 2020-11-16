import os
import re
import subprocess
import sys

print("################ START FOUND LINT ERROR ################")


def run(comm):
    proc = subprocess.Popen(comm, shell=True, stdout=subprocess.PIPE)
    out = proc.stdout.read()
    proc.wait()

    pattern = 'Total errors found: 0'
    res_success = re.findall(pattern, out)
    if not res_success:
        return False
    return True


pattern_filename = r'(\w+\.h|\w+\.cpp)$'

project_directory = os.getcwd()
home_directory = os.getcwd()

cpplint_path = project_directory + '/3rdparty/cpplint.py'
cpplint_path = os.path.abspath(cpplint_path)
project_directory = os.path.abspath(project_directory)
exit_flag = True
for dirs, node, files in os.walk(project_directory):
    for curr_file in files:
        if re.search(pattern_filename, curr_file) is not None:
            regex_string = r'(task_1)|(task_2)|(task_3)|(task_4)|(test_task)'
            if re.search(regex_string, dirs) is not None:
                os.chdir(dirs)
                print(dirs + " -> " + curr_file)
                command = sys.executable + ' ' + cpplint_path + ' --linelength=120 ' + curr_file
                status = run(command)
                if status:
                    print("\033[0;32mSuccess: " + curr_file + "\033[0;0m")
                else:
                    print("\033[0;31mFailed: " + curr_file + "\033[0;0m")
                    exit_flag = False
os.chdir(home_directory)

print("################ END FOUND LINT ERROR   ################")

if exit_flag:
    sys.exit(0)
else:
    sys.exit(1)
