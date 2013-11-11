/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include <bt_sock.h>

#include "cbtdprserviceadvertiser.h"
#include "btprotocolconsts.h"
#include "cbtdiscover.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtDprServiceAdvertiser* CBtDprServiceAdvertiser::NewL()
    {
    CBtDprServiceAdvertiser* self =
        CBtDprServiceAdvertiser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBtDprServiceAdvertiser* CBtDprServiceAdvertiser::NewLC()
    {
    CBtDprServiceAdvertiser* self = new (
        ELeave ) CBtDprServiceAdvertiser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBtDprServiceAdvertiser::ConstructL()
    {
    iServiceDescription = KServiceDescription().AllocL();
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::CBtDprServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CBtDprServiceAdvertiser::CBtDprServiceAdvertiser()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::~CBtDprServiceAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CBtDprServiceAdvertiser::~CBtDprServiceAdvertiser()
    {

    // Delete private member
    delete iServiceDescription;
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::BuildProtocolDescriptionL()
// Builds the protocol description.
// ----------------------------------------------------------------------------
//
void CBtDprServiceAdvertiser
::BuildProtocolDescriptionL( CSdpAttrValueDES* aProtocolDescriptor,
                            TInt aPort )
    {
    TBuf8<1> channel;
    channel.Append( ( TChar )aPort );

    aProtocolDescriptor
    ->StartListL()   //  List of protocols required for this method
        ->BuildDESL()
        ->StartListL()   //  Details of lowest level protocol
            ->BuildUUIDL( KL2CAP )
        ->EndListL()

        ->BuildDESL()
        ->StartListL()
            ->BuildUUIDL( KRFCOMM )
            ->BuildUintL( channel )
        ->EndListL()

        ->BuildDESL()
        ->StartListL()
            ->BuildUUIDL( KBTSDPObex )
        ->EndListL()

        ->BuildDESL()
        ->StartListL()
            ->BuildUUIDL( KBTSDPDPROService )
        ->EndListL()
    ->EndListL();

    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::ServiceName()
// ----------------------------------------------------------------------------
//
const TDesC& CBtDprServiceAdvertiser::ServiceName()
    {
    return KServiceName;
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::ServiceDescription()
// ----------------------------------------------------------------------------
//
const TDesC& CBtDprServiceAdvertiser::ServiceDescription()
    {
     return *iServiceDescription;
    }

// ----------------------------------------------------------------------------
// CBtDprServiceAdvertiser::ServiceClass()
// ----------------------------------------------------------------------------
//
TUint CBtDprServiceAdvertiser::ServiceClass()
    {
    return KServiceClass;
    }

// End of File
