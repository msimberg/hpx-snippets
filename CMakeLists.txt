project(hpx_snippets CXX)

cmake_minimum_required(VERSION 3.4)

find_package(BLAS REQUIRED)

if(EXISTS "${HPX_DIR}")
    find_package(HPX REQUIRED)

    if(NOT HPX_FOUND)
        message(FATAL_ERROR "HPX could not be found")
    endif()
else()
    message(WARNING "HPX_DIR=${HPX_DIR} does not exist. Did you forget to run the install rule?")
endif()

find_package(PkgConfig QUIET)
pkg_check_modules(PC_HWLOC QUIET hwloc)

find_path(HWLOC_INCLUDE_DIR hwloc.h
  HINTS
    ${HWLOC_ROOT} ENV HWLOC_ROOT
    ${PC_HWLOC_MINIMAL_INCLUDEDIR}
    ${PC_HWLOC_MINIMAL_INCLUDE_DIRS}
    ${PC_HWLOC_INCLUDEDIR}
    ${PC_HWLOC_INCLUDE_DIRS}
  PATH_SUFFIXES include)

find_library(HWLOC_LIBRARY NAMES hwloc libhwloc
  HINTS
    ${HWLOC_ROOT} ENV HWLOC_ROOT
    ${PC_HWLOC_MINIMAL_LIBDIR}
    ${PC_HWLOC_MINIMAL_LIBRARY_DIRS}
    ${PC_HWLOC_LIBDIR}
    ${PC_HWLOC_LIBRARY_DIRS}
  PATH_SUFFIXES lib lib64)

set(HWLOC_LIBRARIES ${HWLOC_LIBRARY})
set(HWLOC_INCLUDE_DIRS ${HWLOC_INCLUDE_DIR})

find_package_handle_standard_args(Hwloc DEFAULT_MSG
  HWLOC_LIBRARY HWLOC_INCLUDE_DIR)

get_property(_type CACHE HWLOC_ROOT PROPERTY TYPE)
if(_type)
  set_property(CACHE HWLOC_ROOT PROPERTY ADVANCED 1)
  if("x${_type}" STREQUAL "xUNINITIALIZED")
    set_property(CACHE HWLOC_ROOT PROPERTY TYPE PATH)
  endif()
endif()

mark_as_advanced(HWLOC_ROOT HWLOC_LIBRARY HWLOC_INCLUDE_DIR)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${HPX_INCLUDE_DIRS})

add_executable(future_then_benchmark future_then_benchmark.cpp)
target_link_libraries(future_then_benchmark ${HPX_LIBRARIES})

add_executable(init_benchmark init_benchmark.cpp)
target_link_libraries(init_benchmark ${HPX_LIBRARIES})

add_executable(suspension_benchmark suspension_benchmark.cpp)
target_link_libraries(suspension_benchmark ${HPX_LIBRARIES})

add_executable(start_stop_benchmark start_stop_benchmark.cpp)
target_link_libraries(start_stop_benchmark ${HPX_LIBRARIES})

add_executable(openmp_benchmark openmp_benchmark.cpp)
target_link_libraries(openmp_benchmark ${HPX_LIBRARIES})

add_executable(condition_variable_benchmark condition_variable_benchmark.cpp)
target_link_libraries(condition_variable_benchmark ${HWLOC_LIBRARIES})
