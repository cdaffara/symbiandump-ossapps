// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <smutset.h>
#include <txtrich.h>
#include <smuthdr.h>
#include <msventry.h>
#include <msvuids.h>
#include <mmlist.h>
#include <smutsimparam.h>
#include <biodb.h>
#include <biouids.h>
#include <csmsaccount.h>

#include "SmssSimUtils.h"
#include "SMSSPAN.H"

//Logging constants

#ifndef _MSG_NO_LOGGING
_LIT(KDeleteFromSimLog, "DeleteFromSim.txt");
_LIT(KEnumerateSimLog, "EnumerateSim.txt");
_LIT(KCopyToSimLog, "CopyToSim.txt");
#endif

//CopyToSim constants


CSmsSimUtils* CSmsSimUtils::NewL(CMsvServerEntry& aServerEntry, RFs& aFs, TMsvId aSmsServiceId)
	{
	CSmsSimUtils* self = new (ELeave) CSmsSimUtils(aServerEntry, aFs, aSmsServiceId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
	}

void CSmsSimUtils::ConstructL()
	{
	iSettings = CSmsSettings::NewL();
		
	// load ECOM interface used to create SMS details and description values.
	iGetDetDesc = CSmsGetDetDescInterface::NewL();
	}

CSmsSimUtils::~CSmsSimUtils()
	{
	Cancel();

	iWriteStream.Close();
	iReadStream.Close();

	iSocket.Close();
	iSocketServ.Close();

	delete iSelection;
	delete iCompletedSelection;

	delete iHeader;
	delete iBody;
	delete iCharFormat;
	delete iParaFormat;

	delete iBioDb;
	delete iSettings;

	delete iGetDetDesc;
	REComSession::FinalClose();
	}

void CSmsSimUtils::ReadSimParamsL(TRequestStatus& aStatus)
/**
	Reads the SMS parameters from the SIM

	@since			7.0
	@param			aStatus		Indicates the completion status of a request made to 
								a service provider. 
	@pre 			None
	@post			Starts the reading async operation.
 */
	{
	SMSSLOG(FLogFormat(_L8("Reading SMS SIM Parameters")));

	Queue(aStatus);

	iProgress = TSmsProgress();
	iProgress.iType = TSmsProgress::ESmsProgressTypeReadSimParams;

	ConnectL();

	iSocket.Ioctl(KIoctlReadSmsParams, iStatus, NULL, KSolSmsProv);
	
	SetActive();
	iReadingSimParams = ETrue;
	}

void CSmsSimUtils::WriteSimParamsL(TRequestStatus& aStatus)
/**
	Writes the SMS parameters to the SIM

	@since			7.0
	@param			aStatus		
	@pre 			The parameters are stored in the message store.
	@post			The parameters are retrived form the message store and the writing
					async operation is started.
 */
	{
	SMSSLOG(FLogFormat(_L8("Writing SMS SIM Parameters")));

	Queue(aStatus);

	iProgress = TSmsProgress();
	iProgress.iType = TSmsProgress::ESmsProgressTypeWriteSimParams;

	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));

	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);

	CSmsSimParamOperation::RestoreSimParamsL(*store, *smspList);

	TInt count = 0;
	count = smspList->Enumerate();

	if (!count)
		User::Leave(KErrNotFound);

	ConnectL();
	
	iWriteStream << *smspList;
	iWriteStream.CommitL(); 

	CleanupStack::PopAndDestroy(2);  //smspList, store
	User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));

	iSocket.Ioctl(KIoctlWriteSmsParams, iStatus, NULL, KSolSmsProv);

	SetActive();
	}

void CSmsSimUtils::EnumerateL(const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Enumerate messages in SIM [param=%d]"), aParameter.Length()));

	//Enumerate messages on the SIM.
	//First obtains a count of messages on the SIM.

	Queue(aStatus);
	iProgress = TSmsProgress();
	iProgress.iType = TSmsProgress::ESmsProgressTypeEnumeratingPhoneStores;

	ConstructHeaderAndBodyL();
	SetAndCleanEnumerateFolderL(aParameter);
	SMSSLOG(FLogFormat(_L8("\tUsing folder %d"), iProgress.iEnumerateFolder, aParameter.Length()));
	
	//Obtain a count of the messages on the SIM
	CountMessagesInPhoneStoresL();
	}

