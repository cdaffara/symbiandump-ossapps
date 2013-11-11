/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging utility application for MC Photos
*
*/



#ifndef __GLXLOGGING_PAN__
#define __GLXLOGGING_PAN__

/** glx logging application panic codes */
enum TGlxLoggingPanics 
    {
    EGlxLoggingBasicUi = 1
    // add further panics here
    };

inline void Panic(TGlxLoggingPanics aReason)
    {
	_LIT(applicationName,"GlxLogging");
    User::Panic(applicationName, aReason);
    }

#endif // __GLXLOGGING_PAN__
