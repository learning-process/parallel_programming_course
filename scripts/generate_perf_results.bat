@echo off
mkdir build\perf_stat_dir
python3 scripts/run_tests.py --running-type="performance" > build\perf_stat_dir\perf_log.txt
python scripts\create_perf_table.py --input build\perf_stat_dir\perf_log.txt --output build\perf_stat_dir
