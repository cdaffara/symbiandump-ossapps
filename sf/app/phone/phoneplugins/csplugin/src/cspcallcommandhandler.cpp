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
* Description:  Implements the class CSPCallCommandHandler
*
*/


#include "cspcallcommandhandler.h"
#include "mcspcallcommandhandling.h"
#include "csplogger.h"

// ---------------------------------------------------------------------------
// CSPCallCommandHandler::NewL.
// ---------------------------------------------------------------------------
//
CSPCallCommandHandler* CSPCallCommandHandler::NewL()
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCallCommandHandler::NewL()" );
    CSPCallCommandHandler* self = new ( ELeave ) CSPCallCommandHandler();
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the array including remaining calls.
// ---------------------------------------------------------------------------
//
CSPCallCommandHandler::~CSPCallCommandHandler()
    {
    iActiveHangupArray.Reset();
    iActiveHangupArray.Close();
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCallCommandHandler::~CSPCallCommandHandler");
    }

// ---------------------------------------------------------------------------
// CSPCallCommandHandler::IndicateActiveHangup
// ---------------------------------------------------------------------------
//
TInt CSPCallCommandHandler::IndicateActiveHangup( MCCPCallCommandHandling& aCall )
    {
    TInt err = iActiveHangupArray.Find( &aCall );
    if ( err == KErrNotFound )
        {
        err = iActiveHangupArray.Append( &aCall );
        CSPLOGSTRING2( 
            CSPINT, 
            "CSPCallCommandHandler::IndicateActiveHangup append error: %d", 
            err );
        }
    else
        {
        CSPLOGSTRING( 
            CSPINT, 
            "CSPCallCommandHandler::IndicateActiveHangup call already added");
        }
    CSPLOGSTRING2( 
        CSPINT, 
        "CSPCallCommandHandler::IndicateActiveHangup active hangup count: %d", 
        iActiveHangupArray.Count());
        
    return err;
    }

// ---------------------------------------------------------------------------
// CSPCallCommandHandler::CSPCallCommandHandler::IndicateHangupComplete
// ---------------------------------------------------------------------------
//  
TInt CSPCallCommandHandler::IndicateHangupComplete( MCCPCallCommandHandling& aCall )
    {
    TInt index = iActiveHangupArray.Find( &aCall );
    if ( index != KErrNotFound )
        {
        iActiveHangupArray.Remove( index );
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateHangupComplete remove");
        }

    if( !iActiveHangupArray.Count() && iPendingCall )
        {
        if( iPendingCommand == ECSPAnswerCall )
            {
            CSPLOGSTRING(CSPINT, 
                "CSPCallCommandHandler::IndicateHangupComplete perform answer request");
            iPendingCall->PerformAnswerRequest();
            iPendingCommand = ECSPNone;
            iPendingCall = NULL;
            }
        else if( iPendingCommand == ECSPDialCall )
            {
            CSPLOGSTRING(CSPINT, 
                "CSPCallCommandHandler::IndicateHangupComplete perform dial request");
            iPendingCall->PerformDialRequest();
            iPendingCommand = ECSPNone;
            iPendingCall = NULL;
            }
        }
        
    CSPLOGSTRING2(CSPINT, 
        "CSPCallCommandHandler::IndicateHangupComplete active hangup count: %d", 
        iActiveHangupArray.Count());
        
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CSPCallCommandHandler::IndicateDialRequest
// ---------------------------------------------------------------------------
//
TInt CSPCallCommandHandler::IndicateDialRequest( MCCPCallCommandHandling& aCall )
            
    {
    CSPLOGSTRING(CSPINT, 
        "CSPCallCommandHandler::IndicateDialRequest");
    TInt ret( KErrNone );
    
    if( !iActiveHangupArray.Count() )
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateDialRequest dial");
        aCall.PerformDialRequest();
        }
    else if( !iPendingCall && iPendingCommand == ECSPNone )
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateDialRequest delay dialing");
        iPendingCall = &aCall;
        iPendingCommand = ECSPDialCall;
        }
    else
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateDialRequest ERROR already exists");
        ret = KErrAlreadyExists;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CSPCallCommandHandler::IndicateAnswerRequest
// ---------------------------------------------------------------------------
//
TInt CSPCallCommandHandler::IndicateAnswerRequest( MCCPCallCommandHandling& aCall )
            
    {
    CSPLOGSTRING(CSPINT, "CSPCallCommandHandler::IndicateAnswerRequest" );
        
    TInt ret( KErrNone );
    
    if( !iActiveHangupArray.Count() )
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateAnswerRequest answer");
        aCall.PerformAnswerRequest();
        }
    else if( !iPendingCall && iPendingCommand == ECSPNone )
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateAnswerRequest delay answering");
        iPendingCall = &aCall;
        iPendingCommand = ECSPAnswerCall;
        }
    else
        {
        CSPLOGSTRING(CSPINT, 
            "CSPCallCommandHandler::IndicateAnswerRequest ERROR already exists");
        ret = KErrAlreadyExists;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPCallCommandHandler::CSPCallCommandHandler()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCallCommandHandler::CSPCallCommandHandler");
    }

// End of File
