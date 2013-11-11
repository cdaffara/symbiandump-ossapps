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
* Description:  Sends UUI message.
*
*/


#include "cspuuimessagesender.h"
#include "csplogger.h"
 
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSPUUIMessageSender::CSPUUIMessageSender(
    RMobileCall& aCall ) : 
    CActive( CActive::EPriorityStandard ),
    iCall( aCall ),
    iUUSRequestPckg( iUUSRequest )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CSPUUIMessageSender::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CSPUUIMessageSender* CSPUUIMessageSender::NewL(
    RMobileCall& aCall )
    {
    CSPUUIMessageSender* self = 
        new( ELeave ) CSPUUIMessageSender( aCall );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSPUUIMessageSender::~CSPUUIMessageSender()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Sends the UUI message.
// ---------------------------------------------------------------------------
//
void CSPUUIMessageSender::SendUUIMessage( const TDesC& aMessage )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPUUIMessageSender::SendUUIMessage start");
    Cancel();
    iUUSRequest.iServiceReq = RMobileCall::KUUS1Implicit;
    iUUSRequest.iUUI = aMessage;
    iCall.ActivateUUS( iStatus, iUUSRequestPckg );
    SetActive();    
    }

// ---------------------------------------------------------------------------
// From class CActive
// Cancels the outstanding request.
// ---------------------------------------------------------------------------
//
void CSPUUIMessageSender::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileCallActivateUUS );
    }

// ---------------------------------------------------------------------------
// From class CActive
// No need to react the completion. The message is sent or not.
// ---------------------------------------------------------------------------
//
void CSPUUIMessageSender::RunL()
    {
    CSPLOGSTRING2(CSPINT, "CSPUUIMessageSender::RunL %d iStatus", iStatus.Int() );
    }
