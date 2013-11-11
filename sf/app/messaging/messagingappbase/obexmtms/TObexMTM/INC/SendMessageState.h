// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: SendMessageState.h $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 23/01/02 14:53 $
// 
//

#ifndef __SENDMESSAGESTATE_H__
#define __SENDMESSAGESTATE_H__

#include "harness.h"        //CActiveTestState

// Stub support.
const TInt KMaxStubResultStrLength = 40;

class CObexClientTest;


class CSendMessageState : public CActiveTestState
/**
 * Sends an outgoing message. This is a long-running, asynchronous
 * operation. The state is therefore implemented as an active object.
 * It is possible to check the progress of this operation and cancel
 * it before it completes.
 */
	{
public:
	CSendMessageState(const TDesC& aStubDesiredResult, CObexClientTest& aClientTest);
	void StartL(TRequestStatus& aStatus);

	~CSendMessageState();
	void RunL();
	void DoCancel();

	static TInt TimerCallback(TAny* aPtr);

protected:
	CObexClientTest& iClientTest;
	TUint32 iTargetID;

private:
	CMsvEntrySelection* iSelection;
	CPeriodic* iTimer;
	TBool iSendComplete;
	TBool iCancel;
	TBool iCancelled;
	TBool iCheckProgress;
	TBuf<64> iConnectPassword;
	// Stub support.
	TBuf<KMaxStubResultStrLength>	iStubDesiredResult; // Expected to returned from the stub.
	};


#endif //__SENDMESSAGESTATE_H__
