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
// Contains method implementations for the Active Object classes used by the Phonebook 
// Synchroniser.
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
#include "SyncContactsWithICC.h"


/**
 *  Number of retries to read the ICC in an ICC multiple read operation.
 */
const TInt KMaxICCReadRetryCnt = 1;


/**
 *	Delay for retry (will be multiplied by an increasing factor).
 */
const TInt KReadRetryDelayBase = 100000;



/**
 *  Static factory method used to create a CSyncContactsWithICC object.
 *
 *  @param aSession           Reference to the engine session.
 *  @param aPhonebookManager  Reference to the Phonebook Manager which stores
 *                            all the phonebook parameters.
 *  @param aDb                Reference to the database.
 *  @param aPhone             Handle to the phone object.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 */
CSyncContactsWithICC* CSyncContactsWithICC::NewL(CSyncEngineSession& aSession,
												 CPhoneBookManager& aPhoneBookManager,
												 CContactDatabase& aDb,
												 RMobilePhone& aPhone,
												 TUid aPhonebookUid,
												 const RMessage2& aClientMessage)
	{
	CSyncContactsWithICC*  self = new(ELeave) CSyncContactsWithICC(aSession,
																   aPhoneBookManager,
																   aDb,
																   aPhone,
																   aPhonebookUid,
																   aClientMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CSyncContactsWithICC::NewL


/**
 *  Standard constructor.
 *
 *  @param aSession           Reference to the engine session.
 *  @param aPhonebookManager  Reference to the Phonebook Manager which stores
 *                            all the phonebook parameters.
 *  @param aDb                Reference to the database.
 *  @param aPhone             Handle to the phone object.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 */
CSyncContactsWithICC::CSyncContactsWithICC(CSyncEngineSession& aSession,
										   CPhoneBookManager& aPhonebookManager,
										   CContactDatabase& aDb,
										   RMobilePhone& aPhone,
										   TUid aPhonebookUid,
										   const RMessage2& aClientMessage)
  : CActive(EPriorityNormal),
	iSession(aSession),
	iPhonebookManager(aPhonebookManager),
	iDb(aDb),
	iPhone(aPhone),
	iPhonebookUid(aPhonebookUid),
	iClientMessage(aClientMessage),
	iState(ESyncContactsWithICCStateIdle),
	iPhonebookDataPtr(NULL,0),
	iNumOfEntries(1),
	iSlotNum(1)
	{
	// NOP
	} // CSyncContactsWithICC::CSyncContactsWithICC


/**
 *  Standard destructor. This will cancel any pending active requests.
 */
CSyncContactsWithICC::~CSyncContactsWithICC()
	{
	LOGACTIVE1(_L8("~CSyncContactsWithICC()"));

	Cancel();

	delete iPhonebookData;
	iUnconfirmedEntries.Close();
	iTimer.Close();
	} // CSyncContactsWithICC::~CSyncContactsWithICC


/**
 *  Second phase constructor.
 */
void CSyncContactsWithICC::ConstructL()
	{
	LOGACTIVE1(_L8("CSyncContactsWithICC::ConstructL()"));

	//
	// Create the phonebook data buffer.
	//
	iPhonebookData = HBufC8::NewL(KPBDataClientHeap);
	iPhonebookDataPtr.Set(const_cast<unsigned char*>(iPhonebookData->Ptr()), 0, KPBDataClientHeap);

	//
	// Obtain a few bits of info now to save time later...
	//
	User::LeaveIfError(iPhonebookManager.GetPhoneBookInfo(iPhonebookUid, iPhBkInfo));
	User::LeaveIfError(iPhonebookManager.GetPhoneBookStore(iPhonebookUid, iPhone, iPhonebookStore));

	//
	// Create a timer for use later if needed...
	//
	iTimer.CreateLocal();

	CActiveScheduler::Add(this);
	} // CSyncContactsWithICC::ConstructL


/**
 *  Begin synchronising the ICC phonebook into the Contacts Database. This
 *  function begins the process be ensuring the Look-Up Table is upto date
 *  with the information from the Contacts DB. Any contact entires that are
 *  stored outside the slot range of the ICC (e.g. entries from previous ICCs)
 *  are removed.
 */
void CSyncContactsWithICC::SyncContactsWithICCL()
	{
	LOGACTIVE1(_L8("CSyncContactsWithICC::SyncContactsWithICCL()"));

	//
	// Check that we are not in use!
	//
	if (iState != ESyncContactsWithICCStateIdle)
		{
		PhBkSyncPanic(EPhBkSyncPanicDoMultipleReadError);
		}

	//
	// Fetch all current database entries into the Look-Up Table. Also we
	// need to delete all entires outside the size of the phonebook. Start by
	// getting the size of the phonebook and clearing the current table data.
	//
	TInt  phonebookSize;
	
	User::LeaveIfError(iPhonebookManager.GetLookUpTableSize(iPhonebookUid,
															phonebookSize));
	User::LeaveIfError(iPhonebookManager.ClearLookUpTable(iPhonebookUid));

	//
	// Get the Group ID to search for entries. If no ID exists then the
	// database does not contain any entries.
	//
	TContactItemId  groupId;
	
	User::LeaveIfError(iPhonebookManager.GetGroupId(iPhonebookUid, groupId));

 	
 	//
 	// Populate the lookup table with entries from the contacts database...
 	//
 	if (groupId != KNullContactId  &&  groupId != KGoldenTemplateId)
		{
 		CContactItem*  group = iDb.ReadContactLC(groupId);
 		const CContactIdArray*  members = static_cast<CContactGroup*>(group)->ItemsContained();
 		const TInt  count = members->Count();

		TContactTextDefItem  iccSlotFieldType(KUidContactFieldICCSlot);
		CContactTextDef*  iccSlotFieldTextDef = CContactTextDef::NewLC();
		iccSlotFieldTextDef->AppendL(iccSlotFieldType);

		for (TInt item = 0;  item < count;  item++)
			{
			//
			// Read the item from the database and add to the Look-Up Table.
			// The state is unconfirmed until the synchronisation is complete.
			//
			TBuf<KTemplateNumberLength>  iccSlotField;

			iDb.ReadContactTextDefL((*members)[item], iccSlotField,
									iccSlotFieldTextDef);
			TLex  input(iccSlotField);
			TInt  slotNum = 0;
			input.Val(slotNum);

			//
			// Delete entries that are in slots exceeding the ICC slot range,
			// otherwise add them to the Look Up Table.
			//
			if (slotNum > phonebookSize)
				{
				iDb.DeleteContactL((*members)[item]);
				}
			else
				{
				User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid,
																		slotNum,
																		(*members)[item],
																		ESlotUnconfirmed));
				}
			}

		CleanupStack::PopAndDestroy(2, group);
 		}

	//
	// Now begin the process of reading entries and writting them to the database.
	//
	ReadICCEntries();
	} // CSyncContactsWithICC::SyncContactsWithICCL


