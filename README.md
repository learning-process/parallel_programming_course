[![Build application](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml)
[![Pages](https://github.com/learning-process/parallel_programming_course/actions/workflows/pages.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/pages.yml)
[![CodeQL](https://github.com/learning-process/parallel_programming_course/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/codeql.yml)
[![codecov](https://codecov.io/gh/learning-process/parallel_programming_course/graph/badge.svg?token=qCOtqeFyIz)](https://codecov.io/gh/learning-process/parallel_programming_course)

# Parallel Programming Course
Welcome to the Parallel Programming Course! For more detailed documentation and resources, please visit documentation pages: [en](https://learning-process.github.io/parallel_programming_course/en/), [ru](https://learning-process.github.io/parallel_programming_course/ru/).
Course scoreboard is available [here](https://learning-process.github.io/parallel_programming_course/scoreboard/)

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