void CSmsSimUtils::SetAndCleanEnumerateFolderL(const TDesC8& aParameter)
/**
 * 
 *
 * @param aParameter Optionally contains the ID of the folder to store the phone store messages
 */
	{
	iProgress.iEnumerateFolder = KErrNotFound;

	//Restore the iSimUtilsData.
	//This should be moved to a more general ContructL() if other functions other than EnumerateL() need to use this data
	RestoreSimUtilsDataL();

	//Delete messages under the last folder used to store phone-side messages
	const TInt err = iServerEntry.SetEntry(iSimUtilsData.iLastEnumerateFolder);
	if (err == KErrNotFound)
		{
		iSimUtilsData.iLastEnumerateFolder = KErrNotFound;
		}
	else
		{
		//Clean the last folder used to store phone-side messages
		User::LeaveIfError(err);
		DeleteChildrenL(iSimUtilsData.iLastEnumerateFolder);
		}

	if (aParameter.Length() != 0)
		{
		//Use the TMsvId packaged in aParameter as the folder in which to store phone-side messages
		TPckgC<TMsvId> enumFolder(KErrNotFound);
		enumFolder.Set(aParameter);
		iProgress.iEnumerateFolder = enumFolder();
		SMSSLOG(FLogFormat(_L8("\tAsked to use folder %d [last=%d]"), iProgress.iEnumerateFolder, iSimUtilsData.iLastEnumerateFolder));

		switch (iProgress.iEnumerateFolder)
			{
			case KMsvRootIndexEntryId:
			case KMsvLocalServiceIndexEntryId:
			case KMsvGlobalInBoxIndexEntryId:
			case KMsvGlobalOutBoxIndexEntryId:
			case KMsvDraftEntryId:
			case KMsvSentEntryId:
				User::Leave(KErrArgument); //cannot store phone-side messages under a standard folder
				break;
			default:
				break;
			}

		//Check the folder exists and delete all its children
		User::LeaveIfError(iServerEntry.SetEntry(iProgress.iEnumerateFolder));
		DeleteChildrenL(iProgress.iEnumerateFolder);
		}
	else
		{
		//else use the last folder used to store phone-side messages
		iProgress.iEnumerateFolder = iSimUtilsData.iLastEnumerateFolder;
		}

	if (iProgress.iEnumerateFolder == KErrNotFound)
		{
		//Create a new folder if last is not found AND aParameter,Length() == 0
		//Note: CreateNewEnumerateFolderL() sets iSimUtilsData.iLastEnumerateFolder
		iProgress.iEnumerateFolder = CreateNewEnumerateFolderL();
		}

	if (iProgress.iEnumerateFolder != iSimUtilsData.iLastEnumerateFolder)
		{
		//Delete the last enumerate folder if it is under the SMS service
		DeleteEnumerateFolderL(iSimUtilsData.iLastEnumerateFolder);

		//Store the enumerate folder for next time
		iSimUtilsData.iLastEnumerateFolder = iProgress.iEnumerateFolder;
		StoreSimUtilsDataL();
		}
	}

void CSmsSimUtils::DeleteEnumerateFolderL(TMsvId aId)
/**
 * Deletes entry aId if its parent is iSmsServiceId
 */
	{
	SMSSLOG(FLogFormat(_L8("\tAttempting to delete enumerate folder %d"), aId));
	const TInt err = iServerEntry.SetEntry(aId);
	
	if (err != KErrNotFound)
		{
		User::LeaveIfError(aId);
		
		if (iServerEntry.Entry().Parent() == iSmsServiceId)
			{
			SMSSLOG(FLogFormat(_L8("\tDeleting enumerate folder %d"), aId));
			User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));
			User::LeaveIfError(iServerEntry.DeleteEntry(aId));
			}
		}
	}

void CSmsSimUtils::DeleteChildrenL(TMsvId aId)
/**
 * Deletes all messages under parent aId
 */
	{
	SMSSLOG(FLogFormat(_L8("\tDeleting children from %d"), aId));

	User::LeaveIfError(iServerEntry.SetEntry(aId));

	TMsvSelectionOrdering order;
	order.SetShowInvisibleEntries(ETrue);
	iServerEntry.SetSort(order);

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);
	User::LeaveIfError(iServerEntry.GetChildren(*sel));

	if (sel->Count() != 0)
		{
		User::LeaveIfError(iServerEntry.DeleteEntries(*sel));
		}

	CleanupStack::PopAndDestroy(sel);
	}

TMsvId CSmsSimUtils::CreateNewEnumerateFolderL()
/**
 * Creates a new folder under the SMS service where phone-side messages will be stored
 * iSimUtilsData.iLastEnumerateFolder is set to the new folder's ID then stored against the SMS service
 *
 * @return the new folder's ID
 */
	{
	SMSSLOG(FLogFormat(_L8("\tCreating new folder...")));
	User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));

	TMsvEntry entry;

	//Create a new invisible folder under the local service
	entry.SetVisible(EFalse);
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = KUidMsgTypeSMS;
	entry.iServiceId = iSmsServiceId;
	User::LeaveIfError(iServerEntry.CreateEntry(entry));
	iSimUtilsData.iLastEnumerateFolder = entry.Id();

	TRAPD(err, StoreSimUtilsDataL());
	
	if (err != KErrNone)
		{
		iServerEntry.DeleteEntry(entry.Id()); //remove the folder if StoreSimUtilsDataL() leaves
		User::Leave(err);
		}

	return iSimUtilsData.iLastEnumerateFolder;
	}

