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
// Contains method implementations for Phonebook Synchroniser's CPhoneBook
// class which stores the parameters for each phonebook, including the
// Look-Up Table, Phonebook store handle and flags for the synchroniser
// engine.
// This class can be accessed from either the server side or engine side.
// 
//

/**
 @file
 @internalComponent
*/

#include "phbksync.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"


/**
 *  Granularity of the Look Up Table.
 */
const TInt KLookUpTableGranularity = 16;


/**
 *  Default constructor for a Look-Up Table entry.
 */
CPhoneBook::TLookUpTableEntry::TLookUpTableEntry()
  : iContactId(KNullContactId),
    iSlotState(ESlotEmpty)
	{
	// NOP
	} // CPhoneBook::TLookUpTableEntry::TLookUpTableEntry


/**
 *  Static factory method to create a CPhoneBook object.
 *
 *  @param aPhonebookUid  UID of the phonebook.
 */
CPhoneBook* CPhoneBook::NewL(TUid aPhonebookUid)
	{
	CPhoneBook*  phonebook = new(ELeave) CPhoneBook(aPhonebookUid);
	CleanupStack::PushL(phonebook);
	phonebook->ConstructL();
	CleanupStack::Pop(phonebook);

	return phonebook;
	} // CPhoneBook::NewL


/**
 *  Standard constructor.
 *
 *  @param aPhonebookUid  UID of the phonebook.
 */
CPhoneBook::CPhoneBook(TUid aPhonebookUid)
  : iPhonebookUid(aPhonebookUid),
    iSyncMode(RPhoneBookSession::EManual),
    iPhBkInfoRetrievedResult(KErrNotReady),
    iSyncState(RPhoneBookSession::EUnsynchronised),
    iLastSyncError(KErrNone),
    iTemplateId(KNullContactId),
    iGroupId(KNullContactId),
    iContactFieldsV3(),
    iPhonebookStore(),
    iPhBkInfoV5(),
	iLookUpTable(KLookUpTableGranularity)
	{
	iPhBkInfoV5.iIdentity.Copy(KPhonebookIdUnknown);
	} // CPhoneBook::CPhoneBook


/**
 *  Standard destructor.
 */
CPhoneBook::~CPhoneBook()
	{
	// NOP
	} // CPhoneBook::~CPhoneBook


/**
 *  Second phase constructor.
 */
void CPhoneBook::ConstructL()
	{
	// NOP
	} // CPhoneBook::ConstructL


/**
 *  Changes the size of the look-up table so that it contains the specified
 *  number of elements.
 *
 *  @param aSize  New desired size of the table.
 */
void CPhoneBook::SetLookUpTableSizeL(TInt aSize)
	{
	TInt  currentSize(iLookUpTable.Count());

    //
	// Are we increasing or decreasing the table size???
	//
	if (currentSize < aSize)
		{
		//
		// Expand the table...
		//
		TLookUpTableEntry  blankEntry;

		for (TInt index = currentSize;  index < aSize;  index++)
			{
			iLookUpTable.AppendL(blankEntry);
			}
		}
	else if (currentSize > aSize)
		{
		//
		// Delete any entries that come after 'aSize', e.g. ensure only
		// 'aSize' number of entries remain.  This is basically deleting
		// from the end to the middle.
		//
		for (TInt index = currentSize - 1;  index >= aSize;  index--)
			{
			iLookUpTable.Delete(index);
			}
		}
	} // CPhoneBook::SetLookUpTableSizeL


/**
 *  Clears the look-up table so that it contains only default values.
 */
void CPhoneBook::ClearLookUpTable()
	{
	TInt  currentSize(iLookUpTable.Count());

	//
	// Initilise the table...
	//
	TLookUpTableEntry  blankEntry;

	for (TInt index = 0;  index < currentSize;  index++)
		{
		iLookUpTable[index] = blankEntry;
		}
	} // CPhoneBook::ClearLookUpTable


