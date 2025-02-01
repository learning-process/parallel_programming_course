# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SPHINXINTL    ?= sphinx-intl
SOURCEDIR     = .
BUILDDIR      = _build/html
SPHINXOPTS    += -W --keep-going -n

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)/en" $(SPHINXOPTS) $(O)

.PHONY: help update Makefile

gettext:
	@$(SPHINXBUILD) -b gettext "$(SOURCEDIR)" _build/gettext $(SPHINXOPTS)

update:
	@$(SPHINXINTL) update -p _build/gettext -l en -l ru

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -b html -D language=en "$(SOURCEDIR)" "$(BUILDDIR)/en" $(SPHINXOPTS) $(O)
	@$(SPHINXBUILD) -b html -D language=ru "$(SOURCEDIR)" "$(BUILDDIR)/ru" $(SPHINXOPTS) $(O)
