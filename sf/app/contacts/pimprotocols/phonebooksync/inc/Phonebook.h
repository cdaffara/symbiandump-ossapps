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
// Header file for Phonebook Synchroniser's CPhoneBook class which stores
// the parameters for each phonebook, including the Look-Up Table, Phonebook
// store handle and flags for the synchroniser engine.
// This class can be accessed from either the server side or engine side.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHONEBOOK_H__
#define __PHONEBOOK_H__

#include <etelmm.h>

#include "phbksync.h"
#include "phbksynclog.h"


/**
 *  Default identity for a phonebook before its identity is read.
 */
_LIT8(KPhonebookIdUnknown, "unknown");


/**
 *  State of a slot in the Look-Up Table.
 */
enum TPhonebookSlotState
	{
	ESlotUnconfirmed,
	ESlotEmpty,
	ESlotUsedAvailable,
	ESlotUsedNotAvailable,
	ESlotHiddenAvailable,
	ESlotHiddenNotAvailable
	};


/**
 *  This class is used to store the Phonebook Sync Server's internal phonebook
 *  parameters and Look-Up Table. The Look-Up Table is used to store the Contacts
 *  UID, slot number and slot state.
 */
class CPhoneBook : public CBase 
	{
private:
	class TLookUpTableEntry
		{
	public:
		TLookUpTableEntry();
	
	public:
		TContactItemId  iContactId;
		TPhonebookSlotState  iSlotState;
		};

public:
	static CPhoneBook* NewL(TUid aPhonebookUid);
	void ConstructL();
	~CPhoneBook();
	
	inline TUid GetPhonebookUid() const;
	
	inline RPhoneBookSession::TPhonebookSyncMode GetSyncMode() const;
	inline void SetSyncMode(RPhoneBookSession::TPhonebookSyncMode aSyncMode);
	
	inline TInt GetPhBkInfoRetrievedResult() const;
	inline void SetPhBkInfoRetrievedResult(TInt aErrorCode);
	
	inline RPhoneBookSession::TSyncState GetSyncState() const;
	inline void SetSyncState(RPhoneBookSession::TSyncState aSyncState);

	inline TInt GetLastSyncError() const;
	inline void SetLastSyncError(TInt aLastSyncError);

	inline TContactItemId GetTemplateId() const;
	inline void SetTemplateId(TContactItemId aId);

	inline TContactItemId GetGroupId() const;
	inline void SetGroupId(TContactItemId aId);

	inline const RPhoneBookSession::TContactFieldsV3& GetContactFields() const;
	inline void SetContactFields(const RPhoneBookSession::TContactFieldsV3& aContactFields);

	inline const RMobilePhoneBookStore& GetPhoneBookStore(RMobilePhone& aPhone);
	inline void ClosePhoneBookStore();

	inline const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& GetPhoneBookInfo() const;
	inline void SetPhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo);
	inline void AdjustPhoneBookInfoUsedEntries(TInt aChange);

	inline TInt GetLookUpTableSize() const;
	void SetLookUpTableSizeL(TInt aSize);
	void ClearLookUpTable();

	TInt IsEntryInTable(TContactItemId aContactId) const;
	TInt UpdateEntryInTable(TInt aSlotNum, TContactItemId aContactId);
	TInt UpdateEntryInTable(TInt aSlotNum, TContactItemId aContactId,
							TPhonebookSlotState aSlotState);
	TContactItemId  GetContactIdFromSlotNum(TInt aSlotNum) const;
	TInt GetSlotNumFromContactId(TContactItemId aContactId) const;

	TInt GetNumFreeSlots() const;
	TInt GetFirstEmptySlot() const;
	void GetMatchingEntries(TPhonebookSlotState aSlotState,
							RArray<TInt>& aEntries) const;

	void LogLookUpTable() const;

private:
	CPhoneBook(TUid aPhonebookUid);

private:
	TUid  iPhonebookUid;
	RPhoneBookSession::TPhonebookSyncMode  iSyncMode;
	TInt  iPhBkInfoRetrievedResult;
	RPhoneBookSession::TSyncState  iSyncState;
	TInt  iLastSyncError;
	TContactItemId  iTemplateId;
	TContactItemId  iGroupId;
	RPhoneBookSession::TContactFieldsV3  iContactFieldsV3;
	RMobilePhoneBookStore  iPhonebookStore;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5  iPhBkInfoV5;
	CArrayFixFlat<TLookUpTableEntry>  iLookUpTable;
	};


/**
 *  Return the ICC phonebook type. This will be one of the supported
 *  phonebooks. 
 *
 *  @return UID value used for this phone store.
 */
inline TUid CPhoneBook::GetPhonebookUid() const
	{
	return iPhonebookUid;
	} // CPhoneBook::GetPhonebookUid


/**
 *  Return the phonebook cache state.
 *
 *  @return The RPhoneBookSession::TSyncState cache state for this phonebook.
 */
inline RPhoneBookSession::TSyncState CPhoneBook::GetSyncState() const
	{
	return iSyncState;
	} // CPhoneBook::GetSyncState


/**
 *  Set the phonebook cache state.
 *
 *  @param aSyncState  The phonebook cache state.
 */
inline void CPhoneBook::SetSyncState(RPhoneBookSession::TSyncState aSyncState)
	{
	iSyncState = aSyncState;
	} // CPhoneBook::SetSyncState


/**
 *  Return last error from phone sync operation.
 *
 *  @return Returns the error stored during the last synchronisation.
 */
inline TInt CPhoneBook::GetLastSyncError() const
	{
	return iLastSyncError;
	} // CPhoneBook::GetLastSyncError


/**
 *  Set last error from a synchronisation operation for this phonebook.
 *
 *  @param aLastSyncError  New last error value.
 */
inline void CPhoneBook::SetLastSyncError(TInt aLastSyncError)
	{
	iLastSyncError = aLastSyncError;
	} // CPhoneBook::SetLastSyncError


/**
 *  Provides the contact fields supported by the phonebook.
 *
 *  @return Returns a RPhoneBookSession::TContactFieldsV3 field format object.
 */
inline const RPhoneBookSession::TContactFieldsV3& CPhoneBook::GetContactFields() const
	{
	return iContactFieldsV3;
	} // CPhoneBook::GetContactFields


/**
 *  Sets the contact fields supported by the phonebook.
 *
 *  @param aContactFields  Contacts field format data.
 */
inline void CPhoneBook::SetContactFields(const RPhoneBookSession::TContactFieldsV3& aContactFields)
	{
	iContactFieldsV3 = aContactFields;
	} // CPhoneBook::SetContactFields


/**
 *  Get the Phonebook Store used by the engine for the phonebook.
 *
 *  @param aPhone  Handle to the mobile phone, used to open the store if needed.
 *
 *  @return The mobile phone store handle.
 */
inline const RMobilePhoneBookStore& CPhoneBook::GetPhoneBookStore(RMobilePhone& aPhone)
	{
	//
	// Make sure the store is open...
	//
	if (iPhBkInfoRetrievedResult == KErrNone  &&
		iPhonebookStore.SubSessionHandle() == 0)
		{
		TInt  result(KErrNotSupported);

		if (iPhonebookUid == KUidIccGlobalAdnPhonebook)
			{
			result = iPhonebookStore.Open(aPhone,
										  KETelIccAdnPhoneBook,
										  KEtelGsmPhoneBookType);
			LOGPARAMS2(_L8("OpenPhoneBookStore(): ADN result=%d"), result);
			}
		else if (iPhonebookUid == KUidIccGlobalSdnPhonebook)
			{
			result = iPhonebookStore.Open(aPhone,
										  KETelIccSdnPhoneBook,
										  KEtelGsmPhoneBookType);
			LOGPARAMS2(_L8("OpenPhoneBookStore(): SDN result=%d"), result);
			}
		else if (iPhonebookUid == KUidIccGlobalLndPhonebook)
			{
			result = iPhonebookStore.Open(aPhone,
										  KETelIccLndPhoneBook,
										  KEtelGsmPhoneBookType);
			LOGPARAMS2(_L8("OpenPhoneBookStore(): LND result=%d"), result);
			}
		else if (iPhonebookUid == KUidUsimAppAdnPhonebook)
			{
			result = iPhonebookStore.Open(aPhone,
										  KETelIccAdnPhoneBook,
										  KEtelUSimPhoneBookType);
			LOGPARAMS2(_L8("OpenPhoneBookStore(): USIM ADN result=%d"), result);
			}
		else if (iPhonebookUid == KUidIccGlobalFdnPhonebook)
			{
			result = iPhonebookStore.Open(aPhone,
										  KETelIccFdnPhoneBook,
										  KEtelGsmPhoneBookType);
			LOGPARAMS2(_L8("OpenPhoneBookStore(): FDN result=%d"), result);
			}

		//
		// Store any failures so that store will not be reopened later...
		//
		if (result != KErrNone)
			{
			iPhBkInfoRetrievedResult = result;
			}
		else if (iPhonebookStore.SubSessionHandle() == 0)
			{
			iPhBkInfoRetrievedResult = KErrNotSupported;
			}
		}

	//
	// Return the phone store. A check is made later to ensure it is valid
	// before use.
	//
	return iPhonebookStore;
	} // CPhoneBook::GetPhoneBookStore


/**
 *  Closes the Phonebook Store used by the engine for this phonebook.
 */
inline void CPhoneBook::ClosePhoneBookStore()
	{
	iPhonebookStore.Close();
	} // CPhoneBook::ClosePhoneBookStore


/**
 *  Returns the ICC entry template ID for this phonebook.
 *
 *  @return Contacts Id representing the template UID.
 */
inline TContactItemId CPhoneBook::GetTemplateId() const
	{
	return iTemplateId;
	} // CPhoneBook::GetTemplateId


/**
 *  Sets the ICC entry template ID for this phonebook.
 *
 *  @param aId  Contacts Id representing the template UID.
 */
inline void CPhoneBook::SetTemplateId(TContactItemId aId)
	{
	iTemplateId = aId;
	} // CPhoneBook::SetTemplateId


/**
 *  Returns the ICC entry group ID.
 *
 *  @return Contacts Id representing the group UID.
 */
inline TContactItemId CPhoneBook::GetGroupId() const
	{
	return iGroupId;
	} // CPhoneBook::GetGroupId


/**
 *  Sets the ICC entry group ID for this phonebook.
 *
 *  @param aId  Contacts Id representing the group UID.
 */
inline void CPhoneBook::SetGroupId(TContactItemId aId)
	{
	iGroupId = aId;
	} // CPhoneBook::SetGroupId


/**
 *  Returns the result for this phonebook which specifies if the PhBkInfo data
 *  has been stored or if the phonebook is not supported.
 *
 *  @return The PhBkInfo retrieved result.
 */
inline TInt CPhoneBook::GetPhBkInfoRetrievedResult() const
	{
	return iPhBkInfoRetrievedResult;
	} // CPhoneBook::GetPhBkInfoRetrievedResult


/**
 *  Sets the PhBkInfo retrieved results to say if the data has been retrieved
 *  and stored, or if the phonebook is not supported.
 *
 *  @param aFlag  PhBkInfo retrieved result.
 */
inline void CPhoneBook::SetPhBkInfoRetrievedResult(TInt aErrorCode)
	{
	iPhBkInfoRetrievedResult = aErrorCode;
	} // CPhoneBook::SetPhBkInfoRetrievedResult


/**
 *  Return the current setting for the phonebook synchronisation mode.
 *
 *  @return A RPhoneBookSession::TPhonebookSyncMode phonebook synchronisation mode.
 */
inline RPhoneBookSession::TPhonebookSyncMode CPhoneBook::GetSyncMode() const
	{
	return iSyncMode;
	} // CPhoneBook::GetSyncMode


/**
 *  Set a new value for the phonebook synchronisation mode.
 *
 *  @param aSyncMode  Phonebook synchronisation mode.
 */
