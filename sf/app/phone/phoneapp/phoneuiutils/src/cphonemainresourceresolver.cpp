/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneMainResourceResolver class.
*
*/


// INCLUDE FILES
#include <e32uid.h>
#include "cphonemainresourceresolver.h"

#include "phoneui.pan"
#include "phoneconstants.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPhoneMainResourceResolver::CPhoneMainResourceResolver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneMainResourceResolver::CPhoneMainResourceResolver() :
    CCoeStatic( KUidMainResourceResolverSingleton, EThread )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneMainResourceResolver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhoneMainResourceResolver* CPhoneMainResourceResolver::NewL()
    {
    CPhoneMainResourceResolver* self = 
        new( ELeave ) CPhoneMainResourceResolver();

    return self;
    }

// Destructor
CPhoneMainResourceResolver::~CPhoneMainResourceResolver()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneMainResourceResolver::ResolveResourceID
// Resolve resource id of given resource.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPhoneMainResourceResolver::ResolveResourceID( 
    const TInt& aResource ) const
    {
    return iResolver->ResolveResourceID( aResource );
    }

// -----------------------------------------------------------------------------
// CPhoneMainResourceResolver::RegisterResolverL
// Register given resolver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneMainResourceResolver::RegisterResolver( 
    MPhoneResourceResolver* aResolver )
    {
    if ( !iResolver )
        {
        iResolver = aResolver;
        }
    }

// ---------------------------------------------------------
// CPhoneMainResourceResolver::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneMainResourceResolver* CPhoneMainResourceResolver::Instance()
    {
    CPhoneMainResourceResolver* instance = 
        static_cast<CPhoneMainResourceResolver*>( 
        CCoeEnv::Static ( KUidMainResourceResolverSingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneMainResourceResolver::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

//  End of File  
