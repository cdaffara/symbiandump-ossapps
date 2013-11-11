// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <s32mem.h>
#include <e32svr.h>

#include "pbapsession.h"
#include "clientserver.h"
#include "pbapserver.h"

#include "btaccesshostlog.h"



CPbapSession::CPbapSession()
	{
	LOG_FUNC
	}
	
/**
 Second phase constructor for sessions. Called by the CServer2 framework
 when a session is created (e.g. a connection is made to the server).
 */
void CPbapSession::CreateL()
	{
	LOG_FUNC
	Server().AddSession();
	}

/**
 Destructor for session classes. When this is called it indicates that
 a session is closing its connection with the server.
 */
CPbapSession::~CPbapSession()
	{
	LOG_FUNC
	Server().DropSession();
	}


CPbapServer& CPbapSession::Server()
	{
	LOG_FUNC
	return *static_cast<CPbapServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

/**
 Handle message requests for this session. Leaving is handled by
 CPbapSession::ServiceError() which reports the error code to the client.
 */
void CPbapSession::ServiceL(const RMessage2 &aMessage)
	{
	LOG_FUNC
	switch (aMessage.Function())
		{
		case EPbapStartListening:
			{
			Server().StartObexL();
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapStopListening:
			{
			Server().ReleaseObex();
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapSetPassword:
			{
			HBufC* password = HBufC::NewLC(aMessage.GetDesLengthL(0));
			TPtr passwordPtr(password->Des());
			aMessage.ReadL(0, passwordPtr);
			Server().SetPasswordL(password);	// ownership passed
			CleanupStack::Pop(password);
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapDbgMarkHeap:
			{
			#ifdef _DEBUG
			__UHEAP_MARK;
			#endif
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapDbgCheckHeap:
			{
			#ifdef _DEBUG
			__UHEAP_CHECK(aMessage.Int0());
			#endif
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapDbgMarkEnd:
			{
			#ifdef _DEBUG
			__UHEAP_MARKENDC(aMessage.Int0());
			#endif 
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		case EPbapDbgFailNext:
			{
			#ifdef _DEBUG
			if (aMessage.Int0() == 0)
				{
				__UHEAP_RESET;
				}
			else
				{
				__UHEAP_FAILNEXT(aMessage.Int0());
				}
			#endif
			CompleteRequest(aMessage, KErrNone);
			}
			break;
		default:
			// panic the client
			aMessage.Panic(KPbapServerPanic, EPbapServerPanicIllegalFunction);
			break;
		}
	}

/**
 Completes a client request. This function provides a single point of
 message completion which benefits debugging and maintenance.
 */
void CPbapSession::CompleteRequest(const RMessage2& aMessage, TInt aResult)
	{
	LOG_FUNC

	if (aMessage.IsNull() == EFalse)
		{
	    aMessage.Complete(aResult);
		}
	}


/**
 Handle an error from CPbapSession::ServiceL()
 bad descriptor error implies a badly programmed client, so panic it
 otherwise use the default handling (report the error to the client)
 */
void CPbapSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	LOG_FUNC
	if (aError==KErrBadDescriptor)
		{
		// panic the client
		aMessage.Panic(KPbapServerPanic, EPbapServerPanicBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}
