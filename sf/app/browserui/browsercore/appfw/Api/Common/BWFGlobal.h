/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef BWFGLOBAL_H
#define BWFGLOBAL_H

#include <QtCore/qglobal.h>

#define BWF_ORGANIZATION_DOMAIN "com.nokia"
#define BWF_ORGANIZATION_NAME "Nokia"

// deprecated: inherit BrowserCore rules directly
//#define BWF_EXPORT WRT_BROWSER_EXPORT

// Define Export rules specifically for AppFW
// (deprecated: define symbian32 export explicitly as empty, here's how it all used to look:)
// #ifndef BWF_EXPORT
// # if defined (BUILDING_BWF_CORE)
// #  define BWF_EXPORT Q_DECL_EXPORT
// # else
// #  ifdef __SYMBIAN32__
// #    define BWF_EXPORT
// #  else
// #    define BWF_EXPORT Q_DECL_IMPORT
// #  endif
// # endif
// #endif

#ifndef BWF_EXPORT
# if defined (BUILDING_BWF_CORE)
#  define BWF_EXPORT Q_DECL_EXPORT
# else
#  define BWF_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // BWFGLOBAL_H

