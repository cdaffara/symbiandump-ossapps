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

#include "cimsocketidletimer.h"

#include "IMSK.H"


//
// Factory function for creating and returning a CImSocketIdleTimer instance
//
CImSocketIdleTimer* CImSocketIdleTimer::NewL(CImTextServerSession& aObserver)
	{
	CImSocketIdleTimer* self = new (ELeave) CImSocketIdleTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

//
// Construction
//
CImSocketIdleTimer::CImSocketIdleTimer(CImTextServerSession& aObserver)
: CTimer(CActive::EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

//
// Destruction
//
CImSocketIdleTimer::~CImSocketIdleTimer()
	{
	Cancel();
	}

// methods from CActive

void CImSocketIdleTimer::RunL()
	{
	// Need to tell the text server session that the timer has expired - stop the
	// current send/receive activity.
	iObserver.SocketIdle();
	}
