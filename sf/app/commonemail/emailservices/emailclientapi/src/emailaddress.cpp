/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CEmailAddress.
*
*/

#include "emailaddress.h"
#include "CFSMailClient.h"
#include "emailclientapi.hrh"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailAddress* CEmailAddress::NewL( const TRole aRole, const TDataOwner aOwner )
    {
    CEmailAddress* self = new ( ELeave ) CEmailAddress( aRole, aOwner );

    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailAddress* CEmailAddress::NewLC( const TRole aRole, const TDataOwner aOwner )
    {
    CEmailAddress* self = CEmailAddress::NewL( aRole, aOwner );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailAddress::~CEmailAddress()
    {
    iAddress.Close();
    iDisplayName.Close();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailAddress::CEmailAddress( const TRole aRole, const TDataOwner aOwner ) :
    iRole( aRole ), 
    iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailAddress::InterfaceId() const
    {
    return KEmailIFUidAddress;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailAddress::Release()
    {
    if ( iOwner == EClientOwns )
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailAddress::SetAddressL( const TDesC& aAddress )
    {
    iAddress.Close();
    iAddress.CreateL( aAddress );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailAddress::Address() const
    {    
    return iAddress;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailAddress::SetDisplayNameL( const TDesC& aDisplayName )
    {
    iDisplayName.Close();
    iDisplayName.CreateL( aDisplayName );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailAddress::DisplayName() const
    {
    return iDisplayName;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAddress::TRole CEmailAddress::Role() const
    {
    return iRole;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailAddress::SetRole( const TRole aRole )
    {
    iRole = aRole;
    }

// End of file