void CSmsSimUtils::CountMessagesInPhoneStoresL()
	{
	SMSSLOG(FLogFormat(_L8("\tCounting Messages on SIM. Calling Ioctl KIoctlEnumerateSmsMessages")));

	User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));
	ConnectL();
	CreateBioDbL();
	iSocket.Ioctl(KIoctlEnumerateSmsMessages, iStatus, &iEnumerateCountBuf, KSolSmsProv);
	SetActive();
	}

void CSmsSimUtils::CreateBioDbL()
	{
	if (iBioDb == NULL)
		iBioDb = CBIODatabase::NewL(iFs);
	}

TBool CSmsSimUtils::GetDefaultSendBearerL(TInt aBioType, TBioMsgIdType aBearer, TBioMsgId& rBioMsgId) const
	{
	TInt index = 0;
	TUid uid;
	uid.iUid = aBioType;
	TBool found = EFalse;
	TRAPD(ret,iBioDb->GetBioIndexWithMsgIDL(uid, index)); //leaves with KErrNotFound if aBioType does not exist in bdb
	if (ret==KErrNone)
		{
		const CArrayFix<TBioMsgId>* bioIDs = iBioDb->BIOEntryLC(index);
		const TInt count = bioIDs->Count();

		for (TInt i = 0; !found && i < count; i++) //order important
			{
			rBioMsgId = bioIDs->At(i);
			found = (rBioMsgId.iType == aBearer);
			}

		CleanupStack::PopAndDestroy(); // bioIDs
		}
	else if (ret!=KErrNotFound)
		User::LeaveIfError(ret);
	return found;
	}

TUid CSmsSimUtils::DecodeBioMessageL(TBioMsgId& rId)
	{
	TUid biomsguid=KNullUid;
	rId.iType=EBioMsgIdNbs;
	// Take text off front of message
	CSmsMessage& smsmessage=iHeader->Message();
	const TInt length=smsmessage.Buffer().Length();
	smsmessage.Buffer().Extract(rId.iText,0,length<KMaxBioIdText? length: KMaxBioIdText);
	TInt originator;
	TInt destination;
	// Has the message got application port addressing?
	if (smsmessage.SmsPDU().ApplicationPortAddressing(destination,originator))
		{
		rId.iPort=STATIC_CAST(TUint16,destination);
		rId.iType=EBioMsgIdWap;
		// Is it WAP?
		if (iBioDb->IsBioMessageL(rId,biomsguid)==KErrNotFound)
			{
			rId.iType=EBioMsgIdWapSecure;
			// Is it WAP secure?
			if (iBioDb->IsBioMessageL(rId,biomsguid)==KErrNotFound)
				{
				rId.iType=EBioMsgIdWsp;
				// Is it WSP?
				if(iBioDb->IsBioMessageL(rId,biomsguid)==KErrNotFound)
					{
					rId.iType=EBioMsgIdWspSecure;
					// Is it WSP secure?
					iBioDb->IsBioMessageL(rId,biomsguid);
					}
				}
			}
		}
	else
		{
		// Is it an NBS style text message?
		TInt index=rId.iText.Locate('\r');
		if (index==KErrNotFound)
			index=rId.iText.Locate('\n');
		// Look for text match in BIO database
		if ((index>0) && (iBioDb->IsBioMessageL(EBioMsgIdNbs,rId.iText.Left(index),0,biomsguid) != KErrNotFound))
			smsmessage.Buffer().DeleteL(0,index+1);  // Remove BIO text
		}
	iHeader->SetBioMsgIdType(rId.iType);
	return biomsguid;
	}

void CSmsSimUtils::EncodeBioMessageL()
	{
	CSmsMessage& smsmessage=iHeader->Message();
	TBioMsgId bioMsgId;
	//  Is it a BIO message?  
		if (GetDefaultSendBearerL(iServerEntry.Entry().iBioType, iHeader->BioMsgIdType(), bioMsgId))
		{
		switch (bioMsgId.iType)
			{
			case EBioMsgIdNbs:
				{
				// Put text on NBS message
				TBioMsgIdText text;
				CSmsBufferBase& buffer=smsmessage.Buffer();
				TInt textlength=bioMsgId.iText.Length();
				buffer.Extract(text,0,textlength<buffer.Length()? textlength: 0);
				if (bioMsgId.iText.CompareF(text))
					{
					_LIT(KSmsNewLine,"\n");
					buffer.InsertL(0,KSmsNewLine);
					buffer.InsertL(0,bioMsgId.iText);
					}
				break;
				}
			case EBioMsgIdWap:
			case EBioMsgIdWapSecure:
			case EBioMsgIdWsp:
			case EBioMsgIdWspSecure:
				{
				// Put port addressing on binary messages 
				smsmessage.SmsPDU().SetApplicationPortAddressingL(ETrue,bioMsgId.iPort,bioMsgId.iPort,bioMsgId.iPort>255);
				smsmessage.SmsPDU().SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
				}
			default:
				break;
			}
		}
	}

