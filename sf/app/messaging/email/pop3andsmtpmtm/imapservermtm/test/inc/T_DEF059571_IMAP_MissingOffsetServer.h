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
// t_def057591_imap_missingoffsetserver.h
// 
//

#ifndef __T_DEF057591_IMAP_MISSINGOFFSETSERVER_H__
#define __T_DEF057591_IMAP_MISSINGOFFSETSERVER_H__

#include <testexecuteserverbase.h>
 
//
// CImapMissingOffsetServer
//

class CImapMissingOffsetServer : public CTestServer
	{
public:
	static CImapMissingOffsetServer* NewL();
	~CImapMissingOffsetServer();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();	
private:
	CImapMissingOffsetServer();
private:
	RFs iFs;
	};

#endif  //__T_DEF057591_IMAP_MISSINGOFFSETSERVER_H__