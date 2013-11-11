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

#ifndef __CIMAPCANCELTIMER_H__
#define __CIMAPCANCELTIMER_H__

#include <e32base.h>

class CImImap4Session;

/**
@internalTechnology
@released
*/
NONSHARABLE_CLASS(CImapCancelTimer) : public CTimer
	{
public:
	static CImapCancelTimer* NewL(CImImap4Session& aSession);
	virtual ~CImapCancelTimer();

private:	// methods from CActive
	virtual void RunL();
	
private:
	CImapCancelTimer(CImImap4Session& aSession);
	
private:
	CImImap4Session& iSession;
	};

#endif	//__CIMAPCANCELTIMER_H__