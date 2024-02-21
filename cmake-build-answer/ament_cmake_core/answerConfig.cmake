# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_answer_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED answer_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(answer_FOUND FALSE)
  elseif(NOT answer_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(answer_FOUND FALSE)
  endif()
  return()
endif()
set(_answer_CONFIG_INCLUDED TRUE)

# output package information
if(NOT answer_FIND_QUIETLY)
  message(STATUS "Found answer: 0.0.0 (${answer_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'answer' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${answer_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(answer_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${answer_DIR}/${_extra}")
endforeach()
