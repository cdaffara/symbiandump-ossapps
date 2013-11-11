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


#ifndef     PEPANIC_PAN
#define     PEPANIC_PAN

//  INCLUDES
#include <e32def.h>
#include <e32cmn.h>

// CONSTANTS

// Phone Engine panic
_LIT( KPEPhoneEnginePanic, "Phone Engine Panic!" );

    enum TPEPanic
        {
        EPEPanicAccessingDataOnIncompleteRequest = 1,
        EPEPanicAllSettingsDataObjectsAlreadyInUse = 2,
        EPEPanicBadHandle = 3,
        EPEPanicCallDataNull = 4,
        EPEPanicCallIndexOutOfRange = 5,
        EPEPanicConferenceIndexOutOfRange = 6,
        EPEPanicDebugDangerousMove = 7,
        EPEPanicIllegalCommand = 8,
        EPEPanicIndexOutOfRange = 9,
        EPEPanicInvalidCommand = 10,
        EPEPanicInvalidHandle = 11,
        EPEPanicInvalidMode = 12,
        EPEPanicInvalidRequest = 13,
        EPEPanicInvalidRequestId = 14,
        EPEPanicInvalidState = 15,
        EPEPanicLineNotFound = 16,
        EPEPanicNoCallInState = 17,
        EPEPanicNoCallOrLine = 18,
        EPEPanicNoConferenceCall = 19,
        EPEPanicNoErrorCodeInSuccessfulRequest = 20,
        EPEPanicNoEtel = 21,
        EPEPanicNoFreeCalls = 22,
        EPEPanicNoObserver = 23,
        EPEPanicNoPhoneServer = 24,
        EPEPanicNoRepository = 25,
        EPEPanicNotCompleted = 26,
        EPEPanicNullPointer = 27,
        EPEPanicObserverAlreadySet = 28,
        EPEPanicRepositoryAlreadyActive = 29,
        EPEPanicPropertyAlreadyActive = 30,
        EPEPanicSplitWithoutConference = 31,
        EPEPanicStupidUser = 32,
        EPEPanicUIHasBeenLeaved = 33,
        EPEPanicVideoLibraryNotFound = 34,
        EPEPanicInvalidIncomingCallMonitor = 35,
        EPEPanicInvalidCallAddedMonitor = 36,
        EPEPanicInvalidCallEventMonitor = 37,
        EPEPanicExternalDataIdOutOfRange = 38,
        EPEPanicCallIsAlreadyAddedToConference = 39,
        EPEPanicInvalidParameter = 40,
        EPEPanicAudioInitializeError = 41,
        
        EPEPanicMaximum = 100
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

GLREF_C void Panic( TPEPanic aPanic );

#endif
