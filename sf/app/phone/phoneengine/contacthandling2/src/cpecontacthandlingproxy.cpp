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
* Description:  This module contains the implementation of CPEContactHandlingProxy class
*
*/

//  INCLUDE FILES
#include "cpecontacthandling.h"
#include "cpecontacthandlingproxy.h"
#include "cpecontacthandlingdummy.h"
#include "cpecontactmatch.h"
#include <mpedatastore.h>
#include <e32std.h>
#include <mpephonemodelinternal.h>
#include <pepanic.pan>
#include <barsc.h> 
#include <barsread.h>
#include <talogger.h>
#include <bldvariant.hrh>
#include <featmgr.h>

// ================= MEMBER FUNCTIONS =======================================
// -----------------------------------------------------------------------------
// CPEContactHandlingProxy::CreateFirstPhaseL
// This method creates the contact handling proxy and dummy implementation
// -----------------------------------------------------------------------------
//
EXPORT_C MPEContactHandling* CPEContactHandlingProxy::CreateFirstPhaseL()
    {
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactHandlingProxy::CreateFirstPhaseL start." );
    CPEContactHandlingProxy* self = new ( ELeave ) CPEContactHandlingProxy();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactHandlingProxy::CreateFirstPhaseL Complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEContactHandlingProxy::CreateSecondPhaseL
// This method creates the actual contact handling implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEContactHandlingProxy::CreateSecondPhaseL(
        MPEPhoneModelInternal& aModel,
        RFs& aFsSession
        )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateSecondPhaseL start." );
    iContactHandling = CPEContactHandling::NewL( aModel, aFsSession );
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateSecondPhaseL complete." );
    }

// -----------------------------------------------------------------------------
// CPEContactHandlingProxy::CPEContactHandlingProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEContactHandlingProxy::CPEContactHandlingProxy()
        {
        }

// -----------------------------------------------------------------------------
// CPEContactHandlingProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEContactHandlingProxy::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPEContactHandlingProxy::ConstructL" );
    // Create an instance of the log handling dummy implementation
    iContactHandlingDummy = CPEContactHandlingDummy::NewL();  
    }

// Destructor
CPEContactHandlingProxy::~CPEContactHandlingProxy()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::~CPEContactHandlingProxy" );
    delete iContactHandling;
    delete iContactHandlingDummy;
    }

// -----------------------------------------------------------------------------
// CPEContactHandlingProxy::FindContactInfoSync
// Calls method FindContactInfoSyncL which can leave. This leave is trapped in 
// this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEContactHandlingProxy::FindContactInfoSync
        ( 
        const TInt aCallId,
        const TPEFindContactKey aFindKey
        )     
    {
    if ( iContactHandling )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactHandlingProxy::FindContactInfoSync > CPEContactHandling::FindContactInfoSync" );
        return iContactHandling->FindContactInfoSync( aCallId, aFindKey );
        }
    else if ( iContactHandlingDummy )
        {
        TEFLOGSTRING( KTAINT, "CNT CPEContactHandlingProxy::FindContactInfoSync > CPEContactHandlingDummy::FindContactInfoSync" );
        return iContactHandlingDummy->FindContactInfoSync( aCallId, aFindKey );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "CNT CPECONTACTHANDLINGPROXY::FINDCONTACTINFOSYNC ! NEITHER CONTACT HANDLING NOR CONTACT HANDLING DUMMY EXISTS" );
        User::Invariant();
        }
    return KErrBadHandle;
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