void CSmsSimUtils::SetupMoveDeleteL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));

	Queue(aStatus);
	iProgress = TSmsProgress();

	if (iSelection != &aSelection)
		{
		delete iSelection;
		iSelection = NULL;
		iSelection = aSelection.CopyL();
		iSelection->Delete(0);  // Remove the SMS service entry id
		}

	iProgress.iMsgCount = iSelection->Count();
	}

void CSmsSimUtils::DeleteEachMessageFromPhoneStoreL()
	{
	iState = ESimUtilsStateOther;

	if (iProgress.iMsgDone < iProgress.iMsgCount)
		{
		ConstructHeaderAndBodyL();

		const TMsvId id = iSelection->At(iProgress.iMsgDone);
		const TInt err = iServerEntry.SetEntry(id);

		if (err == KErrNone)
			{
			CMsvStore* store = iServerEntry.ReadStoreL();
			CleanupStack::PushL(store);

			store->RestoreBodyTextL(*iBody);
			iHeader->RestoreL(*store);
			CleanupStack::PopAndDestroy(); //store

			iProgress.iError = KErrNone;

			SMSSLOG(FLogMessage(iServerEntry.Entry(), iHeader->Message(), iHeader->BioMsgIdType(), KDeleteFromSimLog));
			SMSSLOG(FLogFormat(_L8("\tDeleting %d from SIM (%d/%d)"), id, iProgress.iMsgDone+1, iProgress.iMsgCount));

			iWriteStream << iHeader->Message();
			iWriteStream.CommitL();
			iSocket.Ioctl(KIoctlDeleteSmsMessage, iStatus, NULL, KSolSmsProv);
			iState = ESimUtilsDeleteEachMessage;
			}
		else
			{
			SMSSLOG(FLogFormat(_L8("\tIgnoring %d, Error=%d"), id, err));
			RequestComplete(&iStatus, KErrNone);
			}

		SetActive();
		iProgress.iMsgDone++;
		User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));
		}
	else
		{
		iState = ESimUtilsDeletedLastMessage;
		}
	}

void CSmsSimUtils::ConstructHeaderAndBodyL()
	{
	if (!iParaFormat)
		iParaFormat = CParaFormatLayer::NewL();

	if (!iCharFormat)
		iCharFormat = CCharFormatLayer::NewL();

	if (!iBody)
		iBody = CRichText::NewL(iParaFormat, iCharFormat);

	if (!iHeader)
		iHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iBody);
	}

void CSmsSimUtils::RestoreSimUtilsDataL()
/**
 * Restore iSimUtilsData from the SMS service entry
 * Does not attempt to restore if the stream is not present
 */
	{
	User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));
	CMsvStore* store = iServerEntry.ReadStoreL();
	CleanupStack::PushL(store);

	if (iSimUtilsData.IsPresentL(*store))
		{
		iSimUtilsData.RestoreL(*store);
		}

	CleanupStack::PopAndDestroy(store);
	}

void CSmsSimUtils::StoreSimUtilsDataL()
/**
 * Store iSimUtilsData against the SMS service entry
 */
	{
	User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));
	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);
	iSimUtilsData.StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	}

void CSmsSimUtils::DeleteFromPhoneStoreL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Deleting %d messages from SIM"), aSelection.Count()));

	SetupMoveDeleteL(aSelection, aStatus);
	DoDeleteFromPhoneStoreL();
	}

void CSmsSimUtils::DoDeleteFromPhoneStoreL()
	{
	iProgress.iType = TSmsProgress::ESmsProgressTypeDeleteFromPhoneStore;
	
	ConnectL();
	RequestComplete(&iStatus, KErrNone, ETrue);
	}

void CSmsSimUtils::DoDeleteThenMoveFromPhoneStoreL()
	{
	iProgress.iType = TSmsProgress::ESmsProgressTypeMoveFromPhoneStore;
	
	ConnectL();
	RequestComplete(&iStatus, KErrNone, ETrue);
	}

void CSmsSimUtils::LoadClass2FolderIdL() 
	{
	RestoreSmsSettingsL();
	iClass2Folder = iSettings->Class2Folder();
	}

void CSmsSimUtils::RestoreSmsSettingsL()
	{
	CSmsAccount* account = CSmsAccount::NewLC();
	// just v2
	account->LoadSettingsL(*iSettings);
	CleanupStack::PopAndDestroy(account);    
	}

