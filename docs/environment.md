## Set up your environment

### Code style analysis
Please, follow [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

Code style is checked using [clang-format](https://clang.llvm.org/docs/ClangFormat.html) tool.

### Parallel programming technologies
### `MPI`
* **Windows (MSVC)**:

[Installers link.](https://www.microsoft.com/en-us/download/details.aspx?id=105289) You have to install `msmpisdk.msi` and `msmpisetup.exe`.

* **Linux (`gcc` and `clang`)**:
  ```
  sudo apt install -y mpich openmpi-bin libopenmpi-dev
  ```
* **MacOS (apple clang)**:
  ```
  brew install open-mpi
  ```

### `OpenMP`

`OpenMP` is included into `gcc` and `msvc`, but some components should be installed additionally:

* **Linux (`gcc` and `clang`)**:
  ```
  sudo apt install -y libomp-dev
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
