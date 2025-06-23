name: 'Setup Windows Toolchain'
description: 'Configure msbuild, MPI, ccache, ninja, and MSVC'
runs:
  using: 'composite'
  steps:
    - name: Add msbuild to PATH
      uses: microsoft/setup-msbuild@v2
      with:
        vs-version: 'latest'
    - name: Setup MPI
      uses: mpi4py/setup-mpi@v1
      with:
        mpi: msmpi
    - name: Setup ccache
      uses: Chocobo1/setup-ccache-action@v1
      with:
        windows_compile_environment: msvc
    - name: Setup ninja
      uses: seanmiddleditch/gha-setup-ninja@v6
    - name: Setup MSVC for Ninja again
      uses: ilammy/msvc-dev-cmd@v1
