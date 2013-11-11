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
// engine.cpp [stub]
// oscarg, October 2003
// Part of "SDP Agent Stub"
// 
//

#include <es_sock.h>

#include "engine.h"

#include "agconsts.h"
#include "agutil.h"
#include "requester.h"

//
// Bluetooth Assigned Numbers
// The following constants come from the Bluetooth Specification Version 1.0B
//

const TUint32 KObexProtocolUUIDValue = 0x0008;
const TUint32 KObexObjectPushUUIDValue = 0x1105;
const TUint32 K_L2CAP_UUID_Value = 0x0100;
const TUint32 K_RFCOMM_UUID_Value = 0x0003;

const TUint16 KServiceClassIdAttributeId = 0x0001;
const TUint16 KProtocolDescriptorListAttributeId = 0x0004;
const TUint16 KBluetoothProfileDescriptorListAttributeId = 0x0009;
const TUint16 KSupportedFormatsListAttributeId = 0x0303;



EXPORT_C CSdpAgent* CSdpAgent::NewL(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr)
/**
	Creates a new SDP Agent object.
	@param MSdpAgentNotifier   Interface to user object that will receive
					notifiactions of agent commands completing.
	@param TBTDevAddr	Address of the remote device the SDP agent will query.
**/
	{
	CSdpAgent* self = CSdpAgent::NewLC(aNotifier, aDevAddr);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CSdpAgent* CSdpAgent::NewLC(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr)
/**
	Creates a new SDP Agent object.
	@param MSdpAgentNotifier   Interface to user object that will receive
					notifiactions of agent commands completing.
	@param TBTDevAddr	Address of the remote device the SDP agent will query.

	Leaves the agent on the cleanup stack.
**/
	{
	CSdpAgent* self = new(ELeave) CSdpAgent();
	CleanupStack::PushL(self);
	self->ConstructL(aNotifier, aDevAddr);
	return self;
	}

EXPORT_C CSdpAgent::~CSdpAgent()
	{
	delete iAgentEngine;
	}

void CSdpAgent::ConstructL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr)
	{
	iAgentEngine = CSdpAgentEng::NewL(aNotifier, aDevAddr);
	}

CSdpAgent::CSdpAgent()
	{
	}

EXPORT_C void CSdpAgent::SetRecordFilterL(const CSdpSearchPattern& /*aUUIDFilter*/)
	{
	}

EXPORT_C void CSdpAgent::SetAttributePredictorListL(const CSdpAttrIdMatchList& /*aMatchList*/)
	{
	}

EXPORT_C void CSdpAgent::NextRecordRequestL()
/**
	Retrieves the next record handle from the remote server, that matches
	the UUID filter previously set by SetRecordFileterL(). On completion, 
	an upcall will occur on MSdpAgentNotifier::NextRecordRequestComplete().
**/
	{
	iAgentEngine->NextRecordRequestL();
	}

EXPORT_C void CSdpAgent::AttributeRequestL(TSdpServRecordHandle aHandle, 
										  TSdpAttributeID aAttrID)
