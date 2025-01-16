How to submit your work
========================

- There are ``all``, ``mpi``, ``omp``, ``seq``, ``stl``, ``tbb`` folders in the ``tasks`` directory. Move to a folder of your task. Create a directory named ``<last name>_<first letter of name>_<short task name>``.

  Example: ``seq/nesterov_a_vector_sum``. Please name all tasks **with the same** name directory. If the ``seq`` task is named ``seq/nesterov_a_vector_sum``, then the ``omp`` task must be named ``omp/nesterov_a_vector_sum``.

- Navigate into the newly created folder and begin your work on the task. The folder must contain only 4 directories with files:
  
  - ``data``       - Directory with own data files for functional testing of the task.
  - ``func_tests`` - Directory with Google tests for functional testing of the task.
  - ``include``    - Directory for header files with function prototypes.
  - ``perf_tests`` - Directory with Google tests for performance testing. The number of tests must be 2: ``run_task`` and ``run_pipeline``.
  - ``src``        - Directory with source files containing the function implementations.

- There must be 10 executable files for running:
  
  - ``<all, mpi, omp, seq, stl, tbb>_<func, perf>_tests``. For example, ``omp_perf_tests`` - an executable file for performance tests of OpenMP practice tasks.

- All prototypes and classes in the ``include`` directory must be namespace-escaped. Name your namespace as follows:
  
  .. code-block:: cpp

     namespace <last name>_<first letter of name>_<short task name>_<technology> {
     ...
     }
     e.g.
     namespace nesterov_a_test_task_seq {

     class TestTaskSequential : public ppc::core::Task {
     ...
     };

     }  // namespace nesterov_a_test_task_seq

- Name your group of tests and individual test cases as follows:

  - For functional tests (for maximum coverage):

    .. code-block:: cpp

       TEST(<last name>_<first letter of name>_<short task name>_<technology>, <any_name_of_test_case>) {
       ...
       }
       e.g.
       TEST(nesterov_a_vector_sum_omp, test_sum) {
       ...
       }

  - For performance tests (only 2 tests - ``pipeline`` and ``task`` - no more no less):

    .. code-block:: cpp

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

- Name your pull request as follows:
  
  - For tasks:
    
    .. code-block::

       <Last Name First Name>. Task <Number of task>. Variant <Number of variant>. Technology <Name of technology>. <Full name of task>.
       Нестеров Александр. Задача 1. Вариант 123. Технология MPI. Сумма элементов вектора.

- Provide the full task definition in the pull request's description.

  Example pull request can be found in the repository's pull requests.

- Work on your forked repository. Keep your work on a separate branch (not on ``master``)!!! Name your branch the same as your task's folder. To create a branch, run:

  .. code-block:: bash

     git checkout -b nesterov_a_vector_sum_mpi

**Failing to follow the rules will result in a red project build.**
