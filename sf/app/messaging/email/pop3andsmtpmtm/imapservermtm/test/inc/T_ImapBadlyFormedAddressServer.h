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
//

#ifndef __T_IMAPADLYFORMEDADDRESSSERVER_H__
#define __T_IMAPADLYFORMEDADDRESSSERVER_H__

#include <testexecuteserverbase.h>

class CImapBadlyFormedAddressServer : public CTestServer
	{
public:
	static CImapBadlyFormedAddressServer* NewL();
	~CImapBadlyFormedAddressServer();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();

private:
	CImapBadlyFormedAddressServer();

private:
	RFs iFs;
	};

#endif  //__T_IMAPADLYFORMEDADDRESSSERVER_H__
