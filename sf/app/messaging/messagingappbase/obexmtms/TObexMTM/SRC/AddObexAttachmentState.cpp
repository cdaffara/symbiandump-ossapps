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
// $Workfile: AddObexAttachmentState.cpp $
// $Author: Simonk $
// $Revision: 4 $
// $Date: 29/01/02 11:23 $
// 
//

#include "AddObexAttachmentState.h"

//TODO:- check that all these headers are required

// System includes
#include <btmsgtypeuid.h>   // KUidMsgTypeBt
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmsgtypeuid.h>   // KUidMsgTypeIr
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <obexMtmUtil.h>    // CObexMtmFileNameExternaliser
#include <msvuids.h>        // KUidMsvMessageEntry
#include <obexclientmtm.h>

#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>

#include "casyncwaiter.h"

// User includes
#include "obexharness.h"
#include "obextestutils.h"  // CObexTestUtils



//
// CAddObexAttachmentAsEntryState
//

CAddObexAttachmentAsEntryState::CAddObexAttachmentAsEntryState(const TDesC& aFileName, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iFileName(aFileName)
	{
    }

void CAddObexAttachmentAsEntryState::StartL(TRequestStatus& aStatus)
    {
    // Add an attachment as a child entry

    CBaseMtm& clientMtm = iClientTest.ObexTestUtils().GetCurrentObexClientMtm();

	CMsvEntry& entry = clientMtm.Entry();
	CMsvStore* store = entry.EditStoreL();
	CleanupStack::PushL(store);
	
	CAsyncWaiter* waiter = CAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachment);
	TParse fileNameParser;
	User::LeaveIfError(fileNameParser.Set(iFileName, NULL, NULL));
	attachment->SetAttachmentNameL(fileNameParser.NameAndExt());
	store->AttachmentManagerL().AddAttachmentL(iFileName, attachment, waiter->iStatus);
	CleanupStack::Pop(attachment);
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());
	CleanupStack::PopAndDestroy(waiter);
	
	store->CommitL();
	CleanupStack::PopAndDestroy(store);

    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CAddObexAttachmentByNameState
//

CAddObexAttachmentByNameState::CAddObexAttachmentByNameState(const TDesC& aFileName, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iFileName(aFileName)
    {
    }

void CAddObexAttachmentByNameState::StartL(TRequestStatus& aStatus)
    {
	// Get the attachment list and append the file path to it
	RPointerArray<CObexTestFilenameAndHeaders>& attachments = iClientTest.Attachments();
	
	CObexTestFilenameAndHeaders* obexPackage = CObexTestFilenameAndHeaders::NewL();
	CleanupStack::PushL(obexPackage);
	// set the filename but no headers
	obexPackage->iFileName = iFileName;
	
	User::LeaveIfError(attachments.Append(obexPackage));
	CleanupStack::Pop(obexPackage); // ownership passed to array

	// Complete the state.
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


//
// CAddObexAttachmentByNameWithHeadersState
//

CAddObexAttachmentByNameWithHeadersState::CAddObexAttachmentByNameWithHeadersState(const TDesC& aFileName, CObexClientTest& aClientTest) :
 iClientTest(aClientTest),
 iFileName(aFileName)
    {
    }

void CAddObexAttachmentByNameWithHeadersState::StartL(TRequestStatus& aStatus)
    {
	// Get the attachment list and append the file path to it
	RPointerArray<CObexTestFilenameAndHeaders>& attachments = iClientTest.Attachments();

	CObexTestFilenameAndHeaders* obexPackage = CObexTestFilenameAndHeaders::NewL();
	CleanupStack::PushL(obexPackage);
	// set the filename and the headers
	obexPackage->iFileName = iFileName;
	CObexHeaderList* currentHeaders = iClientTest.CurrentHeaders();
	obexPackage->iHeaderList = currentHeaders;
	iClientTest.ReleaseCurrentHeaders();	// Release ownership of current headers.
	
	User::LeaveIfError(attachments.Append(obexPackage));
	CleanupStack::Pop(obexPackage); // ownership passed to array

	// Complete the state.
    TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
    }