/**
 *  Check whether an entry with the given Contacts UID exists in the
 *  look-up-table and whether that entry has already been flagged as
 *  being used.
 *
 *  @param aContactId  Contacts ICC entry UID
 *
 *  @return KErrNone if entry found, otherwise KErrNotFound.
 */
TInt CPhoneBook::IsEntryInTable(TContactItemId aContactId) const
	{
	TInt  totalEntries(iLookUpTable.Count());

	for(TInt index = 0;  index < totalEntries ;  index++)
		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(index);

		if (entry.iContactId == aContactId)
			{
			if (entry.iSlotState == ESlotUsedAvailable  ||
			    entry.iSlotState == ESlotHiddenAvailable)
			    {
				return KErrNone;
				}
			else if (entry.iSlotState == ESlotUsedNotAvailable  ||
			         entry.iSlotState == ESlotHiddenNotAvailable)
				{
				return KErrAccessDenied;
				}
			}
		}

	return KErrNotFound;
	} // CPhoneBook::IsEntryInTable


/**
 *  Update the Contact UID parameter for a phonebook entry in the Look-Up
 *  Table. The entry is identified by the aSlotNum parameter.
 *
 *  @param aSlotNum     Phonebook entry slot number
 *  @param aContactId  Contacts ICC entry UID
 *
 *  @return KErrNone if entry found, otherwise KErrNotFound
 */
TInt CPhoneBook::UpdateEntryInTable(TInt aSlotNum,
									TContactItemId aContactId)
	{
	if (aSlotNum >= 1  &&  aSlotNum <= iLookUpTable.Count())
		{
		TLookUpTableEntry&  entry = iLookUpTable.At(aSlotNum-1);
		
		if (entry.iSlotState == ESlotUsedAvailable  &&
 		    (entry.iContactId == KNullContactId  ||
		     entry.iContactId == KGoldenTemplateId))
			{
			entry.iContactId = aContactId;
			return KErrNone;
			}
		}
	
	return KErrNotFound;
	} // CPhoneBook::UpdateEntryInTable


/**
 *  Update parameters for a phonebook entry in the Look-Up Table. The entry
 *  is identified by the aSlotNum parameter whereas the information to be
 *  updated is supplied by the aContactId & aSlotState parameters.
 *
 *  @param aSlotNum    Phonebook entry slot number
 *  @param aContactId  Contacts ICC entry UID
 *  @param aSlotState  Phonebook entry slot state
 *
 *  @return KErrNone if entry found, otherwise KErrNotFound
 */ 
TInt CPhoneBook::UpdateEntryInTable(TInt aSlotNum,
									TContactItemId aContactId,
									TPhonebookSlotState aSlotState)
	{
	if (aSlotNum >= 1  &&  aSlotNum <= iLookUpTable.Count())
		{
		TLookUpTableEntry&  entry = iLookUpTable.At(aSlotNum-1);
		
		entry.iContactId = aContactId;
		entry.iSlotState = aSlotState;

		return KErrNone;
		}

	return KErrNotFound;
	} // CPhoneBook::UpdateEntryInTable


/**
 *  Return a Contacts UID for the given slot number.
 *
 *  @param aSlotNum  Slot Number of the entry.
 *
 *  @return Contacts UID if that entry found, otherwise KNullContactId. 
 */
TContactItemId CPhoneBook::GetContactIdFromSlotNum(TInt aSlotNum) const
	{
	if (aSlotNum >= 1  &&  aSlotNum <= iLookUpTable.Count())
		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(aSlotNum-1);
		
		return entry.iContactId;
		}

	return KNullContactId;
	} // CPhoneBook::GetContactIdFromSlotNum


/**
 *  Return a slot number for the given Contacts UID.
 *
 *  @param aContactId  Contacts UID of the entry.
 *
 *  @return Slot Number if that entry found, otherwise KErrNotFound.
 */
