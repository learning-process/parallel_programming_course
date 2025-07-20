from main import calculate_performance_metrics


class TestCalculatePerformanceMetrics:
    def test_calculate_performance_metrics_valid_values(self):
        acceleration, efficiency = calculate_performance_metrics("0.5", 4)
        assert acceleration == "2.00"
        assert efficiency == "50.00%"

        acceleration, efficiency = calculate_performance_metrics("0.25", 4)
        assert acceleration == "4.00"
        assert efficiency == "100.00%"

        acceleration, efficiency = calculate_performance_metrics("0.5", 2)
        assert acceleration == "2.00"
        assert efficiency == "100.00%"

    def test_calculate_performance_metrics_edge_cases(self):
        acceleration, efficiency = calculate_performance_metrics("0.1", 4)
        assert acceleration == "10.00"
        assert efficiency == "250.00%"

        acceleration, efficiency = calculate_performance_metrics("1.0", 4)
        assert acceleration == "1.00"
        assert efficiency == "25.00%"

        acceleration, efficiency = calculate_performance_metrics("2.0", 4)
        assert acceleration == "0.50"
        assert efficiency == "12.50%"

    def test_calculate_performance_metrics_invalid_values(self):
        acceleration, efficiency = calculate_performance_metrics("0.0", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("-1.0", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("invalid", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("inf", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("nan", 4)
        assert acceleration == "?"
        assert efficiency == "?"

    def test_calculate_performance_metrics_special_strings(self):
        acceleration, efficiency = calculate_performance_metrics("?", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics("N/A", 4)
        assert acceleration == "?"
        assert efficiency == "?"

        acceleration, efficiency = calculate_performance_metrics(None, 4)
        assert acceleration == "?"
        assert efficiency == "?"

    def test_calculate_performance_metrics_different_proc_counts(self):
        perf_val = "0.25"

        acceleration, efficiency = calculate_performance_metrics(perf_val, 1)
        assert acceleration == "4.00"
        assert efficiency == "400.00%"

        acceleration, efficiency = calculate_performance_metrics(perf_val, 2)
        assert acceleration == "4.00"
        assert efficiency == "200.00%"

        acceleration, efficiency = calculate_performance_metrics(perf_val, 8)
        assert acceleration == "4.00"
        assert efficiency == "50.00%"

        acceleration, efficiency = calculate_performance_metrics(perf_val, 16)
        assert acceleration == "4.00"
        assert efficiency == "25.00%"

    def test_calculate_performance_metrics_precision(self):
        acceleration, efficiency = calculate_performance_metrics("0.3", 3)
        assert acceleration == "3.33"
        assert efficiency == "111.11%"

        acceleration, efficiency = calculate_performance_metrics("0.7", 6)
        assert acceleration == "1.43"
        assert efficiency == "23.81%"
