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
* Description: Implementation of CPhoneMainErrorMessagesHandler class.
*
*/


// INCLUDE FILES
#include <e32uid.h>
#include <pevirtualengine.h>
#include "cphonemainerrormessageshandler.h"

#include "phoneui.pan"
#include "phoneconstants.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPhoneMainErrorMessagesHandler::CPhoneMainErrorMessagesHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneMainErrorMessagesHandler::CPhoneMainErrorMessagesHandler() :
    CCoeStatic( KUidMainErrorMessagesHandlerSingleton, EThread )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneMainErrorMessagesHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhoneMainErrorMessagesHandler* CPhoneMainErrorMessagesHandler::NewL()
    {
    CPhoneMainErrorMessagesHandler* self = 
        new( ELeave ) CPhoneMainErrorMessagesHandler();

    return self;
    }

// Destructor
CPhoneMainErrorMessagesHandler::~CPhoneMainErrorMessagesHandler()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneMainErrorMessagesHandler::ResolveResourceID
// Resolve resource id of given resource.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneMainErrorMessagesHandler::ShowErrorSpecificNoteL( 
    const TPEErrorInfo& aErrorInfo )
    {
    return iHandler->ShowErrorSpecificNoteL( aErrorInfo );
    }

// -----------------------------------------------------------------------------
// CPhoneMainErrorMessagesHandler::RegisterResolverL
// Register given resolver
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneMainErrorMessagesHandler::RegisterErrorMessagesHandler( 
    MPhoneErrorMessagesHandler* aHandler )
    {
    if ( !iHandler )
        {
        iHandler = aHandler;
        }
    }

// ---------------------------------------------------------
// CPhoneMainErrorMessagesHandler::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneMainErrorMessagesHandler* CPhoneMainErrorMessagesHandler::Instance()
    {
    CPhoneMainErrorMessagesHandler* instance = 
        static_cast<CPhoneMainErrorMessagesHandler*>( 
        CCoeEnv::Static ( KUidMainErrorMessagesHandlerSingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneMainErrorMessagesHandler::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

//  End of File  
