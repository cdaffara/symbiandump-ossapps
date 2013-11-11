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
* Description:  Message store address client implementation.
*
*/



//<cmail>
#include "MsgStoreAddress.h"
//</cmail>

// --------------------------
// CLASSES
// --------------------------

// --------------------------
// RMsgStoreAddress
// --------------------------
EXPORT_C void RMsgStoreAddress::Close()
    {
    iEmailAddress.Close();
    iDisplayName.Close();
    }



// --------------------------
// RMsgStoreAddress
// --------------------------
// ==========================================================================
// FUNCTION: SetMRUAddressListL
// ==========================================================================
EXPORT_C CMsgStoreAddress* CMsgStoreAddress::NewL( const TDesC& aEmailAddress, const TDesC& aDisplayName )
    {
    CMsgStoreAddress* self = new(ELeave) CMsgStoreAddress();
    CleanupStack::PushL( self );
    self->ConstructL( aEmailAddress, aDisplayName );
    CleanupStack::Pop( self );
    return self;
    }

// ==========================================================================
// FUNCTION: CMsgStoreAddress
// ==========================================================================
CMsgStoreAddress::CMsgStoreAddress()
    {
    //do nothing
    }

// ==========================================================================
// FUNCTION: EmailAddress
// ==========================================================================
EXPORT_C const TDesC& CMsgStoreAddress::EmailAddress()
    {
    return iEmailAddress;
    }
        
// ==========================================================================
// FUNCTION: DisplayName
// ==========================================================================
EXPORT_C const TDesC& CMsgStoreAddress::DisplayName()
    {
    return iDisplayName;
    }

// ==========================================================================
// FUNCTION: ~CMsgStoreAddress
// ==========================================================================
EXPORT_C CMsgStoreAddress::~CMsgStoreAddress()
    {
    iDisplayName.Close();
    iEmailAddress.Close();
    }
        
// ==========================================================================
// FUNCTION: ~CMsgStoreAddress
// ==========================================================================
void CMsgStoreAddress::ConstructL( const TDesC& aEmailAddress, const TDesC& aDisplayName )
    {
    iEmailAddress.CreateL( aEmailAddress );
    iDisplayName.CreateL( aDisplayName );
    }
