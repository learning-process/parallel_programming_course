Set Up Your Environment
========================

Development Container (Recommended)
------------------------------------
The easiest way to set up your development environment is using the provided ``.devcontainer`` configuration with VS Code and Docker.

**Prerequisites:**

- `Visual Studio Code <https://code.visualstudio.com/>`_
- `Docker Desktop <https://www.docker.com/products/docker-desktop/>`_
- `Dev Containers extension <https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers>`_

**Setup:**

1. Clone the repository and open it in VS Code
2. When prompted, click "Reopen in Container" or use Command Palette: ``Dev Containers: Reopen in Container``
3. VS Code will automatically build the container with all dependencies pre-installed
4. The container includes:

   - Ubuntu environment with gcc-15, CMake, MPI, OSH, OpenMP
   - Pre-configured C++ and Python development tools
   - All project dependencies ready to use

This provides a consistent development environment across all platforms without manual dependency installation.

Manual Setup
------------

If you prefer manual setup or cannot use containers, follow the instructions below.

Build prerequisites
-------------------
- **Windows**: Download and install CMake from https://cmake.org/download (select the Windows installer) or install using Chocolatey:

  .. code-block:: powershell

     choco install cmake

- **Linux (Ubuntu/Debian)**: Install using package manager:

  .. code-block:: bash

     sudo apt update
     sudo apt install -y cmake

- **macOS**: Install using Homebrew:

  .. code-block:: bash

     brew update
     brew install cmake

Code Style Analysis
--------------------
Please follow the `Google C++ Style Guide <https://google.github.io/styleguide/cppguide.html>`_.

Code style is checked using the `clang-format <https://clang.llvm.org/docs/ClangFormat.html>`_ tool.

Optional tools (clang-tidy, gcovr)
----------------------------------
Install these to match the CI toolchain for static analysis and coverage reports.

- Linux (Ubuntu/Debian):

  .. code-block:: bash

     # clang-tidy 22 (recommended)
     sudo apt update && sudo apt install -y clang-tidy-22
     # gcovr via pip (or use your distro package)
     python3 -m pip install gcovr

- macOS (Homebrew):

  .. code-block:: bash

     brew install llvm gcovr
     # clang-tidy path (if not on PATH):
     echo "$(brew --prefix)/opt/llvm/bin/clang-tidy"

- Windows:

  .. code-block:: powershell

     choco install llvm
     py -m pip install gcovr
     # Ensure clang-tidy.exe is available in PATH

Parallel Programming Technologies
---------------------------------

``MPI``
~~~~~~~
On Linux, MPI is built from the shallow ``3rdparty/openmpi`` source backend
together with OSH. A separate system MPI installation is not required for the
Linux course build.

- **Windows (MSVC)**:

  `Installers link <https://www.microsoft.com/en-us/download/details.aspx?id=105289>`_. You have to install ``msmpisdk.msi`` and ``msmpisetup.exe``.

- **Linux (gcc and clang)**:
  
  .. code-block:: bash

     sudo apt install -y autoconf automake flex libtool make perl bash

- **MacOS (apple clang)**:
  
  .. code-block:: bash

     brew install mpich

``OSH``
~~~~~~~~~~~~~
OSH is used for process tasks as a PGAS / one-sided communication model
for distributed-memory systems.

The project builds MPI and OSH from the shallow ``3rdparty/openmpi`` source
backend on Linux. The build installs ``mpirun``, ``libmpi``, ``liboshmem``,
and ``shmem.h`` into the build tree. Open MPI OSH uses the UCX SPML, so Linux
needs the UCX development package. macOS and Windows use their system MPI
installations; OSH implementations are not built there.

- **Linux (gcc and clang)**:

  .. code-block:: bash

     sudo apt install -y autoconf automake flex libtool libucx-dev make perl pkg-config zlib1g-dev bash

- **MacOS (apple clang)**:

  OSH is skipped on macOS.

- **Windows (MSVC)**:

  Install MS-MPI as described in the MPI section. OSH is skipped on Windows.

No separate system ``shmem.h``/``oshcc``/``oshrun`` package is required for the
Linux course build.

``OpenMP``
~~~~~~~~~~
``OpenMP`` is included in ``gcc`` and ``msvc``, but some components should be installed additionally:

- **Linux (gcc and clang)**:
  
  .. code-block:: bash

     sudo apt install -y libomp-dev

- **MacOS (llvm)**:
  
  .. code-block:: bash

     brew install llvm
     brew install libomp
     brew link libomp --overwrite --force

``TBB``
~~~~~~~
- **Windows (MSVC)**, **Linux (gcc and clang)**, **MacOS (apple clang)**:
  Build as 3rdparty in the current project.

``std::thread``
~~~~~~~~~~~~~~~
``std::thread`` is included in STL libraries.
