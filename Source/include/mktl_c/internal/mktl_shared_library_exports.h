/********************************************************************************
 *  MKTL - Matthew Krueger's template library of C++ useful stuff               *
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


#ifndef MKTL_SHARED_LIBRARY_EXPORTS
#define MKTL_SHARED_LIBRARY_EXPORTS

#ifndef MKTL_BUILD_STATIC
#if defined _WIN32 || defined __CYGWIN__
#   ifdef MKTL_BUILD_LIBRARY
// Exporting...
#       ifdef __GNUC__
#           define __MKTL_API __attribute__ ((dllexport))
#       else
#           define __MKTL_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#       endif
#   else
#       ifdef __GNUC__
#           define __MKTL_API __attribute__ ((dllimport))
#       else
#           define __MKTL_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#       endif
#   endif
#   define __MKTL_API_HIDDEN
#else
#   if __GNUC__ >= 4
#       define __MKTL_API __attribute__ ((visibility ("default")))
#       define __MKTL_API_HIDDEN  __attribute__ ((visibility ("hidden")))
#   else
#       define __MKTL_API
#       define __MKTL_API_HIDDEN
#       error Problem configuring
#   endif
#endif
#else
#define __MKTL_API
#define __MKTL_API_HIDDEN
#endif

#endif //MKTL_SHARED_LIBRARY_EXPORTS