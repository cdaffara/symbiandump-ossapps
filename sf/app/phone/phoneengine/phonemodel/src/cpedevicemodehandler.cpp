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
* Description:  Device mode handler
*
*/



// INCLUDE FILES
#include "cpedevicemodehandler.h"
#include "cpemessagehandler.h"
#include "mpecallhandling.h"
#include <talogger.h>
#include <cteldmhandler.h>

// CONSTANTS

_LIT( KPhoneTelDevModeLoadedLib, "telephonydevicemode.dll" );
const TInt KPhoneTelDevModeOrdinal = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEDeviceModeHandler::CPEDeviceModeHandler
// Constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEDeviceModeHandler::CPEDeviceModeHandler(
    MPECallHandling& aCallHandling,
    CPEMessageHandler& aMessageHandler
)
    : iCallHandling( aCallHandling ),
      iMessageHandler ( aMessageHandler)
    {
    }

// -----------------------------------------------------------------------------
// CPEDeviceModeHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEDeviceModeHandler::ConstructL()
    {
    LoadLibraryL();
    }

// -----------------------------------------------------------------------------
// CPEDeviceModeHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEDeviceModeHandler* CPEDeviceModeHandler::NewL(
    MPECallHandling& aCallHandling,
    CPEMessageHandler& aMessageHandler
    )
    {
    CPEDeviceModeHandler* self = new( ELeave ) CPEDeviceModeHandler(
        aCallHandling, aMessageHandler );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

 
// Destructor
CPEDeviceModeHandler::~CPEDeviceModeHandler()
    {
    if ( iTelDevModeHandler )
        {
        delete iTelDevModeHandler;
        iLibrary.Close();
        }
    }


// -----------------------------------------------------------------------------
//  CPEDeviceModeHandler::Answer
// -----------------------------------------------------------------------------
//
void CPEDeviceModeHandler::Answer()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEDeviceModeHandler::Answer" );
    
    // Checking for available ringing call is done in CallHandling,
    // no need to do it here. If a ringing call was not found, the
    // error code equals "ECCPErrorNotFound".
    iMessageHandler.HandleAnswerCall( EFalse );
    }

// -----------------------------------------------------------------------------
// CPEDeviceModeHandler::EndVoiceCalls
// -----------------------------------------------------------------------------
//
void CPEDeviceModeHandler::EndVoiceCalls()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEDeviceModeHandler::EndVoiceCalls" );
    
    if ( iCallHandling.GetNumberOfCalls() > 0 ) // there is ongoing call(s)
        {
        iMessageHandler.HandleReleaseAll();    
        }
    }

// -----------------------------------------------------------------------------
// CPEDeviceModeHandler::LoadLibraryL
// -----------------------------------------------------------------------------
//
void CPEDeviceModeHandler::LoadLibraryL()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEDeviceModeHandler::LoadLibraryL" );
    
    if ( !iTelDevModeHandler )
        {
        
        User::LeaveIfError( iLibrary.Load( KPhoneTelDevModeLoadedLib ) );
             
        TInt res = iLibrary.Lookup( KPhoneTelDevModeOrdinal )();
        iTelDevModeHandler = reinterpret_cast< CTelDMHandler* >( res );
        
        if( !iTelDevModeHandler )
            {
            iLibrary.Close();
            // Null returned, so leave.
            User::Leave( ECCPErrorNotSupported );
            }        
        else
            {
            iTelDevModeHandler->SetObserverL( this );
            }
        }
    }
    
// End of File
