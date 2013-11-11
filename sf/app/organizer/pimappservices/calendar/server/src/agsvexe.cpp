// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "agsmain.h" //only required for EKA2
#include "agmserv.h"

static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// the active scheduler and CAgnServer are both created from undertaker
	CAgnServUndertaker* undertaker = CAgnServUndertaker::NewL();
	CleanupStack::PushL(undertaker);

	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread::RenameMe(KAgendaServerName));
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(undertaker);
	}

TInt E32Main()
// Agenda server process entry point
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r= KErrNoMemory;
	
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		delete CActiveScheduler::Current();
		}
	//
	__UHEAP_MARKEND;
	return r;
	}
