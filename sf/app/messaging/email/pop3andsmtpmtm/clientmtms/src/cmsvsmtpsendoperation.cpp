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

#include "cmsvsmtpsendoperation.h"
#include "SMTPSET.H"

CMsvSmtpProgressOperation* CMsvSmtpProgressOperation::NewL(CMsvSession& aMsvSession, 
										const CMsvEntrySelection& aSelection, TInt aFunctionId, 
										TDes8& aParameter, TRequestStatus& aObserverRequestStatus)
	{
	CMsvSmtpProgressOperation* self = new(ELeave) CMsvSmtpProgressOperation(aMsvSession, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructL(aSelection, aFunctionId, aParameter);
	CleanupStack::Pop(self);
	return self;
	}

CMsvSmtpProgressOperation::CMsvSmtpProgressOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus) :
	CMsvSendOperation(aMsvSession, aObserverRequestStatus)
	{
	}

CMsvSmtpProgressOperation::~CMsvSmtpProgressOperation()
	{
	delete iSelection;
	}

void CMsvSmtpProgressOperation::ConstructL(const CMsvEntrySelection& aSelection, TInt aFunctionId, TDes8& aParameter)
	{
	iSelection = new (ELeave) CMsvEntrySelection;
	
	// Get the service ID for the message being sent and set as first entry in 
	// selection. Append the rest of the entry IDs to the selection.
	TMsvId notUsed;
	TMsvEntry entry;
	User::LeaveIfError(iMsvSession.GetEntry(aSelection.At(0), notUsed, entry));
	
	iSelection->AppendL(entry.iServiceId);
	TInt count = aSelection.Count();
	for( TInt i=0; i < count; ++i )
		{
		iSelection->AppendL(aSelection.At(i));
		}

	iOperation = iMsvSession.TransferCommandL(*iSelection, aFunctionId, aParameter, iStatus);
	// assigns iMtm, iService and sets active
	Start(iOperation);
	}
	
const TDesC8& CMsvSmtpProgressOperation::TranslateProgress(const TDesC8& aProgress)
	{
	// convert SMTP progress into standard progress
	TImSmtpProgress smtpProgress;
	TPckg<TImSmtpProgress> smtpProgressBuf(smtpProgress);
	smtpProgressBuf.Copy(aProgress);

	// default progress info
	iProgress().iProgressMax = 0;
	iProgress().iProgress = 0;
	
	// translate state into send state
	switch (smtpProgress.Status())
		{
		case EMsgOutboxProgressWaiting:
			iProgress().iState = ESendStateWaitingToSend;
			break;
		case EMsgOutboxProgressConnecting:
			iProgress().iState = ESendStateConnecting;
			break;
		case EMsgOutboxProgressSending:
			iProgress().iState = ESendStateSending;
			if (smtpProgress.iSendFileProgress.iSessionState == ESendingImail)
				{
				// get total bytes to send
				iProgress().iProgressMax = smtpProgress.iSendFileProgress.iBytesToSend;
				// get bytes sent
				iProgress().iProgress = smtpProgress.iSendFileProgress.iBytesSent;
				}
			break;
		case EMsgOutboxProgressDone:
			iProgress().iState = ESendStateDone;
			break;
		}
	// get error
	iProgress().iError = smtpProgress.Error();

	return iProgress;
	}