void CSmsSimUtils::CopyToPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& /*aParameter*/, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Copying messages to SIM")));
	
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));
	
	SetupMoveDeleteL(aSelection,aStatus);
	iProgress.iType = TSmsProgress::ESmsProgressTypeCopyToPhoneStore;
	iProgress.iMsgCount = iSelection->Count();
	iProgress.iMsgDone = -1;
	iState=EWritingToSIM;
	iRecipientCount = 0;
	iRecipientIndex = 0;

	LoadClass2FolderIdL();

	ConnectL();
	ConstructHeaderAndBodyL();
	CreateBioDbL();

	DoCopyToPhoneStoreL();
	}

void CSmsSimUtils::MoveToPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& /*aParameter*/, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Moving messages to SIM")));
	
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));
	
	SetupMoveDeleteL(aSelection,aStatus);
	iProgress.iType = TSmsProgress::ESmsProgressTypeMoveToPhoneStore;
	iProgress.iMsgCount = iSelection->Count();
	iProgress.iMsgDone = -1;
	iState=EWritingToSIM;
	iRecipientCount = 0;
	iRecipientIndex = 0;
	
	LoadClass2FolderIdL();
	
	ConnectL();
	ConstructHeaderAndBodyL();
	CreateBioDbL();
	
	DoCopyToPhoneStoreL();
	}


void CSmsSimUtils::DoCopyToPhoneStoreL()
	{
	User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));
	if(iState==EWritingToSIM)
		{
		if (iRecipientCount == 0)
			++iProgress.iMsgDone;

		if (iProgress.iMsgDone < iProgress.iMsgCount)
			{
			User::LeaveIfError(iServerEntry.SetEntry(iSelection->At(iProgress.iMsgDone)));

			CMsvStore* store = iServerEntry.ReadStoreL();
			CleanupStack::PushL(store);

			iHeader->RestoreL(*store);
			iBody->Reset();
			store->RestoreBodyTextL(*iBody);

			CleanupStack::PopAndDestroy(); //store

			// Set correct address on the CSmsMessage if it out going
			CSmsMessage& smsmessage=iHeader->Message();
			if (smsmessage.Type()==CSmsPDU::ESmsSubmit || (smsmessage.Type()==CSmsPDU::ESmsCommand))
				{
				// Get the count of recipients for the current message
				if (iRecipientCount == 0)
					{
					iRecipientCount = iHeader->Recipients().Count();
					iRecipientIndex = 0;
					}
				// Create separate copy of message for each recipient
				if (iRecipientCount > 0)
					{
					CSmsNumber& rcpt = *iHeader->Recipients().At(iRecipientIndex);
					iHeader->Message().SetToFromAddressL(rcpt.Address());
					}
				++iRecipientIndex;
				// Check if all the recipients are processed
				if (iRecipientIndex >= iRecipientCount)
					{
					// Yes all are done
					iRecipientIndex = 0;
					iRecipientCount = 0;
					}
				}
			SMSSLOG(FLogMessage(iServerEntry.Entry(), iHeader->Message(), iHeader->BioMsgIdType(), KCopyToSimLog));

			smsmessage.SetStorage(CSmsMessage::ESmsSIMStorage);	//Set to store on SIM

			if (iServerEntry.Entry().Unread())
				{
				smsmessage.SetStatus(NMobileSmsStore::EStoredMessageUnread);
				}
			else
				{
				smsmessage.SetStatus(NMobileSmsStore::EStoredMessageRead);
				}	

			//  This may be a BIO message, so test and set up SMS message correctly!
			if ((smsmessage.Type()==CSmsPDU::ESmsSubmit) || (smsmessage.Type()==CSmsPDU::ESmsDeliver))
				EncodeBioMessageL();

			iWriteStream << iHeader->Message();
			iWriteStream.CommitL();
			iSocket.Ioctl(KIoctlWriteSmsMessage, iStatus, &iSlotBuffer, KSolSmsProv);
			SetActive();

			// if we have a class 2 folder, and its not the current parent
			// then update the class 2 folder with the new message one the sim
			if(iClass2Folder!=KMsvNullIndexEntryId &&
				iClass2Folder != iServerEntry.Entry().Parent())
				iState=EUpdatingClass2;
			else
				iState=EWritingToSIM;
			}
		}
	else
		{
		__ASSERT_DEBUG(iState==EUpdatingClass2,Panic(ESmssBadState));
		__ASSERT_DEBUG(iClass2Folder!=KMsvNullIndexEntryId,Panic(ESmssNoClass2Folder));
		TMsvId toCopy=iSelection->At(iProgress.iMsgDone);
		User::LeaveIfError(iServerEntry.SetEntry(toCopy));

		// Update the slot array of the CSmsMessage so that
		// it can be deleted at a later time without the need for a re-enumeration
		TMsvEntry entry = iServerEntry.Entry();
		TBool wasReadOnly = entry.ReadOnly();

		if (wasReadOnly)
			{
			entry.SetReadOnly(EFalse);
			iServerEntry.ChangeEntry(entry);
			}

		CMsvStore* store = iServerEntry.EditStoreL();
		CleanupStack::PushL(store);
		iHeader->RestoreL(*store);
		CSmsMessage& smsmessage=iHeader->Message();

		smsmessage.UpdateSlotsL(iSlotBuffer);
		smsmessage.SetStorage(CSmsMessage::ESmsSIMStorage);
		iHeader->StoreL(*store);
		store->CommitL();

		if (wasReadOnly)
			{
			entry.SetReadOnly(ETrue);
			iServerEntry.ChangeEntry(entry);
			}

		CleanupStack::PopAndDestroy(store);
		User::LeaveIfError(iServerEntry.SetEntry(iServerEntry.Entry().Parent()));

		if(iProgress.iType == TSmsProgress::ESmsProgressTypeMoveToPhoneStore) 
			iServerEntry.MoveEntryL(toCopy,iClass2Folder,iStatus);
		else
			iServerEntry.CopyEntryL(toCopy,iClass2Folder,iStatus);

		SetActive();
		iState=EWritingToSIM;		
		}
	}

