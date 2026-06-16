<!--
Pull request title requirement:

"Fix: <LastName> <FirstName>. Technology <TECHNOLOGY_NAME:SEQ|OMP|TBB|STL|MPI>. <Full Task Name>. Variant <Number>"
-->

## Description
<!--
Please provide:
 1) Original task details (what is being fixed)
 2) Fix details (what was wrong and what you changed)
-->

### Original task details

- **Task**: _Enter the full task name here_
- **Variant**: _Enter the variant number here_
- **Technology**: _Enter technology (e.g., SEQ, OMP, TBB, STL, MPI)_
- **Student directory**: _Enter the directory name (e.g., `nesterov_a_vector_sum`)_
- **Original PR / commit / issue link**: _Paste link(s) here_
- **Original task definition**: _Paste the full task statement here_

### Fix details

- **Problem**: _What is broken / incorrect? Include symptoms and expected behavior._
- **Root cause**: _Why did it happen?_
- **Fix summary**: _What exactly did you change?_
- **Verification**: _What did you run locally?_

---

## Checklist
<!--
Please ensure the following items are completed **before** submitting your pull request and requesting a review:
-->

- [ ] **CI Status**: All CI jobs (build, tests, report generation) are passing on my branch in my fork
- [ ] **Task Directory & Naming**: I did not rename the existing task directory and did not introduce extra task
  directories
- [ ] **Full Task Definition**: I have provided the complete original task description and the fix details in the pull
  request body
- [ ] **clang-format**: My changes pass `clang-format` locally in my fork (no formatting errors)
- [ ] **clang-tidy**: My changes pass `clang-tidy` locally in my fork (no warnings/errors)
- [ ] **Functional Tests**: All functional tests are passing locally on my machine
- [ ] **Performance Tests**: All performance tests are passing locally on my machine (if applicable)
- [ ] **Branch**: I am working on a dedicated fix branch (not on `master`)
- [ ] **Truthful Content**: I confirm that every detail provided in this pull request is accurate and truthful to the
  best of my knowledge.

<!--
NOTE: Untruthful entries in this checklist may result in PR rejection and zero points for the associated task.
-->
