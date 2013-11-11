// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Help Launcher module
// 
//

#include "HLPLCH.H"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "hlplch_internal.h"
#endif
#include <apgcli.h>
#include <barsc.h>
#include <bautils.h>
#include <lch.rsg>

EXPORT_C void HlpLauncher::LaunchHelpApplicationL(RWsSession& aWsSession, TUid aUid)
/** Launches the help application, passing a help file UID.

@param aWsSession Application's window server session
@param aUid Help file UID */
	{
	__UHEAP_MARK;
	TBuf<32> cmdLine;
	cmdLine.AppendNum((TInt)aUid.iUid);
	TPtr8 ptr((TText8*)cmdLine.Ptr(), cmdLine.Length(),cmdLine.Length());
	DoLaunchHelpApplicationL(aWsSession, ptr);
	__UHEAP_MARKEND;
	}

EXPORT_C void HlpLauncher::LaunchHelpApplicationL(RWsSession& aWsSession)
/** Launches the help application.

@param aWsSession Application's window server session */
	{
	__UHEAP_MARK;
	TBuf8<1> blank = _L8("");
	DoLaunchHelpApplicationL(aWsSession, blank);
	__UHEAP_MARKEND;
	}

EXPORT_C void HlpLauncher::LaunchHelpApplicationL(RWsSession& aWsSession, CArrayFix<TCoeHelpContext>* aContextList)
/** Launches the help application, passing an array of help contexts.

@param aWsSession Application's window server session
@param aContextList Array of help contexts */
	{
	__UHEAP_MARK;

	CHlpCmdLine* cmdLine=CHlpCmdLine::NewLC(aContextList);
	DoLaunchHelpApplicationL(aWsSession, cmdLine->CmdLineL());
	CleanupStack::PopAndDestroy();

	__UHEAP_MARKEND;
	}

void HlpLauncher::DoLaunchHelpApplicationL(RWsSession& aWsSession, const TDesC8& aCmdLine)
	{
	TApaTaskList taskList(aWsSession);
	TApaTask task = taskList.FindApp(KHlpAppUid);
	if (task.Exists())
		{
		task.SendMessage (KHlpAppWsMsg, aCmdLine); 
		task.BringToForeground();
		}
	else
		{
		CApaCommandLine* cmdLine=CApaCommandLine::NewLC();

		// INC057477 fix
		// Get and open the resource file
		RFs f;
		CleanupClosePushL(f);

		User::LeaveIfError(f.Connect());
		RResourceFile r;

		r.OpenL(f, KHelpLauncherResource);
		CleanupClosePushL(r);
		
		// Read the path of the help application from the resource file
		r.ConfirmSignatureL();
		TResourceReader resReader;
		resReader.SetBuffer(r.AllocReadLC(R_HELP_PATH));
		TPtrC16 helpPath = resReader.ReadTPtrC16();
		cmdLine->SetExecutableNameL(helpPath);
		CleanupStack::PopAndDestroy(); // AllocReadLC
		CleanupStack::PopAndDestroy(); // close r
		CleanupStack::PopAndDestroy(); // close f
		
		cmdLine->SetCommandL(EApaCommandOpen);
		cmdLine->SetTailEndL(aCmdLine);
		//EikDll::StartAppL(*cmdLine);
		RApaLsSession lsSession;
  		User::LeaveIfError(lsSession.Connect());
  		CleanupClosePushL(lsSession);
  		User::LeaveIfError(lsSession.StartApp(*cmdLine));
  		CleanupStack::PopAndDestroy(&lsSession);
		CleanupStack::PopAndDestroy(); //cmdLine
		}
	}

CHlpCmdLine* CHlpCmdLine::NewL(CArrayFix<TCoeHelpContext>* aContextList)
	{
	CHlpCmdLine* self=CHlpCmdLine::NewLC(aContextList);
	CleanupStack::Pop(); // self
	return self;
	}

CHlpCmdLine* CHlpCmdLine::NewLC(CArrayFix<TCoeHelpContext>* aContextList)
	{
	CHlpCmdLine* self=new(ELeave)CHlpCmdLine(aContextList);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHlpCmdLine::CHlpCmdLine(CArrayFix<TCoeHelpContext>* aContextList)
		:iContextList(aContextList)
	{
	}

CHlpCmdLine::~CHlpCmdLine()
	{
	delete iContextList;
	delete iCmdLine;
	}

void CHlpCmdLine::ConstructL()
	{
	iCmdLine=CBufFlat::NewL(KMaxCmdLineLength);
	}

TPtr8 CHlpCmdLine::CmdLineL()
	{
	BuildCmdLineL();
	TInt len = iCmdLine->Ptr(0).Size()/sizeof(TText8);
	TPtr8 ptr((TText8*)iCmdLine->Ptr(0).Ptr(),len,len);

	return ptr;
	}

void CHlpCmdLine::BuildCmdLineL()
	{
	TInt count=iContextList->Count();
	iCmdLine->InsertL(iCmdLine->Size(), &count, sizeof(TInt));
	for (TInt i=0; i < count; i++)
		{
		AddContextL(iContextList->At(i));
		}
	}

void CHlpCmdLine::AddContextL(TCoeHelpContext& aContext)
	{
	TInt length=aContext.iContext.Size();
	iCmdLine->InsertL(iCmdLine->Size(), &length, sizeof(TInt));
	iCmdLine->InsertL(iCmdLine->Size(), aContext.iContext.Ptr(), aContext.iContext.Size());
	iCmdLine->InsertL(iCmdLine->Size(), &aContext.iMajor, sizeof(TUid));
	}