/** 
 *  Issues a read of one or more ICC entries.
 */
void CSyncContactsWithICC::ReadICCEntries()
	{
	LOGACTIVE1(_L8("CSyncContactsWithICC::ReadICCEntries()"));

	//
	// Now issue the ICC read and set the object Active...
	//
	iPhonebookStore.Read(iStatus, iSlotNum, 1, iPhonebookDataPtr);
	iState = ESyncContactsWithICCStateWaitForRead;
	iDbAccessRetryCount=0;
	iICCReadRetryCount=0;
	SetActive();

	LOGACTIVETIMESTAMP();
	} // CSyncContactsWithICC::ReadICCEntries


/** 
 *  Standard Active Object RunL() method to process the various sync requests.
 */
void CSyncContactsWithICC::RunL()
	{
	LOGACTIVE2(_L8("CSyncContactsWithICC::RunL() iStatus=%d)"),
			   iStatus.Int());

	switch (iState)
		{
		case ESyncContactsWithICCStateWaitForRead:
			{
			//
			// If the request was completed, cancelled or no more entries were found...
			//
			if (iStatus.Int() == KErrNone  ||
				iStatus.Int() == KErrCancel  || 
			    iStatus.Int() == KErrNotFound)
				{
				TInt  ret(iStatus.Int());

				iICCReadRetryCount=0;

				//
				// Now decode received phonebook data and store it in Contacts DB...
				//
				TRAPD(decodeErr, DecodeICCAndStoreToDbL(iSlotNum, iReceivedEntries));
				if (decodeErr != KErrNone)
					{
					iSession.Server().RollbackIccTransaction();

					if (iDbAccessRetryCount < KMaxDbAccessRetryCount  &&
						(decodeErr == KErrInUse  ||  decodeErr == KErrLocked))
						{					
						iDbAccessRetryCount++;			
						LOGACTIVE2(_L8("iDbAccessErrorCount is %d"),iDbAccessRetryCount);

						iTimer.After(iStatus, KReadRetryDelayBase*iDbAccessRetryCount);//increase the delay each time
						SetActive();
						LOGACTIVETIMESTAMP();
						}
					else
						{
						iSession.Server().CompleteDoSync(decodeErr);
						}
						
					return;
					}

				iDbAccessRetryCount = 0; // reset the DB retry counter

				//
				// Have we finished reading yet?
				//
				if ((iPhBkInfo.iUsedEntries < 0  ||  iReceivedEntries < iPhBkInfo.iUsedEntries)  &&
				    iSlotNum < iPhBkInfo.iTotalEntries)
					{
					//
					// Increment the slot number and read some more...
					//
					iSlotNum++;
					ReadICCEntries();
					}
				else
					{
					//
					// All entries have been read. It is okay to not find an
					// entry in the case where the TSY could not tell you how
					// many entries were used. If there was an error return now.
					//
					if (ret != KErrNone  &&  ret != KErrNotFound)
						{
						iState = ESyncContactsWithICCStateIdle;
						iSession.Server().CompleteDoSync(ret);
						return;
						}

					//
					// Request the list of unconfirmed entries to remove...
					//
					TInt  result;
					
					result = iPhonebookManager.GetMatchingEntries(iPhonebookUid,
																  ESlotUnconfirmed,
																  iUnconfirmedEntries);
					if (result != KErrNone)
						{
						iSession.Server().CompleteDoSync(result);
						return;
						}

					//
					// Yield and continue processing in the next RunL call.
					//
					iState = ESyncContactsWithICCStateRemoveUnconfirmedEntries;
					YieldToOtherActiveObjects();
					}
				}
			else
				{
				if (iICCReadRetryCount < KMaxICCReadRetryCnt)
					{					
					iICCReadRetryCount++;
					iPhonebookDataPtr.Zero();	
					iPhonebookStore.Read(iStatus, iSlotNum, 1, iPhonebookDataPtr);
					SetActive();
					LOGACTIVETIMESTAMP();
					}
				else
					{
					iSession.Server().CompleteDoSync(iStatus.Int());
					return;
					}
				}
			}
			break;

		case ESyncContactsWithICCStateRemoveUnconfirmedEntries:
			{
			//
			// If we have unconfirmed entries to remove, then delete one of
			// them before yielding to the scheduler (so we don't spend too
			// much time in the RunL() in one go). Otherwise we have finished.
			//
			if (iUnconfirmedEntries.Count() > 0)
				{
				//
				// Get the contact ID for the entry we are going to delete...
				//
				TContactItemId  contactId;
				TInt  result;
				
				result = iPhonebookManager.GetContactIdFromSlotNum(iPhonebookUid,
																   iUnconfirmedEntries[0],
																   contactId);
				if (result != KErrNone)
					{
					iSession.Server().CompleteDoSync(result);
					return;
					}

				//
				// Delete the entry...
				//				
				TRAP(result, iDb.DeleteContactL(contactId));
				if (result != KErrNone)
					{
					iSession.Server().CompleteDoSync(result);
					return;
					}

				//
				// Reset the Look-Up Table entry...
				//
				result = iPhonebookManager.UpdateEntryInTable(iPhonebookUid,
															  iUnconfirmedEntries[0],
															  KNullContactId,
															  ESlotEmpty);
				if (result != KErrNone)
					{
					iSession.Server().CompleteDoSync(result);
					return;
					}
				
				//
				// Remove this entry from the list and yield...
				//
				iUnconfirmedEntries.Remove(0);
				YieldToOtherActiveObjects();
				}
			else
				{
				//
				// Compress the database if needed and we are finished!
				//
				if (iDb.CompressRequired())
					{
					TRAP_IGNORE(iDb.CompactL());
					}

				iState = ESyncContactsWithICCStateIdle;

				iSession.Server().CompleteDoSync(KErrNone);
				}
			}
			break;

		case ESyncContactsWithICCStateWaitForCancel:
			{
			iState = ESyncContactsWithICCStateIdle;
			iSession.Server().CompleteDoSync(iStatus.Int());
			}
			break;

		default:
			{
			PhBkSyncPanic(EPhBkSyncPanicDoMultipleReadError);
			}
			break;
		}
	} // CSyncContactsWithICC::RunL


