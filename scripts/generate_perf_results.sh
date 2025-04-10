set -o pipefail

mkdir -p build/perf_stat_dir
python3 scripts/run_tests.py --running-type="performance" | tee build/perf_stat_dir/perf_log.txt
python3 scripts/create_perf_table.py --input build/perf_stat_dir/perf_log.txt --output build/perf_stat_dir
