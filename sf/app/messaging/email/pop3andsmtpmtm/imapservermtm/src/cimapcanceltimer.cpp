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

#include "cimapcanceltimer.h"

#include "imapsess.h"

CImapCancelTimer* CImapCancelTimer::NewL(CImImap4Session& aSession)
	{
	CImapCancelTimer* self = new (ELeave) CImapCancelTimer(aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CImapCancelTimer::~CImapCancelTimer()
	{
	Cancel();
	}

CImapCancelTimer::CImapCancelTimer(CImImap4Session& aSession)
: CTimer(CActive::EPriorityHigh), iSession(aSession)
	{
	CActiveScheduler::Add(this);
	}

/*
 * methods from CActive
 */
 
void CImapCancelTimer::RunL()
	{
	iSession.CancelTimerExpired();
	}
