include_guard()

option(USE_DOCS "Build Sphinx documentation" OFF)
if(USE_DOCS)
  set(SPHINXBUILD "sphinx-build")
  set(SPHINXINTL "sphinx-intl")
  set(SPHINXOPTS "-W" "--keep-going" "-n")
endif(USE_DOCS)
