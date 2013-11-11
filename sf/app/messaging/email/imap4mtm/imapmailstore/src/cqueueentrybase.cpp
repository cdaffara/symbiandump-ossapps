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


#include "cqueueentrybase.h"
#include <miuthdr.h>

/**
Constructor.
*/
CQueueEntryBase::CQueueEntryBase(TMsvId aId,TQueueEntryType aQueueEntryType,CImapMailStore& aParent,CMsvServerEntry& aServerEntry,MMailStoreObserver& aObserver)
	:CActive(EPriorityStandard), iId(aId),iObserver(aObserver),iQueueEntryType(aQueueEntryType), iParent(aParent), iServerEntry(aServerEntry)
	{
	}
	
void CQueueEntryBase::BaseConstructL()
	{
	CActiveScheduler::Add(this); 
	}

/**
Destructor.
*/
CQueueEntryBase::~CQueueEntryBase()
	{
	Cancel();
	}

/**
Calls back to the client observer with the result of the operation and the entry id.
@param aErrorCode   The result of the store operation
@return
*/		
void CQueueEntryBase::StoreComplete(TInt aErrorCode)
	{
	iObserver.StoreOperationComplete(iId,aErrorCode);				
	}
	
/**
Sets the queue entry active.
@param
@return
*/
void CQueueEntryBase::CompleteSelf()
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}
	
TMsvId CQueueEntryBase::GetId() const 
	{
	return iId;
	}

CQueueEntryBase::TQueueEntryType CQueueEntryBase::GetEntryType() const
	{
	return iQueueEntryType;
	}

MMailStoreObserver& CQueueEntryBase::GetObserver()
	{
	return iObserver;
	}
	
void CQueueEntryBase::DoCancel()
	{
	//default implementation  does nothing		
	}

void CQueueEntryBase::CancelRequest()
	{
	//default implementation  does nothing	
	}
	
	
/**
Calls StoreComplete with the error code and removes the request from the queue.
@param		aError the error code	
@return		KErrNone
*/
TInt CQueueEntryBase::RunError(TInt aError)
	{
	StoreComplete(aError);
	iParent.RemoveFromQueueAndDelete(this);	
	return KErrNone;	
	}
