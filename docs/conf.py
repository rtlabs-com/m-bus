# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

import os
import pathlib
import re

pathobj_docs_dir = pathlib.Path(__file__).parent.absolute()
pathobj_rootdir = pathobj_docs_dir.parent.absolute()


# -- Project information -----------------------------------------------------

try:
    cmakelists_contents = pathobj_rootdir.joinpath("CMakeLists.txt").read_text()
    versiontext_match = re.search(r"MBUS VERSION ([\d.]*)", cmakelists_contents)
    version = versiontext_match.group(1)
except:
    version = "unknown version"

project = 'mbus'
copyright = '2024, RT-Labs AB'
author = 'RT-Labs AB'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "myst_parser",
    "sphinx.ext.autosectionlabel",
    "sphinx_copybutton",
    "sphinxcontrib.kroki",
    "sphinxcontrib.spelling",
    "sphinxcontrib.cairosvgconverter",
]

# Use version as PDF title page date
today = f"v{version}"

# Set the default domain
primary_domain = 'c'

# Do not highlight code
highlight_language = "none"

# Spelling
spelling_word_list_filename = "spelling-wordlist.txt"

# Add any paths that contain templates here, relative to this directory.
templates_path = []

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# Automatically and continuously number figures and tables
numfig = True

# Breathe Configuration
breathe_default_project = "mbus"
breathe_domain_by_extension = {
    "h": "c",
    "c": "c"
}

cpp_id_attributes = ["MB_EXPORT"]
c_id_attributes = cpp_id_attributes

# -- Options for HTML output -------------------------------------------------

html_context = {
   "default_mode": "light"
}

html_theme = "sphinx_book_theme"
html_theme_options = {
    "show_nav_level": 3,
    "home_page_in_toc": True,
    "navigation_with_keys": False,
    "use_repository_button": False,
    "use_fullscreen_button": False,
    "navbar_end": ["navbar-icon-links"],
    "use_download_button": False,
    "extra_footer": f"<div>MBUS v{version}</div>",
}

html_last_updated_fmt = None
html_static_path = ["static"]
html_logo = "static/i/mbus.svg"
html_favicon = "static/i/favicon-rtlabs.png"
html_copy_source = False
html_css_files = []

# -- Options for LaTeX output ------------------------------------------------

latex_engine = "xelatex"
latex_table_style = ["colorrows"]

latex_elements = {
    "fontpkg": r"""
    \setmainfont{Lato-Light}
    \setsansfont{Lato-Light}
    \setmonofont{Liberation Mono}
    """,
    "papersize": "a4paper",
    "pointsize": "10pt",
    "figure_align": "H", # Disable floating figures
    "passoptionstopackages": r"\PassOptionsToPackage{hyphens}{url}",
    "printindex": r"\footnotesize\def\twocolumn[#1]{#1}\printindex",
    "sphinxsetup": ",".join((
        "verbatimwithframe=false",
        "VerbatimColor={gray}{0.95}",
        "TitleColor={black}",
        "InnerLinkColor={black}",
        "OuterLinkColor={black}"
    )),
    "preamble": r"\usepackage{titlepage}"
}

latex_additional_files = [
    "titlepage.sty",
    "illustrations/mbus.pdf",
    "illustrations/rtlabs_logo.eps"
]

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass [howto/manual]).
latex_documents = [
    ("index", "MBUS-User-Manual.tex", "MBUS User Manual", "RT-Labs AB", "manual")
]