/**
 *  Simple function that causes the Active Object to yield to the scheduler
 *  and let any other Active Objects (such as the engine) run if they want.
 */
void CSyncContactsWithICC::YieldToOtherActiveObjects()
	{
	LOGACTIVE1(_L8("CSyncContactsWithICC::YieldToOtherActiveObjects()"));

	//
	// Post a dummy request...
	//
	iStatus = KRequestPending;
	SetActive();
	LOGACTIVETIMESTAMP();

	//
	// Now complete the dummy request...
	//
	TRequestStatus*  reqStatusPtr = &iStatus;

	User::RequestComplete(reqStatusPtr, KErrNone);
	} // CSyncContactsWithICC::YieldToOtherActiveObjects


/**
 *  Decode the phonebook entries supplied in iPhoneData. The entries are
 *  converted from TLV (Tag-Length-Value) format into the phonebook server internal
 *  (CSyncContactICCEntry) format and then written to the Contacts Database.
 *  Once they are written to the DB, the server's look-up table is updated
 *  accordingly.
 *
 *  @param aIndex            Returns the slot number of the last decoded
 *                           phonebook entry.
 *  @param aReceivedEntries  Returns the number of the received entries.
 */
void CSyncContactsWithICC::DecodeICCAndStoreToDbL(TInt& aIndex,
												  TInt& aReceivedEntries)
	{
	LOGACTIVE1(_L8("CSyncContactsWithICC::DecodeICCAndStoreToDbL()"));

	TBuf16<KPBDataClientBuf>  buffer;
	TPtrC16  bufPtr(buffer);

	TUint8  tagValue(0);
	TInt  entryCount(0);
	CPhoneBookBuffer::TPhBkTagType  dataType;

	TBool  isAdditionalNumber(EFalse); // Used to indicate beginning of an Additional Number 
	CSyncContactICCEntry::TSyncAdditionalNumber  additionalNumber;

	//
	// Allocate an ICC Entry...
	//
	CSyncContactICCEntry*  iccEntry = CSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntry);

	//
	// Create a CPhoneBookBuffer ready to decode the entries...
	//
	CPhoneBookBuffer*  pbBuffer = new(ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&iPhonebookDataPtr);

	//
	// Now start decoding the phonebook data received in TLV format...
	//
	pbBuffer->StartRead();

	while (pbBuffer->GetTagAndType(tagValue, dataType) == KErrNone)
		{
		switch (tagValue)
			{
			case RMobilePhoneBookStore::ETagPBAdnIndex:
				{
				entryCount++;     // Don't increment this until we find the index
								  // So that we wait for whole entry to be received
				TUint16  index;
				pbBuffer->GetValue(index);			
				iccEntry->iSlotNum = (TInt) index;
				aIndex = iccEntry->iSlotNum;
				}
				break;

			case RMobilePhoneBookStore::ETagPBTonNpi:
				{
				TUint8  tonNpi;

				pbBuffer->GetValue(tonNpi);			
				if (isAdditionalNumber)
					{
					additionalNumber.iTON = ConvertTypeOfNumber(tonNpi); // TON associated with additional number
					
					// We have rearched to the end of one additional number. So, 
				    // we add the complete additionNumber into the list and set 
					// the flag as EFalse. Note we will get a ETagPBAnrStart for
					// each additional number.
					iccEntry->iNumberList->AppendL(additionalNumber);
					isAdditionalNumber = EFalse;
					}
				else
					{
					// TON associated with first (default) number
					iccEntry->iTON = ConvertTypeOfNumber(tonNpi);
					}
				}
				break;

			case RMobilePhoneBookStore::ETagPBText:
				{
				pbBuffer->GetValue(bufPtr);
				if (isAdditionalNumber)
					{
					// For additional number bufPtr contains number alpha string
					additionalNumber.iNumberString.Copy(bufPtr);
					}
				else
					{
					iccEntry->iName.Copy(bufPtr);
					}
				}
				break;

			case RMobilePhoneBookStore::ETagPBSecondName:
				{
				pbBuffer->GetValue(bufPtr);			
				iccEntry->iSecondName.Copy(bufPtr);
				}
				break;

			case RMobilePhoneBookStore::ETagPBNumber:
				{
				pbBuffer->GetValue(bufPtr);	
				if (isAdditionalNumber)
					{
					additionalNumber.iNumber.Copy(bufPtr);
					}
				else
					{
					// First number so this will be used as default number
					iccEntry->iNumber.Append(bufPtr);
					}
				}
				break;

			case RMobilePhoneBookStore::ETagPBAnrStart:
				{
				// This tag should precede every additional number entry
				isAdditionalNumber = ETrue;
				}
				break;

			case RMobilePhoneBookStore::ETagPBGroupName:
				{
				pbBuffer->GetValue(bufPtr);	
				iccEntry->iGroupList->AppendL(bufPtr);
				}
				break;

			case RMobilePhoneBookStore::ETagPBEmailAddress:
				{
				pbBuffer->GetValue(bufPtr);
				iccEntry->iEmailList->AppendL(bufPtr);
				}
				break;

			case RMobilePhoneBookStore::ETagPBHiddenInfo:
				{
	 			TUint8  hidden;
				
				pbBuffer->GetValue(hidden);
				if (hidden != 0)
					{
					iccEntry->iIsHidden = ETrue;
					}
				}
				break;

			case RMobilePhoneBookStore::ETagPBNewEntry:
				{
				//
				// This signals the end of the entry and is a special case
				// which will be handled below.
				//
				}
				break;

			default:
				{
				//
				// An unsupported field type - just skip this value
				//
				pbBuffer->SkipValue(dataType);
				}
				break;
			}

		//
		// Has the whole entry been extracted?
		//
		if ((tagValue == RMobilePhoneBookStore::ETagPBNewEntry  &&  entryCount > 0) ||
			(pbBuffer->RemainingReadLength() == 0  &&  entryCount > 0))
			{
			iccEntry->iPhonebookUid = iPhonebookUid;

#ifdef _DEBUG
			iccEntry->LogSyncContactICCEntry();
#endif
			
			//
			// Write the entry to the Contacts Database...
			//
			TContactItemId id(KNullContactId);
			TPhonebookSlotState slotState(ESlotUsedAvailable);
			if (iccEntry->iIsHidden)
				// Hidden entry so ignore all fields for that entry, since they will not 
				// be stored in Contacts database
				{
				slotState = ESlotHiddenNotAvailable;
				// remove any existing entry for this slot from database
				TInt  result = iPhonebookManager.GetContactIdFromSlotNum(iPhonebookUid,
																		  iccEntry->iSlotNum,
																		  id);
				if (result == KErrNone)
					{
					iDb.DeleteContactL(id);
					id = KNullContactId;
					// Populate the phonebook look-up table 
					User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid,
																			iccEntry->iSlotNum,
																			id, slotState));
					}
				}
			else // Not hidden entry or it used to be hidden but its not anymore 
				{
				// So add this entry to Contacts database 
				
				id = iSession.Server().WriteICCContactToDBL(*iccEntry);
				}

			// ICC entry successfully written Contacts DB, so update look-up table
			if((id!=KNullContactId) || (iccEntry->iIsHidden)) 
				{
				// Populate the phonebook look-up table 
				User::LeaveIfError(iPhonebookManager.UpdateEntryInTable(iPhonebookUid,
																		iccEntry->iSlotNum,
																		id, slotState));
				}

			//
			// Loop again...
			//
			iccEntry->Reset();
			aReceivedEntries++;
			} 
		}

	iPhonebookDataPtr.Zero();

	CleanupStack::PopAndDestroy(2, iccEntry);
	} // CSyncContactsWithICC::DecodeICCAndStoreToDbL


