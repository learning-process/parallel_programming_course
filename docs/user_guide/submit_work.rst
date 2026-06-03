How to create, open, and submit your work
=========================================

This section reflects the current integration flow used in semester repositories. It is a practical, step-by-step checklist tailored for students.

Repository and branch
---------------------
- Fork the semester repository for your group (processes/threads, program).
- Create a branch named exactly as your task folder:
  ``<last>_<initial>_<short>`` (e.g., ``nesterov_a_elem_vec_sum``).

Prerequisites
-------------
- Install tools and dependencies (see ``User Guide → Environment``).
- Verify you can configure and build the project locally with CMake.
- If you work from a fork, clone your fork and set the course repo as upstream.

Task folder layout (unified)
----------------------------
Create a folder ``tasks/<last>_<initial>_<short>/`` with the following layout:

- ``common/include/common.hpp`` — shared type aliases and `BaseTask`:
  
  .. code-block:: cpp

     using InType = int;        // example
     using OutType = int;       // example
     using TestType = std::tuple<int, std::string>; // if needed
     using BaseTask = ppc::task::Task<InType, OutType>;

- Technology implementations (add only those required by the semester):
  - Processes (MPI/SEQ): ``seq/include``, ``seq/src``, ``mpi/include``, ``mpi/src``
  - Threads: ``seq``, ``omp``, ``tbb``, ``stl`` (same include/src split)

  Each implementation defines a class derived from ``BaseTask`` and
  overrides ``ValidationImpl``, ``PreProcessingImpl``, ``RunImpl``,
  ``PostProcessingImpl``. Also add:
  
  .. code-block:: cpp

     static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() { return ppc::task::TypeOfTask::kMPI; }
     // or kSEQ/kOMP/kTBB/kSTL as appropriate

  Minimal skeleton (example for SEQ):

  .. code-block:: cpp

     // seq/include/ops_seq.hpp
     #pragma once
     #include "<last>_<initial>_<short>/common/include/common.hpp"
     namespace <last>_<initial>_<short> {
     class MyTaskSEQ : public BaseTask {
      public:
       static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() { return ppc::task::TypeOfTask::kSEQ; }
       explicit MyTaskSEQ(const InType& in);
      private:
       bool ValidationImpl() override;
       bool PreProcessingImpl() override;
       bool RunImpl() override;
       bool PostProcessingImpl() override;
      };
     } // namespace <last>_<initial>_<short>

  .. code-block:: cpp

     // seq/src/ops_seq.cpp
     #include "<last>_<initial>_<short>/seq/include/ops_seq.hpp"
     namespace <last>_<initial>_<short> {
     MyTaskSEQ::MyTaskSEQ(const InType& /*in*/) : BaseTask(/*in*/ InType{}) {}
     bool MyTaskSEQ::ValidationImpl() { return true; }
     bool MyTaskSEQ::PreProcessingImpl() { return true; }
     bool MyTaskSEQ::RunImpl() { /* compute */ return true; }
     bool MyTaskSEQ::PostProcessingImpl() { /* write result */ return true; }
     } // namespace <last>_<initial>_<short>

- Tests (unified location):
  - ``tests/functional/main.cpp`` — functional tests built on ``ppc::util::BaseRunFuncTests`` and helpers.
  - ``tests/performance/main.cpp`` — performance tests built on ``ppc::util::BaseRunPerfTests`` + ``MakeAllPerfTasks``.

Functional tests example:

.. code-block:: cpp

   const auto kTasks = std::tuple_cat(
     ppc::util::AddFuncTask<MyTaskMPI, InType>(params, PPC_SETTINGS_<task_id>),
     ppc::util::AddFuncTask<MyTaskSEQ, InType>(params, PPC_SETTINGS_<task_id>)
   );
   INSTANTIATE_TEST_SUITE_P(..., MyFuncTests, ppc::util::ExpandToValues(kTasks), ...);

Use ``PPC_ID_<task_id>`` to resolve resources from ``data/`` when needed.

Performance tests example:

