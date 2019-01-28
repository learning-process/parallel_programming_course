import sys, re

######################################################################
print("################ START STATIC ANALYSIS ERROR ################")
exit_flag = True
if len(sys.argv) == 2:
    name_log_file_cppcheck = sys.argv[1]
    log_file_cppcheck = open(name_log_file_cppcheck, 'r')
    for str_line in log_file_cppcheck:
        if re.search(r'\(error\)', str_line) != None:
            exit_flag = False
            print(str_line)
if exit_flag: print("Not found static analysis error");
print("################ END STATIC ANALYSIS ERROR   ################")
if exit_flag: exit(0)
else: exit(1)
######################################################################
