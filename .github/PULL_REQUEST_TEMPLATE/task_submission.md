<!--
Pull request title requirement:

"<LastName> <FirstName>. Technology <TECHNOLOGY_NAME:SEQ|OMP|TBB|STL|MPI>. <Full Task Name>. Variant <Number>"
-->

## Description
<!--
Please provide a detailed description of your implementation, including:
 - key solution details (chosen algorithm description)
 - parallelism technology application (if applicable)
-->

- **Task**: _Enter the full task name here_
- **Variant**: _Enter the variant number here_
- **Technology**: _Enter technology (e.g., SEQ, OMP, TBB, STL, MPI)_
- **Description** of your implementation and report.  
  _Provide a concise summary of your implementation and report here._

---

## Checklist
<!--
Please ensure the following items are completed **before** submitting your pull request and requesting a review:
-->

- [ ] **CI Status**: All CI jobs (build, tests, report generation) are passing on my branch in my fork
- [ ] **Task Directory & Naming**: I have created a directory named `<lastName>_<firstInitial>_<short_task_name>`
- [ ] **Full Task Definition**: I have provided the complete task description in the pull request body.
- [ ] **clang-format**: My changes pass `clang-format` locally in my fork (no formatting errors)
- [ ] **clang-tidy**: My changes pass `clang-tidy` locally in my fork (no warnings/errors)
- [ ] **Functional Tests**: All functional tests are passing locally on my machine
- [ ] **Performance Tests**: All performance tests are passing locally on my machine
- [ ] **Branch**: I am working on a branch named exactly as my task directory (e.g., `nesterov_a_vector_sum`), not on `master`.
- [ ] **Truthful Content**: I confirm that every detail provided in this pull request is accurate and truthful to the best of my knowledge.

<!--
NOTE: Untruthful entries in this checklist may result in PR rejection and zero points for the associated task.
-->
