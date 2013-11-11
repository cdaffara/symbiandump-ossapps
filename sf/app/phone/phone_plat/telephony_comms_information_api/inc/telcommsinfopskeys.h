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
* Description:  Telephony Comms Information Publish and Subscribe keys.
*
*/


#ifndef TELCOMMSINFOPSKEYS_H
#define TELCOMMSINFOPSKEYS_H

#include <e32std.h>

// CONSTANTS

/******************************************************************************
* Telephony Comms Information API UID
*******************************************************************************/
const TUid KPSUidTelComms = {0x102029A6};

/**
* Dataport to be used for video calls.
* Data encoding: <dataport name><delimiter><port number>
* where <dataport name> = string literal
*       <delimiter> = double colon
*       <port number> = integer value
*
* @type RProperty::EText
*/
const TUint32 KTelCommsDataport = 0x00000001;

#endif      // TELCOMMSINFOPSKEYS_H

// End of file