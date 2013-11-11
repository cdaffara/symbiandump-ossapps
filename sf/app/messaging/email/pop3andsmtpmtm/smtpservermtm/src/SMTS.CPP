// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SMTS.H"
#include "SMTSUTIL.H"
#include "IMSM.H"
#include <smtpcmds.h>
#include <msventry.h>
#include <miutset.h>

EXPORT_C CSmtpSrvMtm* CSmtpSrvMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvServerEntry*  aEntry)
	{
	CSmtpSrvMtm* mysvrmtm = new CSmtpSrvMtm(aRegisteredMtmDll, aEntry);
	if (mysvrmtm==NULL)
		{
		aRegisteredMtmDll.ReleaseLibrary();
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PushL(mysvrmtm);
	mysvrmtm->ConstructL();
	CleanupStack::Pop();
	return mysvrmtm;
	}

CSmtpSrvMtm::CSmtpSrvMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aEntry):
	CBaseServerMtm(aRegisteredMtmDll, aEntry)
	{
	__DECLARE_NAME(_S("CStmpSvrMtm"));
	}
		
void CSmtpSrvMtm::ConstructL()
	{
	iSessionError=KErrNone;
	iProgressBuffer().SetStatus(EMsgOutboxProgressDone);
#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	iHasCapability = ETrue;
#endif
	CActiveScheduler::Add(this);
	}

CSmtpSrvMtm::~CSmtpSrvMtm()
	{
	Cancel();	//classes derived from CActive MUST call Cancel()
	delete iOutboxSend;	//should already be dead
	delete iSendSelection;
	//  Set the Service Entry to be NOT connected
    SetServiceToConnected(EFalse);
   	}


// aServiceId is ignored and the service ids of the messages in the selection
// are used instead

void CSmtpSrvMtm::CopyFromLocalL(const CMsvEntrySelection& aSelection, TMsvId aServiceId, /* Info for protocol....*/ TRequestStatus& aStatus)
	{
	iServiceId=aServiceId;
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	__ASSERT_DEBUG(iServerEntry != NULL,gPanic(EEntryNotSet));
	if (IsActive())
		User::Leave(KErrServerBusy);

	if (iSendSelection)
		iSendSelection->Reset();
	else
		iSendSelection=new (ELeave) CMsvEntrySelection;

	CopyToOurSelectionL(aSelection);
	AddWaitingInOutboxToSelectionL();

	if (iSendSelection->Count())
		DoSendMessagesL(aStatus);
	else
		{
		iSessionError=KSmtpNoMsgsToSendWithActiveSettings;
		TRequestStatus* pStatus=&aStatus;
		User::RequestComplete(pStatus,KErrNone);//nothing to send
		}
	}



void CSmtpSrvMtm::DoSendMessagesL(TRequestStatus& aStatus)
	{
	iSessionError = KErrNone;
    
	// Set the Service entry to be Connected
	User::LeaveIfError(SetServiceToConnected(ETrue));
	
	delete iOutboxSend;
	iOutboxSend=NULL;
	iOutboxSend = CMsgImOutboxSend::NewL(*iSendSelection,*iServerEntry,iServiceId);

#if (defined SYMBIAN_USER_PROMPT_SERVICE)
	iOutboxSend->StartL(iStatus, iClientThreadId, iHasCapability);
#else
	iOutboxSend->StartL(iStatus);
#endif
	
	SetActive();
	aStatus = KRequestPending;
	iReport = &aStatus;
	}


//
//	Progress()
//
//	Args:			None.
//	
//	Return Value:	TDesC& -- reference to a TBufPckg<TImSmtpProgress>
//					maintained by the instantiated CMsgImOutboxSend
//					object...
//
//	Remarks:				
//
const TDesC8& CSmtpSrvMtm::Progress()
	{
	//	load in latest progress, otherwise use saved progress
	if(iOutboxSend!=NULL)
		{
		iProgressBuffer = iOutboxSend->Progress();
		}
	else
		iProgressBuffer().SetError(iSessionError);
	return iProgressBuffer;
	}

