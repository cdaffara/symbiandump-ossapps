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
// Contains method implementations for the Active Object used to write a
// Contact to the ICC.
// 
//

/**
 @file
 @internalComponent
*/

#include "common.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksynclog.h"
#include "phbksyncsvr.h"
#include "SyncEngineSession.h"
#include "SyncEngineServer.h"
#include "phbksyncsess.h"
#include "WriteContactToICC.h"


/**
 *  Static factory method used to create a CWriteContactToICC object.
 *
 *  @param aBufferSize        Size of ICC Entry buffer passed in the request.
 *  @param aSession           Handle to the engine session.
 *  @param aPhonebookManager  Pointer to the Phonebook Manager which stores all
 *                            the phonebook parameters.
 *  @param aPhone             Handle to the phone object.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 *
 *  @return  Pointer to the created CWriteContactToICC object, or NULL.
 */
CWriteContactToICC* CWriteContactToICC::NewL(CSyncEngineSession& aSession,
											 TInt aBufferSize,
											 CPhoneBookManager& aPhonebookManager,
											 RMobilePhone& aPhone,
											 TUid aPhonebookUid,
											 const RMessage2& aClientMessage)
	{
	CWriteContactToICC*  self = new(ELeave) CWriteContactToICC(aSession,
															   aPhonebookManager,
															   aPhone,
															   aPhonebookUid,
															   aClientMessage);
	CleanupStack::PushL(self);
	self->ConstructL(aBufferSize); 
	CleanupStack::Pop(self);

	return self;
	} // CWriteContactToICC::NewL


/**
 *  Standard constructor.
 *
 *  @param aSession           Handle to the engine session.
 *  @param aPhonebookManager  Pointer to the Phonebook Manager which stores all
 *                            the phonebook parameters.
 *  @param aPhone             Handle to the phone object.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 */
CWriteContactToICC::CWriteContactToICC(CSyncEngineSession& aSession,
									   CPhoneBookManager& aPhonebookManager,
									   RMobilePhone& aPhone,
									   TUid aPhonebookUid,
									   const RMessage2& aClientMessage)
  : CActive(EPriorityNormal),
	iSession(aSession),
	iPhonebookManager(aPhonebookManager),
	iPhone(aPhone),
	iPhonebookUid(aPhonebookUid),
	iClientMessage(aClientMessage),
	iIccEntryBuf(NULL),
	iIccEntryPtr(NULL, 0),
	iPhonebookDataPtr(NULL,0),
	iIccWriteState(EPBSyncIccWriteIdle),
	iIsUsedSlot(EFalse)
	{
	// NOP
	} // CWriteContactToICC::CWriteContactToICC


/**
 *  Standard destructor. This will cancel any pending active requests and
 *  free any allocated memory.
 */
CWriteContactToICC::~CWriteContactToICC()
	{
	LOGACTIVE1(_L8("~CWriteContactToICC()"));

	Cancel();

	delete iPhonebookBuffer;
	delete iIccEntry;
	delete iIccEntryBuf;
	delete iPhonebookData;
	} // CWriteContactToICC::~CWriteContactToICC


/**
 *  Second phase constructor.
 */
void CWriteContactToICC::ConstructL(TInt aBufferSize)
	{
	//
	// Create the phonebook buffer for writing to the ICC...
	//
	iPhonebookBuffer = new(ELeave) CPhoneBookBuffer();

	//
	// Create the ICC Entry and phonebook buffer...
	//
	iIccEntry = CSyncContactICCEntry::NewL();

	iIccEntryBuf = CBufFlat::NewL(1);
	iIccEntryBuf->ResizeL(aBufferSize); 
	iIccEntryPtr.Set(iIccEntryBuf->Ptr(0));
	iPhonebookData = HBufC8::NewL(KPBDataClientHeap); // Create client buffer to store write data 
	iPhonebookDataPtr.Set(const_cast<unsigned char*>(iPhonebookData->Ptr()), 0, KPBDataClientHeap);
	iPhonebookBuffer->Set(&iPhonebookDataPtr); // Set iPBBuffer to point to the phonebook data buffer

	//
	// Get the PhBkStore & PhBkInfo for later use...
	//
	User::LeaveIfError(iPhonebookManager.GetPhoneBookStore(iPhonebookUid, iPhone, iPhonebookStore));
	User::LeaveIfError(iPhonebookManager.GetPhoneBookInfo(iPhonebookUid, iPhBkInfoV5));

	//
	// Read the ICC Entry streamed buffer...
	//
	iClientMessage.ReadL(3, iIccEntryPtr);

	CActiveScheduler::Add(this);
	} // CWriteContactToICC::ConstructL


