// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Testing Recieving of messages
// 
//

#include <e32test.h>
#include <f32fsys.h>
#include <s32file.h>
#include <e32base.h>
#include <txtetext.h>
#include <txtrich.h>
#include <bautils.h>
#include <miuthdr.h>
#include <miutmsg.h>
#include <miutset.h>  //KUidMsgTypePOP3
#include <imutdll.h>	
#include <imcvtext.h>
#include <imcvrecv.h>
#include <imcvsend.h>

#include <imapset.h>

//Oyster includes
#include <msvstd.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvruids.h>
#include <msvreg.h>

#include "msvindexadapter.h"

#include "msvserv.h"
#include <msventry.h> 
#include <msvapi.h>
#include <mtclbase.h>
#include <offop.h>

#include "t_log.h"

//----------------------------------------------------------------------------------------

_LIT(KFilePathMailTest, "c:\\mailtest\\");

//----------------------------------------------------------------------------------------

LOCAL_D RTest test(_L("T_DUMP test harness"));

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D RFs theFs;	
LOCAL_C CActiveScheduler* theScheduler;

class CTestTimer;
LOCAL_D	CTestTimer* timer;

class CDummyMsvSessionObserver;

LOCAL_C CMsvServer* theServer;

//----------------------------------------------------------------------------------------

class CDummyMsvSessionObserver : public CBase, public MMsvSessionObserver
	{
public:
	void HandleSessionEvent(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
		{};
	};
class CTestTimer : public CTimer
	{
public:
	static CTestTimer* NewL();
private:
	CTestTimer();
	void RunL();	
	};

CTestTimer::CTestTimer() : CTimer(EPriorityLow) {}

void CTestTimer::RunL()
	{
	CActiveScheduler::Stop();
	}

CTestTimer* CTestTimer::NewL()
	{
	CTestTimer* self = new(ELeave) CTestTimer();
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

class CTestActive : public CActive
	{
public:
	CTestActive();
	void ConstructL();
//	void ChangeEntryL(TMsvId aId);
	~CTestActive();
private:
	void RunL();
	void DoCancel();
private:
//	CImCalculateMsgSize* iMsgSize;
	};

LOCAL_C CTestActive* active;

//ctestactive
CTestActive::CTestActive()
: CActive(EPriorityStandard)
	
	{
	__DECLARE_NAME(_S("CTestActive"));
	}

CTestActive::~CTestActive()
	{
	Cancel();
//	delete iMsgSize;
	}

void CTestActive::ConstructL()
	{
	CActiveScheduler::Add(this);	
//	iMsgSize = CImCalculateMsgSize::NewL(theFs, *sEntry);
	}

#if 0
void CTestActive::ChangeEntryL(TMsvId /*aId*/)
	{
//	TTime time;
//	iMsgSize->StartL(iStatus, aId, ENoAlgorithm, time, domainName);
	SetActive();
	}
#endif

void CTestActive::RunL()
	{
//	TInt32 size=iMsgSize->MessageSize();
//	test.Printf(TRefByValue<const TDesC>_L("message size is %d\n\n"), size);
	CActiveScheduler::Stop();
	}

void CTestActive::DoCancel()
	{
//	iMsgSize->Cancel();
	}

//----------------------------------------------------------------------------------------
LOCAL_C void CreateAllTestDirectories()
//----------------------------------------------------------------------------------------	
	{
	// create c:\mailtest, c:\mailtest\imcv c:\mailtest\attach\ directories used by this test harness
	RFs s;
	TFileName buffer;
	if(!s.Connect())
		{
		TBool stop=EFalse;
		s.SetSessionPath(_L("c:\\"));
		if(!s.MkDir(KFilePathMailTest))
			{
			buffer=KFilePathMailTest;
			test.Printf(TRefByValue<const TDesC>_L("Created %S directory\n"),&buffer);
			stop=ETrue;
			}
#if 0
		if(!s.MkDirAll(KTestMessageDir))
			{
			buffer=KTestMessageDir;
			test.Printf(TRefByValue<const TDesC>_L("Created %S directory\n"),&buffer);
			stop=ETrue;
			}
		if(!s.MkDirAll(KLogsDir))
			{
			buffer=KLogsDir;
			test.Printf(TRefByValue<const TDesC>_L("Created %S directory\n"),&buffer);
			stop=ETrue;
			}
#endif
		if(stop)
			{
			test.Printf(_L("[Press any key to continue...]\n\n"));
			test.Getch();
			}
		s.Close();
		}
	}

//----------------------------------------------------------------------------------------
LOCAL_C void Up()
//----------------------------------------------------------------------------------------
	{
	theScheduler = new (ELeave)CActiveScheduler;
	CActiveScheduler::Install( theScheduler );
	CleanupStack::PushL(theScheduler);

	User::LeaveIfError(theFs.Connect());
	theFs.SetSessionPath(_L("C:\\"));

#if 0
	theFs.SetSessionPath( KFilePathMailTest );

	// Delete out the logs
	CFileMan* fileMan=CFileMan::NewL(theFs);
	fileMan->Delete(KLogsDir, CFileMan::ERecurse);
	delete fileMan;
#endif
	
	// create a CMsvServer
	timer = CTestTimer::NewL();
	theServer = CMsvServer::NewL();
	CleanupStack::PushL(theServer);

	timer->After(100000);
	CActiveScheduler::Start();
	delete timer;

	active = new (ELeave) CTestActive();
	CleanupStack::PushL(active);
	active->ConstructL();	
	}

//----------------------------------------------------------------------------------------
LOCAL_C void Down()
//----------------------------------------------------------------------------------------
	{
	test.Printf(_L("---------------------\n"));
	test.Printf(_L("      Going down     \n"));
	test.Printf(_L("---------------------\n"));
	CleanupStack::PopAndDestroy(3);  //active, msvserver, theScheduler
	theFs.Close();
	}

//----------------------------------------------------------------------------------------
LOCAL_C void doMainL()
//----------------------------------------------------------------------------------------
	{
	CreateAllTestDirectories();
	Up();

	test.Console()->ClearScreen();
	test.Console()->SetPos(0, 0);
	test.Printf(_L("---------------------\n"));
	test.Printf(_L("    Message dumper   \n"));
	test.Printf(_L("---------------------\n"));
		
	__UHEAP_MARK;
	COutputter* putter = COutputter::NewL( &test );
	CleanupStack::PushL(putter);

	putter->SetRFs( &theFs );
	theFs.SetSessionPath( KFilePathMailTest );

	CMsvServerEntry* entry = CMsvServerEntry::NewL( *theServer, KMsvNullIndexEntryId );
	CleanupStack::PushL(entry);

	// enable invisible entries
	TMsvSelectionOrdering sort = entry->Sort();
	sort.SetShowInvisibleEntries(ETrue);
	entry->SetSort(sort);
	
	CMsgOutputter* dumper = CMsgOutputter::NewL( putter, entry );
	CleanupStack::PushL(dumper);

	dumper->DumpL( KMsvRootIndexEntryId, CMsgOutputter::EToOneFile,
				   CMsgOutputter::EToOneFile, ETrue, ETrue);

	CleanupStack::PopAndDestroy(3); // dumper, putter, entry
	__UHEAP_MARKEND;

	Down();
	}

//----------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
//----------------------------------------------------------------------------------------
	{	
	__UHEAP_MARK;
	test.Start(_L("T_DUMP Test harness"));
	theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());

	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
