mkdir build/perf_stat_dir
source scripts/run_perf_collector.sh &> build/perf_stat_dir/perf_log.txt
python3 scripts/create_perf_table.py --input build/perf_stat_dir/perf_log.txt --output build/perf_stat_dir
