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
* Description:  Panic definitions for videosource module.
*
*/


#ifndef VSPANIC_H
#define VSPANIC_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KVSPanicCategory, "VideoSource" );

// DATA TYPES
enum TVSPanic
    {
    EVSPanicSwitchAlreadyPending,           // 0
    EVSPanicProviderNotSet,                 // 1
    EVSPanicProviderAlreadyInitialized,     // 2
    EVSPanicProviderNotReady,               // 3
    EVSPanicOperationNotSupported,          // 4
    EVSPanicThreadOpenFailure               // 5
    };

// FUNCTION PROTOTYPES
extern void Panic( TVSPanic aPanicCode );

#endif // VSPANIC_H

// End of File
