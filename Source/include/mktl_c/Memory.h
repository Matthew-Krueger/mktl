/********************************************************************************
 *  MCKRUEG STL - mckrueg's standard template library of C++ useful stuff       *
 *  Copyright (C) 2024 Matthew Krueger <contact@matthewkrueger.com>             *
 *                                                                              *
 *  This program is free software: you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation, either version 3 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.      *
 ********************************************************************************/

// A C++ and C89 compatible header for the in-code memory-sanitizer. The file will include
// Both C++ and C89 features, depending on what file it is include into. Please define USE_MEMORY_TRACKING
// to override operators for C++. NOTE: Right now, I have the define manually enabled, but the line
// can be removed and set with a build system instead if you prefer.



#include <stdlib.h>
#include "internal/mktl_shared_library_exports.h"

#ifndef KRUEGERM_PGM3_MEMORY_HPP
#define KRUEGERM_PGM3_MEMORY_HPP

#ifdef __cplusplus

__MKTL_API void* operator new(size_t bytes);
__MKTL_API void* operator new[](size_t bytes);
__MKTL_API void operator delete(void* pVoid) noexcept;
__MKTL_API void operator delete[](void* pVoid) noexcept;

extern "C" {
#endif

/**
 * The state of a pointer, whether invalid, allocated, or deallocated
 */
enum PointerState{
    INVALID,
    ALLOCATED,
    DEALLOCATED
};

/**
 * Information about the pointer
 */
struct PointerInfo{
    enum PointerState enumPointerState;
    size_t pointerSize;
};

/**
 * A pAllocator that tracks memory allocations
 * @param bytes The size of the pointer
 * @return The pointer
 */
__MKTL_API void *trackedMalloc(unsigned long bytes);

/**
 * a pAllocator that tracks memory deallocations
 * @param pVoid the pointer to deallocate
 */
__MKTL_API void trackedFree(void *pVoid);

/**
 * Get the pointer info
 * @param pVoid The pointer
 * @return Pointer info
 */
__MKTL_API struct PointerInfo getPointerInfo(void *pVoid);


__MKTL_API unsigned long long getTotalBytesAllocated();
__MKTL_API unsigned long long getAllocationCount();
__MKTL_API unsigned long long getTotalBytesDeallocated();
__MKTL_API unsigned long long getDeallocationCount();
__MKTL_API unsigned long long getBytesCurrentlyAllocated();

#ifdef __cplusplus
}
#endif





#endif //KRUEGERM_PGM3_MEMORY_HPP
