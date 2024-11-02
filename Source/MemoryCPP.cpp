// File: Memory.cpp
// Description: C++ compatible in-code memory sanitizer. This handles c++ extensions, and requires Memory.c to be
//                  compiled as well.
// Author: Matthew Krueger <mckrueg@bgsu.edu>
// Course: CS 3350, fall 2024


#include <mktl_c/Memory.h>

void* operator new(size_t bytes){
#ifdef USE_MEMORY_TRACKING
    return trackedMalloc(bytes);
#else
    return malloc(bytes);
#endif
}

void* operator new[](size_t bytes){
#ifdef USE_MEMORY_TRACKING
    return trackedMalloc(bytes);
#else
    return malloc(bytes);
#endif
}

void operator delete(void* pVoid) noexcept{
#ifdef USE_MEMORY_TRACKING
    return trackedFree(pVoid);
#else
    return free(pVoid);
#endif
}

void operator delete[](void* pVoid) noexcept{
#ifdef USE_MEMORY_TRACKING
    return trackedFree(pVoid);
#else
    return free(pVoid);
#endif
}