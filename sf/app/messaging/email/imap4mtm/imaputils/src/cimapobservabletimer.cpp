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

#include "cimapobservabletimer.h"

/**
A static method that constructs a timer object.
@param aObserver	An implementation of the MImapTimerObserver interface whose OnTimerL() method will
					be called whenever a requested timer event occurs.
@return A pointer to the newly created timer object
*/
EXPORT_C CImapObservableTimer* CImapObservableTimer::NewL(MImapTimerObserver& aObserver)
	{
	CImapObservableTimer* self = new(ELeave)CImapObservableTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(); // this calls CTimer::ConstructL()
	CleanupStack::Pop(self);
	return self;
	}

CImapObservableTimer::CImapObservableTimer(MImapTimerObserver& aObserver)
	: CTimer(CActive::EPriorityStandard)
	, iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CImapObservableTimer::RunL()
	{
	iObserver.OnTimerL(*this);
	}
