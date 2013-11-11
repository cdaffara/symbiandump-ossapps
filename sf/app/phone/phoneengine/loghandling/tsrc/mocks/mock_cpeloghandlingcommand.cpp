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
#include <smcmockclassincludes.h>
#include "CPELogHandlingCommand.h"
#include "CPELogHandling.h"
#include <PEVirtualEngine.h>
#include <TALogger.h>
#include <LogWrap.h>
#include <LogCli.h>

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
    }

// Destructor
CPELogHandlingCommand::~CPELogHandlingCommand
    (
    // None.
    )
    {
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
    SMC_MOCK_METHOD1( TInt, CLogEvent& , aLogEvent)     
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
    SMC_MOCK_METHOD1( TInt, const CLogEvent& , aLogEvent)     
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
    SMC_MOCK_METHOD0( void ) 
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
    SMC_MOCK_METHOD0( void ) 
    }



//  End of File 
