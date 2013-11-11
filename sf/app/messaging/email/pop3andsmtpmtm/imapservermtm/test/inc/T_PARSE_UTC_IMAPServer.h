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
// t_parse_utc_imap_server.h
// 
//

#ifndef __T_PARSE_UTC_IMAP_SERVER_H__
#define __T_PARSE_UTC_IMAP_SERVER_H__

#include <testexecuteserverbase.h>
 
//
// ParseUTCServer
//

class CParseUTCServer : public CTestServer
	{
public:
	static CParseUTCServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();	
private:
	RFs iFs;
	};
#endif  //__T_PARSE_UTC_IMAP_SERVER_H__