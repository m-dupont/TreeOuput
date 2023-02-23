# cmake_minimum_required(VERSION <specify CMake version here>)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(numpy_from_cpp)
set(CMAKE_VERBOSE_MAKEFILE ON)


if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    include(CodeCoverage)
    # exclude /tests and /ext directories from coverage analysis
    set(LCOV_REMOVE_EXTRA "'tests/*'" "'ext/*'")
    setup_target_for_coverage(unit_tests_coverage tests coverage)
endif()

set(CMAKE_CXX_STANDARD 11)

#This should be my default compiler flags
#SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wextra -pedantic -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default")
SET(CMAKE_CXX_FLAGS " -Wall -Wextra -pedantic -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#find_package(libprofiler)

#FIND_PACKAGE(Boost   REQUIRED )

#find_package(PkgConfig)
#pkg_search_module (PROFILE libprofiler)
#link_directories(/data/misc_soft/gperftools/lib)
#link_directories(${PROFILE_LIBRARY_DIRS})

#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})




MESSAGE("CMAKE_PREFIX_PATH = "  ${CMAKE_PREFIX_PATH})
MESSAGE("LINK_DIRECTORIES = "  ${LINK_DIRECTORIES})


include_directories("common" "impl")


set(SOURCE_FILES
        impl/NumpyFile.cc
        impl/AsciiFile.cc
        common/TreeFile.cc
        common/File.cc
        common/FileExceptions.cc
        common/TreeFileManager.cc)


FIND_PACKAGE(ROOT QUIET COMPONENTS Core)
IF(NOT ROOT_INCLUDE_DIRS)
    MESSAGE("Root not found! Please install Root and make it available in PATH")
ELSE()
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIRS})
    SET(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR})
    list(APPEND SOURCE_FILES impl/RootTreeFile.cc)
ENDIF()

find_package(HDF5 COMPONENTS C CXX HL REQUIRED)

IF(HDF5_FOUND)
    MESSAGE("HDF5 found")
    include_directories(${HDF5_INCLUDE_DIRS})
    list(APPEND SOURCE_FILES impl/HDF5File.cc)

ENDIF()


add_library(object_lib OBJECT ${SOURCE_FILES})

IF(ROOT_INCLUDE_DIRS)
    IF(HDF5_FOUND)
    add_executable(numpy_from_cpp main.cpp $<TARGET_OBJECTS:object_lib>)
    target_link_libraries(numpy_from_cpp ${ROOT_LIBRARIES} HDF5::HDF5)

    add_executable(benchmark benchmark.cpp $<TARGET_OBJECTS:object_lib> tests/helpers.cpp)
    target_link_libraries(benchmark ${ROOT_LIBRARIES} HDF5::HDF5)
    ENDIF()
ENDIF()


#add_executable(test tests/)

add_library(numpy_from_cpp_lib $<TARGET_OBJECTS:object_lib>)


set(TESTS_FILES
        tests/0_define_catch2.cpp tests/helpers.cpp
        tests/0_test_numpy.cpp
        tests/0_test_ascii.cpp
        tests/0_test_input_manager.cpp
        tests/0_test_output_manager.cpp
        )

add_executable(tests ${TESTS_FILES} $<TARGET_OBJECTS:object_lib>)
target_include_directories(tests PRIVATE . common)
target_link_libraries(tests ${ROOT_LIBRARIES})



#target_link_libraries(numpy_from_cpp ${PROFILE_LIBRARIES})

