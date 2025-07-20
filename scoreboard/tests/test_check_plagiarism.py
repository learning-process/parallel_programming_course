from main import check_plagiarism_and_calculate_penalty


class TestCheckPlagiarismAndCalculatePenalty:
    def test_check_plagiarism_flagged_task(
        self, sample_config, sample_plagiarism_config
    ):
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example", "seq", 4, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == -2

        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "cheater_task", "stl", 8, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == -4

    def test_check_plagiarism_clean_task(self, sample_config, sample_plagiarism_config):
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "clean_task", "seq", 4, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == 0

        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "another_task", "omp", 6, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == 0

    def test_check_plagiarism_disabled_task_suffix(
        self, sample_config, sample_plagiarism_config
    ):
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example_disabled", "seq", 4, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == -2

    def test_check_plagiarism_different_task_types(
        self, sample_config, sample_plagiarism_config
    ):
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example", "omp", 6, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == 0

        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "cheater_task", "seq", 4, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == 0

    def test_check_plagiarism_zero_points(
        self, sample_config, sample_plagiarism_config
    ):
        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example", "seq", 0, sample_plagiarism_config, sample_config
        )
        assert is_cheated
        assert plagiarism_points == 0

    def test_check_plagiarism_different_coefficients(self, sample_plagiarism_config):
        config_75_percent = {"scoreboard": {"plagiarism": {"coefficient": 0.75}}}

        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example", "seq", 4, sample_plagiarism_config, config_75_percent
        )
        assert is_cheated
        assert plagiarism_points == -3

        config_25_percent = {"scoreboard": {"plagiarism": {"coefficient": 0.25}}}

        is_cheated, plagiarism_points = check_plagiarism_and_calculate_penalty(
            "broken_example", "seq", 8, sample_plagiarism_config, config_25_percent
        )
        assert is_cheated
        assert plagiarism_points == -2
