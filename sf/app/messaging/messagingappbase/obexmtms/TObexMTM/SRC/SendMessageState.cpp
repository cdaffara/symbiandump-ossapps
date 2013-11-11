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
// $Workfile: SendMessageState.cpp $
// $Author: Stevep $
// $Revision: 8 $
// $Date: 27/03/02 10:49 $
// 
//

#include "sendmessagestate.h"

//TODO:- check that all these headers are required

// System includes
#include <btmsgtypeuid.h>   // KUidMsgTypeBt
#include <btmtmcmds.h>      // TBtMtmAsyncCmds
#include <irmsgtypeuid.h>   // KUidMsgTypeIr
#include <irmtmcmds.h>      // TIrMtmAsyncCmds
#include <msvuids.h>        // KUidMsvMessageEntry
#include <obexclientmtm.h>
#include <btcmtm.h>
#include <ircmtm.h>

#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>

#include "cobextestfilenameandheaders.h"
#include "casyncwaiter.h"

// User includes
#include "obexharness.h"
#include "obextestutils.h"
#include "obextestheaderlist.h"

// Stub support.
_LIT(KStubConfigFolderName, "c:\\msgtest\\obexsmoke\\stub\\");
_LIT(KStubConfigFileName, "c:\\msgtest\\obexsmoke\\stub\\conf.txt");

// To test with the old "CObexClientMtm::STimeouts" structure, updated with
// new "CIrClientMtm::STimeouts" structure by DEF36727, remove comment below.
//#define VERIFY_DEF36727_USING_OLD_STURCT

//
// CSendMessageState
//

CSendMessageState::CSendMessageState(const TDesC& aStubDesiredResult, CObexClientTest& aClientTest)
	:	CActiveTestState(EPriorityNormal),
		iClientTest(aClientTest),
		iStubDesiredResult(aStubDesiredResult)
	{
	iTimer = CPeriodic::NewL(EPriorityNormal);
	CActiveScheduler::Add(this);
	}

