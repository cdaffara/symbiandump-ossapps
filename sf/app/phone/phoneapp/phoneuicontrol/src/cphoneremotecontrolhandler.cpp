/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CPhoneRemoteControlHandler implementation.
*
*/


// INCLUDE FILES
#include <PhoneHandler.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include "cphoneremotecontrolhandler.h"
#include "phoneconstants.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

CPhoneRemoteControlHandler::CPhoneRemoteControlHandler( 
    MPhoneStateMachine* aStateMachine ) :
    iStateMachine( aStateMachine ) 
    {
    }

// -----------------------------------------------------------
// CPhoneRemoteControlHandler::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneRemoteControlHandler::ConstructL()
    {
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    
    iPhoneHandler = CPhoneHandler::NewL( *iInterfaceSelector );
    
    iInterfaceSelector->OpenTargetL();

    iButtonRepeatTimer = CPeriodic::NewL( CActive::EPriorityHigh );
    }

// -----------------------------------------------------------
// CPhoneRemoteControlHandler::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneRemoteControlHandler* CPhoneRemoteControlHandler::NewL(
    MPhoneStateMachine* aStateMachine )
    {
    CPhoneRemoteControlHandler* self = 
        new (ELeave) CPhoneRemoteControlHandler( aStateMachine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneRemoteControlHandler::~CPhoneRemoteControlHandler
// -----------------------------------------------------------------------------
//
CPhoneRemoteControlHandler::~CPhoneRemoteControlHandler()
    {
    delete iInterfaceSelector; // it intern deletes iCoreTarget 
    
    if(iButtonRepeatTimer)
        {
        if( iButtonRepeatTimer->IsActive() ) 
            {
            iButtonRepeatTimer->Cancel(); 
            }
        delete iButtonRepeatTimer;
        }
    
    delete iPhoneHandler;
    }
    
// -----------------------------------------------------------------------------
// CPhoneRemoteControlHandler::MrccatoCommand
//
// A command has been received. 
// -----------------------------------------------------------------------------
//
void CPhoneRemoteControlHandler::MrccatoCommand(
    TRemConCoreApiOperationId aOperationId, 
    TRemConCoreApiButtonAction aButtonAct )
    {
    __PHONELOG2( EBasic, EPhoneControl, "CPhoneRemoteControlHandler::MrccatoCommand - aOperationId(%d) aButtonAct(%d)",
        aOperationId, aButtonAct );
    // Save the current command parameters
    iOperationId = aOperationId;
    iButtonAct = aButtonAct;
    
    TBool handled = EFalse;

    // Act on the button
    switch ( aButtonAct )
        {
        case ERemConCoreApiButtonClick:
            {
            // Pass the remote control operation to the current state
            TRAP_IGNORE( handled = 
                iStateMachine->State()->
                    HandleRemConCommandL( aOperationId, aButtonAct ) );
            break;            
            }
        case ERemConCoreApiButtonPress:
            {
            // Pass the remote control operation to the current state
            TRAP_IGNORE( handled = 
                iStateMachine->State()->
                    HandleRemConCommandL( aOperationId, aButtonAct ) );
            // Start the button repeat timer
            if( iButtonRepeatTimer->IsActive() ) 
                {
                iButtonRepeatTimer->Cancel();
                }
            iButtonRepeatTimer->Start(
                KPhoneButtonRepeatDelay,
                KPhoneButtonRepeatDelay,
                TCallBack( DoHandleButtonRepeat, this )  );
            break;
            }
        case ERemConCoreApiButtonRelease:
            {
            // Cancel the button repeat timer
            if( iButtonRepeatTimer->IsActive() ) 
                {
                iButtonRepeatTimer->Cancel();
                }
            break;
            }
        default:
            break;
        }
        
    // send a response if the operation was handled
    if ( handled )
        {        
        TRequestStatus status;
        
        switch ( aOperationId )
            {
            case ERemConCoreApiVolumeUp:
                iCoreTarget->VolumeDownResponse( status, KErrNone );
                User::WaitForRequest( status );
                break;
                
            case ERemConCoreApiVolumeDown:
                iCoreTarget->VolumeUpResponse( status, KErrNone );
                User::WaitForRequest( status );
                break;

            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRemoteControlHandler::DoHandleButtonRepeat
// -----------------------------------------------------------------------------    
//
TInt CPhoneRemoteControlHandler::DoHandleButtonRepeat( TAny* aAny )
    {
    CPhoneRemoteControlHandler* self = 
        reinterpret_cast<CPhoneRemoteControlHandler*>( aAny );

    __PHONELOG2( EBasic, EPhoneControl, "CPhoneRemoteControlHandler::DoHandleButtonRepeat iOperationId(%d) iButtonAct(%d)",
        self->iOperationId, self->iButtonAct );
    // Pass the remote control operation to the current state
    TRAP_IGNORE( self->iStateMachine->State()->
                    HandleRemConCommandL( self->iOperationId, 
                                          self->iButtonAct ) );

    return KErrNone;
    }

//  End of File  
