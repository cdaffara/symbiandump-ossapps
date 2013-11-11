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
// t_DEF062024_IMAP21cnServer_AttachmentCannotBeOpened.h
// 
//

#ifndef __T_DEF062024_IMAP21CNSERVER_ATTACHMENTCANNOTBEOPENEDSTEP_H__
#define __T_DEF062024_IMAP21CNSERVER_ATTACHMENTCANNOTBEOPENEDSTEP_H__
 
#include <test\testexecutestepbase.h>
#include "emailtestutils.h"
#include "smstestutils.h"
#include <stdlib.h>
#include <msvapi.h>
#include <smuthdr.h> 
#include <iapprefs.h>
#include "T_DEF062024_Imap_AttachmentCannotBeOpenedServer.h"
#include "t_def062024_imapclient.h"

//
// CTestImapAttachmentStep
//

class CTestImapAttachmentStep : public CTestStep , MImapTestEventHandler
	{
public:
	CTestImapAttachmentStep();
	~CTestImapAttachmentStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void GetDetailsFromIniFileL();
	void CreateImapAccountL();
	void PrintEntryTime(TTime* aTIme);
	TVerdict CheckAttachmentL();
	virtual void TestComplete(TInt aErrorCode);
	
private:
	CSpoofServer* iSpoofServer;
	CActiveImapClient* iImapClient;
	CConsoleBase* iConsole;
	CActiveScheduler* iScheduler;
	TMsvId	iImapService;
	CEmailTestUtils* iTestUtils;
	CMsvSession* iSession;
	TInt iMessageEntryCount;
	TDummySessionObserver* iSessionObserver;
	TPtrC16 iScriptFile;
	// Path of the source file originally attached to the email fetched
	TPtrC16 iAttachmentSourcePath;
	// Index of the attachment we want to check in this email
	TInt iAttachmentIndex;
	};

 		
_LIT(KTestImapAttachment,"TestImapAttachment");
	
#endif  //__T_DEF062024_IMAP21CNSERVER_ATTACHMENTCANNOTBEOPENEDSTEP_H__