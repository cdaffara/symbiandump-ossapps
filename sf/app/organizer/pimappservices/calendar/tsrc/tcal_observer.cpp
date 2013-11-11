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

#include "tcal_observer.h"
#include "tcal_observer_manager.h"


void CTestApp::ConstructL()
	{
	TInt error = iSemaphore.CreateGlobal(KSemName(), 0);
	
	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphore.OpenGlobal(KSemName());
		}
	else
		{
		User::Leave(error);
		}
	
	iChunk.CreateGlobal(KTestCaseChunkName(), sizeof(TInt), sizeof(TInt));	
	iChunk.OpenGlobal(KTestCaseChunkName(), ETrue);
	iTestCase = reinterpret_cast<TInt*>(iChunk.Base());
	}
	
CTestApp::~CTestApp()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
		
	delete iTimer;
	iChunk.Close();
	iSemaphore.Close();
	}
	
void CTestApp::Completed(TInt /*aError*/)
	{
	if(iActiveSchedulerStarted)
		{
		CActiveScheduler::Stop();
		}
	}
