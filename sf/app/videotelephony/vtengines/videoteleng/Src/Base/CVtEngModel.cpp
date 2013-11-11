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
* Description:  Video telephony engine class implementation.
*
*/



// INCLUDE FILES
#include    "CVtEngModel.h"
#include    "CVtEngHandlerContainer.h"
#include    "CVtEngCommandHandler.h"
#include    "CVtEngEventManager.h"
#include    "CVtEngStateManager.h"
#include    "VtEngUtils.h"
#include    "VtEngPanic.h"
#include    <cvtlogger.h>
#include    "CVtEngExtensions.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngModel::CVtEngModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngModel::CVtEngModel()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngModel::ConstructL(
            MVtEngEventObserver& aEventObserver,
            MVtEngCommandObserver& aCommandObserver )
    {
    VTLOGINIT
  __VTPRINTENTER( "Model.ConstructL" )

    __VTPRINT( DEBUG_GEN | DEBUG_CONSTRUCT, "Model.ConL" )
    iUtils = CVtEngUtility::NewL( );
    iHandlers = CVtEngHandlerContainer::NewL( );

    // Create extensions
    iExtensions = CVtEngExtensions::NewL();

    iUtils->PrepareL(
        *iHandlers,
        iStateManager,
        iEventManager,
        iExtensions );
    iCommandHandler = CVtEngCommandHandler::NewL( *iHandlers );
    iCommandHandler->AddObserverL( aCommandObserver );
    iEventManager = CVtEngEventManager::NewL( aEventObserver );
    iStateManager = CVtEngStateManager::NewL( *iHandlers, *iEventManager );
    __VTPRINTEXIT( "Model.ConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVtEngModel* CVtEngModel::NewL(
            MVtEngEventObserver& aEventObserver,
            MVtEngCommandObserver& aCommandObserver)
    {
    CVtEngModel* self = new( ELeave ) CVtEngModel;

    CleanupStack::PushL( self );
    self->ConstructL( aEventObserver, aCommandObserver );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CVtEngModel::~CVtEngModel()
    {
    __VTPRINTENTER( "Model.~" )
    delete iStateManager;
    delete iCommandHandler;
    delete iHandlers;
    delete iUtils;
    delete iEventManager;
    delete iExtensions;
    __VTPRINTEXIT( "Model.~" )
    VTLOGUNINIT
    }

// -----------------------------------------------------------------------------
// CVtEngModel::CommandHandler
// Returns command handler reference.
// -----------------------------------------------------------------------------
//
EXPORT_C MVtEngCommandHandler& CVtEngModel::CommandHandler() const
    {
    return *iCommandHandler;
    }

// -----------------------------------------------------------------------------
// CVtEngModel::Session
// Returns session reference.
// -----------------------------------------------------------------------------
//
EXPORT_C MVtEngSessionInfo& CVtEngModel::Session() const
    {
    return iHandlers->Session();
    }

// -----------------------------------------------------------------------------
// CVtEngModel::Media
// Returns media reference.
//
// -----------------------------------------------------------------------------
//
EXPORT_C MVtEngMedia& CVtEngModel::Media() const
    {
    return iHandlers->Media();
    }

// -----------------------------------------------------------------------------
// CVtEngModel::Audio
// Returns audio reference.
// -----------------------------------------------------------------------------
//
EXPORT_C MVtEngAudio& CVtEngModel::Audio() const
    {
    return iHandlers->Audio();
    }

// -----------------------------------------------------------------------------
// CVtEngModel::Extension
// Returns extension pointer (currently none)
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CVtEngModel::Extension( const TUid& aExtensionUid )
    {
    __VTPRINTENTER( "Model.Extension" )

    // init local variables
    TInt error = KErrNone;
    TAny* interfacePtr = NULL;
    
    // get mediahandler handle
    MVtEngMedia& msession = iHandlers->Media();
    CVtEngMediaHandler& handler = static_cast<CVtEngMediaHandler&>( msession );
    
    // get API IF from extension
    error = iExtensions->GetClientInterface( &interfacePtr, aExtensionUid ,
    	handler );
    
    // in case of error	
    if ( error != KErrNone )
        {
        __VTPRINTEXITR( "Model.Extension GetClientInterface failed %d", error)
        return NULL;
        }

    __VTPRINTEXIT( "Model.Extension" )
    return interfacePtr;
    }

// Engine's internal panic function
void Panic( TVtEngPanic aPanic )
    {
    __VTPRINT2( DEBUG_GEN, "EnginePanic %d", aPanic)
    User::Panic( KVtEngPanicCategory, aPanic );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
