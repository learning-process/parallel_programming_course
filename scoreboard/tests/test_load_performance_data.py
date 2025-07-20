"""
Tests for the load_performance_data function.
"""

import csv
from main import load_performance_data


class TestLoadPerformanceData:
    """Test cases for load_performance_data function."""

    def test_load_performance_data_valid_csv(self, sample_performance_csv):
        """Test loading performance data from a valid CSV file."""
        result = load_performance_data(sample_performance_csv)

        # Check structure
        assert isinstance(result, dict)
        assert len(result) == 3

        # Check example_task data
        assert "example_task" in result
        example_data = result["example_task"]
        assert example_data["seq"] == "1.0"
        assert example_data["omp"] == "0.5"
        assert example_data["stl"] == "0.3"
        assert example_data["tbb"] == "0.4"
        assert example_data["all"] == "0.2"
        assert example_data["mpi"] == "N/A"

        # Check disabled_task data
        assert "disabled_task" in result
        disabled_data = result["disabled_task"]
        assert disabled_data["seq"] == "2.0"
        assert disabled_data["omp"] == "1.0"

        # Check partial_task data
        assert "partial_task" in result
        partial_data = result["partial_task"]
        assert partial_data["seq"] == "1.5"
        assert partial_data["omp"] == "N/A"
        assert partial_data["mpi"] == "N/A"

    def test_load_performance_data_nonexistent_file(self, temp_dir):
        """Test loading performance data when file doesn't exist."""
        nonexistent_file = temp_dir / "nonexistent.csv"

        result = load_performance_data(nonexistent_file)

        assert result == {}

    def test_load_performance_data_empty_csv(self, temp_dir):
        """Test loading performance data from an empty CSV file."""
        empty_csv = temp_dir / "empty.csv"
        empty_csv.touch()

        result = load_performance_data(empty_csv)

        assert result == {}

    def test_load_performance_data_header_only_csv(self, temp_dir):
        """Test loading performance data from CSV with only headers."""
        header_only_csv = temp_dir / "header_only.csv"

        with open(header_only_csv, "w", newline="") as f:
            writer = csv.DictWriter(
                f, fieldnames=["Task", "SEQ", "OMP", "STL", "TBB", "ALL"]
            )
            writer.writeheader()

        result = load_performance_data(header_only_csv)

        assert result == {}

    def test_load_performance_data_malformed_csv(self, temp_dir):
        """Test loading performance data from malformed CSV."""
        malformed_csv = temp_dir / "malformed.csv"

        with open(malformed_csv, "w") as f:
            f.write("Task,SEQ,OMP\n")
            f.write("test_task,1.0\n")  # Missing OMP value
            f.write("another_task,invalid,0.5\n")  # Invalid SEQ value

        # Should not crash, but may have incomplete data
        result = load_performance_data(malformed_csv)

        # Function should handle this gracefully
        assert isinstance(result, dict)

    def test_load_performance_data_missing_columns(self, temp_dir):
        """Test loading performance data when some columns are missing."""
        partial_csv = temp_dir / "partial.csv"

        data = [
            {"Task": "test_task", "SEQ": "1.0", "OMP": "0.5"}
            # Missing STL, TBB, ALL columns
        ]

        with open(partial_csv, "w", newline="") as f:
            writer = csv.DictWriter(f, fieldnames=["Task", "SEQ", "OMP"])
            writer.writeheader()
            writer.writerows(data)

        # Should handle missing columns gracefully
        result = load_performance_data(partial_csv)

        assert "test_task" in result
        # Missing columns should be handled (likely as empty strings or errors)
        task_data = result["test_task"]
        assert task_data["seq"] == "1.0"
        assert task_data["omp"] == "0.5"
        assert task_data["mpi"] == "N/A"  # This should always be set

    def test_load_performance_data_special_values(self, temp_dir):
        """Test loading performance data with special values."""
        special_csv = temp_dir / "special.csv"

        data = [
            {
                "Task": "special_task",
                "SEQ": "0.0",
                "OMP": "inf",
                "STL": "-1",
                "TBB": "",
                "ALL": "N/A",
            }
        ]

        with open(special_csv, "w", newline="") as f:
            writer = csv.DictWriter(
                f, fieldnames=["Task", "SEQ", "OMP", "STL", "TBB", "ALL"]
            )
            writer.writeheader()
            writer.writerows(data)

        result = load_performance_data(special_csv)

        assert "special_task" in result
        task_data = result["special_task"]
        assert task_data["seq"] == "0.0"
        assert task_data["omp"] == "inf"
        assert task_data["stl"] == "-1"
        assert task_data["tbb"] == ""
        assert task_data["all"] == "N/A"
        assert task_data["mpi"] == "N/A"
