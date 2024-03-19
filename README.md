[![Build application](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml/badge.svg)](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml)

# Parallel Programming Course

The following parallel programming technologies are considered in practice:
  * `MPI`
  * `OpenMP`
  * `TBB`
  * `std::thread`

## Rules for submissions
1. You are not supposed to trigger CI jobs by frequent updates of your pull request. First you should test you work locally with all the scripts (code style)
    * Respect others time and don't slow down the job queue
2. Carefully check if the program can hang

## 0. Download all submodules
  ```
  git submodule update --init --recursive
  ```
## 1. Set up your environment
### Static analysis of project
  * **Windows (MSVC)**:
  
  Unsupported operating system!
  
  * **Linux (`gcc` and `clang`)**:
  ```
  sudo apt install cppcheck
  ```
  * **MacOS (apple clang)**:
  ```
  brew install cppcheck
  ```

### Code style analysis
Please, follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

Code style is checked using [clang-format](https://clang.llvm.org/docs/ClangFormat.html) tool.

### Parallel programming technologies
### `MPI`
  * **Windows (MSVC)**:
  
  [Installers link.](https://www.microsoft.com/en-us/download/details.aspx?id=105289) You have to install `msmpisdk.msi` and `msmpisetup.exe`.
  
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
  * **MacOS (`llvm`)**:
  ```
  brew install llvm
  brew install libomp
  ```

### `TBB`
  * **Windows (`MSVC`)**, **Linux (`gcc` and `clang`)**, **MacOS (apple clang)**: 
    * Build as 3rdparty in the current project

### `std::thread`
  * `std::thread` is included into STL libraries.

## 2. Build the project with `CMake`
Navigate to a source code folder.

1) Configure the build: `Makefile`, `.sln`, etc.

  ```
  mkdir build && cd build
  cmake -D USE_SEQ=ON -D USE_MPI=ON -D USE_OMP=ON -D USE_TBB=ON -D USE_STL=ON -D USE_FUNC_TESTS=ON -D USE_PERF_TESTS=ON ..
  ```
*Help on CMake keys:*
- `-D USE_SEQ=ON` enable `Sequential` labs (based on OpenMP's CMakeLists.txt).
- `-D USE_MPI=ON` enable `MPI` labs.
- `-D USE_OMP=ON` enable `OpenMP` labs.
- `-D USE_TBB=ON` enable `TBB` labs.
- `-D USE_STL=ON` enable `std::thread` labs.
- `-D USE_FUNC_TESTS=ON` enable functional tests.
- `-D USE_PERF_TESTS=ON` enable performance tests.

*A corresponding flag can be omitted if it's not needed.*

2) Build the project:
  ```
  cmake --build . --config RELEASE
  ```
3) Check the task
  * Run `<project's folder>/build/bin`

## 3. How to submit you work
* There are `mpi`, `omp`, `seq`, `stl`, `tbb` folders in `tasks` directory. Move to a folder of your task. Make a directory named `<last name>_<first letter of name>_<short task name>`. Example: `seq/nesterov_a_vector_sum`. Please name all tasks same name directory. If `seq` task named `seq/nesterov_a_vector_sum` then  `omp` task need to be named `omp/nesterov_a_vector_sum`.
* Go into the newly created folder and begin you work on the task. There must be only 4 directory with files:
  - `func_tests` - google tests directory with files for the functional tests of task. The number of tests must be 5 or greater.
  - `include`    - a header files directory with function prototypes.
  - `perf_tests` - google tests directory with files for the performance tests of task. The number of tests must be 2 - `run_task` and `run_pipeline`.
  - `src` - a source files directory with functions realization.
* We need to know that exist 10 executable files for running:
  - `<mpi, omp, seq, stl, tbb>_<func, perf>_tests` e.g. `omp_perf_tests` - executable file for performance tests of OpenMP practice tasks.
* Name your pull request in the following way:
  * for tasks:
  ```
  <Last Name First Name>. Task <Number of task>. Variant <Number of variant>. <Full name of task>.
  Нестеров Александр. Задача 1. Вариант 123. Сумма элементов вектора.
  ```
 
* Provide the full task definition in pull request's description.

  Example pull request is located in repo's pull requests.

* Work on your fork-repository. Keep your work on a separate branch and **NOT on `master`!!!**. Name you branch in the same way as your task's folder. To create a branch run:
  ```
  git checkout -b nesterov_a_vector_sum
  ```

*Failing to follow the rules makes the project build red.*

And finally, 
