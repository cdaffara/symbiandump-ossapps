/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXUTILS_H
#define CXUTILS_H

// This macro changed name between Qt 4.5.2 and Qt 4.6.0.
// Make the old name available, if not defined.
//! @todo Remove once old environments not used anymore.
#include <QGlobalStatic> // Included to get the ifndef working
#ifndef QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
    #define QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION QT_TRAP_THROWING
#endif


// Traces enabled only in debug builds.
// These can heavily affect the performance.
#ifdef _DEBUG

    #include <QDebug>       // Qt Debug
#ifdef Q_OS_SYMBIAN
    #include <e32debug.h>   // Symbian Debug
#endif // Q_OS_SYMBIAN

    #define CX_DEBUG(msg)                   qDebug msg;
    #define CX_DEBUG_ENTER_FUNCTION()       qDebug("%s => ", __PRETTY_FUNCTION__);
    #define CX_DEBUG_IN_FUNCTION()          qDebug("%s =><= ", __PRETTY_FUNCTION__);
    #define CX_DEBUG_EXIT_FUNCTION()        qDebug("%s <= ", __PRETTY_FUNCTION__);
#ifdef Q_OS_SYMBIAN
    #define CX_DEBUG_SYMBIAN(msg)           RDebug::Print msg;
    #define CX_DEBUG_ASSERT(x)              if( !(x) ) {\
                                                qDebug("%s:%d, ASSERT FAIL [%s]", __FILE__, __LINE__, #x); \
                                                ASSERT(x); \
                                            }
#else // !Q_OS_SYMBIAN
    #define CX_DEBUG_SYMBIAN(msg)           qDebug()<< msg;
    #define CX_DEBUG_ASSERT(x)              if( !(x) ) {\
                                                qDebug("%s:%d, ASSERT FAIL [%s]", __FILE__, __LINE__, #x); \
                                                Q_ASSERT(x); \
                                            }
    #define OstTrace0(x,y,z)

#endif // Q_OS_SYMBIAN
    #define CX_ASSERT_ALWAYS(x)             CX_DEBUG_ASSERT(x)

#else // !_DEBUG

    #define CX_DEBUG(msg)
    #define CX_DEBUG_SYMBIAN(msg)
    #define CX_DEBUG_ENTER_FUNCTION()
    #define CX_DEBUG_IN_FUNCTION()
    #define CX_DEBUG_EXIT_FUNCTION()
    #define CX_DEBUG_ASSERT(x)

#ifdef Q_OS_SYMBIAN
    #define CX_ASSERT_ALWAYS(x)             ASSERT(x)
#else // !Q_OS_SYMBIAN
    #define CX_ASSERT_ALWAYS(x)             Q_ASSERT(x)
    #define OstTrace0(x,y,z)
#endif // Q_OS_SYMBIAN

#endif // _DEBUG

#endif // CXUTILS_H
