// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: obexsetpasswordstate.cpp $
// $Author: Stevep $
// $Revision: 2 $
// $Date: 26/03/02 9:46 $
// 
//

#include "obexsetpasswordstate.h"

#include "obexharness.h"
#include "obextestutils.h"  // CObexTestUtils

CObexSetPasswordState::CObexSetPasswordState(CObexClientTest& aClientTest) :
iClientTest(aClientTest)
	{
	}

CObexSetPasswordState* CObexSetPasswordState::NewL(CObexClientTest& aClientTest, const TDesC& aPassword)
	{
	CObexSetPasswordState* self = new (ELeave) CObexSetPasswordState(aClientTest);
	CleanupStack::PushL(self);
	self->ConstructL(aPassword);
	CleanupStack::Pop(self);
	return self;
	}

void CObexSetPasswordState::ConstructL(const TDesC& aPassword)
	{
	iPassword = aPassword.AllocL();
	}

CObexSetPasswordState::~CObexSetPasswordState()
	{
	delete iPassword;
	}

void CObexSetPasswordState::StartL(TRequestStatus& aStatus)
	{
	iClientTest.ObexTestUtils().SetCurrentObexMtmPasswordL(*iPassword);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}
