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
// Header file for Phonebook Synchroniser's CPhoneBookManager class which
// manages the parameters for the phone and each phonebook, including the
// Look-Up Tables, Phonebook store handles and flags for the synchroniser
// engine.
// This class can be accessed from either the server side or engine side.
// All applicable functions are mutex protected to prevent corruption from
// being accessed simutantiously from two threads. CPhoneBook is not protected
// hence why CPhoneBookManager does not give out the CPhoneBook object pointers,
// but performs all operations itself, with mutex protection.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHONEBOOKMANAGER_H__
#define __PHONEBOOKMANAGER_H__

#include "phbksync.h"
#include "phbksynclog.h"
#include "Phonebook.h"
#include "PhonebookIniFile.h"


/**
 *  This class holds data relating to the phonebooks. This includes the
 *  current PIN status and security settings, as well as a table of parameters
 *  associated with each phonebook. Each entry in this table consists of a
 *  CPhoneBook class.
 *
 *  The server owns a instance of this class, while the engine has a reference
 *  to it. The data is protected from thread corruption by a mutex.
 */
class CPhoneBookManager : public CBase
	{
public:
	static CPhoneBookManager* NewL(TUint32 aICCCaps);
	~CPhoneBookManager();

	//
	// Basic management of phonebook UIDs. These do not require shared-data
	// mutex protection as the will not change after construction.
	//
	inline TInt ValidatePhonebookUid(TUid aPhonebookUid) const;
	inline TInt GetPhonebookCount() const;
	inline TInt GetPhonebookUid(TInt aPosition, TUid& aPhonebookUid) const;

	//
	// General settings for all phonebooks. Since the data can change, the
	// functions use mutex protection.
	//
	TUid GetPhonebookUidFromContactId(TContactItemId aContactId);
	TUid GetPhonebookUidFromTemplateId(TContactItemId aGroupId);

	void GetPin1LockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus);
	void SetPin1LockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus);

	void GetPin2LockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus);
	void SetPin2LockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus);

	void GetHiddenKeyLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus);
	void SetHiddenKeyLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus);

	void GetUsimAppLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus);
	void SetUsimAppLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus);

	void GetUsimUniversalPinLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus);
	void SetUsimUniversalPinLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus);

	void RecordSecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent);

	TBool IsPin1Valid();
	TBool IsPin2Valid();
	TBool IsUsimAppPinValid();

	//
	// Settings that are per phonebook. These functions are all mutex
	// protected.
	//
	TInt GetPhBkInfoRetrievedResult(TUid aPhonebookUid, TInt& aErrorCode);
	TInt SetPhBkInfoRetrievedResult(TUid aPhonebookUid, TInt aErrorCode);

	TInt GetSyncMode(TUid aPhonebookUid, RPhoneBookSession::TPhonebookSyncMode& aSyncMode);
	TInt SetSyncMode(TUid aPhonebookUid, RPhoneBookSession::TPhonebookSyncMode aSyncMode);

	TInt GetSyncState(TUid aPhonebookUid, RPhoneBookSession::TSyncState& aSyncState);
	TInt SetSyncState(TUid aPhonebookUid, RPhoneBookSession::TSyncState aSyncState);

	TInt GetLastSyncError(TUid aPhonebookUid, TInt& aLastSyncError);
	TInt SetLastSyncError(TUid aPhonebookUid, TInt aLastSyncError);

	TInt GetTemplateId(TUid aPhonebookUid,
					   TContactItemId& aTemplateId);
	TInt SetTemplateId(TUid aPhonebookUid,
					   TContactItemId aTemplateId);

	TInt GetGroupId(TUid aPhonebookUid,
					TContactItemId& aGroupId);
	TInt SetGroupId(TUid aPhonebookUid,
					TContactItemId aGroupId);

	TInt GetContactFields(TUid aPhonebookUid,
						  RPhoneBookSession::TContactFieldsV3& aContactFields);
	TInt SetContactFields(TUid aPhonebookUid,
						  const RPhoneBookSession::TContactFieldsV3& aContactFields);

	TInt GetPhoneBookStore(TUid aPhonebookUid,
						   RMobilePhone& aPhone,
						   RMobilePhoneBookStore& aPhonebookStore);
	TInt ClosePhoneBookStores();

	TInt GetPhoneBookInfo(TUid aPhonebookUid,
						  RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo);
	TInt SetPhoneBookInfo(TUid aPhonebookUid,
						  const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo);
	TInt AdjustPhoneBookInfoUsedEntries(TUid aPhonebookUid, TInt aChange);

	TInt GetLookUpTableSize(TUid aPhonebookUid, TInt& aSize);
	TInt SetLookUpTableSize(TUid aPhonebookUid, TInt aSize);
	TInt ClearLookUpTable(TUid aPhonebookUid);

	TInt IsEntryInTable(TUid aPhonebookUid, TContactItemId aContactId);
	TInt UpdateEntryInTable(TUid aPhonebookUid, TInt aSlotNum,
							TContactItemId aContactId);
	TInt UpdateEntryInTable(TUid aPhonebookUid, TInt aSlotNum,
							TContactItemId aContactId,
							TPhonebookSlotState aSlotState);

	TInt GetContactIdFromSlotNum(TUid aPhonebookUid, TInt aSlotNum,
								 TContactItemId& aContactItemId);
	TInt GetSlotNumFromContactId(TUid aPhonebookUid,
								 TContactItemId aContactId,
								 TInt& aSlotNum);

	TInt GetNumFreeSlots(TUid aPhonebookUid, TInt& aNumOfFreeSlots);
	TInt GetFirstEmptySlot(TUid aPhonebookUid, TInt& aSlotNum);
	TInt GetMatchingEntries(TUid aPhonebookUid, TPhonebookSlotState aSlotState,
							RArray<TInt>& aEntries);

	TInt LogLookUpTable(TUid aPhonebookUid);