void CSendMessageState::StartL(TRequestStatus& aStatus)
	{
	// If the "stub" folder is present, externalize desired result and
	// verify headers.

	RFs rfs;
	CleanupClosePushL(rfs);		// PUSH
	User::LeaveIfError(rfs.Connect());

	RDir dir;
	CleanupClosePushL(dir);

	TInt fileErr = dir.Open(rfs, KStubConfigFolderName, KEntryAttDir);
	if (fileErr == KErrNone)
		{
		CObexTestHeaderList* verifyHeaderList = iClientTest.VerifyHeaders();

		verifyHeaderList->AutoExternaliseL(KStubConfigFileName, iStubDesiredResult);

		// Clear the verify headers, ready for the next send.
		iClientTest.ClearVerifyHeadersL();
		}

	CleanupStack::PopAndDestroy(2);


	
	// Add any attachments.
	iReportStatus = &aStatus;
    CObexTestUtils& testUtils = iClientTest.ObexTestUtils();
	CObexClientMtm&	obexMtm = (CObexClientMtm&)testUtils.GetCurrentObexClientMtm();
	
	// Create a "selection" object from the currently selected entry.
	iSelection = new (ELeave) CMsvEntrySelection;
    iSelection->AppendL(testUtils.Entry().Id());

	// Add each attachment to the message, if any.
	RPointerArray<CObexTestFilenameAndHeaders>& attachments = iClientTest.Attachments();

	if (attachments.Count() > 0)
		{
		CMsvEntry& entry = obexMtm.Entry();
		CMsvStore* store = entry.EditStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& manager = store->AttachmentManagerL();
		TInt count = manager.AttachmentCount();
		for( TInt ii=0; ii<count; ++ii )
			{
			CAsyncWaiter* waiter = CAsyncWaiter::NewL();
			CleanupStack::PushL(waiter);
			
			CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvLinkedFile);
			CleanupStack::PushL(attachment);
			// Add the headers if any have been set
			CObexHeaderList* headerList = attachments[ii]->iHeaderList;
			if( headerList != NULL )
				{
				headerList->ExportToAttachmentL(*attachment);
				}
			
			TParse fileNameParser;
			User::LeaveIfError(fileNameParser.Set(attachments[ii]->iFileName, NULL, NULL));
			attachment->SetAttachmentNameL(fileNameParser.NameAndExt());
			manager.AddLinkedAttachmentL(attachments[ii]->iFileName, attachment, waiter->iStatus);
			CleanupStack::Pop(attachment);
			waiter->StartAndWait();
			User::LeaveIfError(waiter->Result());
			CleanupStack::PopAndDestroy(waiter);
			}
		store->CommitL();
		CleanupStack::PopAndDestroy(store);
		}
	iClientTest.ClearAttachmentsL();


	// Now carry out the send.
	if(testUtils.CurrentObexMtm() == KUidMsgTypeBt)
		{
		CBtClientMtm::SBtcCmdSendParams sendParams;
			// if not, we set the password length to zero
			iConnectPassword.SetLength(0);

		// initialise T-Class to something sensible.
		sendParams.iTimeouts.iPutTimeout = 300000000;
		sendParams.iTimeouts.iConnectTimeout = 300000000;
		sendParams.iRemoteObexPort = 0;
		sendParams.iConnectPassword = &iConnectPassword;

		TPckgBuf<CBtClientMtm::SBtcCmdSendParams> sendParamsBuf(sendParams);

		// Invoke the "send" command on the Obex Client MTM,
		// asking it to notify us when it completes (which will be when the
		// message has actually been sent).
		
		// Note that the current obex mtm needs to have been set
    	testUtils.InstantiateBTClientMtmL();
		testUtils.GetCurrentObexClientMtm().SwitchCurrentEntryL(testUtils.Entry().Id());
		testUtils.GetCurrentObexClientMtm().LoadMessageL();
		iClientTest.SetCurrentOperation(testUtils.GetCurrentObexClientMtm().InvokeAsyncFunctionL(EBtMtmCmdSend, *iSelection, sendParamsBuf, iStatus));
		}
	else
		{
#ifdef VERIFY_DEF36727_USING_OLD_STURCT
		CObexClientMtm::STimeouts sendParams;
#else
		CIrClientMtm::STimeouts sendParams;
#endif

		// initialise T-Class to something sensible.
		sendParams.iPutTimeout = 300000000;
		sendParams.iConnectTimeout = 300000000;

#ifdef VERIFY_DEF36727_USING_OLD_STURCT		
		TPckgBuf<CObexClientMtm::STimeouts> sendParamsBuf(sendParams);
#else
		TPckgBuf<CIrClientMtm::STimeouts> sendParamsBuf(sendParams);
#endif

		// Invoke the "send" command on the Obex Client MTM,
		// asking it to notify us when it completes (which will be when the
		// message has actually been sent).
		
		// Note that the current obex mtm needs to have been set
    
		testUtils.InstantiateIRClientMtmL();
		testUtils.GetCurrentObexClientMtm().SwitchCurrentEntryL(testUtils.Entry().Id());
		testUtils.GetCurrentObexClientMtm().LoadMessageL();
		iClientTest.SetCurrentOperation(testUtils.GetCurrentObexClientMtm().InvokeAsyncFunctionL(CIrClientMtm::EIrcCmdSend, *iSelection, sendParamsBuf, iStatus));
		}

	// If we want to cancel this request after a certain time, set up
	// the timer which will notify us when the time is up.
	if (iClientTest.CancelGranularity() > 0)
		{
		iCancel = ETrue;
		// NB: the timer interval is specified in microseconds
		iTimer->Start(iClientTest.CancelGranularity() * 1000,
			iClientTest.CancelGranularity() * 1000,
			TCallBack(TimerCallback, this));
		}
	// If we're checking the progress every n milliseconds, set up the
	// timer to check this.
	else if (iClientTest.ProgressGranularity() > 0)
		{
		iCheckProgress = ETrue;
		// NB: the timer interval is specified in microseconds
		iTimer->Start(1, iClientTest.ProgressGranularity() * 1000,
			TCallBack(TimerCallback, this));
		}

	// tell the caller that we're waiting for the request to complete
	aStatus = KRequestPending;
	SetActive();
	}

