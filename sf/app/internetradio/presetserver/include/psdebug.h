/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Debug header file for Preset Server.
*
*/


#ifndef PSDEBUG_H
#define PSDEBUG_H

#include <e32svr.h>

// Define PS_ALL_DEBUG to get debug output from Preset Server.
// It is automatically enabled for debug builds.
// #define PS_ALL_DEBUG

#ifdef _DEBUG
#ifndef PS_ALL_DEBUG
#define PS_ALL_DEBUG
#endif // PS_ALL_DEBUG
#endif // _DEBUG

#ifdef PS_ALL_DEBUG
    #define PSDEBUG( a ) RDebug::Print( _L( a ) )
    #define PSDEBUG2( a, b ) RDebug::Print( _L( a ), b )
    #define PSDEBUG3( a, b, c ) RDebug::Print( _L( a ), b, c )
    #define PSDEBUG4( a, b, c, d ) RDebug::Print( _L( a ), b, c, d )
    #define PSDEBUG5( a, b, c, d, e ) RDebug::Print( _L( a ), b, c, d, e )
    #define PSDEBUGVAR( a ) a
#else
    #define PSDEBUG( a )
    #define PSDEBUG2( a, b )
    #define PSDEBUG3( a, b, c )
    #define PSDEBUG4( a, b, c, d ) 
    #define PSDEBUG5( a, b, c, d, e )
    #define PSDEBUGVAR( a )
#endif // PS_ALL_DEBUG

#endif // PSDEBUG_H