void CSmsSimUtils::CopyFromPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Copying messages from phone store")));
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));

	SetupMoveDeleteL(aSelection, aStatus);
	iProgress.iType = TSmsProgress::ESmsProgressTypeCopyFromPhoneStore;
	SetDestination(aParameter);
	MoveEntriesL(*iSelection, ETrue);
	}

void CSmsSimUtils::MoveEntriesL(const CMsvEntrySelection& aSelection, TBool aCopy)
	{
	// Uses the enumeration folder under the service to copy from
	User::LeaveIfError(iServerEntry.SetEntry(aSelection[0]));
	User::LeaveIfError(iServerEntry.SetEntry(iServerEntry.Entry().Parent()));
	if (aCopy)
		{
		if (iCompletedSelection)
			iCompletedSelection->Reset();
		else
			iCompletedSelection = new(ELeave) CMsvEntrySelection;

		iServerEntry.CopyEntriesL(aSelection, iDestination, *iCompletedSelection, iStatus);
		}
	else
		iServerEntry.MoveEntriesL(aSelection, iDestination, iStatus);
	SetActive();
	}

void CSmsSimUtils::MoveFromPhoneStoreL(const CMsvEntrySelection& aSelection, const TDesC8& aParameter, TRequestStatus& aStatus)
	{
	SMSSLOG(FLogFormat(_L8("Moving messages from phone store")));
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));

	SetupMoveDeleteL(aSelection, aStatus);
	iProgress.iType = TSmsProgress::ESmsProgressTypeMoveFromPhoneStore;
	SetDestination(aParameter);

	DoDeleteThenMoveFromPhoneStoreL();
	}

void CSmsSimUtils::SetDestination(const TDesC8& aParameter)
	{
	TPckgBuf<TMsvId> pkg;
	pkg.Copy(aParameter);
	iDestination = pkg();
	__ASSERT_DEBUG(iDestination, Panic(KSmssPanicDestinationFolderNotSet));
	}

void CSmsSimUtils::SetLocalStorageInfoL(const CMsvEntrySelection& aSelection)
	{
	// After a copy/move from phone store the iSlotArray and Storage members
	// need to be reset to reflect the local storage.
	__ASSERT_DEBUG(aSelection.Count(), Panic(KSmssPanicNoMessagesInSelection));
	TInt count = aSelection.Count();
	ConstructHeaderAndBodyL();
	TBool wasReadOnly = EFalse;

	while (count--)
		{
		// RetrieveMessageFromPhoneStoreL() may have set the TMsvEntry
		// to read only if the message is a CSmsPDU::ESmsSubmit
		iServerEntry.SetEntry(aSelection.At(count));
		TMsvEntry entry = iServerEntry.Entry();
		wasReadOnly = entry.ReadOnly();

		if (wasReadOnly)
			{
			entry.SetReadOnly(EFalse);
			iServerEntry.ChangeEntry(entry);
			}

		CMsvStore* store = iServerEntry.EditStoreL();
		CleanupStack::PushL(store);
		iHeader->RestoreL(*store);
		CSmsMessage& smsmessage = iHeader->Message();

		// Set the destination attributes
		smsmessage.SetStorage(CSmsMessage::ESmsPhoneStorage);
		smsmessage.iSlotArray.Reset();

		iHeader->StoreL(*store);
		store->CommitL();
		CleanupStack::PopAndDestroy(store);

		if (wasReadOnly)
			{
			entry.SetReadOnly(ETrue);
			iServerEntry.ChangeEntry(entry);
			}
		}
	}

void CSmsSimUtils::ConnectL()
	{
	ConnectL(iSocketServ, iSocket, ESmsAddrLocalOperation);
	}

