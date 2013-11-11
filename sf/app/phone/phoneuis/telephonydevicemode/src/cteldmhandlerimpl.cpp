/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the CTelDMFactoryImpl class.
*
*/



// INCLUDE FILES
#include    <featmgr.h>
#include    <mphonedevicemodeobserver.h>
#include    <telinternalpskeys.h>
#include    "cteldmhandlerimpl.h"
#include    "cteldmgriphandler.h"
#include    "cteldmcommandhandler.h"
#include    "cteldmcallstatelistener.h"
#include    "cteldmaudioaccessorylistener.h"
#include    "cteldmdebug.h"
 
// MODULE DATA STRUCTURES

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelDMHandlerImpl
// 
// Creates instance of telephony device mode factory.
// -----------------------------------------------------------------------------
//
EXPORT_C CTelDMHandler* CTelDMHandlerCreateL()
    {
    return CTelDMHandlerImpl::NewL();
    }

// -----------------------------------------------------------------------------
// CTelDMHandlerImpl::CTelDMHandlerImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTelDMHandlerImpl::CTelDMHandlerImpl()
    {
    }

// -----------------------------------------------------------------------------
// CTelDMHandlerImpl::~CTelDMHandlerImpl
// -----------------------------------------------------------------------------
//    
CTelDMHandlerImpl::~CTelDMHandlerImpl()
    {
    delete iCallStateListener;
    delete iAccessory;
    delete iCommandHandler;
    delete iGrip;
    FLOG( _L( "CTelDMHandlerImpl::~CTelDMHandlerImpl()" ) )
    }

// -----------------------------------------------------------------------------
// CTelDMHandlerImpl::NewL
// 
// -----------------------------------------------------------------------------
//
CTelDMHandlerImpl* CTelDMHandlerImpl::NewL()
    {
    CTelDMHandlerImpl* self = new( ELeave ) CTelDMHandlerImpl;
    return self;
    }

// -----------------------------------------------------------------------------
// CTelDMHandlerImpl::SetObserverL
// 
// -----------------------------------------------------------------------------
//
void CTelDMHandlerImpl::SetObserverL( MPhoneDeviceModeObserver* aCallHandler )
    {
    // Default
    RProperty::Set( 
        KPSUidTelAudioPreference,
        KTelAudioOutput,
        EPSAudioPrivate );                
                        
    if ( !FeatureManager::FeatureSupported( KFeatureIdKeypadNoSlider ) )
        {
        iCallStateListener = CTelDMCallStateListener::NewL();
        iAccessory = CTelDMAudioAccessoryListener::NewL();
        // Disconnects and answers to calls 
        iCommandHandler = CTelDMCommandHandler::NewL( 
                                            *aCallHandler, 
                                            *iAccessory );

        iCallStateListener->AddObserverL( *iCommandHandler );
        
        iGrip = CTelDMGripHandler::NewL( *iCommandHandler );

        }
    }

//  End of File


