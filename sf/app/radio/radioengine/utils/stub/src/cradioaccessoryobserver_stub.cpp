/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "cradioenginelogger.h"

#include "cradioaccessoryobserver_stub.h"
#include "mradioheadseteventobserver.h"

#define STUB  iRadioStubManager->iAccessoryObserver

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::CRadioAccessoryObserver()
    {
    LOG_METHOD_AUTO;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::ConstructL()
    {
    LOG_METHOD_AUTO;
    // Open chunk for test configuration/control data
    TInt err = iRadioStubManagerChunk.OpenGlobal(
            KRadioStubManagerLocalChunkName,
            EFalse, // == Read | Write
            EOwnerThread );
    User::LeaveIfError( err );
    if ( sizeof(SRadioStubManager) > iRadioStubManagerChunk.MaxSize() )
        {
        User::Leave( KErrTooBig );
        }
    TUint8* basePtr = iRadioStubManagerChunk.Base();
    User::LeaveIfNull( basePtr );    
    iRadioStubManager = (SRadioStubManager*)basePtr;
    if ( STUB.iLeaveNewL.iError )
        {
        User::Leave( STUB.iLeaveNewL.iError );
        }
    if ( STUB.iLeaveConstructL.iError )
        {
        User::Leave( STUB.iLeaveConstructL.iError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver* CRadioAccessoryObserver::NewL()
    {
    LOG_METHOD_AUTO;
    CRadioAccessoryObserver* self = new( ELeave ) CRadioAccessoryObserver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::~CRadioAccessoryObserver()
    {
    LOG_METHOD_AUTO;
    iRadioStubManagerChunk.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsHeadsetConnectedL() const
    {
    LOG_METHOD_AUTO;
    if ( STUB.iLeaveIsHeadsetConnectedL.iError )
        {
        User::Leave( STUB.iLeaveIsHeadsetConnectedL.iError );
        }
    return STUB.iHeadsetConnected;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::SetObserver( MRadioHeadsetEventObserver* aObserver )
    {
    LOG_METHOD_AUTO;
    STUB.iObserver = aObserver;
    }

