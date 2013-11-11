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
* Description:  Data types passed through Publish and Subscribe
*
*/


#ifndef TELINTERNALPSTYPES_H
#define TELINTERNALPSTYPES_H

// INCLUDES
#include <cntdef.h>

// CONSTANTS


// DATA TYPES
typedef TUint8 TCallId;

/**
* Contact information of the remote party
* Note: Size of the data must not exceed RProperty::KMaxPropertySize.
*/
struct TTelRemotePartyContactInfo
    {
    /**
    * Type of remote address
    */
    enum TCLIAddressType
        {
        ENotSet,
        EHome,
        EMobile,
        EWork,
        EPhone,
        EFaxNumber,
        EPager,
        EVideo,
        ESIP,
        EEmergency
        };

    // call identity
    TCallId                 iCallId;

    // Contact item id
#ifdef RD_VIRTUAL_PHONEBOOK
    // Contact Link packed
    TBuf8<255>               iPackedLinks;
#else // RD_VIRTUAL_PHONEBOOK
    // Contact item id
    TContactItemId          iContactItemId;
#endif // RD_VIRTUAL_PHONEBOOK

    // CLI address
    TPhCltTelephoneNumber   iAddress;

    // Address type
    TCLIAddressType         iAddressType;

    typedef TPckg<TTelRemotePartyContactInfo> TInfoPckg;
    };

#endif    // TELINTERNALPSTYPES_H

// End of file