/** 
 *  Method to write to ICC after checking slot is empty and memory is available.
 */
void CWriteContactToICC::DoIccWriteL()
	{
	LOGACTIVE1(_L8("CWriteContactToICC::DoIccWriteL()"));

	//
	// Check that we are not in use!
	//
	if (iIccWriteState != EPBSyncIccWriteIdle)
		{
		PhBkSyncPanic(EPhBkSyncPanicDoIccWriteError);
		}

	//
	// Restore streamed contact data...
	//
	iIccEntry->RestoreL(iIccEntryPtr);
	iIccEntry->iPhonebookUid = iPhonebookUid;

#ifdef _DEBUG
	iIccEntry->LogSyncContactICCEntry();
#endif

	//
	// Determine if the requested write is to add a new entry, edit an
	// existing entry, or move an existing entry from one slot to another.
	//
	if (iIccEntry->iContactId == KNullContactId ||
	    iIccEntry->iContactId == KGoldenTemplateId)
		{
		iTypeOfWrite = CWriteContactToICC::EWriteAddNew;
		}
	else
		{
		User::LeaveIfError(iPhonebookManager.GetSlotNumFromContactId(iPhonebookUid,
		 															 iIccEntry->iContactId,
		 															 iOldSlot));

		if (iOldSlot != iIccEntry->iSlotNum)
			{
			iTypeOfWrite = CWriteContactToICC::EWriteEditSlotNumber;
			}
		else
			{
			iTypeOfWrite = CWriteContactToICC::EWriteEdit;
			}
		}

	//
	// Check if a destination slot is specified and if that slot is already used...
	//
	if (iIccEntry->iSlotNum != KSyncIndexNotSupplied)
		{
		TContactItemId  tempId;
		TInt  result;
		
		result = iPhonebookManager.GetContactIdFromSlotNum(iIccEntry->iPhonebookUid,
														   iIccEntry->iSlotNum,
														   tempId);
		if (result != KErrNotFound)
			{
			iIsUsedSlot = ETrue;
			}
		}

	//
	// This is an entry requiring an empty slot...
	//
	if (iIccEntry->iSlotNum == KSyncIndexNotSupplied)
		{
		TInt  result;

		result = iPhonebookManager.GetFirstEmptySlot(iIccEntry->iPhonebookUid,
													 iIccEntry->iSlotNum);

		if (result != KErrNone)
			{
			//
			// No empty slots exist...
			//
			LOGACTIVE1(_L8("CWriteContactToICC::DoIccWriteL(): No empty slots found."));
			iSession.Server().CompleteWriteContactToICC(KErrNoMemory);
			return;
			}
		}

	//
	// Encode ICC entry into TLV format...
	//
	TRAPD(ret, EncodeICCEntryL());
	if (ret != KErrNone)
		{
		//
		// Complete client request with the error...
		//
		iSession.Server().CompleteWriteContactToICC(ret);
		return;
		}
	
	//
	// Write the entry to the ICC...
	//
	iPhonebookStore.Write(iStatus, iPhonebookDataPtr, iIccEntry->iSlotNum);
	iIccWriteState = EPBSyncIccWriteWaitForIccWrite;
	SetActive();
	
	LOGACTIVETIMESTAMP();
	} // CWriteContactToICC::DoIccWriteL


/**
 *  Encodes the iIccEnty into the TLV format used by the ETel server. 
 */
