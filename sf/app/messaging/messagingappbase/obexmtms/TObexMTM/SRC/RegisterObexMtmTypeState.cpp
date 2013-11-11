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
// $Workfile: RegisterObexMtmTypeState.cpp $
// $Author: Robertl $
// $Revision: 2 $
// $Date: 22/08/02 17:14 $
// 
//


#include "RegisterObexMtmTypeState.h"

//TODO:- check that all these headers are required

// System includes
#include <btmsgtypeuid.h>   // KUidMsgTypeBt
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmsgtypeuid.h>   // KUidMsgTypeIr
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <msvuids.h>        // KUidMsvMessageEntry

// User includes
#include "obexharness.h"
#include "obextestutils.h"

//
// CRegisterObexMtmTypeState
//

CRegisterObexMtmTypeState::CRegisterObexMtmTypeState(const TDesC& aType, CObexClientTest& aClientTest) :
 iType(aType), 
 iClientTest(aClientTest)
    {

    }

void CRegisterObexMtmTypeState::StartL(TRequestStatus& aStatus)
    {
	_LIT(KBT, "BT");

	iClientTest.ObexTestUtils().GoClientSideL();

    if (iType.CompareF(KBT) == KErrNone) 
		{
        iClientTest.ObexTestUtils().RegisterBTMtmL();
		}
	else
		{
		iClientTest.ObexTestUtils().RegisterIRMtmL();
		}
    
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }




