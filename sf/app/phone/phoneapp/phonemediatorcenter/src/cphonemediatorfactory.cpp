/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneMediatorFactory class.
*
*/


// INCLUDE FILES
#include "cphonemediatorfactory.h"
#include "phonelogger.h"
#include "phoneconstants.h"
#include "phoneui.pan"
#include "cphonemediatorsender.h"
#include "cphonemediatorcommandlistener.h"
#include "cphonemediatormessagefactory.h"
#include "mphonemenuandcbaevents.h"
#include "mphoneenginemessagesender.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneMediatorFactory::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneMediatorFactory* CPhoneMediatorFactory::Instance()
    {
    CPhoneMediatorFactory* instance = static_cast<CPhoneMediatorFactory*> 
        ( CCoeEnv::Static ( KUidMediatorFactorySingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneMediatorFactory::NewL() );
        if ( err )
            {
            Panic( EPhoneMediatorCenterCouldNotCreateSingleton );   
            }
        }
    return instance;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorFactory::CPhoneMediatorFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneMediatorFactory::CPhoneMediatorFactory() :
    CCoeStatic( KUidMediatorFactorySingleton )
    {
    }

// Destructor
CPhoneMediatorFactory::~CPhoneMediatorFactory()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneMediatorFactory::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorFactory::ConstructL( ) ");
    }

// -----------------------------------------------------------
// CPhoneMediatorFactory::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneMediatorFactory* CPhoneMediatorFactory::NewL()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorFactory::NewL( ) ");
    
    CPhoneMediatorFactory* self = new ( ELeave ) CPhoneMediatorFactory;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorFactory::Sender
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneMediatorSender* CPhoneMediatorFactory::Sender()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorFactory::Sender( ) ");
    return CPhoneMediatorSender::Instance();
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorFactory::CommandListener
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneMediatorCommandListener* CPhoneMediatorFactory::CommandListener( 
    MPhoneMenuAndCbaEvents* aMenuAndCbaEventHandler, 
    MPhoneEngineMessageSender* aMessageSender,
    MPEEngineInfo* aEngineInfo )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorFactory::CommandListener( ) ");
    
    if( aEngineInfo && !iEngineInfo )
        {
        iEngineInfo = aEngineInfo;          
        }
    
    if( aMenuAndCbaEventHandler && aMessageSender )
        {
        iMenuAndCbaEvents = aMenuAndCbaEventHandler;
        iMessageSender = aMessageSender;
        CPhoneMediatorCommandListener::Instance()->Initialize( aMenuAndCbaEventHandler,
            aMessageSender, aEngineInfo );
        }
    else
        {
        // Sanity checks
        __ASSERT_DEBUG( iMenuAndCbaEvents, Panic( EPhoneMediatorCenterParameterNotInitialized ) );
        __ASSERT_DEBUG( iMessageSender, Panic( EPhoneMediatorCenterParameterNotInitialized ) );
        __ASSERT_DEBUG( iEngineInfo, Panic( EPhoneMediatorCenterParameterNotInitialized ) );
        }
    return CPhoneMediatorCommandListener::Instance();
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorFactory::MediatorMessage
// -----------------------------------------------------------------------------
//
EXPORT_C MPhoneMediatorMessage* CPhoneMediatorFactory::MediatorMessage(
    const TInt aMessage, TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorFactory::MediatorMessage( ) ");
    __ASSERT_DEBUG( iEngineInfo, Panic( EPhoneMediatorCenterParameterNotInitialized ) );

    CPhoneMediatorMessageFactory* messageFactory = NULL;
    MPhoneMediatorMessage* message = NULL;
    
    TRAPD( error, messageFactory = CPhoneMediatorMessageFactory::NewL( *iEngineInfo ) );
    if( error == KErrNone )
        {
        TRAP_IGNORE( message = messageFactory->CreateMessageL( aMessage, aCallId ) );
        delete messageFactory;
        }

    return message;
    }

// End of File
