/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSconSyncSession implementation
*
*/


#include "sconsyncsession.h"

#include "sconsyncserver.h"
#include "sconsyncclientserver.h"
#include "sconasynchandler.h"
#include "debug.h"


CSconSyncSession::CSconSyncSession() 
    {
    TRACE_FUNC;
    }

CSconSyncSession* CSconSyncSession::NewL()
    {
    TRACE_FUNC;
    CSconSyncSession* self = new(ELeave) CSconSyncSession();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CSconSyncSession::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iAsyncHandler = CSconAsyncHandler::NewL();
    TRACE_FUNC_EXIT;
    }

void CSconSyncSession::CreateL()
    {
    TRACE_FUNC;
    Server().AddSession();
    }

CSconSyncSession::~CSconSyncSession()
    {
    TRACE_FUNC_ENTRY;
    delete iAsyncHandler;
    
    Server().RemoveSession();
    
    TRACE_FUNC_EXIT;
    }

CSconSyncServer& CSconSyncSession::Server()
    {
    return *static_cast<CSconSyncServer*>(const_cast<CServer2*>(CSession2::Server()));
    }


// -----------------------------------------------------------------------------
// CSconSyncSession::ServiceL()
// Handles the client request
// -----------------------------------------------------------------------------
//
void CSconSyncSession::ServiceL(const RMessage2& aMessage)
    {
    TRACE_FUNC_ENTRY;
    
    TRAPD( err, iAsyncHandler->HandleServiceL( aMessage ));
    if ( err )
        {
        LOGGER_WRITE_1("iAsyncHandler->HandleServiceL leaved: %d", err);
        if ( iAsyncHandler->IsActive() )
            {
            LOGGER_WRITE(" and it was active -> Cancel it");
            iAsyncHandler->Cancel();
            }
        // do server error processing
        User::Leave( err );
        }
    
    TRACE_FUNC_EXIT;
    }


