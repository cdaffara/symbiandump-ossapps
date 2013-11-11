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
// $Workfile: obexgetpasswordstate.cpp $
// $Author: Stevep $
// $Revision: 4 $
// $Date: 27/03/02 10:24 $
// 
//


#include "obexgetpasswordstate.h"

#include "obexharness.h"
#include "obextestutils.h"  // CObexTestUtils


CObexGetPasswordState::CObexGetPasswordState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)
    {
    }

void CObexGetPasswordState::StartL(TRequestStatus& aStatus)
	{
	HBufC* password = iClientTest.ObexTestUtils().GetCurrentObexMtmPasswordLC();
	
	if (password->Length())
		{
		_LIT(KPasswordIsString, "Password currently set to: ");
		HBufC* message = HBufC::NewLC(password->Length() + (&KPasswordIsString)->Length());
		TPtr messagePtr = message->Des();
		messagePtr.Append(KPasswordIsString);
		messagePtr.Append(*password);
		iClientTest.ObexTestUtils().WriteComment(*message);
		CleanupStack::PopAndDestroy(message);
		}
	else
		{
		_LIT(KNoPasswordWarning, "PASSWORD NOT SET!");
		iClientTest.ObexTestUtils().WriteComment(KNoPasswordWarning);
		}

	CleanupStack::PopAndDestroy(password);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}
	 
