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
// EngineRequest.h
// Header file for the Engine Request class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCENGINEREQUEST_H__
#define __SYNCENGINEREQUEST_H__

#include "phbksyncsvr.h"



/**
 *  This class is responsible for montioring request against the Background Sync
 *  Engine.
 */
class CSyncEngineRequest : public CActive
	{
public:
	static CSyncEngineRequest*  NewL(CPhoneBookServer& aServer,
									 RSyncEngineSession& aSyncEngine,
									 TPhonebookSyncRequest aPhonebookSyncRequest,
									 TUid aPhonebookUid,
									 TContactItemId aContactId,
									 CPhoneBookSession* aClientSession,
									 const RMessage2& aClientMessage);
	~CSyncEngineRequest();
	
	void StartRequest();
	void CompleteClientRequestL();

	inline TPhonebookSyncRequest PhonebookSyncRequest() const;
	inline TUid PhonebookUid() const;
	inline TContactItemId ContactId() const;
	inline CPhoneBookSession* ClientSession() const;
	inline const RMessage2& ClientMessage() const;
	
	void DoCancel();

protected:
	TInt RunError(TInt aError);

private:
	CSyncEngineRequest(CPhoneBookServer& aServer,
					   RSyncEngineSession& aSyncEngine,
					   TPhonebookSyncRequest aPhonebookSyncRequest,
					   TUid aPhonebookUid,
					   TContactItemId aContactId,
					   CPhoneBookSession* aClientSession,
					   const RMessage2& aClientMessage);

	void ConstructL();
	void RunL();

private:
	CPhoneBookServer&  iServer;
	RSyncEngineSession&  iSyncEngine;
	TPhonebookSyncRequest  iPhonebookSyncRequest;
	TUid  iPhonebookUid;
	TContactItemId  iContactId;
	CPhoneBookSession*  iClientSession;
	RMessage2  iClientMessage;

	//
	// The following fields are all used to store data for WriteContact
	// for use by the engine.
	//
	TInt  iWriteContactSlotNumber;
	RPhoneBookSession::TTemplateAndBufferSize  iWriteContactTemplateIdAndBufferSize;
	CBufFlat*  iWriteContactICCEntryBuf;
	TPtrC8  iWriteContactTemplateIdAndBufferSizePtr;
	TPtr8  iWriteContactSlotNumberPtr;
	TPtr8  iWriteContactPhoneBookIDPtr;
	TPtr8  iWriteContactICCEntryBufPtr;
	};


/**
 *  Return the sync request type.
 *
 *  @return The type of sync request used by this engine request.
 */
inline TPhonebookSyncRequest CSyncEngineRequest::PhonebookSyncRequest() const
	{
	return iPhonebookSyncRequest;
	} // CSyncEngineRequest::iPhonebookSyncRequest


/**
 *  Return the ICC phonebook UID. 
 *
 *  @return The UID of the phonebook being used by this engine request.
 */
inline TUid CSyncEngineRequest::PhonebookUid() const
	{
	return iPhonebookUid;
	} // CSyncEngineRequest::PhonebookUid


/**
 *  Return the Contact ID.
 *
 *  @return The Contact ID of the entry being used by this engine request.
 */
inline TContactItemId CSyncEngineRequest::ContactId() const
	{
	return iContactId;
	} // CSyncEngineRequest::ContactId


/**
 *  Return the client session for this engine request.
 *
 *  @return The client session pointer.
 */
inline CPhoneBookSession* CSyncEngineRequest::ClientSession() const
	{
	return iClientSession;
	} // CSyncEngineRequest::ClientSession


/**
 *  Return the client message handle. 
 *
 *  @return  A reference to the client message request being used by this
 *           engine request.
 */
inline const RMessage2& CSyncEngineRequest::ClientMessage() const
	{
	return iClientMessage;
	} // CSyncEngineRequest::ClientMessage


#endif // __SYNCENGINEREQUEST_H__
