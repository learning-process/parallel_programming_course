[![Build Status](https://travis-ci.com/allnes/parallel_programming_course.svg?branch=master)](https://travis-ci.com/allnes/parallel_programming_course)
[![Build status](https://ci.appveyor.com/api/projects/status/6kpr5wpfp35f01n2/branch/master?svg=true)](https://ci.appveyor.com/project/allnes/parallel-programming-course-6g25d/branch/master)

# Практика по параллельному программированию
В практике ркассматриваются следующие технологии параллельного программирования:
* MPI
* OpenMP
* TBB

### Правила работы
1. <b>Не</b> отлаживаемся в репозитории, для этого у вас есть локальные машины и все скрипты (стиль кодирования)
2. <b>Уважаем</b> время других и не задерживаем очередь
3. <b>Тщательно</b> проверяем программу на зависания

## 1. Установка компонент для корректной работы
### MPI
  * <b>Windows (MSVC)</b>:
    Ссылка на установочные файлы [здесь](https://www.microsoft.com/en-us/download/details.aspx?id=57467).
    Обязательно надо установить следующие файлы: `msmpisdk.msi` и `msmpisetup.exe`
  * <b>Linux (gcc и clang)</b>:
  ```
  sudo apt install mpich
  sudo apt install openmpi-bin
  sudo apt install libopenmpi-dev
  ```
  * <b>MacOS (apple clang)</b>:
  ```
  brew install open-mpi
  ```

### OpenMP
  OpenMP встроен в компиляторы `gcc` и `msvc`, но все таки часть компонент нужно установить для некоторых систем:
  * <b>Linux (gcc и clang)</b>:
  ```
  sudo apt install libomp-dev
  ```
  * <b>MacOS (apple clang)</b>: Система сильно нестабильная, пока не рекомендуется использовать ее для OpenMP!
  ```
  brew install libomp
  ```

### TBB
  * <b>Windows (MSVC)</b>: CMake при использовании этого проекта на Windows сам устанвливает TBB.
  * <b>Linux (gcc и clang)</b>:
  ```
  sudo apt-get install libtbb-dev
  ```
  * <b>MacOS (apple clang)</b>:
  ```
  brew install tbb
  ```

## 2. Построение проекта с помощью CMake
Переходим в директорию с исходным кодом `parallel_programming_course`.

1) Получаем конфигурационные файлы для сборки: makefile, .sln и т.д.

  ```
  mkdir build
  cd build
  cmake -D USE_MPI=ON -D USE_OMP=ON -D USE_TBB=ON ..
  cd ..
  ```
<i>Комментарий про ключи CMake:</i>
- `-D USE_MPI=ON` отвечает за сборку зависимостей и проектов свзанных с MPI.
- `-D USE_OMP=ON` отвечает за сборку зависимостей и проектов свзанных с OpenMP.
- `-D USE_TBB=ON` отвечает за сборку зависимостей и проектов свзанных с TBB.

<i>Соотвественно, если что-то не потребуется, то флаг можно не указывать.</i>

2) Собираем  проект:
  ```
  cmake --build build --config RELEASE
  ```
3) Находим и запускаем исполняемый файл в директории `<наш проект>/build/bin`

## 3. Инстркция по размещению своих исходных кодов в проекте
* В директории `modules` есть папки с задачами: `task_1`, `task_2`, `task_3`.
Находим директорию соотвествующую вашей задаче и переходим в нее. Создаем папку с названием `<фамилия>_<инициал имени>_<краткое название задачи>_<технология (mpi - для программ написанных с помощью MPI, omp - для программ с последовательным кодом или написанных на OpenMP, tbb - для программ написанных с помощью TBB)>`. К примеру: `task1/nesterov_a_vector_sum_omp`.
* Теперь переходим в созданную нами директорию и начинаем работу над задачей. В данной директории должны быть всего 2 файла и оба написанных вами:
  - `main.cpp` - исходной код вашей задачи.
  - `CMakeLists.txt` - конфигурация вашего проекта. Пример для каждой конфигурации находятся в директории `test_tasks`. Единственное, что нужно указать - название вашего подпроекта - название аналогично названию вашей папки:
  ```
  set(PROJECT_NAME "nesterov_a_vector_sum_omp")
  ...
  ```
* Название pull-request'а выглядит следующим образом:
  ```
  <Фамилия Имя>. Задача <Номер задачи>. <Полное название задачи>.
  Нестеров Александр. Задач 1. Сумма элементов вектора.  
  ```
  и отправляется в проект [allnes/parallel_programming_course](https://github.com/allnes/parallel_programming_course)
* В описание pull-request'а пишем полную постановку задачи.

  Пример pull-request'а находится в pull-request'ах проекта.

* Работаем со своим fork-репозитроием. Работаем в отдельной ветке и <b>НЕ в `master`!!!</b> Название ветки аналогично названию директории для вашей задачи. К примеру создание ветки:
  ```
  git checkout -b nesterov_a_vector_sum_omp
  ```

## Использование стиля кодирования
Для проверки стиля кодирования используется Google C++ Style.
* Описание стиля находится [здесь](https://google.github.io/styleguide/cppguide.html).
* Проверить стиль можно с помощью скрипта:
  ```
  python scripts/lint.py
  ```
<i>Невывполнение правил ведет к покраснению сборки проекта.</i>
