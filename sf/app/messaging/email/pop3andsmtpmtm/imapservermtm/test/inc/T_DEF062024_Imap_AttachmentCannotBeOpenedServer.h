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
//

#ifndef __T_DEF062024_IMAP_ATTACHMENTCANNOTBEOPENEDSERVER_H__
#define __T_DEF062024_IMAP_ATTACHMENTCANNOTBEOPENEDSERVER_H__

#include <testexecuteserverbase.h>
 
//
// ImapAttachmentServer
//

class CImapAttachmentServer : public CTestServer
	{
public:
	static CImapAttachmentServer* NewL();
	~CImapAttachmentServer();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();	
private:
	CImapAttachmentServer();
private:
	RFs iFs;
	};
#endif  //__T_DEF062024_IMAP_ATTACHMENTCANNOTBEOPENEDSERVER_H__