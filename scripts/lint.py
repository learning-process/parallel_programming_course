import os, subprocess, re, sys

######################################################################
print("################ START FOUND LINT ERROR ################")
def StartTests(command):
    proc = subprocess.Popen(command, shell=True, stdout = subprocess.PIPE)
    out = proc.stdout.read()
    proc.wait()

    pattern = 'Total errors found: 0';
    res_succ = re.findall(pattern, out)

    if len(res_succ) != 0: return True
    else:                  return False

pattern_filename = r'(\w+\.h|\w+\.cpp)$'

project_directory = os.getcwd();
home_directory = os.getcwd();

cpplint_path = project_directory + '/3rdparty/cpplint.py'
cpplint_path = os.path.abspath(cpplint_path)
project_directory = os.path.abspath(project_directory)
exit_flag = True
for dirs, node, files in os.walk(project_directory):
    for file in files:
        if re.search(pattern_filename, file) != None:
            if re.search(r'(task_1)|(task_2)|(task_3)|(test_task)', dirs) != None:
                os.chdir(dirs)
                print(dirs + " -> " + file)
                command = sys.executable + ' ' + cpplint_path + ' --linelength=120 ' + file
                status = StartTests(command)
                if status:
                    print("\033[0;32mSuccess: " + file + "\033[0;0m");
                else:
                    print("\033[0;31mFailed: "  + file + "\033[0;0m");
                    exit_flag = False
os.chdir(home_directory)
print("################ END FOUND LINT ERROR   ################")
if exit_flag: exit(0)
else: exit(1)
######################################################################
