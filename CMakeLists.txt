cmake_minimum_required(VERSION 3.28)
project(mkstl)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)



###############################
#        Configuration        #
###############################
set(MKTL_BUILD_SHARED_LIBS ON)
set(MKTL_BUILD_WITH_OPENMP ON)

set(MKTL_VERSION_MAJOR 0)
set(MKTL_VERSION_MINOR 0)
set(MKTL_VERSION_PATCH 1)

###############################
#  Loading Required Options   #
###############################
set(MKTL_OPENMP_BUILD_FLAG 0)
if(MKTL_BUILD_WITH_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        message(STATUS "MKTL -- Using OpenMP for Multiprocessing Acceleration.")
        set(MKTL_OPENMP_BUILD_FLAG 1)
    endif()
endif()

###############################
#   Import the actual Lib     #
###############################
add_subdirectory(Source)

###############################
#         Import Demos        #
###############################