// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CASYNCWAITER.H
// 
//

#include "casyncwaiter.h"

CAsyncWaiter* CAsyncWaiter::NewL()
	{
	CAsyncWaiter* self = new(ELeave) CAsyncWaiter();
	return self;
	}

CAsyncWaiter::CAsyncWaiter()
	: CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}	

CAsyncWaiter::~CAsyncWaiter()
	{
	Cancel();
	}
	
void CAsyncWaiter::StartAndWait()
	{
	SetActive();
	CActiveScheduler::Start();
	}
	
TInt CAsyncWaiter::Result() const
	{
	return iError;
	}
	
void CAsyncWaiter::RunL()
	{
	iError = iStatus.Int();
	CActiveScheduler::Stop();
	}
	
void CAsyncWaiter::DoCancel()
	{
	iError = KErrCancel;
	CActiveScheduler::Stop();
	}

