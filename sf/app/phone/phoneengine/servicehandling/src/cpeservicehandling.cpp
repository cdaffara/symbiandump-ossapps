/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPEServiceHandling class
*
*/


#include <mpephonemodelinternal.h>
#include <mpedatastore.h>
#include <pevirtualengine.h>
#include <e32debug.h>

#include "talogger.h"
#include "cpeservicehandling.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPEServiceHandling::CPEServiceHandling( MPEPhoneModelInternal& aModel )
    : iModel( aModel )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEServiceHandling::CPEServiceHandling" );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CPEServiceHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEServiceHandling::ConstructL" );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CPEServiceHandling* CPEServiceHandling::NewL( MPEPhoneModelInternal& aModel )
    {
    CPEServiceHandling* self = CPEServiceHandling::NewLC(aModel);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CPEServiceHandling* CPEServiceHandling::NewLC( MPEPhoneModelInternal& aModel )
    {
    CPEServiceHandling* self = new( ELeave ) CPEServiceHandling( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CPEServiceHandling::~CPEServiceHandling()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEServiceHandling::~CPEServiceHandling" );
    }

// ---------------------------------------------------------------------------
// CPEServiceHandling::EnableService
// ---------------------------------------------------------------------------
//
void CPEServiceHandling::EnableServiceL( TInt /*aServiceId*/ )
    {
    TEFLOGSTRING( KTAREQIN, "PE CPEServiceHandling::EnableServiceL" );
    User::LeaveIfError( 0 );
    iModel.SendMessage( MEngineMonitor::EPEMessageServiceEnabled );
    }
    
// ---------------------------------------------------------------------------
// CPEServiceHandling::CancelServiceEnabling
// ---------------------------------------------------------------------------
//
void CPEServiceHandling::CancelServiceEnabling() const
    {
    TEFLOGSTRING( KTAREQIN, "PE CPEServiceHandling::CancelServiceEnabling" );
    }
        
// -----------------------------------------------------------------------------
// CPEServiceHandling::DisableService
// -----------------------------------------------------------------------------
//
void CPEServiceHandling::DisableService() const
    {
    TEFLOGSTRING( KTAREQIN, "PE CPEServiceHandling::DisableService" );
    } 

//  End of File
