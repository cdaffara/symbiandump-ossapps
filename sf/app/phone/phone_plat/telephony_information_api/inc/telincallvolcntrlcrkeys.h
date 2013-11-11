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
* Description:  Telephony in-call volume control central repository keys
*
*/


#ifndef TELINCALLVOLCNTRLCRKEYS_H
#define TELINCALLVOLCNTRLCRKEYS_H

#include <e32std.h>

/******************************************************************************
* Telephony In-Call Volume Control API
* This API provides information related to in-call volume.
******************************************************************************/
const TUid KCRUidInCallVolume = {0x102828B1}; 

/**
* Volume for non-IHF (loudspeaker) mode. 
* Clients must use only integer values between 0 and 10.
* Change affects to in-call volume.
* Default value is 4.  
*/
const TUint32 KTelIncallEarVolume                           = 0x00000001;

/**
* Volume for IHF mode. 
* Clients must use only integer values between 0 and 10.
* Change affects to in-call volume.
* Default value is 4.
*/
const TUint32 KTelIncallLoudspeakerVolume                   = 0x00000002;

#endif      // TELINCALLVOLCNTRLCRKEYS_H

// End of file