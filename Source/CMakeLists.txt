
###############################
#      Shared Lib setup       #
###############################
if(MKTL_BUILD_SHARED_LIBS)
    set(MKTL_LIBRARY_TYPE OBJECT)
    set(MKTL_MAIN_LIBRARY_TYPE SHARED)
    message(STATUS "MKTL -- Building MKTL and dependencies as DLLs")
else(MKTL_BUILD_SHARED_LIBS)
    set(MKTL_LIBRARY_TYPE OBJECT)
    set(MKTL_MAIN_LIBRARY_TYPE STATIC)
    target_compile_definitions("MKTL_Interface" INTERFACE MKTL_BUILD_STATIC)
    message(STATUS "MKTL -- Building MKTL and dependencies as Object Libraries")
endif(MKTL_BUILD_SHARED_LIBS)

###############################
#      Original Lib Def       #
###############################
add_library(MKTL_Interface INTERFACE include/mktl/Traps.hpp include/mktl_c/Memory.h include/mktl_c/internal/mktl_shared_library_exports.h include/mktl/Result.hpp)

###############################
#    C++ Macro Definitions    #
###############################
target_compile_definitions("MKTL_Interface" INTERFACE MKTL_VERSION_MAJOR=${MKTL_VERSION_MAJOR})
target_compile_definitions("MKTL_Interface" INTERFACE MKTL_VERSION_MINOR=${MKTL_VERSION_MINOR})
target_compile_definitions("MKTL_Interface" INTERFACE MKTL_VERSION_PATCH=${MKTL_VERSION_PATCH})
#target_compile_definitions("MKTL_Interface" INTERFACE MKTL_BUILD_WITH_PROFILING=${MKTL_PROFILING_BUILD_FLAG})
target_compile_definitions("MKTL_Interface" INTERFACE MKTL_BUILD_WITH_OPENMP=${MKTL_OPENMP_BUILD_FLAG})

target_include_directories("MKTL_Interface" INTERFACE include/
)

###############################
#   Enable memory tracking    #
#    Only in debug builds     #
###############################
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MKTL_TRACK_MEMORY_ALLOC)
        target_compile_definitions("MKTL_Interface" INTERFACE MKTL_TRACK_MEMORY_ALLOC_FLAG=1)
    endif()
endif()

###############################
#      Link with OpenMP       #
###############################
if(OpenMP_CXX_FOUND)
    target_link_libraries(GEOVK_Interface INTERFACE OpenMP::OpenMP_CXX)
endif()

###############################
#     Alias for easy use      #
###############################
add_library(MKTL::Interface ALIAS MKTL_Interface)

###############################
# And add compiled libraries  #
###############################
add_library(MKTL_Main ${MKTL_MAIN_LIBRARY_TYPE} Memory.c MemoryCPP.cpp)

target_link_libraries(MKTL_Main MKTL::Interface)


