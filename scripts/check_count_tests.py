import re
import sys

project_name = sys.argv[1]
list_check_files = sys.argv[2].split()
pattern_header = r'gtest/gtest.h'
pattern_test = r'TEST\w*\((\s*\w*\s*)*,(\s*\w*\s*)*\)\s*\{'

counter_gtest_header = 0
counter_gtest_tests = 0

for check_file in list_check_files:
    # open and parse the file
    file_descriptor = open(check_file, "r")
    file_content = file_descriptor.read()
    content_list = file_content.splitlines()
    file_descriptor.close()

    # find pattern "gtest/gtest.h" in the file
    for contents_line in content_list:
        result_header = re.findall(pattern_header, contents_line)
        counter_gtest_header += len(result_header)

    # get count tests in the file
    for contents_line in content_list:
        result_tests = re.findall(pattern_test, contents_line)
        counter_gtest_tests += len(result_tests)

if counter_gtest_header > 0 and counter_gtest_tests >= 5:
    print("\033[0;32mSuccess: " + project_name + "\033[0;0m")
    sys.exit(0)
else:
    if counter_gtest_header == 0:
        print("\033[0;31mFailed: " + project_name +
              "\ncount of \"gtest/gtest.h\" patterns in project need to more than 0 : current = " +
              str(counter_gtest_header) +
              "\033[0;0m")
    if counter_gtest_tests < 5:
        print("\033[0;31mFailed: " + project_name +
              "\ncount of tests in project need to more than or equal 5 : current = " + str(counter_gtest_tests) +
              "\033[0;0m")

    sys.exit(1)