void CSmtpSrvMtm::DoCancel()
	{
	if(iOutboxSend)
		{
		iOutboxSend->Cancel();
		Progress();	//update iProgressBuffer
		delete  iOutboxSend;
		iOutboxSend = NULL;
		}

    //  Set the Service Entry to be NOT connected
    SetServiceToConnected(EFalse);

	User::RequestComplete(iReport,KErrCancel);
	}

void CSmtpSrvMtm::DoRunL()
	{
	iSessionError = iStatus.Int();
	Progress();

	//	Delete the CImOutboxSend object.
	delete  iOutboxSend;
	iOutboxSend = NULL;

    //  Set the Service Entry to be NOT connected
    SetServiceToConnected(EFalse);

	User::RequestComplete(iReport,KErrNone);	// always returns KErrNone; errors are returned in Progress
	}

void CSmtpSrvMtm::DoComplete(TInt aError)
	{
	iSessionError=aError;	// store error away to set Progress() later
	User::RequestComplete(iReport,KErrNone);	// always returns KErrNone; errors are returned in Progress
	}


TBool CSmtpSrvMtm::CommandExpected()
	{
	return EFalse;
	}

void CSmtpSrvMtm::StartCommandL(CMsvEntrySelection& aSelection,
                                TInt aCommand,
                                const TDesC8& /*aParameter*/,
                                TRequestStatus& aStatus)
	{
	TInt error = KErrNone;

    switch (aCommand)
        {
    //  Is session connected...
    case KSMTPMTMIsConnected:
		if (iOutboxSend)
			iOutboxSend->SessionIsConnected();
		else 
            error = KErrDisconnected;    
        break;
    case KSMTPMTMSendOnNextConnection:
		CopyFromLocalL(aSelection, aSelection.At(0), aStatus);
		return;
    default:
        error = KErrNotSupported;
        break;
        }
	iReport=&aStatus;
	aStatus=KRequestPending;
	User::RequestComplete(iReport, error);
	}


void CSmtpSrvMtm::CopyToOurSelectionL(const CMsvEntrySelection& aSourceSelection)
	{
  	TInt count=aSourceSelection.Count();
  	TMsvEntry entry;
  	while (count--)
  		{
  		TInt err=iServerEntry->SetEntry(aSourceSelection[count]);
		if(err==KErrNone)
			{
  			entry=iServerEntry->Entry();
  
			// If this entry is a message using the SMTP service & it hasn't been sent, 
			// then add it to the list of entries to send.
  			if (entry.iMtm==KUidMsgTypeSMTP && entry.iType==KUidMsvMessageEntry &&	
				entry.iServiceId==iServiceId && entry.SendingState() != KMsvSendStateSent)
				{
				iSendSelection->AppendL(aSourceSelection[count]);						
				}
			}
		}
  	}

void CSmtpSrvMtm::AddWaitingInOutboxToSelectionL()
	{
	User::LeaveIfError(iServerEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId));
	CMsvEntrySelection *sel = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(sel);
	User::LeaveIfError(iServerEntry->GetChildrenWithService(iServiceId,*sel));
	TInt count=sel->Count();
	TMsvEntry entry;
	while (count--)
		{
		TInt err=iServerEntry->SetEntry((*sel)[count]);
		if(err==KErrNone)
			{
			entry=iServerEntry->Entry();
			if ((entry.SendingState()==KMsvSendStateWaiting ||entry.SendingState()==KMsvSendStateScheduled || entry.SendingState()==KMsvSendStateResend) && //i.e send on next connection
				iSendSelection->Find((*sel)[count])==KErrNotFound && 
				entry.iType==KUidMsvMessageEntry) 
				iSendSelection->AppendL((*sel)[count]);						
			}
		}
	CleanupStack::PopAndDestroy(sel);
	}


