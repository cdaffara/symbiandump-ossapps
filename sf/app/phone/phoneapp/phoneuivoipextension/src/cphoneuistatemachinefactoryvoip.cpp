/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneUIStateMachineFactoryVoIP class.
*
*/


// INCLUDES
#include "cphoneuistatemachinefactoryvoip.h"
#include "cphonestatemachinevoip.h"
#include "cphoneresourceresolvervoip.h"
#include "cphonevoiperrormessageshandler.h"
#include "cphoneerrormessageshandler.h"
#include "mphoneviewcommandhandle.h"

// -----------------------------------------------------------
// CPhoneUIStateMachineFactoryVoIP::CreatePhoneStateMachineL()
// Create instance of CPhoneStateMachine class.
// (other items were commented in a header).
// -----------------------------------------------------------
MPhoneStateMachine* CPhoneUIStateMachineFactoryVoIP::CreatePhoneStateMachineL(
    MPhoneViewCommandHandle* aViewCommandHandle ) 
    {
    return CPhoneStateMachineVoIP::NewL( aViewCommandHandle );
    }

// ---------------------------------------------------------
// CPhoneUIStateMachineFactoryVoIP::CreatePhoneResourceResolverL
// Create instance of CPhoneResourceResolverBase class.
// (other items were commented in a header).
// ---------------------------------------------------------
CPhoneResourceResolverBase*
    CPhoneUIStateMachineFactoryVoIP::CreatePhoneResourceResolverL()
    {
    return CPhoneResourceResolverVoIP::NewL();
    }

// ---------------------------------------------------------
// CPhoneUIStateMachineFactoryVoIP::CreatePhoneErrorMessagesHandlerL
// Create instance of CPhoneErrorMessagerHandler class.
// (other items were commented in a header).
// ---------------------------------------------------------
CPhoneErrorMessagesHandler*
    CPhoneUIStateMachineFactoryVoIP::CreatePhoneErrorMessagesHandlerL( 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneStateMachine* aPhoneStateMachine )
    {
    return CPhoneVoIPErrorMessagesHandler::NewL( aViewCommandHandle, 
                                                 aPhoneStateMachine );
    }

// ---------------------------------------------------------
// Polymorphic DLL entry point for ordinal 1.
// ---------------------------------------------------------
EXPORT_C CPhoneUIStateMachineFactoryBase* NewPhoneUIStateMachineFactoryL() 
    {
    return new( ELeave ) CPhoneUIStateMachineFactoryVoIP;
    }
    
// End of File
