// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// SmssServ.cpp
// 
//

#include "SMSSERV.H"
#include "SMSSPAN.H"
#include "SMSSOUTB.H"

#include <c32comm.h>
#include <msventry.h>
#include <smss.rsg>
#include <smscmds.h>
#include <smut.h>
#include "SMSSchSend.h"
#include "SmssSimUtils.h"
#include <centralrepository.h>
#include <tmsvsystemprogress.h>

EXPORT_C CSmsServerMtm* CSmsServerMtm::NewL(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aServerEntry)
	{
	CSmsServerMtm* smsservermtm=new(ELeave) CSmsServerMtm(aRegisteredMtmDll, aServerEntry);
	CleanupStack::PushL(smsservermtm);
	smsservermtm->ConstructL();
	CleanupStack::Pop();
	return smsservermtm;
	}

CSmsServerMtm::~CSmsServerMtm()
	{
	Cancel();
	delete iScheduleSend;
	delete iOutboxSend;
	delete iSimUtils;
	}

void CSmsServerMtm::CopyToLocalL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iServerEntry!=NULL,Panic(ESmssEntryNotSet));
	__ASSERT_ALWAYS(!iOutboxSend, Panic(KSmssPanicMtmAlreadyActive));
	
	Queue(aStatus);
	RestoreSettingsL();
	iOutboxSend=CSmsOutboxSend::NewL(*iServerEntry, *iScheduleSend, iFs);
	iState=ESmsServerMtmSending;
#if (defined SYMBIAN_USER_PROMPT_SERVICE)	
	iOutboxSend->StartL(iStatus,aSelection, EFalse, KNullDesC8, iClientThreadId, iHasCapability);
#else
	iOutboxSend->Start(iStatus,aSelection, EFalse, KNullDesC8);
#endif	
	SetActive();
	}

