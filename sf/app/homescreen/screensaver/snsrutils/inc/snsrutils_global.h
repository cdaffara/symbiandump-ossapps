/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: snsrutils_global.h
*
*/

#ifndef SNSRUTILS_GLOBAL_H
#define SNSRUTILS_GLOBAL_H

#include <qglobal.h>

#ifdef SNSRUTILS_LIB
    #define SNSRUTILS_EXPORT Q_DECL_EXPORT
#else
    #ifdef SNSRUTILS_TEST
        #define SNSRUTILS_EXPORT
    #else
        #define SNSRUTILS_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // SNSRUTILS_GLOBAL_H