private:
	CPhoneBookManager();

	void ConstructL(TUint32 aICCCaps);
	TInt GetPhonebook(TUid aPhonebookUid, CPhoneBook*& aPhoneBookPtr) const;
	inline void AcquireMutex();
	inline void ReleaseMutex();

	RMutex  iMutex;
	RPointerArray<CPhoneBook>  iPhonebookList;
	CPhoneBookIniFile*  iIniFile;
	TBool  iIsPuk1Verified;
	RMobilePhone::TMobilePhoneLockStatus  iPin1LockStatus;
	RMobilePhone::TMobilePhoneLockStatus  iPin2LockStatus;
	RMobilePhone::TMobilePhoneLockStatus  iHiddenKeyLockStatus;
	RMobilePhone::TMobilePhoneLockStatus  iUsimAppLockStatus;
	RMobilePhone::TMobilePhoneLockStatus  iUsimUniversalPinLockStatus;
	};


/**
 *  Validates a phonebook UID as being supported by the Phonebook
 *  Synchroniser with the current phone. This does not mean that
 *  the phonebook is supported by the phone, but that PhBkSync
 *  can use it.
 *
 *  @param aPhonebookUid  UID of the phonebook to validate.
 *
 *  @return Returns KErrNone if valid, a system error code otherwise.
 */
inline TInt CPhoneBookManager::ValidatePhonebookUid(TUid aPhonebookUid) const
	{
	//	
	// Validate the UID by getting the parameters for the phonebook. If
	// the UID is not valid, then it won't be found.
	//
	CPhoneBook*  phonebookParams;
	TInt  result = GetPhonebook(aPhonebookUid, phonebookParams);

	return result;
	} // CPhoneBookManager::ValidatePhonebookUid


/**
 *  Returns the number of phonebooks supported by the Phonebook Synchroniser
 *  using the current phone.
 *
 *  @return Number of phonebooks supported.
 *
 *  @see CPhoneBookManager::GetPhonebookUid
 */
inline TInt CPhoneBookManager::GetPhonebookCount() const
	{
	return iPhonebookList.Count();
	} // CPhoneBookManager::GetPhonebookCount


/**
 *  Get the phonebook UID for a particular index in the list of phonebooks.
 *  This can be used to iterate the list of phonebooks.
 *
 *  @param aPosition      Index of the phonebook in the list.
 *  @param aPhonebookUid  Set with the phonebook UID on successful exit.
 *
 *  @return Returns KErrNone if successful, a system error code otherwise.
 *
 *  @see CPhoneBookManager::GetPhonebookCount()
 */
inline TInt CPhoneBookManager::GetPhonebookUid(TInt aPosition,
											   TUid& aPhonebookUid) const
	{
	//
	// Check the position is valid and if so the return the UID...
	//
	if (aPosition < 0  ||  aPosition >= iPhonebookList.Count())
		{
		return KErrArgument;
		}
	
	aPhonebookUid = iPhonebookList[aPosition]->GetPhonebookUid();

	return KErrNone;
	} // CPhoneBookManager::GetPhonebookUid


/**
 *  Acquires the process local mutex used to protect the shared data.
 */
inline void CPhoneBookManager::AcquireMutex()
	{
	LOGPARAMS2(_L8("AcquireMutex(): Thread ID=%d"), (TUint) RThread().Id());
	
	iMutex.Wait();
	} // CPhoneBookManager::AcquireMutex


/**
 *  Releases the process local mutex used to protect the shared data.
 */
inline void CPhoneBookManager::ReleaseMutex()
	{
	LOGPARAMS2(_L8("ReleaseMutex(): Thread ID=%d"), (TUint) RThread().Id());
	
	iMutex.Signal();
	} // CPhoneBookManager::ReleaseMutex


#endif // __PHONEBOOKMANAGER_H__
