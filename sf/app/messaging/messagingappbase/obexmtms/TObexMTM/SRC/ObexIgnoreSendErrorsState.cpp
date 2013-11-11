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
// $Workfile: ObexIgnoreSendErrorsState.cpp $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 29/01/02 17:44 $
// 
//


#include "obexignoresenderrorsstate.h"

//TODO:- check that all these headers are required

// System includes
#include <btmsgtypeuid.h>   // KUidMsgTypeBt
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmsgtypeuid.h>   // KUidMsgTypeIr
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <msvuids.h>        // KUidMsvMessageEntry

// User includes
#include "obexharness.h"    // CObexClientTest


CObexIgnoreSendErrorsState::CObexIgnoreSendErrorsState(CObexClientTest& aClientTest) :
iClientTest(aClientTest)
    {
    }

void CObexIgnoreSendErrorsState::StartL(TRequestStatus& aStatus)
    {
    iClientTest.SetIgnoreSendErrors(ETrue);
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }
