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
* Description:  Panic definitions for video telephone engine.
*
*/



#ifndef CVTENGPANIC_H
#define CVTENGPANIC_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KVtEngPanicCategory, "VTENG" );

// DATA TYPES
enum TVtEngPanic
    {
    EVtEngPanicInvalidInitializationEntry,      /** 0 */
    EVtEngPanicInvalidInitializationState,      /** 1 */
    EVtEngPanicIncorrectUtility,                /** 2 */
    EVtEngPanicTlsPanic,                        /** 3 */
    EVtEngPanicNoVideoProvider,                 /** 4 */
    EVtEngPanicInvalidSessionState,             /** 5 */
    EVtEngPanicMediaHandlerOpStateFailure,      /** 6 */
    EVtEngPanicInvalidFlag,                     /** 7 */
    EVtEngPanicInvalidPointer,                  /** 8 */
    EVtEngPanicInvalidAudioPointer,             /** 9 */
    EVtEngPanicCommsDestructOnInvalidState,     /** 10 */
    EVtEngPanicUnInitIncorrectOperation,        /** 11 */
    EVtEngPanicDisconnectTimerExpired,          /** 12 */
    EVtEngPanicInvalidDtmfState,                /** 13 */
    EVtEngPanicInvalidBitmapHandle,             /** 14 */
    EVtEngPanicInvalidPortRead,                 /** 15 */
    /** Too many asynchronous events pending in queue */
    EVtEngPanicInvalidTooManyPendingEvents,     /** 16 */
    /** VT variation cenrep file is missing or corrupt */
    EVtEngPanicVtCenrepVariation,               /** 17 */
    /** VT configuration cenrep file is missing or corrupt */
    EVtEngPanicVtCenrepConfiguration,           /** 18 */
    EVtEngPanicInvalidVideoQualityValue,         /** 19 */
    EVtEngPanicWrongMediatorSenderState         /** 20 */
    };

// FUNCTION PROTOTYPES
void Panic( TVtEngPanic aPanic );

#endif // CVTENGPANIC_H

// End of File