void CWriteContactToICC::EncodeICCEntryL()
	{
	//
	// First add new entry tag...
	//
	User::LeaveIfError(iPhonebookBuffer->AddNewEntryTag());

	TInt i;

	// Get first representation of name
	// convert text into TLV format and append it to allocated buffer
	User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, iIccEntry->iName));

	// Get second representation of name
	// convert text into TLV format and append it to allocated buffer
	User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBSecondName, iIccEntry->iSecondName));

	// Get default number and its TON 
	// First convert number into TLV format and append it to allocated buffer
	User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, iIccEntry->iNumber));
		
	// convert type into TLV format and append it to allocated buffer
	User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, ConvertTonNpiByte(iIccEntry->iTON)));

	// Get list of additional numbers 
	TInt count = iIccEntry->iNumberList->Count();
	for(i=0; i < count; ++i)
		{
		CSyncContactICCEntry::TSyncAdditionalNumber additionalNumber;
		additionalNumber = iIccEntry->iNumberList->At(i);

		// Mark the beginning of an additional number 
		User::LeaveIfError(iPhonebookBuffer->AddNewNumberTag());

		// convert text into TLV format and append it to allocated buffer
		if (iPhBkInfoV5.iMaxTextLengthAdditionalNumber > 0)
			{
			User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, additionalNumber.iNumberString));
			}
	
		// First convert number into TLV format and append it to allocated buffer
		User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, additionalNumber.iNumber));
			
		// convert type into TLV format and append it to allocated buffer
		User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, ConvertTonNpiByte(additionalNumber.iTON)));
	}

	// Get list of groups 
	count = iIccEntry->iGroupList->Count();
	for(i=0; i < count; ++i)
		{
		CSyncContactICCEntry::TSyncEntryName group(iIccEntry->iGroupList->At(i));
		// convert group into TLV format and append it to allocated buffer
		User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBGroupName, group));
		}

	// Get list of e-mails
	count = iIccEntry->iEmailList->Count();
	for(i=0; i < count; ++i)
		{
		CSyncContactICCEntry::TSyncEntryName email;
		email.Append(iIccEntry->iEmailList->At(i));
		// convert e-mail into TLV format and append it to allocated buffer
		User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress, email));
		}

	// Append hidden entry info here - this can only be added when V2 format is being used
	if (iSession.Server().IsV2Tsy())
		{
		User::LeaveIfError(iPhonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBHiddenInfo,(TUint8)iIccEntry->iIsHidden));
		}
	} // CWriteContactToICC::EncodeICCEntryL


/** 
 *  Standard Active Object RunL() method to process the write request.
 */
void CWriteContactToICC::RunL()
	{
	LOGACTIVE2(_L8("CWriteContactToICC::RunL(): iStatus=%d"), iStatus.Int());
	LOGACTIVETIMESTAMP();
	
	switch (iIccWriteState)
		{
		case EPBSyncIccWriteWaitForIccWrite:
			{
			if (iStatus.Int() == KErrNone)
				{
				//
				// Update server's look-up table...
				//
				User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid, iIccEntry->iSlotNum,
																		KNullContactId, ESlotUsedAvailable));

				if (iTypeOfWrite != EWriteAddNew)
					{
					// If this is a new entry the ID will be set by the contact model
					// If this is an edit the ID needs to be set here
					User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid, iIccEntry->iSlotNum, iIccEntry->iContactId));
					}

				// Are we editing slot number and need to remove entry at old slot?
				if (iTypeOfWrite == EWriteEditSlotNumber  &&
					iOldSlot != KErrNotFound)
					{
					// Decrement the number of used entries for the phonebook
					User::LeaveIfError(iPhonebookManager.AdjustPhoneBookInfoUsedEntries(iIccEntry->iPhonebookUid, -1));
						
					iIccWriteState = EPBSyncIccWriteDeleteOldSlot;//need to delete entry at old slot
					iPhonebookStore.Delete(iStatus, iOldSlot);
					SetActive();
					}
				else
					{
					iIccWriteState = EPBSyncIccWriteIdle;
					// If the entry is written to an unused slot increment the number of entries for the phonebook 

					if (!iIsUsedSlot)
						{
						User::LeaveIfError(iPhonebookManager.AdjustPhoneBookInfoUsedEntries(iIccEntry->iPhonebookUid, 1));
						}
						
					iSession.Server().CompleteWriteContactToICC(iStatus.Int());
					}
				}
			else
				{
				iIccWriteState = EPBSyncIccWriteIdle;
				iSession.Server().CompleteWriteContactToICC(iStatus.Int()); 
				}
			}
			break;

		case EPBSyncIccWriteDeleteOldSlot:
			{
			//
			// The client has requested to change the slot number of an entry
			// after the entry has been written at the new slot.
			//
			// Update the look up table even if the delete was not successful
			// (It would be very bad to have 2 entries with the same UID on
			// the table).
			//
			User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid, iOldSlot,
																	KNullContactId, ESlotEmpty));

			//
			// If the entry is written to an unused slot increment the number
			// of entries for the phonebook...
			//
			if (iIsUsedSlot == EFalse)
				{
				User::LeaveIfError(iPhonebookManager.AdjustPhoneBookInfoUsedEntries(iIccEntry->iPhonebookUid, 1));
				}
	
			iIccWriteState = EPBSyncIccWriteIdle;
			iSession.Server().CompleteWriteContactToICC(iStatus.Int()); 
			}
			break;

		case EPBSyncIccWriteWaitForCancel:
			{
			iIccWriteState = EPBSyncIccWriteIdle;
			iSession.Server().CompleteWriteContactToICC(iStatus.Int());
			}
			break;

		default:
			{
			PhBkSyncPanic(EPhBkSyncPanicDoIccWriteError);
			}
			break;
		}
	} // CWriteContactToICC::RunL