void CSmsSimUtils::ConnectL(RSocketServ& arSocketServ, RSocket& arSocket, TSmsAddrFamily aSmsAddrFamily)
	{
	if (!arSocketServ.Handle())
		{
		User::LeaveIfError(arSocketServ.Connect());
		}

	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(arSocketServ.FindProtocol(protocolname,protoinfo));

	if (!arSocket.SubSessionHandle())
		{
		User::LeaveIfError(arSocket.Open(arSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
		}

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(aSmsAddrFamily);
	User::LeaveIfError(arSocket.Bind(smsaddr));
	}


CSmsSimUtils::CSmsSimUtils(CMsvServerEntry& aServerEntry, RFs& aFs, TMsvId aSmsServiceId)
	: CSmssActive(aFs, aServerEntry), 
	iState(ESimUtilsStateOther), 
	iReadStream(iSocket),
	iSmsServiceId(aSmsServiceId),
	iWriteStream(iSocket)
	{
	CActiveScheduler::Add(this);
	}

void CSmsSimUtils::DoRunReadSimParamsL()
/**
	Retrive and store the parameters to the message store. 

	@since			7.0
	@leave 		KErrNotFound
	@pre 			The reading async operation is complete.
	@post			The parameters are retrived and stored in the message store.
 */
	{
	iProgress.iMsgDone = iSmsServiceId;
	
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
		
	iReadStream >> *smspList;
	
	iProgress.iMsgCount = smspList->Enumerate();

	if (!iProgress.iMsgCount)
		User::Leave(KErrNotFound);

	User::LeaveIfError(iServerEntry.SetEntry(iSmsServiceId));

	CMsvStore* store = iServerEntry.EditStoreL();
	CleanupStack::PushL(store);

	CSmsSimParamOperation::StoreSimParamsL(*store, *smspList);

	CleanupStack::PopAndDestroy(2); //smspList, store
	User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));
	}

void CSmsSimUtils::DoRunL()
/**
	Handles completed async operations

	@since			7.0
	@leave		ESmscPanicUnexpectedCommand, KErrNone
	@pre 			The async operation is complete
	@post			The operation is complete
 */
	{
	switch (iProgress.iType)
		{
		case TSmsProgress::ESmsProgressTypeCopyFromPhoneStore:
			{
			__ASSERT_DEBUG(iCompletedSelection, Panic(KSmssPanicNoMessagesInSelection));
			SetLocalStorageInfoL(*iCompletedSelection);
			break;
			}
		case TSmsProgress::ESmsProgressTypeWriteSimParams:
			{
			break;
			}
		case TSmsProgress::ESmsProgressTypeReadSimParams:
			{
			if (iReadingSimParams)
				{
				iReadingSimParams = EFalse;
				TRAP(iProgress.iError, DoRunReadSimParamsL());
				iSocket.Ioctl(KIoctlCompleteReadSmsParams, iStatus, NULL, KSolSmsProv);
				SetActive();
				}
			break;
			}
		case TSmsProgress::ESmsProgressTypeEnumeratingPhoneStores:
			{
			iProgress.iMsgCount = iEnumerateCountBuf();
			RetrieveMessageFromPhoneStoreL();
			break;
			}
		case TSmsProgress::ESmsProgressTypeDeleteFromPhoneStore:
			{
			DeleteEachMessageFromPhoneStoreL();
			break;
			}
		case TSmsProgress::ESmsProgressTypeMoveFromPhoneStore:
			{
			DeleteEachMessageFromPhoneStoreL();

			if (iState == ESimUtilsDeletedLastMessage)
				{
				// We have finished deleting the messages from the SIM so set
				// the Storage & iSlotArray then move the entries
				SetLocalStorageInfoL(*iSelection);
				iState = ESimUtilsStateOther;
				iProgress.iType = TSmsProgress::ESmsProgressTypeMovingEntries;
				MoveEntriesL(*iSelection, EFalse);
				}
			break;
			}
		case TSmsProgress::ESmsProgressTypeCopyToPhoneStore:
		case TSmsProgress::ESmsProgressTypeMoveToPhoneStore:
			{
			DoCopyToPhoneStoreL();
			break;
			}
		case TSmsProgress::ESmsProgressTypeMovingEntries:
			break;
		default:
			{
			Panic(KSmssPanicUnexpectedState);
			}
		}
	}

