Set Up Your Environment
========================

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
