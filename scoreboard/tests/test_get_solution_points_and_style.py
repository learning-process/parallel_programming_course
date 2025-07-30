import pytest
from main import get_solution_points_and_style


class TestGetSolutionPointsAndStyle:
    def test_get_solution_points_done_status(self, sample_config):
        sol_points, solution_style = get_solution_points_and_style(
            "seq", "done", sample_config
        )
        assert sol_points == 4
        assert solution_style == "background-color: lightgreen;"

        sol_points, solution_style = get_solution_points_and_style(
            "omp", "done", sample_config
        )
        assert sol_points == 6
        assert solution_style == "background-color: lightgreen;"

        sol_points, solution_style = get_solution_points_and_style(
            "all", "done", sample_config
        )
        assert sol_points == 10
        assert solution_style == "background-color: lightgreen;"

    def test_get_solution_points_disabled_status(self, sample_config):
        sol_points, solution_style = get_solution_points_and_style(
            "seq", "disabled", sample_config
        )
        assert sol_points == 4
        assert solution_style == "background-color: #6495ED;"

        sol_points, solution_style = get_solution_points_and_style(
            "omp", "disabled", sample_config
        )
        assert sol_points == 6
        assert solution_style == "background-color: #6495ED;"

        sol_points, solution_style = get_solution_points_and_style(
            "all", "disabled", sample_config
        )
        assert sol_points == 10
        assert solution_style == "background-color: #6495ED;"

    def test_get_solution_points_missing_status(self, sample_config):
        sol_points, solution_style = get_solution_points_and_style(
            "seq", None, sample_config
        )
        assert sol_points == 0
        assert solution_style == ""

        sol_points, solution_style = get_solution_points_and_style(
            "omp", "missing", sample_config
        )
        assert sol_points == 0
        assert solution_style == ""

        sol_points, solution_style = get_solution_points_and_style(
            "all", "", sample_config
        )
        assert sol_points == 0
        assert solution_style == ""

    def test_get_solution_points_all_task_types(self, sample_config):
        task_types = ["seq", "omp", "stl", "tbb", "all", "mpi"]
        expected_points = [4, 6, 8, 6, 10, 0]

        for task_type, expected in zip(task_types, expected_points):
            sol_points, solution_style = get_solution_points_and_style(
                task_type, "done", sample_config
            )
            assert sol_points == expected
            assert solution_style == "background-color: lightgreen;"

    def test_get_solution_points_invalid_task_type(self, sample_config):
        with pytest.raises((KeyError, ValueError, TypeError)):
            get_solution_points_and_style("invalid_type", "done", sample_config)

    def test_get_solution_points_malformed_config(self):
        malformed_config = {
            "scoreboard": {"task": {"seq": {"solution": {"max": "invalid"}}}}
        }

        with pytest.raises((ValueError, TypeError)):
            get_solution_points_and_style("seq", "done", malformed_config)

    def test_get_solution_points_missing_config_keys(self):
        incomplete_config = {"scoreboard": {}}

        with pytest.raises(KeyError):
            get_solution_points_and_style("seq", "done", incomplete_config)
