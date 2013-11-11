// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// requester [stub]
// 
//


#include <es_sock.h>

#include "agtypes.h"
#include "engine.h"
#include "requester.h"


void CSdpRequesterBase::NewRequestL(const TBTDevAddr& /*aRemDev*/)
	{
	}

void CSdpRequesterBase::ConnectL()
	{
	}

void CSdpRequesterBase::Disconnect()
	{
	}

void CSdpRequesterBase::RetrieveResponseL()
	{
	}

CSdpRequesterBase::	CSdpRequesterBase(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
: CActive(EPriorityNormal),
 iSdpSession(aSdpSession),
 iParent(aParent),
 iResponse(0,0)
	{
	}

CSdpRequesterBase::~CSdpRequesterBase()
	{
	}


void CSdpRequesterBase::RunL()
	{
	RequestCompleteL();
	}

void CSdpRequesterBase::DoCancel()
	{
	}

CSdpSearchRequester* CSdpSearchRequester::NewL(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
	{
	CSdpSearchRequester* self = new(ELeave) CSdpSearchRequester(aSdpSession, aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpSearchRequester::~CSdpSearchRequester()
	{
	}

void CSdpSearchRequester::SearchRequestL(const TBTDevAddr& /*aRemoteDev*/,
										 CSdpSearchPattern& /*aUUIDFilter*/,
										 TUint16 /*aMaxRecCount*/, 
										 const TDesC8& /*aContState*/)
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


CSdpSearchRequester::CSdpSearchRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
:CSdpRequesterBase(aSdpSession, aParent)
/**
	Constructor: Adds this CSdpAttributeRequester to Active Scheduler.
**/
	{
	CActiveScheduler::Add(this);
	}

void CSdpSearchRequester::ConstructL()
	{
	}


void CSdpSearchRequester::IssueRequestL()
	{
	}

void CSdpSearchRequester::RequestCompleteL()
/*
Simply calls its parent object so that this can send the 
response to the MTM
*/
	{
	iParent.HandleServiceSearchResponseL(0,  // dummy parameters
										 0, 
										 KNullDesC8, 
										 KNullDesC8);
	}

void CSdpSearchRequester::Reset()
	{
	}

void CSdpSearchRequester::DoCancel()
/**
	Cancels current asynchronous requests.
**/
	{
	CSdpRequesterBase::DoCancel();
	}

TInt CSdpSearchRequester::RunError(TInt aError)
/**
	Called automatically when a RunL leaves.
	Calls appropriate error handling function in the CSdpAgentEng parent.
**/
	{
	Reset();
	iParent.HandleServiceSearchError(aError);
	return KErrNone;
	}

//*********************************************
// Implementation of CSdpAttributeRequester
// This class is not used in the SDP agent stub.
//*********************************************
CSdpAttributeRequester* CSdpAttributeRequester::NewL(RSdpSession& /*aSdpSession*/, CSdpAgentEng& /*aParent*/)
	{
	return NULL;
	}

CSdpAttributeRequester::~CSdpAttributeRequester()
	{
	}

void CSdpAttributeRequester::AttributeRequestL(const TBTDevAddr& /*aRemoteDev*/,
											   TSdpServRecordHandle /*aHandle*/, 
											   TUint16 /*aMaxAttrByteCount*/,
											   CSdpAttrIdMatchList& /*aMatchList*/,
											   const TDesC8& /*aContState*/)
	{
	}


CSdpAttributeRequester::CSdpAttributeRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent)
:CSdpRequesterBase(aSdpSession, aParent)
	{
	}

void CSdpAttributeRequester::ConstructL()
	{
	}


void CSdpAttributeRequester::IssueRequestL()
	{
	}

void CSdpAttributeRequester::RequestCompleteL()
	{
	}

void CSdpAttributeRequester::Reset()
	{
	}

TInt CSdpAttributeRequester::RunError(TInt aError)
	{
	return aError;
	}

void CSdpAttributeRequester::DoCancel()
	{
	}




