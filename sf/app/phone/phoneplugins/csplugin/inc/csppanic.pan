/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains panic enumerations
*
*/


#ifndef     CSPPANIC_PAN
#define     CSPPANIC_PAN

//  INCLUDES
#include <e32def.h>
#include <e32cmn.h>

// CONSTANTS

// CSPlugin panic text
_LIT( KCSPPanic, "CSPlugin Panic!" );

    enum TCSPPanic
        {
        ECSPPanicAccessingDataOnIncompleteRequest = 1,
        ECSPPanicAllSettingsDataObjectsAlreadyInUse = 2,
        ECSPPanicBadHandle = 3,
        ECSPPanicCallDataNull = 4,
        ECSPPanicCallIndexOutOfRange = 5,
        ECSPPanicConferenceIndexOutOfRange = 6,
        ECSPPanicDebugDangerousMove = 7,
        ECSPPanicIllegalCommand = 8,
        ECSPPanicIndexOutOfRange = 9,
        ECSPPanicInvalidCommand = 10,
        ECSPPanicInvalidHandle = 11,
        ECSPPanicInvalidMode = 12,
        ECSPPanicInvalidRequest = 13,
        ECSPPanicInvalidRequestId = 14,
        ECSPPanicInvalidState = 15,
        ECSPPanicLineNotFound = 16,
        ECSPPanicNoCallInState = 17,
        ECSPPanicNoCallOrLine = 18,
        ECSPPanicNoConferenceCall = 19,
        ECSPPanicNoErrorCodeInSuccessfulRequest = 20,
        ECSPPanicNoEtel = 21,
        ECSPPanicNoFreeCalls = 22,
        ECSPPanicNoObserver = 23,
        ECSPPanicNoPhoneServer = 24,
        ECSPPanicNoRepository = 25,
        ECSPPanicNotCompleted = 26,
        ECSPPanicNullPointer = 27,
        ECSPPanicObserverAlreadySet = 28,
        ECSPPanicRepositoryAlreadyActive = 29,
        ECSPPanicPropertyAlreadyActive = 30,
        ECSPPanicSplitWithoutConference = 31,
        ECSPPanicUIHasBeenLeaved = 32,
        ECSPPanicVideoLibraryNotFound = 33,
        ECSPPanicInvalidIncomingCallMonitor = 34,
        ECSPPanicInvalidCallAddedMonitor = 35,
        ECSPPanicInvalidCallEventMonitor = 36,
        ECSPPanicExternalDataIdOutOfRange = 37,
        ECSPPanicCallIsAlreadyAddedToConference = 38,
        ECSPPanicInvalidParameter = 39,
        ECSPPanicDataConnectionInitError = 40,
        ECSPPanicTimerAlreadyActive = 41,
        ECSPPanicAudioStreamInitFailure = 42,
        
        ECSPPanicMaximum = 100
        };

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None.

// CLASS DECLARATION
// None.

GLREF_C void Panic( TCSPPanic aPanic );

#endif