TInt CSmtpSrvMtm::SetServiceToConnected(TBool aConnected)
// Set the Service Entry to be either connected or not. 
	{
	TInt error = iServerEntry->SetEntry( iServiceId );
    if (error == KErrNone)
        {
        TMsvEntry entry = iServerEntry->Entry();

		// Change the connected flag if not already set to this value
		if (entry.Connected()!=aConnected)
			{
		    entry.SetConnected(aConnected);
			error = iServerEntry->ChangeEntry(entry);
			}
        }

	// returns any error that occured
	return error;
	}


//*****************************************************************************
//The rest of the commands are unsupported
//*****************************************************************************

void CSmtpSrvMtm::MoveFromLocalL(const CMsvEntrySelection& /*aSelection*/,
                                 TMsvId /*aServiceId*/,
                                 TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::CopyToLocalL(const CMsvEntrySelection& /*aSelection*/,
                               TMsvId /*aDestination*/, 
                               TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::MoveToLocalL(const CMsvEntrySelection& /*aSelection*/,
                               TMsvId /*aDestination*/,
                               TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::CopyWithinServiceL(const CMsvEntrySelection& /*aSelection*/,
                                     TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::MoveWithinServiceL(const CMsvEntrySelection& /*aSelection*/,
                                     TMsvId /*aDestination*/,
                                     TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::DeleteAllL(const CMsvEntrySelection& /*aSelection*/,
                             TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::CreateL(TMsvEntry /*aNewEntry*/,
                          TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

void CSmtpSrvMtm::ChangeL(TMsvEntry /*aNewEntry*/,
                          TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iOutboxSend == NULL,gPanic(ESmtsBusy));
	iReport=&aStatus;
	User::RequestComplete(iReport,KErrNotSupported);
	}

/**
The extension method provides a polymorphic behaviour to call the correct
MTM.
@param aExtensionId The Uid passed in as KUIDMsgClientThreadInfo to copy the
					Client Thread Info.
@return KErrNone.
*/
TInt CSmtpSrvMtm::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)	
	{
	switch(aExtensionId)
		{
#if (defined SYMBIAN_USER_PROMPT_SERVICE)		
		case KUIDMsgClientThreadInfo:
			{
			iClientThreadId = *(TThreadId*) (a1);
	    	iHasCapability = (TBool)*(TInt*)(a0);
	    	return KErrNone;
			}
#endif

		case KUidMsgNonOperationMtmData:
			{
			TNonOperationMtmDataType* mtmDataType = reinterpret_cast<TNonOperationMtmDataType*>(a0);
			TPtrC8* mtmDataBuffer = reinterpret_cast<TPtrC8*>(a1);
			return GetNonOperationMtmData(*mtmDataType, *mtmDataBuffer);
			}

		default:
			{
			// Chain to base class
			return CBaseServerMtm::Extension_(aExtensionId, a0, a1);
			}
		}
	}

/**
Gets MTM information that is not related to the current operation

@param aMtmDataType Type of data to fetch
@param aMtmDataBuffer On return this points to a descriptor holding the data

@return KErrNone if successful, or a system wide error code
*/
TInt CSmtpSrvMtm::GetNonOperationMtmData(TNonOperationMtmDataType aMtmDataType, TPtrC8& aMtmDataBuffer)
	{
	if (aMtmDataType == EMtmDataAccessPointId)
		{
		if (iOutboxSend)
			{
			TNonOperationMtmDataAccessPointId mtmDataAccessPointId;

			TInt err = iOutboxSend->GetAccessPointIdForConnection(mtmDataAccessPointId.iAccessPointId);

			if (err == KErrNone)
				{
				iMtmDataAccessPointIdBuffer = TNonOperationMtmDataAccessPointIdBuffer(mtmDataAccessPointId);
				aMtmDataBuffer.Set(iMtmDataAccessPointIdBuffer);
				return KErrNone;
				}
			}

		return KErrNotFound;
		}

	return KErrNotSupported;
	}