TInt CPhoneBook::GetSlotNumFromContactId(TContactItemId aContactId) const
	{
	//
	// Take care of some default values...
	//
	if (aContactId == KNullContactId  ||
		aContactId == KGoldenTemplateId)
		{
		return KErrNotFound;
		}
	
	//
	// Search the table for the Contact UID...
	//
	TInt  totalEntries(iLookUpTable.Count());

	for (TInt index = 0;  index < totalEntries;  index++)
 		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(index);

		if (entry.iContactId == aContactId)
			{
			return index+1; // Slot numbers start from 1
			}
		}

	return KErrNotFound;
	} // CPhoneBook::GetSlotNumFromContactId


/**
 *  Return the count of free slots for this phonebook. 
 *
 *  @return Number of slots free.
 */
TInt CPhoneBook::GetNumFreeSlots() const
	{
	TInt  numFreeSlots(0);

	//
	// Scan through the table and count the empty slots...
	//
	TInt  totalEntries(iLookUpTable.Count());

	for (TInt index = 0;  index < totalEntries;  index++)
 		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(index);

		if (entry.iSlotState == ESlotEmpty)
			{
			numFreeSlots++;
			}
		}
		
	return numFreeSlots;
	} // CPhoneBook::GetNumFreeSlots


/**
 *  Return the first available slot from the look-up table.
 *
 *  @return Slot Number if an empty slot found, otherwise KErrNotFound.
 */
TInt CPhoneBook::GetFirstEmptySlot() const
	{
	TInt  totalEntries(iLookUpTable.Count());

	for (TInt index = 0;  index < totalEntries;  index++)
 		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(index);

		if (entry.iSlotState == ESlotEmpty)
			{
			return index+1; // Slot numbers start from 1
			}
		}

	return KErrNotFound;
	} // CPhoneBook::GetFirstEmptySlot


/**
 *  Return the slot numbers of all matching entries.
 *
 *  @param aSlotState  Slot state to search for.
 *  @param aEntries    RArray containing the slot numbers of all matching entries.
 */
void CPhoneBook::GetMatchingEntries(TPhonebookSlotState aSlotState,
									RArray<TInt>& aEntries) const
	{
	TInt  totalEntries(iLookUpTable.Count());

	for (TInt index = 0;  index < totalEntries;  index++)
 		{
		const TLookUpTableEntry&  entry = iLookUpTable.At(index);

		if (entry.iSlotState == aSlotState)
			{
			aEntries.Append(index+1); // Slot numbers start from 1
			}
		}
	} // CPhoneBook::GetMatchingEntries


/**
 *  Output the Look-Up Table to the debug log file.
 *
 *  @note This function only works in debug mode.
 */
void CPhoneBook::LogLookUpTable() const
	{
	LOGPARAMS2(_L8("LogLookUpTable(): Phonebook=0x%08x"), iPhonebookUid);
	
#ifdef _DEBUG
	//
	// Write the contents of the table...
	//
	TInt  tableSize(iLookUpTable.Count());

	for (TInt index = 0;  index < tableSize;  index++)
		{
		const TLookUpTableEntry&  entry = iLookUpTable[index];
		TBuf8<32>  slotStateStr;

		switch (entry.iSlotState)
			{
			case ESlotUnconfirmed:
				{
				slotStateStr.Copy(_L8("Unconfirmed       "));
				}
				break;

			case ESlotEmpty:
				{
				slotStateStr.Copy(_L8("Empty             "));
				}
				break;

			case ESlotUsedAvailable:
				{
				slotStateStr.Copy(_L8("UsedAvailable     "));
				}
				break;

			case ESlotUsedNotAvailable:
				{
				slotStateStr.Copy(_L8("UsedNotAvailable  "));
				}
				break;

			case ESlotHiddenAvailable:
				{
				slotStateStr.Copy(_L8("HiddenAvailable   "));
				}
				break;

			case ESlotHiddenNotAvailable:
				{
				slotStateStr.Copy(_L8("HiddenNotAvailable"));
				}
				break;

			default:
				{
				slotStateStr.Copy(_L8("Illegal_Enum_value"));
				}
				break;
			}

		LOGPARAMS5(_L8("LUT[%03d]:  %3d  0x%08x  %S"), index, index+1,
				   entry.iContactId, &slotStateStr);
		}
#endif
	} // CPhoneBook::LogLookUpTable
