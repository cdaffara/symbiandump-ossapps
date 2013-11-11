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

#include <cobexsendoperation.h>
#include <obexclientmtm.h>


EXPORT_C CObexSendOperation::CObexSendOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus) :
	CMsvSendOperation(aMsvSession, aObserverRequestStatus)
	{
	}
	
EXPORT_C const TDesC8& CObexSendOperation::TranslateProgress(const TDesC8& aProgress)
	{
	TObexMtmProgress obexProgress;
	TPckg<TObexMtmProgress> obexProgressBuf(obexProgress);
	obexProgressBuf.Copy(aProgress);

	// default progress
	iProgress().iProgressMax = 0;
	iProgress().iProgress = 0;

	// translate obex progress to standard send progress
	switch (obexProgress.iSendState)
		{
		case TObexMtmProgress::ENullOp:
		case TObexMtmProgress::EUserCancelled:
		case TObexMtmProgress::ESendError:
			iProgress().iState = ESendStateFailed;
			break;
		case TObexMtmProgress::EDisconnected:
			iProgress().iState = ESendStateWaitingToSend;
			break;
		case TObexMtmProgress::EConnect:
		case TObexMtmProgress::EConnectAttemptComplete:
			iProgress().iState = ESendStateConnecting;
			break;
		case TObexMtmProgress::EInitialise:
		case TObexMtmProgress::EInitialised:
			iProgress().iState = ESendStateInPreparation;
			break;
		case TObexMtmProgress::ESendObject:
			iProgress().iState = ESendStateSending;
			iProgress().iProgressMax = obexProgress.iTotalEntryCount;
			iProgress().iProgress = obexProgress.iEntriesDone;
			break;
		case TObexMtmProgress::ESendNextObject:
			iProgress().iState = ESendStateSending;
			iProgress().iProgressMax = obexProgress.iCurrentEntrySize;
			iProgress().iProgress = obexProgress.iCurrentBytesTrans;
			break;
		case TObexMtmProgress::ESendComplete:
			iProgress().iState = ESendStateDone;
			break;
		case TObexMtmProgress::EMovedToSent:
			iProgress().iState = ESendStateDone;
			break;
		}
	// get error
	iProgress().iError = obexProgress.iError;
	return iProgress;
	}
