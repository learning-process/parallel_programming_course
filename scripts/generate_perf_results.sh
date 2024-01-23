source scripts/run_perf_collector.sh &> build/perf_log.txt
python3 scripts/create_perf_table.py --input build/perf_log.txt --output build/bin
rm build/perf_log.txt
