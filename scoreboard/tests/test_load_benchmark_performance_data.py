"""
Tests for loading Google Benchmark performance JSON files.
"""

import json

from main import load_benchmark_performance_data, parse_benchmark_name


class TestLoadBenchmarkPerformanceData:
    """Test cases for Google Benchmark performance data loading."""

    def test_parse_threads_benchmark_name(self):
        assert parse_benchmark_name(
            "example_threads_omp_enabled/iterations:5/manual_time"
        ) == (
            "example_threads",
            "omp",
            "",
        )

    def test_parse_processes_benchmark_name(self):
        assert parse_benchmark_name("example_processes_t2_mpi_enabled") == (
            "example_processes_t2",
            "mpi",
            "",
        )

    def test_load_benchmark_json_in_seconds(self, temp_dir):
        benchmarks_dir = temp_dir / "benchmarks"
        benchmarks_dir.mkdir()
        (benchmarks_dir / "threads.json").write_text(
            json.dumps(
                {
                    "benchmarks": [
                        {
                            "name": "example_threads_seq_enabled",
                            "real_time": 1.5,
                            "time_unit": "s",
                        },
                        {
                            "name": "example_threads_omp_enabled",
                            "real_time": 0.75,
                            "time_unit": "s",
                        },
                        {
                            "name": "example_threads_omp_disabled",
                            "real_time": 0.8,
                            "time_unit": "s",
                        },
                    ]
                }
            ),
            encoding="utf-8",
        )

        result = load_benchmark_performance_data(benchmarks_dir)

        assert result["example_threads"]["seq"] == "1.5"
        assert result["example_threads"]["omp"] == "0.75"

    def test_load_benchmark_json_converts_units_to_seconds(self, temp_dir):
        benchmarks_dir = temp_dir / "benchmarks"
        benchmarks_dir.mkdir()
        (benchmarks_dir / "processes.json").write_text(
            json.dumps(
                {
                    "benchmarks": [
                        {
                            "name": "example_processes_t1_seq_enabled",
                            "real_time": 250,
                            "time_unit": "ms",
                        },
                        {
                            "name": "example_processes_t1_mpi_enabled",
                            "real_time": 100000,
                            "time_unit": "us",
                        },
                    ]
                }
            ),
            encoding="utf-8",
        )

        result = load_benchmark_performance_data(benchmarks_dir)

        assert result["example_processes_t1"]["seq"] == "0.25"
        assert result["example_processes_t1"]["mpi"] == "0.1"

    def test_load_benchmark_json_prefers_median_statistic(self, temp_dir):
        benchmarks_dir = temp_dir / "benchmarks"
        benchmarks_dir.mkdir()
        (benchmarks_dir / "threads.json").write_text(
            json.dumps(
                {
                    "benchmarks": [
                        {
                            "name": "example_threads_tbb_enabled",
                            "real_time": 0.4,
                            "time_unit": "s",
                        },
                        {
                            "name": "example_threads_tbb_enabled_mean",
                            "real_time": 0.3,
                            "time_unit": "s",
                        },
                        {
                            "name": "example_threads_tbb_enabled_median",
                            "real_time": 0.2,
                            "time_unit": "s",
                        },
                    ]
                }
            ),
            encoding="utf-8",
        )

        result = load_benchmark_performance_data(benchmarks_dir)

        assert result["example_threads"]["tbb"] == "0.2"