inline void CPhoneBook::SetSyncMode(RPhoneBookSession::TPhonebookSyncMode aSyncMode)
	{
	iSyncMode = aSyncMode;
	} // CPhoneBook::SetSyncMode


/**
 *  Get the PhBkInfo parameter data for this phonebook.
 *
 *  @return A RMobilePhoneBookStore::TMobilePhoneBookInfoV5 structure containing
 *          information about the phonebook.
 */
inline const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& CPhoneBook::GetPhoneBookInfo() const
	{
	return iPhBkInfoV5;
	} // CPhoneBook::GetPhoneBookInfo


/**
 *  Set the Phonebook Info parameter for this phonebook. This function can safely accept
 *  a V1 and V2 structures if they are cast to a V5.
 *
 *  @param aPhBkInfo  The PhBkInfo for this phonebook. 
 */
inline void CPhoneBook::SetPhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo)
	{
	//
	// For the V1 and V2 format info we need to safely initialise V5 attributes that
	// are not handled by static cast copy...
	//	
	if (aPhBkInfo.ExtensionId() == RMobilePhoneStore::KETelMobilePhonebookStoreV1)
		{
		static_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV1&>(iPhBkInfoV5) =
				static_cast<const RMobilePhoneBookStore::TMobilePhoneBookInfoV1&>(aPhBkInfo);

		iPhBkInfoV5.iPhBkMode.Zero();
		iPhBkInfoV5.iMaxSecondNames = -1;
		iPhBkInfoV5.iMaxTextLengthSecondName = -1;
		iPhBkInfoV5.iMaxAdditionalNumbers = -1;
		iPhBkInfoV5.iMaxNumLengthAdditionalNumber = -1;
		iPhBkInfoV5.iMaxGroupNames = -1;
		iPhBkInfoV5.iMaxTextLengthGroupName = -1;
		iPhBkInfoV5.iMaxEmailAddr = -1;
		iPhBkInfoV5.iMaxTextLengthEmailAddr = -1;
		}
	else if (aPhBkInfo.ExtensionId() == RMobilePhoneStore::KETelMobilePhonebookStoreV2)
		{
		static_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV2&>(iPhBkInfoV5) =
				static_cast<const RMobilePhoneBookStore::TMobilePhoneBookInfoV2&>(aPhBkInfo);
		
		iPhBkInfoV5.iMaxSecondNames = -1;
		iPhBkInfoV5.iMaxTextLengthSecondName = -1;
		iPhBkInfoV5.iMaxAdditionalNumbers = -1;
		iPhBkInfoV5.iMaxNumLengthAdditionalNumber = -1;
		iPhBkInfoV5.iMaxGroupNames = -1;
		iPhBkInfoV5.iMaxTextLengthGroupName = -1;
		iPhBkInfoV5.iMaxEmailAddr = -1;
		iPhBkInfoV5.iMaxTextLengthEmailAddr = -1;
				
		iPhBkInfoV5 = aPhBkInfo;
		}
	else
		{
		iPhBkInfoV5 = aPhBkInfo;
		}
	} // CPhoneBook::SetPhoneBookInfo


/**
 *  Adjusted the count of used entries in the PhBkInfo data. This
 *  is the same as performing a get and set combination.
 *
 *  @note The data can only be updated if the phone TSY supports it in the
 *        first place.
 *
 *  @param aChange  The modification value (e.g. -1 or 1).
 */
inline void CPhoneBook::AdjustPhoneBookInfoUsedEntries(TInt aChange)
	{
	if (iPhBkInfoV5.iUsedEntries != -1)
		{
		iPhBkInfoV5.iUsedEntries += aChange;
		}
	} // CPhoneBook::AdjustPhoneBookInfoUsedEntries


/**
 *  Returns the current size of the Look-Up Table.
 */
inline TInt CPhoneBook::GetLookUpTableSize() const
	{
	return iLookUpTable.Count();
	} // CPhoneBook::GetLookUpTableSize


#endif // __PHONEBOOK_H__