/**
 *  Converts an ETSI Type-Of-Number value to a RMobilePhone::TMobileTON.
 *
 *  @param aValue  An ETSI Type-Of-Number field.
 *
 *  @return The equivalent RMobilePhone::TMobileTON value.
 */
RMobilePhone::TMobileTON CSyncContactsWithICC::ConvertTypeOfNumber(TUint8 aValue) const
	{
	//
	// Mask off NPI and move TON to least-sig byte...
	//
	TUint8  aTon = (aValue & KEtsiTonPosition) >> 4;

	//
	// The below 'magic numbers' come from the ETSI 03.40
	// specification for Address Fields (section 9.1.2.5)
	//
	RMobilePhone::TMobileTON  result;

	switch(aTon)
		{
		case 0:
			{
			result = RMobilePhone::EUnknownNumber;
			}
			break;

		case 1:		
			{
			result = RMobilePhone::EInternationalNumber;
			}
			break;

		case 2:
			{
			result = RMobilePhone::ENationalNumber;
			}
			break;

		case 3:
			{
			result = RMobilePhone::ENetworkSpecificNumber;
			}
			break;

		case 4:
			{
			result = RMobilePhone::ESubscriberNumber;
			}
			break;

		default:
			{
			result = RMobilePhone::EUnknownNumber;
			}
			break;
		}
		
	return result;
	} // CSyncContactsWithICC::ConvertTypeOfNumber


/** 
 *  Standard Active Object DoCancel method called when the objects Cancel()
 *  method is called.
 */
void CSyncContactsWithICC::DoCancel()
	{
	if (iState == ESyncContactsWithICCStateWaitForRead)
		{
		iPhonebookStore.CancelAsyncRequest(EMobilePhoneBookStoreRead);
		iState = ESyncContactsWithICCStateWaitForCancel;
		}
	} // CSyncContactsWithICC::DoCancel


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
TInt CSyncContactsWithICC::RunError(TInt aError)
	{
#ifdef _DEBUG
	LOGACTIVE2(_L8("CSyncContactsWithICC::RunError(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);

	return KErrNone;	
	} // CSyncContactsWithICC::RunError
