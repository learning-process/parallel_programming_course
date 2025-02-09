if (NOT USE_DOCS)
  return()
endif()

set(SOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BUILDDIR "${CMAKE_CURRENT_BINARY_DIR}/_build/html")
set(GETTEXTDIR "${CMAKE_CURRENT_BINARY_DIR}/_build/gettext")

add_custom_target(docs_help
  COMMAND ${SPHINXBUILD} -M help "${SOURCEDIR}" "${BUILDDIR}/en" ${SPHINXOPTS}
  WORKING_DIRECTORY "${SOURCEDIR}"
  COMMENT "Displaying Sphinx Makefile help"
)

# 'gettext' target: generate gettext catalogs.
add_custom_target(docs_gettext
  COMMAND ${SPHINXBUILD} -b gettext "${SOURCEDIR}" "${GETTEXTDIR}" ${SPHINXOPTS}
  WORKING_DIRECTORY "${SOURCEDIR}"
  COMMENT "Generating gettext output"
)

# 'update' target: update translations for the specified languages.
add_custom_target(docs_update
  COMMAND ${SPHINXINTL} update -p "${GETTEXTDIR}" -l en -l ru
  WORKING_DIRECTORY "${SOURCEDIR}"
  COMMENT "Updating Sphinx translations"
)

# 'html' target: build the HTML documentation in both English and Russian.
add_custom_target(docs_html
  # Build English documentation.
  COMMAND ${SPHINXBUILD} -b html -D language=en "${SOURCEDIR}" "${BUILDDIR}/en" ${SPHINXOPTS}
  # Build Russian documentation.
  COMMAND ${SPHINXBUILD} -b html -D language=ru "${SOURCEDIR}" "${BUILDDIR}/ru" ${SPHINXOPTS}
  WORKING_DIRECTORY "${SOURCEDIR}"
  COMMENT "Building HTML documentation for English and Russian"
)
