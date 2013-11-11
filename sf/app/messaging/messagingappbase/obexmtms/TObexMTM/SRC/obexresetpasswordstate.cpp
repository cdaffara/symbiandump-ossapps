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
// $Workfile: obexresetpasswordstate.cpp $
// $Author: Stevep $
// $Revision: 2 $
// $Date: 26/03/02 9:57 $
// 
//

#include "obexresetpasswordstate.h"

#include "obexharness.h"
#include "obextestutils.h"  // CObexTestUtils

CObexResetPasswordState::CObexResetPasswordState(CObexClientTest& aClientTest):
iClientTest(aClientTest)
	{
	}

void CObexResetPasswordState::StartL(TRequestStatus& aStatus)
	{
	iClientTest.ObexTestUtils().ResetCurrentObexMtmPasswordL();

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}
