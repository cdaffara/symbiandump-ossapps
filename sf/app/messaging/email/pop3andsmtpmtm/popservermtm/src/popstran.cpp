// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// POPSTRAN.H
// 
//

#include "popstran.h"
#include "msventry.h"	// CMsvServerEntry
#include <txtetext.h>
#include <txtrich.h>
#include <miuthdr.h>
#include <msvutils.h>
#include <msvapi.h>

_LIT(KPop3DriveSeparator, ":");

CImPop3TransferMessage* CImPop3TransferMessage::NewL(CMsvServerEntry& aEntry)
	{
	CImPop3TransferMessage* self = new (ELeave) CImPop3TransferMessage(aEntry);
	CActiveScheduler::Add(self);
	return self;
	}

void CImPop3TransferMessage::StartL(TMsvId aSourceMessage, TMsvId aDestFolder, TImPop3TransferMethod aTransferMethod, TRequestStatus& aStatus)
	{
	iSourceMessage = aSourceMessage;
	iDestFolder = aDestFolder;
	iTransferMethod = aTransferMethod;
	iReportStatus = &aStatus;

	User::LeaveIfError(iEntry.SetEntry(aSourceMessage));
	TMsvEntry entry = iEntry.Entry();

	HBufC* details = HBufC::NewLC(entry.iDetails.Size());
	(*details) = entry.iDetails;
	HBufC* description = HBufC::NewLC(entry.iDescription.Size());
	(*description) = entry.iDescription;

	TFileName mailDirectory;
	mailDirectory.Append('a' + MessageServer::CurrentDriveL(iEntry.FileSession()));
	mailDirectory.Append(KPop3DriveSeparator);
	mailDirectory.Append(KMsvDefaultFolder2);
	
	// Get the store filename from the source message
	TFileName sourceStoreFilename = mailDirectory;
	User::LeaveIfError(iEntry.SetEntry(aSourceMessage));

	TBool storeFound = iEntry.HasStoreL();
	if (storeFound)
		{
		MsvUtils::ConstructEntryName(iEntry.Entry().iServiceId, iEntry.Entry().Id(), sourceStoreFilename, MsvUtils::EStore);	
		}

	// Create a message entry in the destination folder
	iEntry.SetEntry(aDestFolder);
	TMsvEmailEntry emailEntry(entry);
	emailEntry.SetVisible(ETrue);
	emailEntry.iDetails.Set(details->Des());
	emailEntry.iDescription.Set(description->Des());
	emailEntry.SetDisconnectedOperation(ENoDisconnectedOperations);
  
	User::LeaveIfError(iEntry.CreateEntry(emailEntry));
	iCopiedMessageId = emailEntry.Id();

	User::LeaveIfError(iEntry.SetEntry(emailEntry.Id()));
	TMsvId newEntry = emailEntry.Id();

	// Get the serviceId of the service the destination ID is located in.
	TMsvId destinationServiceId = iEntry.OwningService();

	if (storeFound)
		{
		// Get the name that the destination file should be copied to
		TFileName destinationStoreFilename = mailDirectory;
		MsvUtils::ConstructEntryName(destinationServiceId, iEntry.Entry().Id(), destinationStoreFilename, MsvUtils::EStore);
		CFileMan* fileMan = CFileMan::NewL(iEntry.FileSession());
		CleanupStack::PushL(fileMan);
		User::LeaveIfError(fileMan->Copy(sourceStoreFilename, destinationStoreFilename, CFileMan::ERecurse|CFileMan::EOverWrite));
		CleanupStack::PopAndDestroy(); // fileMan

		// This change entry isn't actually changing anything.
		// However the store contains a hidden stream created by the Message Server.
		// This is used to rebuild the index if it's corrupted
		// Calling ChangeEntry will make sure the stream is updated correctly
		iEntry.ChangeEntry(iEntry.Entry());
		}

	// Move all child entries from under the remote entry to the new entry
	User::LeaveIfError(iEntry.SetEntry(aSourceMessage));
	CMsvEntrySelection* childEntries = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(childEntries);
	User::LeaveIfError(iEntry.GetChildren(*childEntries));
	if (childEntries->Count() > 0)
		{
		if (iTransferMethod == EImPop3CopyTransfer)
			// If we are in disconnected mode then we really want to copy the
			// entries, however if we are not then we want to move them.
			// This is because the only reason there are entries under the
			// service is because the remote server does not support the TOP
			// command.
			{
			iStatus = KRequestPending;
			iEntry.CopyEntriesL(*childEntries, newEntry, iStatus);
			SetActive();
			}
		else
			{
			iStatus = KRequestPending;
			iEntry.MoveEntriesL(*childEntries, newEntry, iStatus);
			SetActive();
			}
		}
	else
		{
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}

	CleanupStack::PopAndDestroy(); // childEntries

	CleanupStack::PopAndDestroy(2); // description, details

	}

void CImPop3TransferMessage::DoCancel()
	{
	iEntry.Cancel();
	
	if (iEntry.SetEntry(iDestFolder) == KErrNone)
		{
		iEntry.DeleteEntry(iCopiedMessageId);
		}
	
	User::RequestComplete(iReportStatus, KErrCancel);
	}

void CImPop3TransferMessage::RunL()
	{
	if (iStatus == KErrNone)
		{
		if (iTransferMethod == EImPop3MoveTransfer)
			// If the data under the message entry has been moved then mark it as incomplete
			{
			TInt err;
			err = iEntry.SetEntry(iSourceMessage);
			if (err == KErrNone)
				{
				TMsvEntry entry = iEntry.Entry();
				entry.SetComplete(EFalse);
				err = iEntry.ChangeEntry(entry);
				if (err != KErrNone)
					iStatus = err;
				}
			}
		}
	else
		// If the message hasn't been copied or moved correctly then
		// delete the new message.
		// The old source message is still in a valid state.
		{
		TInt err;
		err = iEntry.SetEntry(iDestFolder);
		if (err == KErrNone)
			{
			err = iEntry.DeleteEntry(iCopiedMessageId);
			if (err != KErrNone)
				iStatus = err;
			}
		}

	User::RequestComplete(iReportStatus, iStatus.Int());
	}

CImPop3TransferMessage::~CImPop3TransferMessage()
	{
	}

CImPop3TransferMessage::CImPop3TransferMessage(CMsvServerEntry& aEntry) : CActive(EPriorityStandard), iEntry(aEntry)
	{
	}
