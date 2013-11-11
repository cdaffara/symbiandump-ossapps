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
// $Workfile: CreateObexMessageState.cpp $
// $Author: Simonk $
// $Revision: 5 $
// $Date: 31/01/02 10:07 $
// 
//


#include "CreateObexMessageState.h"

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
// CCreateObexMessageState
//

CCreateObexMessageState::CCreateObexMessageState(CObexClientTest& aClientTest) :
 iClientTest(aClientTest)
    {
    }

void CCreateObexMessageState::StartL(TRequestStatus& aStatus)
    {
    // create the entry in the message store - note that these
	// members have to be set up, or creating the entry panics

    CObexTestUtils& testUtils = iClientTest.ObexTestUtils();
    TMsvId service = testUtils.GetCurrentServiceId();
    TUid mtm = testUtils.CurrentObexMtm();
    (mtm == KUidMsgTypeBt) ? testUtils.InstantiateBTClientMtmL() : testUtils.InstantiateIRClientMtmL();
    CBaseMtm& client = testUtils.GetCurrentObexClientMtm();
    
	//Create an entry
	TMsvSelectionOrdering ordering;
	CMsvEntry* entry = CMsvEntry::NewL(*testUtils.iMsvSession, KMsvRootIndexEntryId, ordering);
	CleanupStack::PushL(entry);
		
	//Set the entry to the global outbox
	entry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
    //NB - the first thing this function does is to put the argument on to the clean up stack.
	client.SetCurrentEntryL(entry);
    CleanupStack::Pop();		//entry--since ownership is being passed
    
	//At last we can create the new message
	client.CreateMessageL(service);
    client.SaveMessageL();
    
    testUtils.SetEntryL(client.Entry().EntryId());

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);

    }

