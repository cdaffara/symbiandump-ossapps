/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPELogHandlingCommand class  

*
*/


// INCLUDE FILES
#include "cpeloghandlingcommand.h"
#include "cpeloghandling.h"
#include <pevirtualengine.h>
#include <talogger.h>
#include <logwrap.h>
#include <logcli.h>

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None. 

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPELogHandlingCommand::CPELogHandlingCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
CPELogHandlingCommand::CPELogHandlingCommand
        ( 
        CPELogHandling& aOwner,                   //The owner of this object
        CLogClient& aLogClient                    //The log client
        ) : CActive( EPriorityStandard ),
            iOwner( aOwner),
            iLogClient( aLogClient )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingCommand::CPELogHandlingCommand start.");
    CActiveScheduler::Add( this );    
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingCommand::CPELogHandlingCommand complete.");
    }

// Destructor
CPELogHandlingCommand::~CPELogHandlingCommand
    (
    // None.
    )
    {
    Cancel( );
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingCommand::~CPELogHandlingCommand: complete." );
    }

// -----------------------------------------------------------------------------
// CPELogHandlingCommand::AddEvent
// Adds an event to the log
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
TInt CPELogHandlingCommand::AddEvent
        (
        CLogEvent& aLogEvent
        )
    {
    TInt errorCode ( KErrNone );
    
    iLogId = aLogEvent.Id( );        
    
    TEFLOGSTRING( KTAINT, "==========================================" );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Log ID: %d",iLogId );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Calling Name: %S",&(aLogEvent.RemoteParty()) );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Call Number: %S",&(aLogEvent.Number()) );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Direction: %S",&(aLogEvent.Direction()) );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Duration: %d",aLogEvent.Duration() );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent(), Subject: %S", &(aLogEvent.Subject()) );
    TEFLOGSTRING( KTAINT, "==========================================" );
    
    if ( !IsActive( ) )
        {
        TEFLOGSTRING( KTAREQOUT, 
            "LOG CPELogHandlingCommand::AddEvent() > CLogClient::AddEvent()" );
        iLogClient.AddEvent( aLogEvent, iStatus );
        errorCode = KErrNone;
        SetActive( );        
        }
    else
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandlingCommand::AddEvent(), Event NOT added" );
        errorCode = KErrInUse;
        }
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPELogHandlingCommand::ChangeEvent
// Updates an event in the log
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
TInt CPELogHandlingCommand::ChangeEvent
        (
        const CLogEvent& aLogEvent
        )
    {
    TInt errorCode( KErrNone );

    iLogId = aLogEvent.Id( );        

    TEFLOGSTRING( KTAINT, "==========================================" );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::ChangeEvent() > CLogClient::ChangeEvent(), Log ID: %d",aLogEvent.Id() );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::ChangeEvent() > CLogClient::ChangeEvent(), Call Number: %S",&(aLogEvent.Number()) );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::ChangeEvent() > CLogClient::ChangeEvent(), Direction: %S",&(aLogEvent.Direction()) );
    TEFLOGSTRING2( KTAINT, "LOG CPELogHandlingCommand::ChangeEvent() > CLogClient::ChangeEvent(), Duration: %d",aLogEvent.Duration() );
    TEFLOGSTRING( KTAINT, "==========================================" );
    
    if ( !IsActive( ) )
        {
        TEFLOGSTRING( KTAREQOUT, 
            "LOG CPELogHandlingCommand::ChangeEvent() > CLogClient::ChangeEvent()" );
        
        iLogClient.ChangeEvent( aLogEvent, iStatus );
        SetActive( );
        }
    else
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandlingCommand::ChangeEvent(), Event NOT changed" );
        errorCode = KErrInUse;        
        } 
        
    return errorCode;       
    }

// -----------------------------------------------------------------------------
// CPELogHandlingCommand::RunL
// Description: Method gets notification from etel that asyncronous request is completed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPELogHandlingCommand::RunL
        (
        // None.
        )
    {
    TEFLOGSTRING2( KTAREQEND, "LOG CPELogHandlingCommand::RunL: < CLogClient LogId = %d. ", iLogId );   
    if ( iStatus.Int( ) != KErrNone && iStatus.Int( ) != KErrCancel )
        {
        TEFLOGSTRING2( KTAERROR, "LOG CPELogHandlingCommand::RunL: error = %d.", iStatus.Int() );   
        }
    iOwner.SendMessage( MEngineMonitor::EPEMessageLogEventSaved, iStatus.Int() );  
    }

// -----------------------------------------------------------------------------
// CPELogHandlingCommand::DoCancel
// Cancels request to the log client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / LOGENG
*    Series 60  LOGENG API
*****************************************************/
void CPELogHandlingCommand::DoCancel
        (
        // None.
        )
    {
    TEFLOGSTRING( KTAINT, "LOG CPELogHandlingCommand::DoCancel" );
    
    iLogClient.Cancel( );  
    }



//  End of File 
