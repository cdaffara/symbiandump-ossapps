
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


// User Include
#include "TestCalInterimApiInstance.h"
#include "TestCalInterimApiInstanceSuite.h"
#include "TestCalInterimApiInstanceData.h"


// Destructor for CActive object
CTestCalInterimApiInstanceData::~CTestCalInterimApiInstanceData()
	{
	Cancel();
	}

// Constructor for CActive object
CTestCalInterimApiInstanceData::CTestCalInterimApiInstanceData()
	: CActive(EPriorityHigh)
	{
	}
	
// for MCalProgressCallBack function
void CTestCalInterimApiInstanceData::Progress(TInt /*aPercentageCompleted*/) 
	{
	}
	
void CTestCalInterimApiInstanceData::Completed(TInt /*aError*/) 
	{
	// start of active object
	if(!IsActive())
		{
		SetActive();
		}
		
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	
	}
	
TBool CTestCalInterimApiInstanceData::NotifyProgress() 
	{
	return ETrue;
	}
	
// Cancellation of an outstanding request
void CTestCalInterimApiInstanceData::DoCancel()
	{
	}

// Handles the active object's request	
void CTestCalInterimApiInstanceData::RunL()
	{
	User::RequestComplete(iExtStatus,KErrNone);
	Close();
	}
	
// Stops the active scheduler
void CTestCalInterimApiInstanceData::Close()
	{
	CActiveScheduler::Stop();
	}

// Constructor for CActive object
CTestCalInterimApiInstanceData::CTestCalInterimApiInstanceData(TRequestStatus* aStatus)
	:CActive(0)
	{
	iExtStatus=aStatus;
	}




