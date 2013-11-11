/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony engine TLS container.
*
*/



// INCLUDE FILES
#include    "CVtEngCRProxy.h"
#include    "CVtEngAccessoryHandler.h"
#include    "CVtEngSATClient.h"
#include    "VtEngUtils.h"
#include    "VtEngPanic.h"
#include    "CVtEngInitializer.h"
#include    "CVtEngSettings.h"
#include    "CVtEngDeviceLockMonitor.h"

#include    "CVtEngExtensions.h"
#include    "cvtengmdtrmessagelistener.h"
#include    "cvtengmdtrcommandsender.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngUtility::CVtEngUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngUtility::CVtEngUtility()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngUtility::ConstructL( )
    {
    __ASSERT_DEBUG( Dll::Tls() == NULL, Panic( EVtEngPanicTlsPanic ));
    Dll::SetTls( this );
    iCRProxy = CVtEngCRProxy::NewL();
    iAccessoryHandler = CVtEngAccessoryHandler::NewL();
    iSettings = CVtEngSettings::NewL();
    
    iLockMonitor = CVtEngDeviceLockMonitor::NewL();  
    iMdtrCommandSender = CVtEngMdtrCommandSender::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngUtility* CVtEngUtility::NewL()
    {
    CVtEngUtility* self = new( ELeave ) CVtEngUtility;

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();

    return self;
    }


// Destructor
CVtEngUtility::~CVtEngUtility()
    {
    delete iMdtrMessageListener;
    delete iMdtrCommandSender;            
    delete iCRProxy;
	delete iAccessoryHandler;
    delete iEngineInit;
    delete iSettings;
    delete iSATClient;
    delete iLockMonitor;  
    Dll::SetTls( NULL );
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::EngineUtils
// Return pointer to Engine Utility.
//
// -----------------------------------------------------------------------------
//
CVtEngUtility* CVtEngUtility::EngineUtils()
    {
    return reinterpret_cast<CVtEngUtility*>( Dll::Tls() );
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::StateManager
// Returns state manager.
//
// -----------------------------------------------------------------------------
//
CVtEngStateManager* CVtEngUtility::StateManager()
    {
    return *(CVtEngUtility::EngineUtils()->iStateManager);
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::EventManager
// -----------------------------------------------------------------------------
//
CVtEngEventManager& CVtEngUtility::EventManager()
    {
    return **CVtEngUtility::EngineUtils()->iEventManager;
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::PrepareL
// -----------------------------------------------------------------------------
//
void CVtEngUtility::PrepareL(
    CVtEngHandlerContainer& aHandlers,
    CVtEngStateManager*& aStateManagerPtr,
    CVtEngEventManager*& aEventManagerPtr,
    CVtEngExtensions* aExtensionPtr )
    {
    iEngineInit = CVtEngInitializer::NewL( aHandlers );
    iStateManager = &aStateManagerPtr;
    iEventManager = &aEventManagerPtr;
    iExtensions = aExtensionPtr;
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::StartMediatorListenerL    
// Creates mediator message (event/command) listener. Cannot be done in
// constructor because session control may be unavailable when we would
// receive first mediator event => session state is still uninitialized.
// -----------------------------------------------------------------------------
//
void CVtEngUtility::StartMediatorListenerL()
    {
    iMdtrMessageListener = CVtEngMdtrMessageListener::NewL( *this );
    }


// -----------------------------------------------------------------------------
// CVtEngUtility::Settings
// -----------------------------------------------------------------------------
//
CVtEngSettings& CVtEngUtility::Settings()
    {
    return *CVtEngUtility::EngineUtils()->iSettings;
    }
    
// -----------------------------------------------------------------------------
// CVtEngUtility::MediatorCommands
// -----------------------------------------------------------------------------
//
CVtEngMdtrCommandSender& CVtEngUtility::MediatorCommands()    
    {
    return *CVtEngUtility::EngineUtils()->iMdtrCommandSender;
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::CRProxy
// -----------------------------------------------------------------------------
//
CVtEngCRProxy& CVtEngUtility::CRProxy()
    {
    return *CVtEngUtility::EngineUtils()->iCRProxy;
	}

// -----------------------------------------------------------------------------
// CVtEngUtility::AccessoryHandler
// -----------------------------------------------------------------------------
//
CVtEngAccessoryHandler& CVtEngUtility::AccessoryHandler()
    {
    return *CVtEngUtility::EngineUtils()->iAccessoryHandler;
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::StartSATListenL
// -----------------------------------------------------------------------------
//
void CVtEngUtility::StartSATListenL()
    {
    iSATClient = CVtEngSATClient::NewL();    
    }

// -----------------------------------------------------------------------------
// CVtEngUtility::NotifyLockState
// -----------------------------------------------------------------------------
//
void CVtEngUtility::NotifyLockState()
    {
    iLockMonitor->NotifyState();
    }
    
// -----------------------------------------------------------------------------
// CVtEngUtility::Extensions
// -----------------------------------------------------------------------------
//
CVtEngExtensions* CVtEngUtility::Extensions()
    {   
    return CVtEngUtility::EngineUtils()->iExtensions;
    }    

//  End of File
