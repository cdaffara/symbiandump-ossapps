/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for CSP
*
*/



#ifndef CCSPCONSTS_H
#define CCSPCONSTS_H

_LIT( KCSPName,"CSP" );


//The length of a pause created during dialling by the comma character (',') measured in seconds.  
const TUint KCSPDefaultInterval = 2;

// The boolean indicating the autoreadial is on or off
const TBool KCSPDefaultAutoRedial = EFalse;

/**
* Required for notifying Incoming Call Monitor.
*/ 
const TInt KCSPImplementationUidInt = 0x102828E5;

/**
* Implementation uid of CS Plugin.
*/
const TUid KCSPImplementationUid = { KCSPImplementationUidInt };

/**
* Number of message slots in RTelServer
*/
const TInt KNbrOfMessageSlots = 128;

const TInt KCSPPhoneNumberMaxLength = 100;

const TInt KCSPCallingNameMaxLength = 80;

#endif // CCSPCONSTS_H
