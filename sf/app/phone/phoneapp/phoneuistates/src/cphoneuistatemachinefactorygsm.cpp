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
* Description: Implementation of CPhoneUIStateMachineFactoryGSM class.
*
*/


// INCLUDES
#include "cphoneuistatemachinefactorygsm.h"
#include "cphonestatemachinegsm.h"
#include "cphoneresourceresolvergsm.h"
#include "cphoneerrormessageshandler.h"

// -----------------------------------------------------------
// CPhoneUIControllerFactoryGSM::CreatePhoneStateMachineL()
// Create instance of CPhoneStateMachine class.
// (other items were commented in a header).
// -----------------------------------------------------------
MPhoneStateMachine* CPhoneUIStateMachineFactoryGSM::CreatePhoneStateMachineL(
    MPhoneViewCommandHandle* aViewCommandHandle
     )
    {
    return CPhoneStateMachineGSM::NewL( aViewCommandHandle );
    }

// ---------------------------------------------------------
// CPhoneUIStateMachineFactoryGSM::CreatePhoneResourceResolverL
// Create instance of CPhoneResourceResolverBase class.
// (other items were commented in a header).
// ---------------------------------------------------------
CPhoneResourceResolverBase*
    CPhoneUIStateMachineFactoryGSM::CreatePhoneResourceResolverL()
    {
    return CPhoneResourceResolverGSM::NewL();
    }

// ---------------------------------------------------------
// CPhoneUIStateMachineFactoryGSM::CreatePhoneResourceResolverL
// Create instance of CPhoneResourceResolverBase class.
// (other items were commented in a header).
// ---------------------------------------------------------
CPhoneErrorMessagesHandler*
    CPhoneUIStateMachineFactoryGSM::CreatePhoneErrorMessagesHandlerL(
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneStateMachine* aStateMachine )
    {
    return CPhoneErrorMessagesHandler::NewL( aViewCommandHandle,
                                             aStateMachine );
    }

// ---------------------------------------------------------
// Polymorphic DLL entry point for ordinal 1.
// ---------------------------------------------------------
EXPORT_C CPhoneUIStateMachineFactoryBase* NewPhoneUIStateMachineFactoryL() 
    {
    return new( ELeave ) CPhoneUIStateMachineFactoryGSM;
    }
    
// End of File
