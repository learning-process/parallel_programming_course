# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os

project = "Parallel Programming Course"
copyright = "2025, Learning Process"
author = "Learning Process"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "breathe",
]

breathe_projects = {
    "ParallelProgrammingCourse": os.path.join(os.path.dirname(__file__), "..", "xml"),
}
breathe_default_project = "ParallelProgrammingCourse"

# Suppress specific warnings for API documentation
suppress_warnings = [
    "ref.ref",  # undefined label warnings
    "ref.identifier",  # cpp:identifier reference target not found
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]
locale_dirs = ["locale"]
gettext_compact = False

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

html_sidebars = {
    "**": [
        "globaltoc.html",
        "sourcelink.html",
        "searchbox.html",
        "language_switcher.html",
    ],
}

html_theme_options = {
    "collapse_navigation": False,
    "navigation_depth": 2,
}


def setup(app):
    """Add custom CSS files."""
    app.add_css_file("custom.css")
