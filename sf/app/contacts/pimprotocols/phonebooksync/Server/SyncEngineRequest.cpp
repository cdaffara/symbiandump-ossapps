// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The CSyncEngineRequest class is used to store and monitor a request which
// has, or will be, sent to the Background Sync Engine.
// 
//

/**
 @file
 @internalComponent
*/

#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"
#include "phbksynclog.h"
#include "SyncEngineSession.h"
#include "SyncEngineServer.h"
#include "phbksyncsess.h"
#include "SyncEngineRequest.h"
#include "common.h"


/**
 *  Static factory method used to create a CSyncEngineRequest object.
 *
 *  @param aServer                Reference to the front-end server.
 *  @param aSyncEngine            Handle to the background sync engine.
 *  @param aPhonebookSyncRequest  Type of engine sync request.
 *  @param aPhonebookUid          UID of the phonebook being operated on.
 *  @param aContactId             Contact ID of the entry being modified.
 *  @param aClientSession         Pointer to the client session (if one exists).
 *  @param aClientMessage         Reference to the client message (if one exists).
 *
 *  @return  Returns the pointer to a newly created CSyncEngineRequest object.
 */
CSyncEngineRequest*  CSyncEngineRequest::NewL(CPhoneBookServer& aServer,
									  RSyncEngineSession& aSyncEngine,
									  TPhonebookSyncRequest aPhonebookSyncRequest,
									  TUid aPhonebookUid,
									  TContactItemId aContactId,
									  CPhoneBookSession* aClientSession,
									  const RMessage2& aClientMessage)
	{
	CSyncEngineRequest*  engineRequest = new(ELeave) CSyncEngineRequest(aServer, aSyncEngine,
																aPhonebookSyncRequest,
																aPhonebookUid, aContactId,
																aClientSession, aClientMessage);
	CleanupStack::PushL(engineRequest);
	engineRequest->ConstructL();
	CleanupStack::Pop(engineRequest);

	return engineRequest;
	} // CActiveEngineRequest::NewL


/**
 *  Private constructor for an engine request.
 *
 *  @param aServer                Reference to the front-end server.
 *  @param aSyncEngine            Handle to the background sync engine.
 *  @param aPhonebookSyncRequest  Type of engine sync request.
 *  @param aPhonebookUid          UID of the phonebook being operated on.
 *  @param aContactId             Contact ID of the entry being modified.
 *  @param aClientSession         Pointer to the client session (if one exists).
 *  @param aClientMessage         Reference to the client message (if one exists).
 */
CSyncEngineRequest::CSyncEngineRequest(CPhoneBookServer& aServer,
							   RSyncEngineSession& aSyncEngine,
							   TPhonebookSyncRequest aPhonebookSyncRequest,
							   TUid aPhonebookUid,
							   TContactItemId aContactId,
							   CPhoneBookSession* aClientSession,
							   const RMessage2& aClientMessage)
  : CActive(EPriorityNormal),
    iServer(aServer),
    iSyncEngine(aSyncEngine),
    iPhonebookSyncRequest(aPhonebookSyncRequest),
    iPhonebookUid(aPhonebookUid),
    iContactId(aContactId),
    iClientSession(aClientSession),
    iClientMessage(aClientMessage),
    iWriteContactSlotNumber(-1),
    iWriteContactICCEntryBuf(NULL),
	iWriteContactTemplateIdAndBufferSizePtr(NULL, 0),
	iWriteContactSlotNumberPtr(NULL, 0),
	iWriteContactPhoneBookIDPtr(NULL, 0),
	iWriteContactICCEntryBufPtr(NULL, 0)
    {
	// NOP
	} // CSyncEngineRequest::CSyncEngineRequest


/**
 *  Destructor for the engine request.
 */
CSyncEngineRequest::~CSyncEngineRequest()
	{
	LOGACTIVE1(_L8("~CSyncEngineRequest()"));
	Cancel();
	
	delete iWriteContactICCEntryBuf;
	} // CSyncEngineRequest::~CSyncEngineRequest
	

/**
 *  Second phrase constructor for the engine request. The request will be initialised
 *  and any client side data will be stored for lating sending to the engine.
 */
void CSyncEngineRequest::ConstructL()
	{
	LOGACTIVE1(_L8("CSyncEngineRequest::ConstructL()"));

	//
	// Read any data required into the engine request's buffers...
	//
	if (iPhonebookSyncRequest == ESyncWriteCntToICC)
		{
		TPckg<RPhoneBookSession::TTemplateAndBufferSize>  templateIdAndBufferSizePckg(iWriteContactTemplateIdAndBufferSize);
		TPckg<TInt>  slotNumPckg(iWriteContactSlotNumber);

		iClientMessage.ReadL(0, templateIdAndBufferSizePckg); 
		iClientMessage.ReadL(1, slotNumPckg);

		iWriteContactICCEntryBuf = CBufFlat::NewL(1);
		iWriteContactICCEntryBuf->ResizeL(iWriteContactTemplateIdAndBufferSize.bufferSize); 
		iWriteContactICCEntryBufPtr.Set(iWriteContactICCEntryBuf->Ptr(0));
		iClientMessage.ReadL(3, iWriteContactICCEntryBufPtr);

		//
		// Call the engine's write contact function...
		//		
		iWriteContactTemplateIdAndBufferSizePtr.Set(reinterpret_cast<const TText8*>(&iWriteContactTemplateIdAndBufferSize),
													sizeof(RPhoneBookSession::TTemplateAndBufferSize));
		iWriteContactSlotNumberPtr.Set(reinterpret_cast<TText8*>(&iWriteContactSlotNumber),
									   sizeof(iWriteContactSlotNumber), sizeof(iWriteContactSlotNumber));
		iWriteContactPhoneBookIDPtr.Set(reinterpret_cast<TText8*>(&iPhonebookUid),
										sizeof(iPhonebookUid), sizeof(iPhonebookUid));		
		iWriteContactICCEntryBufPtr.Set(iWriteContactICCEntryBuf->Ptr(0));
		}

	CActiveScheduler::Add(this);
	} // CSyncEngineRequest::ConstructL