.. code-block:: cpp

   const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, MyTaskMPI, MyTaskSEQ>(PPC_SETTINGS_<task_id>);
   INSTANTIATE_TEST_SUITE_P(..., MyPerfTests, ppc::util::TupleToGTestValues(kAllPerfTasks), ...);

Tips for tests
--------------
- Keep tests deterministic and under time limits; prefer env vars (see ``User Guide → Environment Variables``) over sleeps.
- Use ``PPC_ID_<task_id>`` to access files from ``data/``.
- Cover edge cases in functional tests; add exactly two performance test styles (``task`` and ``pipeline``) inside the suite.

- ``data/`` — optional input files for tests (e.g., images).
- ``settings.json`` — enable required technologies for your semester, e.g.:
  
  .. code-block:: json

     { "tasks_type": "processes", "tasks": { "mpi": "enabled", "seq": "enabled" } }

- ``info.json`` — student metadata used in automation (scoreboard, macros):
  
  .. code-block:: json

     { "student": { "full_name": "Фамилия Имя Отчество", "group_number": "Группа", "task_number": "1" } }

Build and local run
-------------------
- Configure and build:
  
  .. code-block:: bash

     cmake -S . -B build -DENABLE_ADDRESS_SANITIZER=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
     cmake --build build -j

- Run tests via helper:
  
  .. code-block:: bash

     export PPC_NUM_THREADS=4
     export PPC_NUM_PROC=2
     python3 scripts/run_tests.py --running-type threads --counts 1 2 4
     python3 scripts/run_tests.py --running-type processes --counts 2 4
     python3 scripts/run_tests.py --running-type performance

Executables (where to find tests)
---------------------------------
- ``build/bin`` (or ``install/bin``):
  - ``core_func_tests`` — core library tests first
  - ``ppc_func_tests`` — functional tests for all tasks/technologies
  - ``ppc_perf_tests`` — performance tests for all tasks/technologies

The runner applies gtest filters automatically to select technology suites.

Pull Request
------------
- Title format (example):
  
  ``<Фамилия Имя>. Технология <SEQ/MPI/...>. <Название задачи>. Вариант <N>.``

- Description should include:
  - Полное описание задачи; номер варианта; используемая технология
  - Краткое описание реализации и отчёта
  - Чек-лист (CI зелёный в форке, clang-format/clang-tidy пройдены, функциональные/перф тесты ок, ветка названа как директория задачи, достоверность сведений).

PR checklist template (body)
----------------------------

.. code-block:: markdown

   ## Description
   - Task: <Full task name>
   - Variant: <N>
   - Technology: <SEQ | MPI | OMP | TBB | STL | ALL>
   - Summary: Brief description of your implementation and report

   ---

   ## Checklist
   - [ ] CI is green in my fork (build, tests, docs)
   - [ ] Task folder is named `<last>_<initial>_<short>` and matches branch name
   - [ ] clang-format passed locally
   - [ ] clang-tidy passed locally (no warnings/errors introduced)
   - [ ] Functional tests pass locally
   - [ ] Performance tests pass locally
   - [ ] Report (`report.md`) is added and follows the template
   - [ ] I confirm that provided information is truthful

Common pitfalls (read before pushing)
-------------------------------------
- Wrong folder/branch name. Must be ``<last>_<initial>_<short>`` everywhere.
- Missing or wrong ``GetStaticTypeOfTask`` value for a technology.
- Tests rely on randomness or sleeps instead of env time limits.
- ``settings.json`` doesn’t enable a required technology — tests won’t run.
- Namespace doesn’t match the folder name and collides with others.
- Performance tests count or naming deviates from the required patterns.

Useful examples to reference
----------------------------
- Processes: ``tasks/example_processes``, ``tasks/example_processes_2``, ``tasks/example_processes_3``
- Threads: ``tasks/example_threads``

- Work from your fork in a dedicated branch (not ``master``). Branch name must match your task folder.

Notes
-----
- All classes should live in a unique namespace (e.g., ``<last>_<initial>_<short>``).
- Keep tests deterministic and within time limits; prefer env vars over sleeps.
- Follow code style (clang-format/clang-tidy), and run pre-commit hooks locally.
