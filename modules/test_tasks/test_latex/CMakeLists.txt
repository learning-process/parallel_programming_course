get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if ( USE_LATEX )
    message( STATUS "-- " ${ProjectId} )
    file(GLOB_RECURSE report_files "*.tex")
    get_filename_component(report_names ${report_files} NAME)
    add_latex_document(${report_names})

else( USE_LATEX )
    message( STATUS "-- ${ProjectId} - NOT BUILD!"  )
endif( USE_LATEX )
