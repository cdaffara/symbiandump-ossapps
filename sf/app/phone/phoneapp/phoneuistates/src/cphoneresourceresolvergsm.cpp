/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneResourceResolverGSM class.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <featmgr.h>

#include "cphoneresourceresolvergsm.h"
#include "cphonemainresourceresolver.h"
#include "phoneconstants.h"
#include "phonerssgsm.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "phoneresourceids.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneResourceResolverGSM::CPhoneResourceResolverGSM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneResourceResolverGSM::CPhoneResourceResolverGSM(): 
    iEnv( *CEikonEnv::Static() ) // codescanner::performance::eikonenvstatic
    {
    }

// -----------------------------------------------------------------------------
// ourceResolverGSM::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneResourceResolverGSM::ConstructL()
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, "CPhoneResourceResolverGSM::ConstructL");

    // Register resolver
    CPhoneMainResourceResolver::Instance()->RegisterResolver( this );
    }

// -----------------------------------------------------------------------------
// CPhoneResourceResolverGSM::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhoneResourceResolverGSM* CPhoneResourceResolverGSM::NewL()
    {
    CPhoneResourceResolverGSM* self = 
        new( ELeave ) CPhoneResourceResolverGSM();
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }

// Destructor
EXPORT_C CPhoneResourceResolverGSM::~CPhoneResourceResolverGSM()
    {   
    }

// -----------------------------------------------------------------------------
// CPhoneResourceResolverGSM::ResolveResourceID
// Resolve resource id of given resource.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhoneResourceResolverGSM::ResolveResourceID( 
    const TInt& aResource ) const
    {    
    return CPhoneResourceResolverBase::ResolveResourceID( aResource );
    }

//  End of File  
