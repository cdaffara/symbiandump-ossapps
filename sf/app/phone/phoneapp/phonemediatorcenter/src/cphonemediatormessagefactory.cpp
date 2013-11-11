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
* Description: Implementation of CPhoneMediatorMessageFactory class.
*
*/


// INCLUDE FILES
#include "pevirtualengine.h"
#include "phoneui.pan"
#include "cphonemediatormessagefactory.h"
#include "cphonedataportmessage.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneMediatorMessageFactory::CPhoneMediatorMessageFactory
// ---------------------------------------------------------
//
CPhoneMediatorMessageFactory::CPhoneMediatorMessageFactory( MPEEngineInfo& aEngineInfo ) : 
    iEngineInfo( aEngineInfo )
    {
    }

// ---------------------------------------------------------
// CPhoneMediatorMessageFactory::ConstructL
// ---------------------------------------------------------
//
void CPhoneMediatorMessageFactory::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneMediatorMessageFactory::NewL
// ---------------------------------------------------------
//
CPhoneMediatorMessageFactory* CPhoneMediatorMessageFactory::NewL( MPEEngineInfo& aEngineInfo )
    {
    CPhoneMediatorMessageFactory* self = new( ELeave ) CPhoneMediatorMessageFactory( aEngineInfo );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneMediatorMessageFactory::~CPhoneMediatorMessageFactory
// ---------------------------------------------------------
//
CPhoneMediatorMessageFactory::~CPhoneMediatorMessageFactory()
    {
    }

// ---------------------------------------------------------
// CPhoneMediatorMessageFactory::CreateMessageL
// ---------------------------------------------------------
//
MPhoneMediatorMessage* CPhoneMediatorMessageFactory::CreateMessageL( 
    const TInt aMessage, TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorMessageFactory::CreateMessageL( ) ");

    MPhoneMediatorMessage* message = NULL;

    switch( aMessage )
        {
        case MEngineMonitor::EPEMessageDataPortLoaned:
            message = CPhoneDataPortMessage::NewL( iEngineInfo );
            break;
            
        default:
            break;
        }
            
    return message;
    }

//  End of File  
