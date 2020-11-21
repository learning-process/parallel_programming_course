![Build application](https://github.com/learning-process/parallel_programming_course/workflows/Build%20application/badge.svg?branch=master)
[![deepcode](https://www.deepcode.ai/api/gh/badge?key=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJwbGF0Zm9ybTEiOiJnaCIsIm93bmVyMSI6ImxlYXJuaW5nLXByb2Nlc3MiLCJyZXBvMSI6InBhcmFsbGVsX3Byb2dyYW1taW5nX2NvdXJzZSIsImluY2x1ZGVMaW50IjpmYWxzZSwiYXV0aG9ySWQiOjI0ODQyLCJpYXQiOjE2MDU0NTM3MDV9.CAHpnB6YIOUynZL6wXqSHSQfztfAGhtyqhEpbPeP9XA)](https://www.deepcode.ai/app/gh/learning-process/parallel_programming_course/_/dashboard?utm_content=gh%2Flearning-process%2Fparallel_programming_course)

# Parallel Programming Course

The following parallel programming technologies are considered in practice:
  * `MPI`
  * `OpenMP`
  * `TBB`
  * `std::thread`

The following reporting technologies are considered in practice:
  * `LaTeX`

## Rules for submissions
1. You are not supposed to trigger CI jobs by frequent updates of your pull request. First you should test you work locally with all the scripts (code style)
    * Respect others time and don't slow down the job queue
2. Carefully check if the program can hang

## 1. Set up your environment
### Fetch submodules before building the project
```
git submodule update --init --recursive
```

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

```
pip install cpplint # (Python3)
```

### Parallel programming technologies
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
  * **Windows (`MSVC`)**: 
  
  `CMake` installs `TBB` while you run `cmake` for that project on Windows.
  
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

### Reporting technologies
### `LaTeX`
  * **Windows**:

  Run powershell script `scripts/appveyor_install_miktex-latest-minimal.ps1` for install LaTeX and build project.

  * **Linux**:
  ```
  sudo apt install texlive*
  ```
  * **MacOS (apple clang)**:

  Unsupported operating system!

## 2. Build the project with `CMake`
Navigate to a source code folder.

1) Configure the build: `Makefile`, `.sln`, etc.

  ```
  mkdir build && cd build
  cmake -D USE_SEQ=ON -D USE_MPI=ON -D USE_OMP=ON -D USE_TBB=ON -D USE_STD=ON -D USE_LATEX=ON ..
  ```
*Help on CMake keys:*
- `-D USE_SEQ=ON` enable `Sequential` labs (based on OpenMP's CMakeLists.txt).
- `-D USE_MPI=ON` enable `MPI` labs.
- `-D USE_OMP=ON` enable `OpenMP` labs.
- `-D USE_TBB=ON` enable `TBB` labs.
- `-D USE_STD=ON` enable `std::thread` labs.
- `-D USE_LATEX=ON` enable `LaTeX` reports.

*A corresponding flag can be omitted if it's not needed.*

2) Build the project:
  ```
  cmake --build . --config RELEASE
  ```
3) Check the task
  * Run `<project's folder>/build/bin`
  * View report `<project's folder>/build/modules/reports/<report's folder>`

## 3. How to submit you work
* There are `task_1`, `task_2`, `task_3`, `task_4`, `reports` folders in `modules` directory. There are 4 tasks and 1 report for the semester. Move to a folder of your task. Make a directory named `<last name>_<first letter of name>_<short task name>`. Example: `task1/nesterov_a_vector_sum`.
* Go into the newly created folder and begin you work on the task. There must be only 4 files and 3 of them must be written by you:
  - `main.cpp` - google tests for the task. The number of tests must be 5 or greater.
  - `vector_sum.h`   - a header file with function prototypes, name it in the same way as `<short task name>`.
  - `vector_sum.cpp` - the task implementation, name it in the same way as `<short task name>`.
  - `CMakeLists.txt` - a file to configure your project. Examples for each configuration can be found in `test_tasks`.
* Go into the newly created folder and begin you work on the report. There must be only 2 files and 1 of them must be written by you:
  - `vector_sum.tex` - a LaTeX report file which consider information about your program, name it in the same way as `<short task name>`.
  - `CMakeLists.txt` - a file to configure your project. Examples for each configuration can be found in `test_tasks/test_latex`.
* The number of directories will increase with time. To build only your project, you need to do the following:
  ```
  cmake --build . --target <name task's directory> --config RELEASE
  ```
  Example:
  ```
  cmake --build . --target nesterov_a_vector_sum --config RELEASE
  ```
* Name your pull request in the following way:
  * for tasks:
  ```
  <Фамилия Имя>. Задача <Номер задачи>. <Полное название задачи>.
  Нестеров Александр. Задача 1. Сумма элементов вектора.
  ```
  * for report:
  ```
  <Фамилия Имя>. Отчет. <Полное название задачи>.
  Нестеров Александр. Отчет. Сумма элементов вектора.
  ```
* Provide the full task definition in pull request's description.

  Example pull request is located in repo's pull requests.

* Work on your fork-repository. Keep your work on a separate branch and **NOT on `master`!!!**. Name you branch in the same way as your task's folder. To create a branch run:
  ```
  git checkout -b nesterov_a_vector_sum
  ```

*Failing to follow the rules makes the project build red.*

And finally, 

**All information of the course is in WIKI!!!**
