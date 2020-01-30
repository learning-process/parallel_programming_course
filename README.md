[![Build Status](https://travis-ci.com/learning-process/parallel_programming_course.svg?branch=master)](https://travis-ci.com/learning-process/parallel_programming_course)
[![Build status](https://ci.appveyor.com/api/projects/status/t46nd9gyt7iirdy8/branch/master?svg=true)](https://ci.appveyor.com/project/allnes/parallel-programming-course/branch/master)

# Parallel programming course
The following parallel programming technologies are considered in practice:
* `MPI`
* `OpenMP`
* `TBB`
* `std::thread`

## Rules for submissions
1. You are not supposed to trigger CI jobs by frequent updates of your pull request. First you should test you work locally with all the scripts (code style)
    * Respect others time and don't slow down the job queue
2. Carefully check if the program can hang

## 1. Set up your environment
### Fetch submodules before building the project
```
git submodule update --init --recursive
```
### `MPI`
  * **Windows (MSVC)**:
    [Installers link.](https://www.microsoft.com/en-us/download/details.aspx?id=57467) You have to install `msmpisdk.msi` and `msmpisetup.exe`.
  * **Linux (`gcc` and `clang`)**:
  ```
  sudo apt install mpich
  sudo apt install openmpi-bin
  sudo apt install libopenmpi-dev
  ```
  * **MacOS (apple clang)**:
  ```
  brew install open-mpi
  ```

### `OpenMP`
  `OpenMP` is included into `gcc` and `msvc`, but some components should be installed additionally:
  * **Linux (`gcc` and `clang`)**:
  ```
  sudo apt install libomp-dev
  ```
  * **MacOS (apple clang)**: The system is completely unstable thus you are not recommended to use it with `OpenMP`!
  ```
  brew install libomp
  ```

### `TBB`
  * **Windows (`MSVC`)**: `CMake` installs `TBB` while you run `cmake` for that project on Windows.
  * **Linux (`gcc` and `clang`)**:
  ```
  sudo apt-get install libtbb-dev
  ```
  * **MacOS (apple clang)**:
  ```
  brew install tbb
  ```

### `std::thread`
  * `std::thread` is included into STL libraries.

## 2. Build the project with `CMake`
Navigate to a source code folder.

1) Configure the build: `Makefile`, `.sln`, etc.

  ```
  mkdir build && cd build
  cmake -D USE_MPI=ON -D USE_OMP=ON -D USE_TBB=ON -D USE_STD=ON ..
  ```
*Help on CMake keys:*
- `-D USE_MPI=ON` enbale `MPI` labs.
- `-D USE_OMP=ON` enable `OpenMP` labs.
- `-D USE_TBB=ON` enable `TBB` labs.
- `-D USE_STD=ON` enable `std::thread` labs.

*A corresponding flag can be omitted if it's not needed.*

2) Build the project:
  ```
  cmake --build . --config RELEASE
  ```
3) Run `<project's folder>/build/bin`

## 3. How to submit you work
* There are `task_1`, `task_2`, `task_3` folders in `modules` directory. There are 3 task for the semester. Move to a folder of your task. Make a directory named `<last name>_<first letter of name>_<short task name>`. Example: `task1/nesterov_a_vector_sum`.
* Go into the newly created folder and begin you work on the task. There must be only 4 files and 3 of them must be written by you:
  - `main.cpp` - google tests for the task. The number of tests must be 4 or greater.
  - `vector_sum.h`   - a header file with function prototypes, name it in the same way as `<short task name>`.
  - `vector_sum.cpp` - the task implementation, name it in the same way as `<short task name>`.
  - `CMakeLists.txt` - a file to configure your project. Examples for each configuration can be found in `test_tasks`.
* Name your pull request in the following way:
  ```
  <Фамилия Имя>. Задача <Номер задачи>. <Полное название задачи>.
  Нестеров Александр. Задача 1. Сумма элементов вектора.
  ```
* Provide the full task definition in pull request's description.

  Example pull request is located in repo's pull requests.

* Work on your fork-repository. Keep your work on a separate branch and **NOT on `master`!!!**. Name you branch in the same way as your task's folder. To create a branch run:
  ```
  git checkout -b nesterov_a_vector_sum
  ```

## 4. How to submit your report to the project

* Place `<last name>_<first letter of name>_<short task name>.pdf` containing the report in [the `reports` folder](reports).

  ```
  nesterov_a_vector_sum.pdf
  ```
* Pull request's name for the report looks in the following way:
  ```
  <Фамилия Имя>. Отчет. <Полное название задачи>.
  Нестеров Александр. Отчет. Сумма элементов вектора.
  ```

## Code style
Please, follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
Code style can be verified with [the script](scripts/lint.py) (it runs with Python 2):
```
cd <source project root>
python2 scripts/lint.py
```
*Failing to follow the rules makes the project build red.*
