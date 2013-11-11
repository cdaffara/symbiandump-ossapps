/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include "CMobileInfo.h"

CMobileInfo::CMobileInfo(TDes& aMCC)
	: CActive(CActive::EPriorityStandard),
	iMCC(aMCC),
	iMCCMNCV1Pkg(iMCCMNCV1)
	{
	CActiveScheduler::Add(this);
	}

CMobileInfo::~CMobileInfo()
	{
	Cancel();
	delete iTelephony;
	delete iWait;
	}

void CMobileInfo::GetInfoL(TDes& aMCC)
	{
	CMobileInfo* self= new (ELeave) CMobileInfo(aMCC);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::PopAndDestroy(self);
	}

void CMobileInfo::ConstructL()
	{
	iWait = new (ELeave) CActiveSchedulerWait();

	iTelephony = CTelephony::NewL();
	iTelephony->GetCurrentNetworkInfo(iStatus, iMCCMNCV1Pkg);
	SetActive();

	iWait->Start();
	}

void CMobileInfo::RunL()
	{
	if(iStatus==KErrNone)
		{
		iMCC = iMCCMNCV1.iCountryCode;
		}
	iWait->AsyncStop();
	}

void CMobileInfo::DoCancel()
	{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkInfoCancel);
	}

// End of file
