/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines the panics of the ConnTest
*
*/

#ifndef __CONNTEST_PAN__
#define __CONNTEST_PAN__

#include <e32std.h>

_LIT(KPanicConnTest, "ConnTest");
_LIT(KPanicSocketsEngine, "SocketsEngine");
_LIT(KPanicSocketsWrite, "SocketsWrite");

/** ConnTest application panic codes */
enum TConnTestPanics 
    {
    EConnTestBasicUi = 1,
    EConnTestBadStatus,
    EConnTestAppView,
    EConnTestContainer,
    EConnTestBadRoamingStatus,
    EConnTestHttpClientInitializationFailed
    // add further panics here
    };

#endif // __CONNTEST_PAN__
