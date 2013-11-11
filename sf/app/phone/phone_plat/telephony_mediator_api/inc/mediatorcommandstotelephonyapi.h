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
* Description:  Mediator API for commanding Telephony.
*
*/


#ifndef MEDIATORCOMMANDSTOTELEPHONYAPI_H
#define MEDIATORCOMMANDSTOTELEPHONYAPI_H

// INCLUDES
#include <e32base.h>

/**  The category UID for the messages in this header file. */
const TUid KCatCommandsToTelephony = { 0x100058B8 };

/**  Version number of this API */
const TInt KTelephonyCommandsVersionMajor( 1 );
const TInt KTelephonyCommandsVersionMinor( 0 );
const TInt KTelephonyCommandsVersionBuild( 0 );

/**  Generic Commands to Phone Application. */
enum TCommandsToTelephony
    {
    /**
     * Commands Telephony to end currently active phone call.
     */
    EPhoneCmdEndActiveCall = 0
    };

#endif // MEDIATORCOMMANDSTOTELEPHONYAPI_H

