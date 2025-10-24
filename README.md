[![Build application](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/main.yml)
[![Pages](https://github.com/learning-process/parallel_programming_course/actions/workflows/pages.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/pages.yml)
[![CodeQL](https://github.com/learning-process/parallel_programming_course/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/learning-process/parallel_programming_course/actions/workflows/codeql.yml)
[![codecov](https://codecov.io/gh/learning-process/parallel_programming_course/graph/badge.svg?token=qCOtqeFyIz)](https://codecov.io/gh/learning-process/parallel_programming_course)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/learning-process/parallel_programming_course/badge)](https://scorecard.dev/viewer/?uri=github.com/learning-process/parallel_programming_course)

# Parallel Programming Course

Welcome to the Parallel Programming Course!

For more detailed documentation and resources, please visit documentation pages: [en](https://learning-process.github.io/parallel_programming_course/en/), [ru](https://learning-process.github.io/parallel_programming_course/ru/).

Course scoreboard is available [here](https://learning-process.github.io/parallel_programming_course/scoreboard/).

Coverage report is available [here](https://learning-process.github.io/parallel_programming_course/coverage/).

### Parallel programming technologies:
  The following parallel programming technologies are considered in practice:
  * `Message Passing Interface (MPI)` 
  * `OpenMP (Open Multi-Processing)`
  * `oneAPI Threading Building Blocks (oneTBB)`
  * `Multithreading in C++ (std::thread)`

## Quickstart

Build
-----
- Configure: `cmake -S . -B build [-DENABLE_ADDRESS_SANITIZER=ON]`
- Build: `cmake --build build -j`
- Optional install: `cmake --install build --prefix install`

Run tests locally (preferred)
-----------------------------
Set environment and use the helper runner:

```
export PPC_NUM_THREADS=4
export PPC_NUM_PROC=2

# Shared-memory tests
python3 scripts/run_tests.py --running-type threads --counts 1 2 4

# MPI tests
python3 scripts/run_tests.py --running-type processes --counts 2 4

# Performance
python3 scripts/run_tests.py --running-type performance
```

Scoreboard generation
---------------------
- `pip install -r scoreboard/requirements.txt`
- `python3 scoreboard/main.py -o scoreboard/out`

Docs
----
- `cmake -S . -B build -DUSE_DOCS=ON`
- `cmake --build build -t docs_html`

Pre-commit
----------
- `pre-commit install && pre-commit run -a`

### Rules for submissions:
1. You are not supposed to trigger CI jobs by frequent updates of your pull request. First you should test you work locally with all the scripts (code style).
    * Respect others time and don't slow down the job queue
2. Carefully check if the program can hang.
