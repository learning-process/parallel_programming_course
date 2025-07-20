"""
Tests for the discover_tasks function.
"""

from main import discover_tasks


class TestDiscoverTasks:
    """Test cases for discover_tasks function."""

    def test_discover_tasks_with_valid_structure(self, sample_task_structure):
        """Test discovering tasks with a valid directory structure."""
        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]

        result = discover_tasks(sample_task_structure, task_types)

        # Check that tasks are discovered correctly
        assert "example_task" in result
        assert "disabled_task" in result
        assert "partial_task" in result

        # Check task statuses
        assert result["example_task"]["seq"] == "done"
        assert result["example_task"]["omp"] == "done"
        assert result["example_task"]["stl"] == "done"

        assert result["disabled_task"]["seq"] == "disabled"
        assert result["disabled_task"]["omp"] == "disabled"

        assert result["partial_task"]["seq"] == "done"
        assert "omp" not in result["partial_task"]  # No omp implementation

    def test_discover_tasks_empty_directory(self, temp_dir):
        """Test discovering tasks in an empty directory."""
        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]

        result = discover_tasks(temp_dir / "nonexistent", task_types)

        assert result == {}

    def test_discover_tasks_no_task_directories(self, temp_dir):
        """Test discovering tasks when no valid task directories exist."""
        tasks_dir = temp_dir / "tasks"
        tasks_dir.mkdir()

        # Create common directory (should be ignored)
        (tasks_dir / "common").mkdir()
        (tasks_dir / "common" / "utils.hpp").touch()

        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]

        result = discover_tasks(tasks_dir, task_types)

        assert result == {}

    def test_discover_tasks_with_mixed_implementations(self, temp_dir):
        """Test discovering tasks with mixed implementation availability."""
        tasks_dir = temp_dir / "tasks"

        # Create task with only some implementations
        task_dir = tasks_dir / "mixed_task"
        (task_dir / "seq").mkdir(parents=True)
        (task_dir / "omp").mkdir(parents=True)
        # No stl, tbb, all, mpi implementations

        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]

        result = discover_tasks(tasks_dir, task_types)

        assert "mixed_task" in result
        assert result["mixed_task"]["seq"] == "done"
        assert result["mixed_task"]["omp"] == "done"
        assert "stl" not in result["mixed_task"]
        assert "tbb" not in result["mixed_task"]
        assert "all" not in result["mixed_task"]
        assert "mpi" not in result["mixed_task"]

    def test_discover_tasks_disabled_suffix_handling(self, temp_dir):
        """Test correct handling of _disabled suffix in task names."""
        tasks_dir = temp_dir / "tasks"

        # Create disabled task
        disabled_dir = tasks_dir / "test_task_disabled"
        (disabled_dir / "seq").mkdir(parents=True)
        (disabled_dir / "omp").mkdir(parents=True)

        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]

        result = discover_tasks(tasks_dir, task_types)

        # Should be indexed under clean name without _disabled
        assert "test_task" in result
        assert "test_task_disabled" not in result

        # Should be marked as disabled
        assert result["test_task"]["seq"] == "disabled"
        assert result["test_task"]["omp"] == "disabled"

    def test_discover_tasks_custom_task_types(self, sample_task_structure):
        """Test discovering tasks with custom task types list."""
        # Only look for seq and omp
        task_types = ["seq", "omp"]

        result = discover_tasks(sample_task_structure, task_types)

        assert "example_task" in result
        assert result["example_task"]["seq"] == "done"
        assert result["example_task"]["omp"] == "done"
        # stl should not be included even though directory exists
        assert "stl" not in result["example_task"]
