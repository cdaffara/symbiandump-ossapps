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
* Description:
*
*/

#ifndef NMAILSETTINGUIDEF_H
#define NMAILSETTINGUIDEF_H

#include <QtGlobal>

#if defined(Q_OS_WIN)
    #define NMAILSETTINGUI_EXPORT __declspec(dllexport)
#else
    #if defined(BUILD_NMAILSETTINGUI_DLL)
        #define NMAILSETTINGUI_EXPORT Q_DECL_EXPORT
    #else
        #define NMAILSETTINGUI_EXPORT Q_DECL_IMPORT
    #endif
#endif

#if defined(TEST_BUILD_NMAILSETTINGUI_DLL)
    #ifdef NMAILSETTINGUI_EXPORT
        #undef NMAILSETTINGUI_EXPORT
    #endif

    #define NMAILSETTINGUI_EXPORT
#endif

#endif // NMAILSETTINGUIDEF_H