/** 
 *  Standard Active Object DoCancel method called when the objects Cancel()
 *  method is called.
 */
void CWriteContactToICC::DoCancel()
	{
 	if (iIccWriteState == EPBSyncIccWriteWaitForIccWrite)
		{
 		iPhonebookStore.CancelAsyncRequest(EMobilePhoneBookStoreWrite);
 		iIccWriteState = EPBSyncIccWriteWaitForCancel;
		}
	else if(iIccWriteState == EPBSyncIccWriteDeleteOldSlot)
		{
		//
		// Don't change the state here. We will still need to update the look
		// up table if the entry is not deleted from the ICC in order not to
		// have 2 entries with the same UID.
		//
		iPhonebookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
		}
	} // CWriteContactToICC::DoCancel


/**
 *  Standard Active Object RunError method called when the objects RunL()
 *  method leaves.
 *
 *  Hopefully this method should never be called.
 *
 *  @param aError  Leave code from the RunL().
 *
 *  @return  KErrNone is returned although the server will panic first.
 */
TInt CWriteContactToICC::RunError(TInt aError)
	{
#ifdef _DEBUG
	LOGACTIVE2(_L8("CWriteContactToICC::RunError(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);

	return KErrNone;	
	} // CWriteContactToICC::RunError


/**
 *  Convert a ETel TON to an ICC TON value.
 *
 *  @param aTon  Should be an ETSI Type-Of-Number field.
 *
 *  @return The equivalent RMobilePhone::TMobileTON value for aValue
 */
TUint8 CWriteContactToICC::ConvertTonNpiByte(RMobilePhone::TMobileTON aTon) 
	{
	TUint8  result(0x81);
	
	//
	// The below 'magic numbers' come from the ETSI 03.40
	// specification for Address Fields (section 9.1.2.5)
	//
	switch (aTon)
		{
		case RMobilePhone::EUnknownNumber:
			{	
			result = 0x81;
			}
			break;

		case RMobilePhone::EInternationalNumber:
			{	
			result = 0x91;
			}
			break;

		case RMobilePhone::ENationalNumber:
			{	
			result = 0xA1;
			}
			break;

		case RMobilePhone::ENetworkSpecificNumber:
			{	
			result = 0xB1;
			}
			break;

		case RMobilePhone::ESubscriberNumber:
			{	
			result = 0xC1;
			}
			break;

		default:
			{
			result = 0x81;
			}
			break;
		}	

	return result;
	} // CWriteContactToICC::ConvertTonNpiByte

