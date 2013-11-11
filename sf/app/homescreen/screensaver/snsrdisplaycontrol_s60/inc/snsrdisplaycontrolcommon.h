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
* Description: Common constants for client and server sides of
*              Screensaver Display Control.
*
*/

#ifndef SNSRDISPLAYCONTROLCOMMON_H
#define SNSRDISPLAYCONTROLCOMMON_H

#include <e32def.h>
#include <e32debug.h>


_LIT( KSnsrDispCtrlSrvName, "SnsrDisplayControlServer" ); 

enum TSnsrDispCtrlSrvCmd
    {
    ESnsrDispCtrlSrvDisplayOff = 0,
    ESnsrDispCtrlSrvDisplayLowPower,
    ESnsrDispCtrlSrvDisplayFullPower
    };

const TInt KSnsrDispCtrlSrvVerMajor = 1;
const TInt KSnsrDispCtrlSrvVerMinor = 0;
const TInt KSnsrDispCtrlSrvVerBuild = 0;

#if defined(_DEBUG)
#define RDEBUG( x ) RDebug::Printf( "%s %s (%u) %s", __FILE__, __PRETTY_FUNCTION__, __LINE__, x );
#define RDEBUG1( x, y ) RDebug::Printf( "%s %s (%u) %s=%d", __FILE__, __PRETTY_FUNCTION__, __LINE__, x, y );
#else
#define RDEBUG( x )
#define RDEBUG1( x, y )
#endif

#endif /* SNSRDISPLAYCONTROLCOMMON_H */
