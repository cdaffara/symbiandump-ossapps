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
* Description: Implementation of CPhoneStateHandle class.
*
*/


// INCLUDE FILES
#include <pevirtualengine.h>

#include "phoneui.pan"
#include "phoneconstants.h"
#include "cphonestatehandle.h"
#include "mphonestate.h"
#include "phonelogger.h"
#include "cphoneerrormessageshandler.h"

// Constant defines state singleton priority which must be lower
// than KUidNoteControllerSingleton priority(which is default 100) 
// this ensures that NoteControl is destructed before iFactoryLibrary 
// is closed.
const TInt SingletonDestructionPriority(99);

// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CPhoneStateHandle::CPhoneStateHandle
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneStateHandle::CPhoneStateHandle(
    MPhoneViewCommandHandle* aViewCommandHandle) : 
    CCoeStatic( KUidStateHandleSingleton, SingletonDestructionPriority ),
    iViewCommandHandle( aViewCommandHandle )
    {
    }

// Destructor
CPhoneStateHandle::~CPhoneStateHandle()
    {   
    if ( iFactoryLibrary.Handle() )
        {
        delete iPhoneErrorMessagesHandler; 
        delete iPhoneResourceResolver;
        delete iPhoneStateMachine;
        delete iStateMachineFactory;
        iFactoryLibrary.Close();
        }
    }

// ---------------------------------------------------------
// CPhoneStateHandle::CreateL
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPhoneStateHandle* CPhoneStateHandle::CreateL( 
    MPhoneViewCommandHandle* aViewCommandHandle,
    const TDesC& aFileName, 
    const TUid aFactoryUid )
    {
    CPhoneStateHandle* instance = static_cast<CPhoneStateHandle*>(
        CCoeEnv::Static( KUidStateHandleSingleton ) );
    if ( !instance )
        {
        instance = new( ELeave ) CPhoneStateHandle( aViewCommandHandle );
        CleanupStack::PushL( instance );
        instance->ConstructL( aFileName, aFactoryUid );
        CleanupStack::Pop( instance );
        }
    return instance;
    }

// ---------------------------------------------------------
// CPhoneStateHandle::InstanceL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneStateHandle* CPhoneStateHandle::Instance()
    {
    CPhoneStateHandle* instance = static_cast<CPhoneStateHandle*>(
        CCoeEnv::Static( KUidStateHandleSingleton ) );
    __ASSERT_DEBUG( instance, Panic( EPhoneCtrlSingletonNotInitialized ) );
    return instance;
    }

// ---------------------------------------------------------
// CPhoneStateHandle::StateMachine
// ---------------------------------------------------------
//
EXPORT_C MPhoneStateMachine* CPhoneStateHandle::StateMachine()
    {   
    __ASSERT_DEBUG( Instance(), Panic( EPhoneCtrlSingletonNotInitialized ) );
    return iPhoneStateMachine;
    }

// ---------------------------------------------------------
// CPhoneStateHandle::ViewCommandHandle
// ---------------------------------------------------------
//
EXPORT_C MPhoneViewCommandHandle* CPhoneStateHandle::ViewCommandHandle()
    {
    return iViewCommandHandle;        
    }
    
// -----------------------------------------------------------------------------
// CPhoneStateHandle::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneStateHandle::ConstructL( 
    const TDesC& aFileName,
    const TUid aFactoryUid )
    {
    TFileName fileName( KDriveZ );
    fileName.Append( KDC_SHARED_LIB_DIR );
    fileName.Append( aFileName );   
    LoadLibraryHandleL( fileName, aFactoryUid );
    }

// ---------------------------------------------------------
// CPhoneStateHandle::LoadLibraryHandleL
// ---------------------------------------------------------
void CPhoneStateHandle::LoadLibraryHandleL( 
    const TDesC& aFileName,
    const TUid aFactoryUid )
    {

    TBool factoryFound = EFalse;
    TParse fullentry;
    fullentry.Set( aFileName, NULL, NULL );

    if ( iFactoryLibrary.Load( fullentry.Name(), fullentry.DriveAndPath() ) 
        == KErrNone )
        {
        if ( iFactoryLibrary.Type()[1] == aFactoryUid )
            {
            factoryFound = ETrue;
            }
        else
            {
            iFactoryLibrary.Close();
            }
        }

    // If Factory not found:
    if ( !factoryFound )
        {
        Panic( EPhoneCtrlFactoryLibraryNotFound );
        }
    // Look for the 1st exported function
    iEntry = iFactoryLibrary.Lookup( KPhoneUiStateMachineOrdinal ); 

    // Create the state machine factory
    iStateMachineFactory = (CPhoneUIStateMachineFactoryBase*) iEntry();

    // Create the state machine
    iPhoneStateMachine = iStateMachineFactory->CreatePhoneStateMachineL(
        iViewCommandHandle );

    // Create the phone resource resolver
    iPhoneResourceResolver = 
        iStateMachineFactory->CreatePhoneResourceResolverL();
    
    // Create the phone error messages handler    
    iPhoneErrorMessagesHandler = 
        iStateMachineFactory->CreatePhoneErrorMessagesHandlerL( 
                            iViewCommandHandle, iPhoneStateMachine );
    }

// End of File
