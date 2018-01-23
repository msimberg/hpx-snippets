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

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${HPX_INCLUDE_DIRS})

add_executable(future_then_benchmark future_then_benchmark.cpp)
target_link_libraries(future_then_benchmark ${HPX_LIBRARIES})

add_executable(init_benchmark init_benchmark.cpp)
target_link_libraries(init_benchmark ${HPX_LIBRARIES})

add_executable(suspension_benchmark suspension_benchmark.cpp)
target_link_libraries(suspension_benchmark ${HPX_LIBRARIES})
