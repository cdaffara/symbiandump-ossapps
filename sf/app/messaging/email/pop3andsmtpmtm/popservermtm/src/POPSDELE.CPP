// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32std.h>
#include "PopsDele.h"
#include "POPS.H"
#include "POPSOP.H" // CImPop3Dele

#include <msventry.h>
#include <popsmtm.h>
#include "POPSMBX.H"

#include "POPS.PAN" // imrc's own panic codes

#include "mobilitytestmtmapi.h"

//  Panic function
GLREF_C void Panic(TPopsPanic aPanic);

CImPop3Delete::CImPop3Delete(CMsvServerEntry& aRemoteEntry, CImPop3Session* aPop3Session, TMsvId aServiceId)
	: CMsgActive( KMsgPop3RefreshMailboxPriority ),iRemoteEntry(aRemoteEntry) ,iPopSession(aPop3Session), iServiceId(aServiceId)
	{
	__DECLARE_NAME(_S("CImPop3Delete"));
	}


CImPop3Delete* CImPop3Delete::NewL(CMsvServerEntry& aRemoteEntry, const CMsvEntrySelection& aMsvSelection, CImPop3Session* aPop3Session, TMsvId aServiceId)
	{
	CImPop3Delete* self = new (ELeave) CImPop3Delete(aRemoteEntry, aPop3Session, aServiceId);

	CleanupStack::PushL(self);
	self->ConstructL(aMsvSelection);
	CleanupStack::Pop();
	return self;
	}


void CImPop3Delete::ConstructL( const CMsvEntrySelection& aMsvSelection)
	{
	// make our copy of aMsvSelection
	iSource = aMsvSelection.CopyL();

	iPopDelete=CImPop3Dele::NewL(iPopSession);
	CActiveScheduler::Add(this);	  // Add CImPop3Delete to scheduler's queue
	iCancelToMigrate = EFalse;
	}


CImPop3Delete::~CImPop3Delete()
	{
	// delete everything here
	Cancel();
	delete iSource;
	delete iPopDelete;
	}

//
// Cancel any current operation
//
void CImPop3Delete::DoCancel()
	{
	if (!iMigratingToNewBearer)
		{
		iPopSession->SetOpNotPending();
		}

	iPopDelete->Cancel();
	CMsgActive::DoCancel();
	}
	
//
// Start me up
//
void CImPop3Delete::Start(TRequestStatus& aStatus)
	{
	iMsgCtr=0;
	iProgress.iTotalMsgs=iSource->Count();
	iProgress.iMsgsToProcess=iProgress.iTotalMsgs;

	if(iSource->Count())
		{
		DoDelete();
		Queue(aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* pS=&aStatus;
		User::RequestComplete(pS,KErrNone);
		}
	}

// ****************************************************************************************** 
// Resume function called by the POP Server MTM, once it has completed Migrating to new bearer
// 
// ******************************************************************************************	
void CImPop3Delete::ResumeL(CImPop3Session* aPopSession, TRequestStatus& aStatus)
	{
	iMigratingToNewBearer = EFalse;
	iPopSession = aPopSession;

	delete iPopDelete;
	iPopDelete = NULL;
	iPopDelete = CImPop3Dele::NewL(iPopSession);

	if(iSource->Count())
		{
		DoDelete();
		Queue(aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* pS = &aStatus;
		User::RequestComplete(pS,KErrNone);
		}
	}	

//
// Result of last active call
//
void CImPop3Delete::DoRunL()
    {
	//  No really good error handling can occur below.
	TInt err = iRemoteEntry.DeleteEntry(iSource->At(iMsgCtr));
	//DMC __ASSERT_DEBUG(err == KErrNone, Panic(EPopFailedDebugAssert));
	 
	iMsgCtr++;
	iProgress.iMsgsToProcess--;

	// Decrement iMtmData3 attribute of the service entry - count of messages on POP3 server
	// NB our handle to the CMsvServerEntry must always be set to the service entry
	// prior to call to CImPop3Delete::NewL.
	TMsvEntry serviceEntry  = iRemoteEntry.Entry();
	serviceEntry.SetMtmData3(serviceEntry.MtmData3()-1);
	err = iRemoteEntry.ChangeEntry(serviceEntry);

	if(iMsgCtr<iSource->Count())
		{
		// If we are migrating halt the operation here
		if (iMigratingToNewBearer)
			{			
			// If we don't SetActive, the RunL will complete the request
			return;
			}
		else
			{
			DoDelete();
			}
		}
	else
		{
		iProgress.iMsgsToProcess=0;
		}
    }

//
// Fire off the delete process
//
void CImPop3Delete::DoDelete()
	{
	TMsvId msgId=iSource->At(iMsgCtr);
//DMC
	if (iPopDelete->SetMessage(msgId))
 		{
		iPopDelete->Start(iStatus);
		// activate ourselves
		SetActive();
		MOBILITY_TEST_MTM_STATE(iServiceId, KMobilityTestMtmStatePopDeleting);
		}
	else
		{
		SetActive();
		iStatus = KRequestPending;
		TRequestStatus* pS=&iStatus;
		User::RequestComplete(pS,KErrNone);
		}
	}

//
// Report the refreshing news back to the UI
//
TPop3Progress CImPop3Delete::Progress()
	{
	return iProgress;
	}

// ****************************************************************************************** 
// This is called by the POP Server MTM when it starts Migrating Bearer
// 
// ******************************************************************************************
void CImPop3Delete::Pause()
	{
	// Set the Migration flag
	iMigratingToNewBearer = ETrue;
	}

// ****************************************************************************************** 
// This is called by the POP Server MTM when it starts Migrating Bearer
// 
// ******************************************************************************************	
void CImPop3Delete::CancelAllowResume()
	{
	iMigratingToNewBearer = ETrue;
	
	// Cancel the deletion of the current message  and decrement counters 
	// so we can restart from this message onwards when we have migrated.
	// Use the normal cancel, as we really need to cancel here.
	Cancel();		
	}
