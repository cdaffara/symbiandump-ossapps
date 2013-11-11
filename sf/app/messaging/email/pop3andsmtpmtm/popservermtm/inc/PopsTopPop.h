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
// popservermtm module header
// @internalComponent
// @released
// 
//

#ifndef __POPSTOPPOP_H__
#define __POPSTOPPOP_H__

#include <pop3set.h>
#include <popsmtm.h>

class CImPop3Session;
class CImLogMessage;
class CImPop3Top;

//
// CImPop3TopPopulate 
//
// 
//
//
class CImPop3TopPopulate : public  CMsgActive
	{
public:
	static CImPop3TopPopulate *NewL(const CMsvEntrySelection&, CMsvServerEntry&, TInt aLimit, CImPop3Session*, RFs&, CImLogMessage* aLogMessage, TBool aDisconnectedMode, RArray<TUint>& aRemoteSizes);
	~CImPop3TopPopulate();

	void StartL(TRequestStatus& aStatus);
	TPop3Progress Progress();
	void Pause();
	void ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus);
	void CancelAllowResume();
	
private:

	CImPop3TopPopulate( CMsvServerEntry&, TInt aLimit, CImPop3Session*, RFs&, CImLogMessage* aLogMessage, TBool aDisconnectedMode, RArray<TUint>& aRemoteSizes);
	void ConstructL( const CMsvEntrySelection& );

	void DoTransferL(TMsvEntry& aEntry );
	void DoRetrieveL();
	void RetrievalCompleteL();

	void DoRunL();
	void DoComplete(TInt& aCompleteStatus);
	void DoCancel();

	void RunLProcessingL();
	void LogFetchedMessageL();



	enum 
		{
		EPopTopPopRetrieving, 
		EPopTopPopDeleting,
		EPopTopPopLogging
		};	
private:
	CMsvEntrySelection*			iSource;
	CMsvServerEntry&			iDestination;
	CImPop3Session*				iPopSession;
	CImPop3Top*					iTop;

	TBool						iCopy;
	RFs							iFs;
	TPop3Progress				iProgress;
	// CRecv things
	CImRecvConvert*				iRecvConverter;
	CImLogMessage*				iLogMessage;
	TBool						iDisconnectedMode;
	TMsvId						iServiceId;
	TInt						iPopTopPopState;
	TBool						iProcessComplete;
	TBool						iAlreadyComplete;
	TBool						iMessageMarkedForDelete;
	TBool						iRetrieveMessage;
	TInt						iMsgCtr;
	TMsvId						iDestId;
	TInt						iSavedError;
	TInt						iTopLimit;
	TInt						iCurrentMsgSize;
	RArray<TUint>				iRemoteSizes;
	TBool						iMigratingToNewBearer;
	};
#endif
