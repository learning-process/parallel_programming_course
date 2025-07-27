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

   - Ubuntu environment with gcc-14, CMake, MPI, OpenMP
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

Parallel Programming Technologies
---------------------------------

``MPI``
~~~~~~~
- **Windows (MSVC)**:

  `Installers link <https://www.microsoft.com/en-us/download/details.aspx?id=105289>`_. You have to install ``msmpisdk.msi`` and ``msmpisetup.exe``.

- **Linux (gcc and clang)**:
  
  .. code-block:: bash

     sudo apt install -y mpich openmpi-bin libopenmpi-dev

- **MacOS (apple clang)**:
  
  .. code-block:: bash

     brew install open-mpi

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

``TBB``
~~~~~~~
- **Windows (MSVC)**, **Linux (gcc and clang)**, **MacOS (apple clang)**:
  Build as 3rdparty in the current project.

``std::thread``
~~~~~~~~~~~~~~~
``std::thread`` is included in STL libraries.
