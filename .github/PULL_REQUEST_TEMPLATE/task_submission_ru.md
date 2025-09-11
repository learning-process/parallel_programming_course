Формат заголовка PR (проверяется CI):
- Шаблон: [TASK] <Task>-<Variant>. <Last Name> <First Name> <Middle Name>. <Group>. <Task name>.
- Примечания: `[TASK]` — опционально; <Group> может быть любым текстом; после каждого блока — точка и пробел.
- Пример (RU): 2-12. Иванов Иван Иванович. 2341-а234. Вычисление суммы элементов вектора.
- Example (EN): 2-12. Ivanov Ivan Ivanovich. 2341-a234. Vector elements sum calculation.

Описание PR (проверяется CI):
- Используйте 12 разделов ниже с точными заголовками; не добавляйте HTML-комментарии.
- После каждой метки (например, `Assignment:`) укажите непустой текст.

Описания коммитов (проверяется CI):
- Паттерн заголовка: <type>(<technology>|<task_folder>): <short_change_summary>
- Допустимые type: feat, fix, perf, test, refactor, docs, build, chore
- Допустимые technology: seq, omp, mpi, stl, tbb, all; либо используйте имя папки задачи вместо technology
- Длина первой строки ≤ 72 символа, затем пустая строка
- Обязательные секции тела: [What], [Why], [How], Scope: (Task/Variant/Technology/Folder), Tests:, Local runs:
- Пример subject: feat(omp|nesterov_a_vector_sum): implement parallel vector sum
- Пример тела:
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

Заполните ВСЕ разделы ниже (без HTML-комментариев). Заголовки разделов оставьте на английском, как указано.

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
