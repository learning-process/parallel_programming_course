## How to submit your work

* There are `mpi`, `omp`, `seq`, `stl`, `tbb` folders in `tasks` directory. Move to a folder of your task. Make a directory named `<last name>_<first letter of name>_<short task name>`. Example: `seq/nesterov_a_vector_sum`. Please name all tasks **same** name directory. If `seq` task named `seq/nesterov_a_vector_sum` then  `omp` task need to be named `omp/nesterov_a_vector_sum`.
* Go into the newly created folder and begin you work on the task. There must be only 4 directory with files:
    - `func_tests` - google tests directory with files for the functional tests of task.
    - `include`    - a header files directory with function prototypes.
    - `perf_tests` - google tests directory with files for the performance tests of task. The number of tests must be 2 - `run_task` and `run_pipeline`.
    - `src` - a source files directory with functions realization.
* We need to know that exist 10 executable files for running:
    - `<mpi, omp, seq, stl, tbb>_<func, perf>_tests` e.g. `omp_perf_tests` - executable file for performance tests of OpenMP practice tasks.
* All prototypes and classes in the `include` directory must be namespace escaped, name your namespace in the following way:
  ```
  namespace <last name>_<first letter of name>_<short task name>_<technology> {
  ...
  }
  e.g.
  namespace nesterov_a_test_task_seq {
  
  class TestTaskSequential : public ppc::core::Task {
  ...
  };

  }  // namespace nesterov_a_test_task_seq
  ```
* Name your group of tests and current test in the following way:
    * for functional tests (for maximum coverage):
  ```
  TEST(<last name>_<first letter of name>_<short task name>_<technology>, <any_name_of_test_case>) {
  ...
  }
  e.g.
  TEST(nesterov_a_vector_sum_omp, test_sum) {
  ...
  }
  ```
    * for performance tests (only 2 tests - `pipeline/task` - no more no less):
  ```
  TEST(<last name>_<first letter of name>_<short task name>_<technology>, <type_of_performance_validation>) {
  ...
  }
  e.g.
  TEST(nesterov_a_vector_sum_stl, test_pipeline_run) {
  ...
  }
  TEST(nesterov_a_vector_sum_stl, test_task_run) {
  ...
  }
  ```
* Name your pull request in the following way:
    * for tasks:
  ```
  <Last Name First Name>. Task <Number of task>. Variant <Number of variant>. Technology <Name of technology>. <Full name of task>.
  Нестеров Александр. Задача 1. Вариант 123. Технология MPI. Сумма элементов вектора.
  ```

* Provide the full task definition in pull request's description.

  Example pull request is located in repo's pull requests.

* Work on your fork-repository. Keep your work on a separate branch and **NOT on `master`!!!**. Name you branch in the same way as your task's folder. To create a branch run:
  ```
  git checkout -b nesterov_a_vector_sum_mpi
  ```

*Failing to follow the rules makes the project build red.*
