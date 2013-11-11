/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  General definitions for Video telephony call control
*
*/



#ifndef VTCTLDEFS_H
#define VTCTLDEFS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// Null session id
const TInt KNullSessionId = -1;

// Session id for active session
const TInt KActiveSessionId = -2;

/**
* Active object priorities
*/
const TInt KVtCtlCallAdditionMonitorPriority = CActive::EPriorityStandard;
const TInt KVtCtlCallStatusMonitorPriority = CActive::EPriorityStandard;

/**
* Maximum length for telephone number
*/
const TInt KVtCtlTelNumberLength = 100;


// DATA TYPES

/**
* Remote end address
*/
typedef TBuf<KVtCtlTelNumberLength> TVtCtlTelNumber;

/**
* Session duration
*/
typedef TTimeIntervalSeconds TVtCtlDuration;

/**
* Session array
*/
typedef RArray<TInt> RVtCtlSessionIdArray;

/**
* Session id
*/
typedef TInt TVtCtlSessionId;

/**
* Session command
*/
typedef TInt TVtCtlCmd;

/**
* Extension ids
*/
enum {
    EVtCtlCSSessionInfoExtensionId = 1
    };

#endif      // VTCTLDEFS_H

// End of File
