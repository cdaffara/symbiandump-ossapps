// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef _DEBUG
#undef _MSG_NO_LOGGING
#endif

#include <msvstd.h>
#include <msventry.h>

#include "SMSSchSend.h"
#include <smutset.h>

#ifndef _MSG_NO_LOGGING
#include <flogger.h>
#endif

/*
 *	CSmsScheduleSend Implementation
 */
 
CSmsScheduleSend* CSmsScheduleSend::NewL(CMsvServerEntry& aServerEntry)
	{
	CSmsScheduleSend* self = new (ELeave) CSmsScheduleSend(aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSmsScheduleSend::CSmsScheduleSend(CMsvServerEntry& aServerEntry)
	: CMsvScheduleSend(aServerEntry)
	{
	}

/*
Returns a pointer to a newly created CSmsScheduledEntry for a specified message.

The CSmsScheduledEntry encapsulates the scheduling and recipient information
relating to the message identified by aId.

@param	Id
The ID for the specified message.

@return
A pointer to the created CSmsScheduledEntry object,
*/
CMsvScheduledEntry* CSmsScheduleSend::GetMessageL(const TMsvId aId) const
	{
	//Set the iServerEntry to the message identified by aId
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	
	//Create a new CSmsScheduledEntry
	CSmsScheduledEntry* entry = CSmsScheduledEntry::NewL(iServerEntry.Entry());
	CleanupStack::PushL(entry);
	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);
	//Restore the entry from the message's store.
	entry->RestoreL(*store);

	CleanupStack::PopAndDestroy(store);
	CleanupStack::Pop(entry);

	return entry;
	}

/*
 *	CSmsScheduledEntry Implementation
 */

CSmsScheduledEntry* CSmsScheduledEntry::NewL(const TMsvEntry& aEntry)
	{
	CSmsScheduledEntry* self = new (ELeave) CSmsScheduledEntry(aEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSmsScheduledEntry::CSmsScheduledEntry(const TMsvEntry& aEntry)
	: CMsvScheduledEntry(aEntry)
	{
	}

void CSmsScheduledEntry::ConstructL()
	{
	iParaLayer = CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL( iParaLayer, iCharLayer, CEditableText::EFlatStorage, 256 );
	iHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText);
	}

CSmsScheduledEntry::~CSmsScheduledEntry()
	{
	// don't delete iRecipient
	delete iHeader;
	delete iRichText;
	delete iCharLayer;
	delete iParaLayer;
	}

void CSmsScheduledEntry::RecipientsStoreL(CMsvStore& aStore) const
	{
	iHeader->StoreL(aStore);
	}

void CSmsScheduledEntry::RecipientsRestoreL(CMsvStore& aStore)
	{
	iHeader->RestoreL(aStore);
	}

TBool CSmsScheduledEntry::CanSendToAnyRecipients(const TMsvSendErrorAction& aAction)
	{
	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();

	TBool retVal = EFalse;

	while (count--)
		{
		CSmsNumber* rcpt = numbers[count];

		retVal |= CheckRecipient(*rcpt, aAction);
		}

	return retVal;
	}

TBool CSmsScheduledEntry::RecipientsAllSent() const
	{
	//Returns whether all recipients of this SMS have sent successfully.
	//If there are NO recipients, then returns EFalse.

	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();
	TBool retVal = count;

	while (retVal && count--)
		{
		CSmsNumber* rcpt = numbers[count];
		retVal &= (rcpt->Status() == CMsvRecipient::ESentSuccessfully);
		}

	return retVal;
	}

TBool CSmsScheduledEntry::CheckRecipient(CSmsNumber& aRcpt, const TMsvSendErrorAction& aAction)
	{
#ifndef _MSG_NO_LOGGING
	TPtrC address(aRcpt.Address());
	RFileLogger::WriteFormat(KSchSendLogDir, KSchSendLogFile, EFileLoggingModeAppend,
		_L("\t\tCSmsScheduledEntry::CheckRecipient() aRcpt %S: Status %d Retries %d, aAction: Action %d Retries %d MaxRetries %d"),
		&address,
		aRcpt.Status(),
		aRcpt.Retries(),
		aAction.iAction,
		aAction.iRetries,
		aAction.MaxRetries());
#endif

	TBool retVal = EFalse;

	if (aRcpt.Status() != CMsvRecipient::ESentSuccessfully)
		{
		if (aAction.iAction == ESendActionSentAlready)
			{
			aRcpt.SetStatus(CMsvRecipient::ESentSuccessfully);
			}
		else if (aAction.iAction == ESendActionFail)
			{
			aRcpt.SetStatus(CMsvRecipient::EFailedToSend);
			}
		else if (aAction.iRetries == ESendRetriesInfinite || aRcpt.Retries() < aAction.MaxRetries())
			{
			aRcpt.SetStatus(CMsvRecipient::ENotYetSent);
			retVal = ETrue;
			}
		else
			{
			aRcpt.SetStatus(CMsvRecipient::EFailedToSend);
			}
		}

	return retVal;
	}

/**
Returns ETrue if the message can still sent to any of its recipients.
Otherwise returns EFalse.

The appropriate error action is also returned. The error action determines if a
particular recipient can be re-scheduled. The appropriate error action is given
from the list of error actions, which defines the action for a particular error
code.

@param	aErrorActions
The list of error actions that define what re-schedule is required for particular
error codes.

@param	aAction
An output argument with the error action appropriate for the recipient send error.

@return
A boolean value indicatinng whether the message can be sent to any of its
recipients.
*/
TBool CSmsScheduledEntry::CanSendToAnyRecipients(const CMsvSendErrorActions& aErrorActions, TMsvSendErrorAction& aAction)
	{
	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();

	TBool retVal = EFalse;

	while (count--)
		{
		CSmsNumber* rcpt = numbers[count];

		TMsvSendErrorAction action;

		if (aErrorActions.GetSendErrorAction(rcpt->Error(), action) != KErrNone)
			{
			action = aErrorActions.Default();
			}

		if (CheckRecipient(*rcpt, action))
			{
			retVal = ETrue;
			aAction = action;
			}
		}

	return retVal;
	}

void CSmsScheduledEntry::RecipientsResetRetries()
	{
	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();

	while (count--)
		{
		CSmsNumber* rcpt = numbers[count];
		
		if (rcpt->Status() != CMsvRecipient::ESentSuccessfully)
			{
			rcpt->ResetRetries();
			}
		}
	}

void CSmsScheduledEntry::RecipientsIncreaseRetries()
	{
	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();

	while (count--)
		{
		CSmsNumber* rcpt = numbers[count];
		
		if (rcpt->Status() != CMsvRecipient::ESentSuccessfully)
			{
			rcpt->IncreaseRetries();
			}
		}
	}

void CSmsScheduledEntry::RecipientsSetFailed()
	{
	CArrayPtrFlat<CSmsNumber>& numbers = iHeader->Recipients();
	TInt count = numbers.Count();

	while (count--)
		{
		CSmsNumber* rcpt = numbers[count];
		
		if (rcpt->Status() != CMsvRecipient::ESentSuccessfully)
			{
			rcpt->SetStatus(CMsvRecipient::EFailedToSend);
			}
		}
	}