/**
	Retrieve an attribute from a record on the remote server.
	@param aHandle	Record handle to retrieve attribute from
	@param aAttrID  Attribute ID to retrieve

	On completion, an up call on MSdpAgentNotifier::AttributeRequestComplete 
	will occur. If the attribute is found, it will be passsed up through a call 
	to MSdpAgentNotifier::AttributeRequestResult.
**/
	{
	CSdpAttrIdMatchList* list = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(list);
	list->AddL(TAttrRange(aAttrID));
	iAgentEngine->AttributeRequestL(aHandle, *list);
	CleanupStack::PopAndDestroy(list);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(TSdpServRecordHandle aHandle, 
										  const CSdpAttrIdMatchList& aMatchList)
	{
	iAgentEngine->AttributeRequestL(aHandle, aMatchList);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(MSdpElementBuilder* aBuilder,
										  TSdpServRecordHandle aHandle, 
										  TSdpAttributeID aAttrID)
	{
	CSdpAttrIdMatchList* list = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(list);
	list->AddL(TAttrRange(aAttrID));
	iAgentEngine->AttributeRequestL(aBuilder, aHandle, *list);
	CleanupStack::PopAndDestroy(list);
	}

EXPORT_C void CSdpAgent::AttributeRequestL(MSdpElementBuilder* aBuilder,
										  TSdpServRecordHandle aHandle, 
										  const CSdpAttrIdMatchList& aMatchList)
	{
	iAgentEngine->AttributeRequestL(aBuilder, aHandle, aMatchList);
	}

EXPORT_C void CSdpAgent::Cancel()
	{
	iAgentEngine->Cancel();
	}

/************************************************************************/
//
//   ENGINE for SDP agent client API
//
/************************************************************************/


CSdpAgentEng* CSdpAgentEng::NewL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr)
	{
	CSdpAgentEng* self = new(ELeave) CSdpAgentEng(aNotifier, aDevAddr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpAgentEng::~CSdpAgentEng()
	{				
	//delete iUUIDList;
	delete iUUIDFilter;
	delete iMatchList;
	delete iParser;
	delete iBuiltRecHandles;
	delete iPredictorList;
	
	if (iSearchRequester)
		{
		iSearchRequester->Cancel();
		delete iSearchRequester;
		}
	}

void CSdpAgentEng::ConstructL()
/**
	Create active objects which look after requests.
**/
	{
	iSearchRequester = CSdpSearchRequester::NewL(iSdpSession, *this);
	}

CSdpAgentEng::CSdpAgentEng(MSdpAgentNotifier& aNotifier, TBTDevAddr /*aDevAddr*/)
: iNotifier(aNotifier)
	{
	}

void CSdpAgentEng::SetRecordFilterL(const CSdpSearchPattern& /*aUUIDFilter*/)
	{
	}

void CSdpAgentEng::SetAttributePredictorListL(const CSdpAttrIdMatchList& /*aMatchList*/)
	{
	}

void CSdpAgentEng::NextRecordRequestL()
	{
	// start request
	iSearchRequester->SearchRequestL(iRemoteAddress,
									 *iUUIDFilter,
									 KSdpAgMaxResultCount, 
									 iSearchRequestContState);
	iServiceSearchState = ERequesting;
	}

void CSdpAgentEng::AttributeRequestL(TSdpServRecordHandle /*aHandle*/, 
									 const CSdpAttrIdMatchList& /*aMatchList*/)
	{
	}

void CSdpAgentEng::AttributeRequestL(MSdpElementBuilder* aBuilder,
									 TSdpServRecordHandle aHandle, 
									 const CSdpAttrIdMatchList& aMatchList)
/**
	[SDP Agent Stub]
	Simulates getting attributes from a record on a remote server. Builds up 
	an attribute response matching what the MTM is expecting for the SDP query
	to succeed.

  	On completion, an up call on MSdpAgentNotifier::AttributeRequestComplete 
	will occur. As attributes are "found", calls the MSdpElementBuilder
	class to describe the attribute.
**/
	{
	if (aMatchList.InMatchList(TSdpAttributeID(KServiceClassIdAttributeId /* 1 */)))
		{
		aBuilder->BuildUintL(TSdpIntBuf<TUint8>(KServiceClassIdAttributeId /* 4 */))
			->BuildUUIDL(KObexObjectPushUUIDValue /* 0x1105 */)
			->BuildUintL(TSdpIntBuf<TUint8>(100));
		}
	else if (aMatchList.InMatchList(TSdpAttributeID(KProtocolDescriptorListAttributeId)))
		{
		aBuilder->BuildUintL(TSdpIntBuf<TUint8>(KProtocolDescriptorListAttributeId))
			->BuildUUIDL(K_L2CAP_UUID_Value)
			->BuildUUIDL(K_RFCOMM_UUID_Value)
			->BuildUintL(TSdpIntBuf<TUint8>(1))
			->BuildUUIDL(KObexProtocolUUIDValue);
		}
	else if (aMatchList.InMatchList(TSdpAttributeID(KBluetoothProfileDescriptorListAttributeId)))
		{
		aBuilder->BuildUintL(TSdpIntBuf<TUint8>(KBluetoothProfileDescriptorListAttributeId))
			->BuildUUIDL(KObexObjectPushUUIDValue)
			->BuildUintL(TSdpIntBuf<TUint8>(1));
		}
	else if (aMatchList.InMatchList(TSdpAttributeID(KSupportedFormatsListAttributeId)))
		{
		aBuilder->BuildUintL(TSdpIntBuf<TUint16>(KSupportedFormatsListAttributeId))
			->BuildUintL(TSdpIntBuf<TUint8>(1));
		}

	// notify parent (CObexSdpUtils in the MTM)
	iNotifier.AttributeRequestComplete(aHandle,KErrNone);
	}

void CSdpAgentEng::Cancel()
	{
	//Cancel active objects
	iSearchRequester->Cancel();
	}

void CSdpAgentEng::HandleServiceSearchResponseL(TUint16 /*aTotalRecCount*/,
											 TUint16 /*aCurrentRecCount*/,
											 const TDesC8& /*aRecHandles*/,
											 const TDesC8& /*aContState*/)
/**
	In the real SDP agent, this is un upcall from service search requester.
	In the SDP agent stub, this is called by CSdpSearchRequester::RequestCompleteL()
**/
	{
	TInt sdpQueryResult = KErrNone;

	// 3rd parameter: 1 record found (the minimum necessary)
	iNotifier.NextRecordRequestComplete(sdpQueryResult, /* TSdpServRecordHandle */ 0 ,1);
	}

void CSdpAgentEng::HandleServiceSearchError(TInt /*aError*/)
	{
	}

void CSdpAgentEng::HandleAttributeResponseL(const TDesC8& /*aAttributeList*/, 
										    const TDesC8& /*aContState*/)
	{
	}

void CSdpAgentEng::HandleAttributeError(TInt /*aError*/)
	{
	}

void CSdpAgentEng::SendAttributeRequestL()
	{
	}

TBool CSdpAgentEng::TryToCompleteNextRecordRequest()
	{
	return ETrue;
	}

//
// Main agent DLL entry point
//
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
/**
	DLL entry point
**/
	{
	return KErrNone;
	}