/**
 *  This function is used to issue a request to the Background Sync Engine via the
 *  Sync Engine Session. Once this function completes the request will be active and
 *  when the request is completed, RunL() will be called.
 */
void CSyncEngineRequest::StartRequest()
	{
	LOGACTIVE1(_L8("CSyncEngineRequest::StartRequest()"));

	if (iPhonebookSyncRequest == ESyncDoSynchronisation)
		{
		iSyncEngine.DoSynchronisation(iStatus, iPhonebookUid);
		SetActive();
		}
	else if (iPhonebookSyncRequest == ESyncDeleteCntFromICC)
		{
		iSyncEngine.DeleteContact(iStatus, iPhonebookUid, iContactId);
		SetActive();
		}
	else if (iPhonebookSyncRequest == ESyncWriteCntToICC)
		{
		iSyncEngine.WriteContact(iStatus,
								 iWriteContactTemplateIdAndBufferSizePtr,
								 iWriteContactSlotNumberPtr,
								 iWriteContactPhoneBookIDPtr,
								 iWriteContactICCEntryBufPtr);
		SetActive();
		}
	else
		{
		PhBkSyncPanic(EPhBkSyncPanicInvalidEngineRequest);
		}
	} // CSyncEngineRequest::StartRequest


/**
 *  Completes the clients request. This may involve performing additional
 *  actions such as informing the front-end server of status changes, or
 *  relaying information from the engine to the client.
 *
 *  The client request will be completed (if one exists) and this request
 *  will be completed (e.g. removed from the queue and deleted).
 */
void CSyncEngineRequest::CompleteClientRequestL()
	{
	LOGACTIVE1(_L8("CSyncEngineRequest::CompleteClientRequestL()"));

	//
	// Perform any special tasks for this request...
	//
	if (iPhonebookSyncRequest == ESyncDoSynchronisation)
		{
		//
		// Let the server know that the cache state has changed...
		//
		iServer.CompleteNotifyStateChange(iPhonebookUid);
		}
	else if (iStatus.Int() == KErrNone  &&
			 iPhonebookSyncRequest == ESyncWriteCntToICC)
		{
		//
		// Pass back the slot number and phonebook which may have been
		// updated (in the cases where the slot or phonebook was not)
		// specified.
		//
		TPckg<TInt>  slotNumPckg(iWriteContactSlotNumber);
		TPckg<TUid>  phonebookUidPckg(iPhonebookUid);

		iClientMessage.WriteL(1, slotNumPckg);
		iClientMessage.WriteL(2, phonebookUidPckg);
		}

	//
	// Complete the client's request (if one exists)...
	//
	if (iClientSession != NULL)
		{
		iClientSession->CompleteRequest(iClientMessage, iStatus.Int());
		}

	//
	// Remove this request from the request queue and start the next request...
	//
	iServer.CompleteEngineRequest(*this);
	
	//
	// Delete this request...
	//
	delete this;
	} // CSyncEngineRequest::CompleteClientRequestL


/**
 *  Standard Active Object RunL() method. This completes the client request.
 */
void CSyncEngineRequest::RunL()
	{
	LOGACTIVE1(_L8("CSyncEngineRequest::RunL()"));

	CompleteClientRequestL();
	} // CSyncEngineRequest::RunL


/** 
 *  Standard Active Object DoCancel method. This is called when the Cancel()
 *  method is called on an active request. It cancels the pending request
 *  with the Background Sync Engine.
 */
void CSyncEngineRequest::DoCancel()
	{
	if (iPhonebookSyncRequest == ESyncDoSynchronisation)
		{
		iSyncEngine.DoSynchronisationCancel(iPhonebookUid);
		}
	else if (iPhonebookSyncRequest == ESyncDeleteCntFromICC)
		{
		iSyncEngine.DeleteContactCancel(iPhonebookUid);
		}
	else if (iPhonebookSyncRequest == ESyncWriteCntToICC)
		{
		iSyncEngine.WriteContactCancel(iPhonebookUid);
		}
	else
		{
		iStatus = KErrCancel;
		}
	} // CSyncEngineRequest::DoCancel


/**
 *  Standard Active Object RunError() method. This is called if the RunL()
 *  method leaves. Hopefully this should not happen.
 *
 *  @param aError  Leave code from the RunL().
 *
 *  @return KErrNone (although the code will panic first).
 */
TInt CSyncEngineRequest::RunError(TInt aError)
	{
#ifdef _DEBUG
	LOGACTIVE2(_L8("CSyncEngineRequest::RunError(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);

	return KErrNone;	
	} // CSyncEngineRequest::RunError

