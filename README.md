[![Build Status](https://travis-ci.com/learning-process/parallel_programming_course.svg?branch=master)](https://travis-ci.com/learning-process/parallel_programming_course)
[![Build status](https://ci.appveyor.com/api/projects/status/t46nd9gyt7iirdy8/branch/master?svg=true)](https://ci.appveyor.com/project/allnes/parallel-programming-course/branch/master)

# По техническим причинам продолжение обслуживания репозитория по 25.02.2019
# Практика по параллельному программированию
В практике ркассматриваются следующие технологии параллельного программирования:
* MPI
* OpenMP
* TBB

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

### Построение проекта с помощью CMake
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
