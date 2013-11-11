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

#ifndef __POPSCPMV_H__
#define __POPSCPMV_H__

#include <pop3set.h>

class CImPop3Session;
class CImLogMessage;
class CImppServerMtm;
class CImPop3Delete;
class CImPop3Retr;
class CImPop3TransferMessage;
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;	

//
// CImPop3CopyMove
//
// 
//
//
class CImPop3CopyMove : public  CMsgActive
	{
public:
	static CImPop3CopyMove *NewL(const CMsvEntrySelection&, CMsvServerEntry&, CImPop3Session*, TBool, RFs&, TMsvId, CImLogMessage* aLogMessage, TBool aDisconnectedMode);
	static CImPop3CopyMove *NewL(const CMsvEntrySelection&, CMsvServerEntry&, CImPop3Session*, TBool, RFs&, CImLogMessage* aLogMessage, TBool aDisconnectedMode);
	~CImPop3CopyMove();

	void StartL(TRequestStatus& aStatus);
	TPop3Progress Progress();
	void Pause();
	void ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus);
	void CancelAllowResume();

private:
	CImPop3CopyMove( CMsvServerEntry&, CImPop3Session*, TBool, RFs&, TMsvId, CImLogMessage* aLogMessage, TBool aDisconnectedMode);
	CImPop3CopyMove( CMsvServerEntry&, CImPop3Session*, TBool, RFs&, CImLogMessage* aLogMessage, TBool aDisconnectedMode);
	void ConstructL( const CMsvEntrySelection& );

	void DoTransferL(TMsvEntry& aEntry );
	void DoRetrieveL();
	void RetrievalCompleteL();

	void DoRunL();
	void DoComplete(TInt& aCompleteStatus);
	void DoCancel();

	void RunLProcessingL();
	void LogFetchedMessageL();
	void LeaveIfLowDiskL(const CMsvEntrySelection& aMsgSelection);
	
private:

	CMsvEntrySelection*			iSource;
	CMsvServerEntry&			iDestination;
	
	CImPop3Session*				iPopSession;
	CImPop3Delete*				iDelete;
	CImPop3Retr*				iRetrieve;

	TBool						iCopy;
	RFs							iFs;
	TPop3Progress				iProgress;
	// CRecv things
	CImRecvConvert*				iRecvConverter;
	CImPop3TransferMessage*		iTransfer;

	TInt						iMsgCtr;
	TInt						iPopCopyMoveState;
	TBool						iRetrieveMessage;
	TBool						iProcessComplete;
	TMsvId						iServiceId;
	TMsvId						iDestId;
	TMsvEntry					iEntry;
	TBool						iPopulate;
	CImLogMessage*				iLogMessage;
	TInt						iSavedError;
	TBool						iDoingTransfer;
	TBool						iDisconnectedMode;
	TBool						iAlreadyComplete;
	TBool						iMessageMarkedForDelete;
	TBool						iMigratingToNewBearer;
	};

//
// Defines copy/move from remote mailbox to remote collection
//
enum
	{ EPopCpMvRetrieving, EPopCpMvDeleting, EPopCpMvLogging };
#endif
