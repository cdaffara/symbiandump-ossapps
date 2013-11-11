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
// $Workfile: ObexFileDebugReceiveState.cpp $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 24/01/02 16:29 $
// 
//

#include "ObexFileDebugReceiveState.h"      //CObexFileDebugReceiveState

// User includes
#include "obexharness.h"
#include "obextestutils.h"  // CObexTestUtils

CObexFileDebugReceiveState::CObexFileDebugReceiveState(const TDesC& aSource, const TDesC& aDestination, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iSource(aSource),
 iDestination(aDestination)
    {
    }

void CObexFileDebugReceiveState::StartL(TRequestStatus& aStatus)
    {
    CFileMan* fileMan = CFileMan::NewL(iClientTest.ObexTestUtils().iMsvSession->FileSession());
    CleanupStack::PushL(fileMan);                                               // PUSH
    User::LeaveIfError(fileMan->Copy(iSource, iDestination));
    CleanupStack::PopAndDestroy(); // fileMan                                   // POP(1)

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }
