#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2025 rt-labs AB, Sweden.
# See LICENSE file in the project root for full license information.
#*******************************************************************/

find_package(Doxygen 1.9.0...<2.0.0)
find_package(Sphinx)

if (DOXYGEN_FOUND)

  # Doxygen configuration
  set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
  set(DOXYGEN_TYPEDEF_HIDES_STRUCT YES)
  set(DOXYGEN_EXTRACT_ALL NO)
  set(DOXYGEN_EXTRACT_STATIC NO)
  set(DOXYGEN_STRIP_CODE_COMMENTS NO)
  set(DOXYGEN_MACRO_EXPANSION YES)
  set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
  set(DOXYGEN_PREDEFINED MB_EXPORT)
  set(DOXYGEN_EXPAND_AS_DEFINED MB_EXPORT)
  set(DOXYGEN_COLLABORATION_GRAPH NO)
  set(DOXYGEN_INCLUDE_GRAPH NO)
  set(DOXYGEN_INCLUDED_BY_GRAPH NO)
  set(DOXYGEN_RECURSIVE NO)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
  set(DOXYGEN_GENERATE_XML YES)
  set(DOXYGEN_GENERATE_HTML NO)
  set(DOXYGEN_SHOW_FILES NO)
  set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS) # Doxygen 1.9+
  doxygen_add_docs(doxygen
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    USE_STAMP_FILE
    README.md
    include/mb_error.h
    include/mb_rtu.h
    include/mb_slave.h
    include/mb_tcp.h
    include/mbus.h
    )
endif()

if (SPHINX_FOUND AND DOXYGEN_FOUND)
  set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
  set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

  # Targets for Sphinx html, spell checking and link checking
  macro(add_sphinx_builder)
    add_custom_target(${ARGV0}
      COMMAND
      ${CMAKE_COMMAND} -E env LATEXMKOPTS='-silent -xelatex'
      ${SPHINX_EXECUTABLE} -M ${ARGV1}
      ${SPHINX_SOURCE} ${SPHINX_BUILD}
      -a -E -q --keep-going
      -Dbreathe_projects.mbus=${CMAKE_CURRENT_BINARY_DIR}/xml
      COMMENT "Running sphinx-build ${ARGV1}"
    )
    add_dependencies(${ARGV0} doxygen)
  endmacro()

  add_sphinx_builder(sphinx-html html)
  add_sphinx_builder(sphinx-pdf latexpdf)
  add_sphinx_builder(sphinx-spelling spelling)
  add_sphinx_builder(sphinx-linkcheck linkcheck)

endif()
