// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "csmssendmessageoperation.h"
#include <smscmds.h>
#include "SMUTSET.H"


CSmsSendMessageOperation* CSmsSendMessageOperation::NewL(CMsvSession& aMsvSession, 
														 const CMsvEntrySelection& aSelection, 
														 const TDesC8& aParameter, 
														 TRequestStatus& aObserverRequestStatus)
	{
	CSmsSendMessageOperation* self = new(ELeave) CSmsSendMessageOperation(aMsvSession, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aSelection, aParameter);
	CleanupStack::Pop(self);
	return self;
	}

void CSmsSendMessageOperation::ConstructL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter)
	{
	iOperation = iMsvSession.TransferCommandL(aSelection, ESmsMtmCommandScheduleCopy, (TDesC8&)aParameter, iStatus);
	// assigns iMtm, iService and sets active
	Start(iOperation);
	}

CSmsSendMessageOperation::CSmsSendMessageOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus) :
	CMsvSendOperation(aMsvSession, aObserverRequestStatus)
	{
	}
	
const TDesC8& CSmsSendMessageOperation::TranslateProgress(const TDesC8& aProgress)
	{
	TSmsProgressBuf& progress = (TSmsProgressBuf&) aProgress;

	// default progress
	iProgress().iProgressMax = 0;
	iProgress().iProgress = 0;
	
	// convert SMS progress into standard send progress
	switch (progress().iType)	
		{
		case TSmsProgress::ESmsProgressTypeSending:
		case TSmsProgress::ESmsProgressTypeScheduling:
			iProgress().iProgressMax = progress().iMsgCount;
			iProgress().iProgress = progress().iMsgDone;
			iProgress().iState = ESendStateSending;
			break;
		default:
			iProgress().iState = ESendStateWaitingToSend;
			break;
		}
	// get error
	iProgress().iError = progress().iError;

	return iProgress;
	}