void CSmsSimUtils::RetrieveMessageFromPhoneStoreL()
	{
	// If one message fails to be restored the rest fail 
	if (iProgress.iMsgDone < iProgress.iMsgCount)
		{
		CSmsMessage& smsmessage=iHeader->Message();
		iReadStream >> smsmessage;
		
		TUid biomsguid=KNullUid;
		TBioMsgId id;
		if ((smsmessage.Type()==CSmsPDU::ESmsSubmit) || (smsmessage.Type()==CSmsPDU::ESmsDeliver))
			biomsguid=DecodeBioMessageL(id);

		TMsvEntry entry;
		RestoreSmsSettingsL();
		TInt length = iSettings->DescriptionLength();
		HBufC* buf = HBufC::NewLC(length);
		TPtr description = buf->Des();	

		if (biomsguid != KNullUid)
			{
			TSmsUtilities::PopulateMsgEntry(entry, smsmessage, iSmsServiceId, *iSettings, KUidBIOMessageTypeMtm);
			
			// BioficateEntry!!!
			// Set up all the needed ids
			entry.iBioType = biomsguid.iUid;
			entry.iServiceId = KMsvLocalServiceIndexEntryId; 
			entry.iMtm = KUidBIOMessageTypeMtm;

			// Look up and set the description
			TInt index;
			iBioDb->GetBioIndexWithMsgIDL(biomsguid, index);
			description.Copy(iBioDb->BifReader(index).Description().Left(length));
			}
		else
			{
			// Set the details
			TSmsUtilities::PopulateMsgEntry(entry, smsmessage, iSmsServiceId, *iSettings);

			iGetDetDesc->GetDescription(smsmessage, description, length);
			}
		
		entry.iDescription.Set(description);

		if (smsmessage.Status() == NMobileSmsStore::EStoredMessageUnread)
			entry.SetUnread(ETrue);
		else
			entry.SetUnread(EFalse);
				
		TBuf<KSmsDetailsLength> details;	
		if (iGetDetDesc->GetDetails(iFs, smsmessage, details) == KErrNone)
			entry.iDetails.Set(details);

		User::LeaveIfError(iServerEntry.SetEntry(iProgress.iEnumerateFolder));
		User::LeaveIfError(iServerEntry.CreateEntry(entry));
		User::LeaveIfError(iServerEntry.SetEntry(entry.Id()));
		CMsvStore* store = iServerEntry.EditStoreL();
		CleanupStack::PushL(store);
		store->StoreBodyTextL(*iBody);
		iHeader->StoreL(*store);
		store->CommitL();
				
		if (iHeader->Type() != CSmsPDU::ESmsSubmit)
			{
			entry.SetReadOnly(ETrue);
			}
		
		entry.iSize = store->SizeL();
		CleanupStack::PopAndDestroy(store);
		User::LeaveIfError(iServerEntry.ChangeEntry(entry));

		SMSSLOG(FLogMessage(entry, iHeader->Message(), iHeader->BioMsgIdType(), KEnumerateSimLog));
		
		User::LeaveIfError(iServerEntry.SetEntry(KMsvNullIndexEntryId));

		iSocket.Ioctl(KIoctlReadMessageSucceeded, iStatus, NULL, KSolSmsProv);
		SetActive();
			
		iProgress.iMsgDone++;
			
		SMSSLOG(FLogFormat(_L8("\tRead %d from SIM (%d/%d)"), entry.Id(), iProgress.iMsgDone, iProgress.iMsgCount));
			
		CleanupStack::PopAndDestroy(buf);
		}
	}

void CSmsSimUtils::DoComplete(TInt& aStatus)
	{
	if (iState == ESimUtilsDeleteEachMessage && aStatus == KErrNotFound)
		{
#ifndef _MSG_NO_LOGGING
		const TMsvId id = iServerEntry.Entry().Id();
		_LIT(KLogTxt, "\tERROR: %d NOT deleted from the SIM. Error=KErrNotFound");
		SMSSLOG(FLogFormat(KDeleteFromSimLog, KLogTxt, id));
		SMSSLOG(FLogFormat(KLogTxt, id));
#endif

		TRAP(iProgress.iError, DeleteEachMessageFromPhoneStoreL());
		}
	else if (aStatus != KErrNone)
		{
		iServerEntry.SetEntry(KMsvNullIndexEntryId);
		iProgress.iError = aStatus;
		}

#ifndef _MSG_NO_LOGGING
	if (!IsActive())
		SMSSLOG(FLogFormat(_L8("SmsSimUtils completed. Error %d, MsgCount %d, MsgDone %d"), iProgress.iError, iProgress.iMsgCount, iProgress.iMsgDone));
#endif

	aStatus = KErrNone;
	}

void CSmsSimUtils::DoSmssCancel()
	{
	if (iSocket.SubSessionHandle())
		iSocket.CancelIoctl();
	}


/**
 * TSmsSimUtilsData
 */

void TSmsSimUtilsData::StoreL(CMsvStore& aStore) const
	{
	RMsvWriteStream writeStream;
	writeStream.AssignLC(aStore, KSmsSimUtilsDataUid);
	ExternalizeL(writeStream);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	}

void TSmsSimUtilsData::RestoreL(const CMsvStore& aStore)
	{
	RMsvReadStream readStream;
	readStream.OpenLC(aStore, KSmsSimUtilsDataUid);
	InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	}

void TSmsSimUtilsData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(KSmsSimUtilsDataVersion);
	aStream.WriteInt32L(iLastEnumerateFolder);
	}

void TSmsSimUtilsData::InternalizeL(RReadStream& aStream)
	{
	aStream.ReadInt16L(); // version, not used until version > 1
	iLastEnumerateFolder = aStream.ReadInt32L();
	}
