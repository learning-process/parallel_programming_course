[![Build application](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml/badge.svg)](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml)
[![codecov](https://codecov.io/gh/learning-process/parallel_programming_course/graph/badge.svg?token=qCOtqeFyIz)](https://codecov.io/gh/learning-process/parallel_programming_course)

# Parallel Programming Course

### Parallel programming technologies:
  The following parallel programming technologies are considered in practice:
  * `Message Passing Interface (MPI)` 
  * `OpenMP (Open Multi-Processing)`
  * `oneAPI Threading Building Blocks (oneTBB)`
  * `Multithreading in C++ (std::thread)`

### Rules for submissions:
1. You are not supposed to trigger CI jobs by frequent updates of your pull request. First you should test you work locally with all the scripts (code style).
    * Respect others time and don't slow down the job queue
2. Carefully check if the program can hang.

### Steps to create a task:
1. [Download all submodules](docs/download.rst)
2. [Set up your environment](docs/environment.rst)
3. [Build the project with `CMake`](docs/build.rst)
4. [How to submit your current work](docs/submit_work.rst)