TInt CSendMessageState::TimerCallback(TAny* aPtr)
	{
	CSendMessageState* self = static_cast<CSendMessageState*>(aPtr);

	// if we want to cancel the timer, then do so
	if (self->iCancel)
		{
		// stop the timer
		self->iTimer->Cancel();
		delete self->iTimer;
		self->iTimer = NULL;
		
		// cancel and delete the operation
		self->iClientTest.CurrentOperation().Cancel();
		self->iClientTest.SetCurrentOperation(NULL);

		self->iCancelled = ETrue;

		// tell the caller that we're complete - if the send isn't
		// already complete
		if (!self->iSendComplete)
			User::RequestComplete(self->iReportStatus, KErrNone);
		}
	else if (self->iCheckProgress)
		{
		TPckgBuf<TObexMtmProgress> package;
// Assume that a callback function shouldn't leave
		TRAPD(err, package.Copy(self->iClientTest.CurrentOperation().ProgressL()));

        if(err == KErrNone)
            {
		    TBuf<256> progressBuf;
			_LIT(KFormatString, "Sent %d/%d messages (%d/%d bytes): error code = %d");
		    progressBuf.Format(KFormatString,
			    package().iEntriesDone, 
                package().iTotalEntryCount,
			    package().iCurrentBytesTrans, 
                package().iCurrentEntrySize,
			    package().iError);
		    TRAP(err, self->iClientTest.LogCommentL(progressBuf));
            }

		if (self->iSendComplete)
			{
			self->iTimer->Cancel();
			delete self->iTimer;
			self->iTimer = NULL;
			self->iClientTest.SetCurrentOperation(NULL);
			return EFalse;
			}
		return ETrue;
		}
	return EFalse;
	}

void CSendMessageState::RunL()
	{
	// set the send to be complete
	iSendComplete = ETrue;
	
	if (iCancelled)
		{
		// select the parent entry (usually the Outbox) for convenience
		iClientTest.ObexTestUtils().SetEntryL(
			iClientTest.ObexTestUtils().Entry().Parent());

		// exit now, before progress is checked
		return;
		}

	// Check the progress so that we can extract the error code, to see
	// whether the send operation succeeded or failed.
	TPckgBuf<TObexMtmProgress> package;
	package.Copy(iClientTest.CurrentOperation().ProgressL());
	if (package().iError != KErrNone)
		{
        TInt err = iClientTest.IgnoreSendErrors() ? KErrNone : package().iError;
		User::RequestComplete(iReportStatus, err);
		}
	else
		{
		User::RequestComplete(iReportStatus, iStatus.Int());
		}

	// NB: we can't delete the message operation if we need to
	// check for progress etc., because it will be used by a timer event
	// after this point.
	if (!iCheckProgress && !iCancel)
		iClientTest.SetCurrentOperation(NULL);

	// select the parent entry (usually the Outbox) for convenience
	iClientTest.ObexTestUtils().SetEntryL(
		iClientTest.ObexTestUtils().Entry().Parent());
	}

void CSendMessageState::DoCancel()
	{
	iClientTest.CurrentOperation().Cancel();
	// delete the operation
	iClientTest.SetCurrentOperation(NULL);
	}

CSendMessageState::~CSendMessageState()
	{
	delete iTimer;
	if (iSelection)
		iSelection->Reset();
	delete iSelection;
	}

//
//

