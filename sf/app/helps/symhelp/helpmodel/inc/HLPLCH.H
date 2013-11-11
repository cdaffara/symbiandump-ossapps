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

#ifndef __HLPLCH_H__
#define __HLPLCH_H__

#if !defined(__APGTASK_H__)
#include <apgtask.h>
#endif

#if !defined(__EIKDLL_H__)
#include <eikdll.h>
#endif

#if !defined(__WSERV_H__)
#include <w32std.h>
#endif

#if !defined(__COEHELP_H__)
#include <coehelp.h>
#endif

/** Help application UID. 
@internalComponent
@released
*/
const TUid KHlpAppUid={ 0x10005234 };

/** UID of window message to send to application. 
@internalComponent
@released
*/
const TUid KHlpAppWsMsg={ 0x100055c7 };

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** Maximum length of command line to launch application. 
@internalComponent
@released
*/
const TInt KMaxCmdLineLength = 512;

// resource file name
_LIT(KHelpLauncherResource, "z:\\resource\\apps\\lch.rsc");

class CHlpCmdLine : public CBase
/**
@internalComponent
@released
*/
	{
public:
	static CHlpCmdLine* NewL(CArrayFix<TCoeHelpContext>* aContextList);
	static CHlpCmdLine* NewLC(CArrayFix<TCoeHelpContext>* aContextList);
	~CHlpCmdLine();
public:
	TPtr8 CmdLineL();
private:
	void BuildCmdLineL();
	void AddContextL(TCoeHelpContext& aContext);
private:
	void ConstructL();
	CHlpCmdLine(CArrayFix<TCoeHelpContext>* aContextList);
private:
	CArrayFix<TCoeHelpContext>* iContextList;
	CBufFlat* iCmdLine;
	};
	
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class HlpLauncher
/** Launches the help application.

It assumes that such an application is at z:\\Sys\\Bin\\CsHelp.exe

@publishedAll
@released
*/
	{
public:
	IMPORT_C	static void LaunchHelpApplicationL(RWsSession& aWsSession, CArrayFix<TCoeHelpContext>* aContextList);
	IMPORT_C	static void LaunchHelpApplicationL(RWsSession& aWsSession, TUid aUid);
	IMPORT_C	static void LaunchHelpApplicationL(RWsSession& aWsSession);
private:
	static void DoLaunchHelpApplicationL(RWsSession& aWsSession, const TDesC8& aCmdLine);
	};

#endif