void CSmsServerMtm::CopyWithinServiceL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::MoveToLocalL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::MoveFromLocalL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::MoveWithinServiceL(const CMsvEntrySelection& /*aSelection*/,TMsvId /*aDestination*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	iProgress = TSmsProgress();
	iProgress.iType = TSmsProgress::ESmsProgressTypeDeleteAll;
	iProgress.iMsgCount = aSelection.Count();
	User::LeaveIfError(iServerEntry->SetEntry(aSelection[0]));
	User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().Parent()));
	CMsvEntrySelection* sel = aSelection.CopyL();
	iProgress.iError = iServerEntry->DeleteEntries(*sel);
	delete sel;

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CSmsServerMtm::CreateL(TMsvEntry /*aNewEntry*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::ChangeL(TMsvEntry /*aNewEntry*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status=&aStatus;
	User::RequestComplete(status,KErrNotSupported);
	}

void CSmsServerMtm::StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iServerEntry!=NULL,Panic(ESmssEntryNotSet));
	__ASSERT_ALWAYS(iState==ESmsServerMtmWaiting,Panic(KSmssPanicMtmAlreadyActive));

	RestoreSettingsL();	
	iProgress = TSmsProgress();
	TInt err = KErrNone;

	switch (aCommand)
		{
		case ESmsMtmCommandScheduleCopy:
		case ESmsMtmCommandScheduleMove:
		case ESmsMtmCommandDeleteSchedule:
		case ESmsMtmCommandCheckSchedule:
		case ESmsMtmCommandSendScheduledCopy:
		case ESmsMtmCommandSendScheduledMove:
			{
			TRAP(err, ScheduleCommandL(aSelection, aCommand, aParameter, aStatus));
			break;
			}
		case ESmsMtmCommandEnumeratePhoneStores:
		case ESmsMtmCommandDeleteFromPhoneStore:
		case ESmsMtmCommandCopyFromPhoneStore:
		case ESmsMtmCommandMoveFromPhoneStore:
		case ESmsMtmCommandReadSimParams:
		case ESmsMtmCommandWriteSimParams:
		case ESmsMtmCommandCopyToPhoneStore:
		case ESmsMtmCommandMoveToPhoneStore:
			{
			TRAP(err, SimUtilsL(aSelection, aCommand, aParameter, aStatus));
			break;
			}
		default:
			{
			Panic(KSmssPanicUnexpectedCommand);
			}
		}

	if (err != KErrNone)
		{
		iServerEntry->SetEntry(KMsvNullIndexEntryId);
		User::Leave(err);
		}
	}

TBool CSmsServerMtm::CommandExpected()
	{
	return EFalse;
	}

const TDesC8& CSmsServerMtm::Progress()
	{
	switch (iState)
		{
		case ESmsServerMtmSending:
			{
			iProgress = iOutboxSend->Progress();
			break;
			}
		case ESmsServerMtmSimUtils:
			{
			iProgress = iSimUtils->Progress();
			break;
			}
		default:
			{
			//Do not change iProgress
			}
		}

	iProgressBuf = TSmsProgressBuf(iProgress);
	return iProgressBuf;
	}

void CSmsServerMtm::DoCancel()
	{
	switch (iState)
		{
		case ESmsServerMtmSending:
			{
			iOutboxSend->Cancel();
			break;
			}
		case ESmsServerMtmSimUtils:
			{
			iSimUtils->Cancel();
			break;
			}
		case ESmsServerMtmScheduling:
			{
			break;
			}
		default:
			{
			Panic(KSmssPanicUnexpectedCommand);
			}
		}
	CompleteCommand(KErrCancel);
	}

void CSmsServerMtm::DoRunL()
	{
	DoComplete(KErrNone);
	}

void CSmsServerMtm::CompleteCommand(TInt aError)
	{
	if (iReport)
		{
		switch (iState)
			{
			case ESmsServerMtmSending:
				{
				Progress();
				delete iOutboxSend;
				iOutboxSend=NULL;
				break;
				}
			case ESmsServerMtmScheduling:
				{
				break;
				}
			case ESmsServerMtmSimUtils:
				{
				Progress();
				delete iSimUtils;
				iSimUtils = NULL;
				break;
				}
			default:
				{
				Panic(KSmssPanicUnexpectedState);
				}
			}
		if (aError != KErrNone)         //  iProgress.iError may be recording a send error 
			{
			iProgress.iError = aError;  //  so it shouldn't be over-written with KErrNone
			}

		User::RequestComplete(iReport, KErrNone);
		}

	iState = ESmsServerMtmWaiting;
	iReport = NULL;

	if (iServerEntry)
		iServerEntry->SetEntry(KMsvNullIndexEntryId); //ignore error
	}

void CSmsServerMtm::DoComplete(TInt aError)
	{
	CompleteCommand(aError);
	}

CSmsServerMtm::CSmsServerMtm(CRegisteredMtmDll& aRegisteredMtmDll, CMsvServerEntry* aServerEntry):
	CScheduleBaseServerMtm(aRegisteredMtmDll, aServerEntry)
	{
	CActiveScheduler::Add(this);
	}
		
void CSmsServerMtm::ConstructL()
	{
	CScheduleBaseServerMtm::ConstructL();

#if defined (__WINS__)
	TInt ret=StartC32();
	if (ret!=KErrAlreadyExists)
		User::LeaveIfError(ret);
#endif

	iScheduleSend = CSmsScheduleSend::NewL(*iServerEntry);
    TSmsUtilities::ServiceIdL(*iServerEntry, iSmsServiceId);
	RestoreSettingsL();
	}

void CSmsServerMtm::RestoreSettingsL()
	{
	__ASSERT_ALWAYS(iServerEntry!=NULL,Panic(ESmssEntryNotSet));

	User::LeaveIfError(iServerEntry->SetEntry(iSmsServiceId));
	RestoreScheduleSettingsL();
	User::LeaveIfError(iServerEntry->SetEntry(KMsvNullIndexEntryId));
	}

void CSmsServerMtm::LoadResourceFileL()
	{
	CScheduleBaseServerMtm::LoadResourceFileL(KSmsResourceFile);
	}

void CSmsServerMtm::RestoreScheduleSettingsL()
	{
	CRepository* repository = CRepository::NewLC(KUidMsgTypeSMS);
	// just v2
	CScheduleBaseServerMtm::LoadScheduleSettingsL(*repository, ETrue, ERROR_ACTIONS);
	CleanupStack::PopAndDestroy(repository);    
	}

void CSmsServerMtm::PopulateSchedulePackage(const TDesC8& aParameter, const TBool aMove, TMsvSchedulePackage& aPackage) const
	{
	aPackage.iPollProgress = 3000000;
	aPackage.iParameter = aParameter;
	aPackage.iCommandId = aMove ? ESmsMtmCommandSendScheduledMove : ESmsMtmCommandSendScheduledCopy;
	}

void CSmsServerMtm::ScheduleCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	iState = ESmsServerMtmScheduling;
	iProgress.iType = TSmsProgress::ESmsProgressTypeScheduling;

	switch (aCommand)
		{
		case ESmsMtmCommandScheduleCopy:
			ScheduleL(aSelection, EFalse, aParameter, aStatus);
			break;
		case ESmsMtmCommandScheduleMove:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNotSupported);
			break;
			}
		case ESmsMtmCommandDeleteSchedule:
			iProgress.iType  = TSmsProgress::ESmsProgressTypeDeleteSchedule;
			DeleteScheduleL(aSelection, aParameter, aStatus);
			break;
		case ESmsMtmCommandCheckSchedule:
			iProgress.iType = TSmsProgress::ESmsProgressTypeCheckSchedule;
			CheckScheduleL(aSelection, aParameter, aStatus);
			break;
		case ESmsMtmCommandSendScheduledCopy:
			SendScheduledL(aSelection, EFalse, aParameter, aStatus);
			break;
		case ESmsMtmCommandSendScheduledMove:
			SendScheduledL(aSelection, ETrue, aParameter, aStatus);
			break;
		default:
			Panic(KSmssPanicUnexpectedCommand);
		}
	}

void CSmsServerMtm::SimUtilsL(const CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iSimUtils == NULL, Panic(KSmssPanicAlreadyUsingSimUtils));
	
	Queue(aStatus);

	iSimUtils = CSmsSimUtils::NewL(*iServerEntry, iFs, iSmsServiceId);

	iState = ESmsServerMtmSimUtils;

	switch (aCommand)
		{
		case ESmsMtmCommandEnumeratePhoneStores:
			iSimUtils->EnumerateL(aParameter, iStatus);
			break;
		case ESmsMtmCommandDeleteFromPhoneStore:
			iSimUtils->DeleteFromPhoneStoreL(aSelection, iStatus);
			break;
		case ESmsMtmCommandCopyFromPhoneStore:
			iSimUtils->CopyFromPhoneStoreL(aSelection, aParameter, iStatus);
			break;
		case ESmsMtmCommandMoveFromPhoneStore:
			iSimUtils->MoveFromPhoneStoreL(aSelection, aParameter, iStatus);
			break;
		case ESmsMtmCommandReadSimParams:
			iSimUtils->ReadSimParamsL(iStatus);
			break;
		case ESmsMtmCommandWriteSimParams:
			iSimUtils->WriteSimParamsL(iStatus);
			break;
		case ESmsMtmCommandCopyToPhoneStore:
			iSimUtils->CopyToPhoneStoreL(aSelection, aParameter, iStatus);
			break;
		case ESmsMtmCommandMoveToPhoneStore:
			iSimUtils->MoveToPhoneStoreL(aSelection, aParameter, iStatus);
			break;
		default:
			Panic(KSmssPanicUnexpectedCommand);
		}

	SetActive();
	}

/**
The extension method provides a polymorphic behaviour to call the correct
MTM.
@param aExtensionId The Uid passed in as KUIDMsgClientThreadInfo to copy the
					Client Thread Info.
@return KErrNone.
*/	
TInt CSmsServerMtm::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
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
		case KUIDMsgMsvSystemProgress:
		default:
			{
			// Chain to base class
			return CBaseServerMtm::Extension_(aExtensionId, a0, a1);
			}
		}
	}
