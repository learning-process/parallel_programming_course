PR Title (enforced by CI):
- Pattern: [TASK] <Task>-<Variant>. <Last Name> <First Name> <Middle Name>. <Group>. <Task name>.
- Notes: `[TASK]` is optional; <Group> can be any text; there must be a dot and a space after each block.
- Example (RU): 2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора.
- Example (EN): 2-12. Ivanov Ivan Ivanovich. 2341-a234. Vector elements sum calculation.

PR Body (enforced by CI):
- Use the 12 sections below exactly as titled; do not include HTML comments.
- After each label line (e.g., `Assignment:`), provide non-empty text.

Commit Messages (enforced by CI):
- Subject pattern: <type>(<technology>|<task_folder>): <short_change_summary>
- Allowed types: feat, fix, perf, test, refactor, docs, build, chore
- Allowed technology: seq, omp, mpi, stl, tbb, all; or use your task folder name instead of a technology
- Subject length: ≤ 72 characters, then one blank line
- Required body sections: [What], [Why], [How], Scope: (Task/Variant/Technology/Folder), Tests:, Local runs:
- Example subject: feat(omp|nesterov_a_vector_sum): implement parallel vector sum
- Example body:
  [What]
  Add OMP reduction for vector sum.

  [Why]
  Improve performance and parallel coverage.

  [How]
  Use #pragma omp parallel for reduction(+:sum).

  Scope:
  - Task: 2
  - Variant: 12
  - Technology: omp
  - Folder: nesterov_a_vector_sum

  Tests:
  Added unit and perf tests.

  Local runs:
  make test

Please fill in ALL sections below (no HTML comments). Use English headers as given.

## 1. Full name and group
Name and group:

## 2. Assignment / Topic / Task
Assignment:

## 3. Technology / Platform used
Technology:

## 4. Goals of the work
Goals:

## 5. Solution description and structure
Description:

## 6. System requirements and build instructions
Build & Run:

## 7. Testing and verification
Testing:

## 8. Results
Results:

## 9. Performance analysis
Analysis:

## 10. Conclusions and possible improvements
Conclusions:

## 11. Limitations / known issues
Limitations:

## 12. CI / static analysis / code style
CI & Style:
