"""
Pytest configuration and shared fixtures for scoreboard tests.
"""

import pytest
import tempfile
import shutil
from pathlib import Path
import yaml
import csv


@pytest.fixture
def temp_dir():
    """Create a temporary directory for tests."""
    temp_path = Path(tempfile.mkdtemp())
    yield temp_path
    shutil.rmtree(temp_path)


@pytest.fixture
def sample_config():
    """Sample configuration dictionary."""
    return {
        "scoreboard": {
            "task": {
                "seq": {"solution": {"max": 4}},
                "omp": {"solution": {"max": 6}},
                "stl": {"solution": {"max": 8}},
                "tbb": {"solution": {"max": 6}},
                "all": {"solution": {"max": 10}},
                "mpi": {"solution": {"max": 0}},
            },
            "plagiarism": {"coefficient": 0.5},
            "efficiency": {"num_proc": 4},
            "deadlines": {
                "seq": "2025-01-15",
                "omp": "2025-02-15",
                "stl": "2025-03-15",
                "tbb": "2025-04-15",
                "all": "2025-05-15",
                "mpi": "2025-06-15",
            },
        }
    }


@pytest.fixture
def sample_plagiarism_config():
    """Sample plagiarism configuration dictionary."""
    return {
        "plagiarism": {
            "seq": ["broken_example"],
            "omp": [],
            "stl": ["cheater_task"],
            "tbb": [],
            "all": [],
            "mpi": [],
        }
    }


@pytest.fixture
def sample_task_structure(temp_dir):
    """Create a sample task directory structure."""
    tasks_dir = temp_dir / "tasks"

    # Create task directories
    task_dirs = [
        "example_task/seq",
        "example_task/omp",
        "example_task/stl",
        "disabled_task_disabled/seq",
        "disabled_task_disabled/omp",
        "partial_task/seq",
    ]

    for task_dir in task_dirs:
        (tasks_dir / task_dir).mkdir(parents=True)
        # Create a dummy source file
        (tasks_dir / task_dir / "main.cpp").touch()

    return tasks_dir


@pytest.fixture
def sample_performance_csv(temp_dir):
    """Create a sample performance CSV file."""
    csv_file = temp_dir / "performance.csv"

    data = [
        {
            "Task": "example_task",
            "SEQ": "1.0",
            "OMP": "0.5",
            "STL": "0.3",
            "TBB": "0.4",
            "ALL": "0.2",
        },
        {
            "Task": "disabled_task",
            "SEQ": "2.0",
            "OMP": "1.0",
            "STL": "0.8",
            "TBB": "0.9",
            "ALL": "0.7",
        },
        {
            "Task": "partial_task",
            "SEQ": "1.5",
            "OMP": "N/A",
            "STL": "N/A",
            "TBB": "N/A",
            "ALL": "N/A",
        },
    ]

    with open(csv_file, "w", newline="") as f:
        writer = csv.DictWriter(
            f, fieldnames=["Task", "SEQ", "OMP", "STL", "TBB", "ALL"]
        )
        writer.writeheader()
        writer.writerows(data)

    return csv_file


@pytest.fixture
def sample_config_files(temp_dir, sample_config, sample_plagiarism_config):
    """Create sample configuration files."""
    data_dir = temp_dir / "data"
    data_dir.mkdir()

    # Create threads-config.yml
    config_file = data_dir / "threads-config.yml"
    with open(config_file, "w") as f:
        yaml.dump(sample_config, f)

    # Create plagiarism.yml
    plagiarism_file = data_dir / "plagiarism.yml"
    with open(plagiarism_file, "w") as f:
        yaml.dump(sample_plagiarism_config, f)

    return data_dir
