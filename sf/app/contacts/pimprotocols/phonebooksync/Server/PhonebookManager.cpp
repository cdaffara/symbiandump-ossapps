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
// Contains method implementations for Phonebook Synchroniser's
// CPhoneBookManager class which manages the parameters for the phone and
// each phonebook, including the Look-Up Tables, Phonebook store handles
// and flags for the synchroniser engine.
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

#include "phbksync.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"
#include "phbksynclog.h"


/**
 *  Static factory method used to create a CPhoneBookManager object.
 *
 *  @param aICCCaps  The capabilities of the ICC.
 *
 *  @return  Pointer to the created CPhoneBookManager object, or NULL.
 */
CPhoneBookManager* CPhoneBookManager::NewL(TUint32 aICCCaps)
	{
	CPhoneBookManager*  ptr = new (ELeave) CPhoneBookManager();
	CleanupStack::PushL(ptr);
	ptr->ConstructL(aICCCaps);
	CleanupStack::Pop(ptr);
	return ptr;
	} // CPhoneBookManager::NewL


/**
 *  Standard constructor.
 */
CPhoneBookManager::CPhoneBookManager()
  : iMutex(),
  	iPhonebookList(),
  	iIniFile(NULL),
	iIsPuk1Verified(ETrue),
	iPin1LockStatus(RMobilePhone::EStatusLockUnknown),
	iPin2LockStatus(RMobilePhone::EStatusLockUnknown),
	iHiddenKeyLockStatus(RMobilePhone::EStatusLockUnknown),
	iUsimAppLockStatus(RMobilePhone::EStatusLockUnknown),
	iUsimUniversalPinLockStatus(RMobilePhone::EStatusLockUnknown)
	{
	// NOP
	} // CPhoneBookManager::CPhoneBookManager


/**
 *  Standard denstructor.
 */
CPhoneBookManager::~CPhoneBookManager()
	{
	delete iIniFile;
	iPhonebookList.ResetAndDestroy();
	iMutex.Close();
	} // CPhoneBookManager::~CPhoneBookManager


/**
 *  Construct the list containing a table of parameters associated with each
 *  supported phonebook.  The list is constructed according to the type of the
 *  ICC specified by the aICCCaps parameter.
 *
 *  @note  The tables for supported phonebooks are created and added in the
 *         priority order. Therefore the global ADN will be synchronised first
 *         if its auto-sync setting is set.
 *
 *  @param aICCCaps  ICC caps indicating the type of ICC. 
 */
void CPhoneBookManager::ConstructL(TUint32 aICCCaps)
	{
	LOGPARAMS2(_L8("ConstructL(0x%08x)"), aICCCaps);
	
	//
	// Create the mutex which will protect the shared data...
	//
	iMutex.CreateLocal();
	
	//
	// By default following phonebook types are supported for all ICC types (SIM, R-UIM and USIM)
	//
	CPhoneBook*  ADNphonebook = CPhoneBook::NewL(KUidIccGlobalAdnPhonebook);
	iPhonebookList.Append(ADNphonebook);

	CPhoneBook*  SDNphonebook = CPhoneBook::NewL(KUidIccGlobalSdnPhonebook);
	iPhonebookList.Append(SDNphonebook);

	CPhoneBook*  LNDphonebook = CPhoneBook::NewL(KUidIccGlobalLndPhonebook);
	iPhonebookList.Append(LNDphonebook);

	// 3GPP USIM specific phonebook 
	if (aICCCaps & RMobilePhone::KCapsUSimAccessSupported)
		{
		CPhoneBook*  USIMphonebook = CPhoneBook::NewL(KUidUsimAppAdnPhonebook);
		iPhonebookList.Append(USIMphonebook);
		}

	CPhoneBook*  FDNphonebook = CPhoneBook::NewL(KUidIccGlobalFdnPhonebook);
	iPhonebookList.Append(FDNphonebook);

	//
	// Create the INI File class...
	//
	iIniFile = CPhoneBookIniFile::NewL(iPhonebookList);
	} // CPhoneBookManager::ConstructL


/**
 *  Gets the phonebook UID for the phonebook that contains the supplied
 *  Contact UID. Phonebooks are searched in order.
 *
 *  @param aContactId  Contact ID to search for.
 *
 *  @return The phonebook UID of the containing phonebook, or
 *          KUidIccPhonebookNotSpecified.
 */
TUid CPhoneBookManager::GetPhonebookUidFromContactId(TContactItemId aContactId)
	{
	LOGPARAMS2(_L8("GetPhonebookUidFromContactId(0x%08x)"), aContactId);

	TInt  count(iPhonebookList.Count());
	TUid  result(KUidIccPhonebookNotSpecified);
	
	AcquireMutex();

	for (TInt index = 0;  index < count;  index++)
		{
		TInt  slotNum = iPhonebookList[index]->GetSlotNumFromContactId(aContactId);
		
		if (slotNum > 0)
			{
			result = iPhonebookList[index]->GetPhonebookUid();
			break;
			}
		}

	ReleaseMutex();
		
	return result;
	} // CPhoneBookManager::GetPhonebookUidFromContactId


/**
 *  Return the phonebook type for the given template ID.
 *
 *  @param aTemplateId  The template ID associated with the phonebook.
 *
 *  @return TUid  UID for the phonebook or KUidIccPhonebookNotSpecified.
 */
TUid CPhoneBookManager::GetPhonebookUidFromTemplateId(TContactItemId aTemplateId)
	{
	LOGPARAMS2(_L8("GetPhonebookUidFromTemplateId(0x%08x)"), aTemplateId);

	TInt  count(iPhonebookList.Count());
	TUid  result(KUidIccPhonebookNotSpecified);
	
	AcquireMutex();

	for (TInt index = 0;  index < count;  index++)
		{
		if (iPhonebookList[index]->GetTemplateId() == aTemplateId)
			{
			result = iPhonebookList[index]->GetPhonebookUid();
			break;
			}
		}

	ReleaseMutex();
		
	return result;
	} // CPhoneBookManager::GetPhonebookUidFromTemplateId


/**
 *  Returns the stored PIN1 lock status.
 *
 *  @param aLockStatus  Filled on return with the stored value.
 */
void CPhoneBookManager::GetPin1LockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus)
	{
	LOGPARAMS1(_L8("GetPin1LockStatus()"));

	AcquireMutex();
	aLockStatus = iPin1LockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::GetPin1LockStatus


/**
 *  Sets the stored PIN1 lock status.
 *
 *  @param aLockStatus  Value of the lock to store.
 */
void CPhoneBookManager::SetPin1LockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus)
	{
	LOGPARAMS1(_L8("SetPin1LockStatus()"));

	AcquireMutex();
	iPin1LockStatus = aLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::SetPin1LockStatus


/**
 *  Returns the stored PIN2 lock status.
 *
 *  @param aLockStatus  Filled on return with the stored value.
 */
void CPhoneBookManager::GetPin2LockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus)
	{
	LOGPARAMS1(_L8("GetPin2LockStatus()"));

	AcquireMutex();
	aLockStatus = iPin2LockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::GetPin2LockStatus


/**
 *  Sets the stored PIN2 lock status.
 *
 *  @param aLockStatus  Value of the lock to store.
 */
void CPhoneBookManager::SetPin2LockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus)
	{
	LOGPARAMS1(_L8("SetPin2LockStatus()"));

	AcquireMutex();
	iPin2LockStatus = aLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::SetPin2LockStatus


/**
 *  Returns the stored Hidden Key lock status.
 *
 *  @param aLockStatus  Filled on return with the stored value.
 */
void CPhoneBookManager::GetHiddenKeyLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus)
	{
	LOGPARAMS1(_L8("GetHiddenKeyLockStatus()"));

	AcquireMutex();
	aLockStatus = iHiddenKeyLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::GetHiddenKeyLockStatus


/**
 *  Sets the stored Hidden Key lock status.
 *
 *  @param aLockStatus  Value of the lock to store.
 */
void CPhoneBookManager::SetHiddenKeyLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus)
	{
	LOGPARAMS1(_L8("SetHiddenKeyLockStatus()"));

	AcquireMutex();
	iHiddenKeyLockStatus = aLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::SetHiddenKeyLockStatus


/**
 *  Returns the stored USIM App lock status.
 *
 *  @param aLockStatus  Filled on return with the stored value.
 */
void CPhoneBookManager::GetUsimAppLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus)
	{
	LOGPARAMS1(_L8("GetUsimAppLockStatus()"));

	AcquireMutex();
	aLockStatus = iUsimAppLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::GetUsimAppLockStatus


/**
 *  Sets the stored USIM App lock status.
 *
 *  @param aLockStatus  Value of the lock to store.
 */
void CPhoneBookManager::SetUsimAppLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus)
	{
	LOGPARAMS1(_L8("SetUsimAppLockStatus()"));

	AcquireMutex();
	iUsimAppLockStatus = aLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::SetPin2UsimAppLockStatus


/**
 *  Returns the stored Universal PIN lock status.
 *
 *  @param aLockStatus  Filled on return with the stored value.
 */
void CPhoneBookManager::GetUsimUniversalPinLockStatus(RMobilePhone::TMobilePhoneLockStatus& aLockStatus)
	{
	LOGPARAMS1(_L8("GetUsimUniversalPinLockStatus()"));

	AcquireMutex();
	aLockStatus = iUsimUniversalPinLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::GetUsimUniversalPinLockStatus


/**
 *  Sets the stored USIM Universal PIN lock status.
 *
 *  @param aLockStatus  Value of the lock to store.
 */
void CPhoneBookManager::SetUsimUniversalPinLockStatus(RMobilePhone::TMobilePhoneLockStatus aLockStatus)
	{
	LOGPARAMS1(_L8("SetUsimUniversalPinLockStatus()"));

	AcquireMutex();
	iUsimUniversalPinLockStatus = aLockStatus;
	ReleaseMutex();
	} // CPhoneBookManager::SetUsimUniversalPinLockStatus


/**
 *  Records a security event. Since events are stateless, the event is not actually
 *  stored for retrieval but an internal state is updated.
 *
 *  @param aSecurityEvent  Security event to store.
 */
void CPhoneBookManager::RecordSecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent)
	{
	LOGPARAMS2(_L8("RecordSecurityEvent(%d)"), aSecurityEvent);

	if (aSecurityEvent == RMobilePhone::ENoICCFound  ||
		aSecurityEvent == RMobilePhone::EICCTerminated  ||
		aSecurityEvent == RMobilePhone::EPuk1Required)
		{
		iIsPuk1Verified = EFalse;
		}
	else if (aSecurityEvent == RMobilePhone::EPuk1Verified)
		{
		iIsPuk1Verified = ETrue;
		}
	} // CPhoneBookManager::RecordSecurityEvent


/**
 *  Returns the current state of access to PIN 1 priviledged facilities.
 *
 *  @return ETrue if the ICC is in a usable state with PIN 1 access, otherwise
 *          return EFalse.
 */
TBool CPhoneBookManager::IsPin1Valid()
	{
	LOGPARAMS1(_L8("IsPin1Valid()"));
	
	AcquireMutex();

	//
	// Return true if all of the following conditions are true.  Note that we
	// only look for specific statuses, and ignore unknowns.  Thus if the TSY
	// doesn't implement some locks, then the default expected behaviour will
	// occur.
	//
	TBool  result = (iIsPuk1Verified  &&
					 iPin1LockStatus != RMobilePhone::EStatusLocked  &&
					 iPin1LockStatus != RMobilePhone::EStatusBlocked  &&
					 iHiddenKeyLockStatus != RMobilePhone::EStatusLocked  &&
					 iHiddenKeyLockStatus != RMobilePhone::EStatusBlocked);

	ReleaseMutex();
		
	return result;
	} // CPhoneBookManager::IsPin1Valid


/**
 *  Returns the current state of access to PIN 2 priviledged facilities.
 *
 *  @return ETrue if the ICC is in a usable state with PIN 2 access, otherwise
 *          return EFalse.
 */
TBool CPhoneBookManager::IsPin2Valid()
	{
	LOGPARAMS1(_L8("IsPin2Valid()"));
	
	AcquireMutex();

	//
	// Return true if PIN2 is unlocked.  Note that we only look for specific
	// statuses, and ignore unknowns.  Thus if the TSY doesn't implement some
	// locks, then the default expected behaviour will occur.
	//
	TBool  result = (iPin2LockStatus == RMobilePhone::EStatusUnlocked);

	ReleaseMutex();
		
	return result;
	} // CPhoneBookManager::IsPin2Valid


/**
 *  Returns the current state of access to USIM App priviledged facilities.
 *
 *  @return ETrue if the USIM App is in a usable state with PIN access, otherwise
 *          return EFalse.
 */
TBool CPhoneBookManager::IsUsimAppPinValid()
	{
	LOGPARAMS1(_L8("IsUsimAppPinValid()"));
	
	AcquireMutex();

	//
	// Return true if all of the following conditions are true.  Note that we
	// only look for specific statuses, and ignore unknowns.  Thus if the TSY
	// doesn't implement some locks, then the default expected behaviour will
	// occur.
	//
	TBool  result = (iUsimAppLockStatus != RMobilePhone::EStatusLocked  &&
					 iUsimAppLockStatus != RMobilePhone::EStatusBlocked  &&
					 iUsimUniversalPinLockStatus != RMobilePhone::EStatusLocked  &&
					 iUsimUniversalPinLockStatus != RMobilePhone::EStatusBlocked);

	ReleaseMutex();
		
	return result;
	} // CPhoneBookManager::IsUsimAppPinValid


/**
 *  Returns the result for the given phonebook which specifies whether the
 *  PhBkInfo data has been stored or if the phonebook is not supported.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aErrorCode     Filled with the value on successful exit.
 *
 *  @return KErrNone if the result is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetPhBkInfoRetrievedResult(TUid aPhonebookUid,
											       TInt& aErrorCode)
	{
	LOGPARAMS2(_L8("GetPhBkInfoRetrievedResult(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Get the PhBkInfo retrieved flag from the phonebook parameters.
	//
	AcquireMutex();
	aErrorCode = phonebook->GetPhBkInfoRetrievedResult();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetPhBkInfoRetrievedResult


/**
 *  Sets the result for the given phonebook which specifies whether the
 *  PhBkInfo data has been stored or if it is not supported.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value on.
 *  @param aErrorCode     Value to set.
 *
 *  @return KErrNone if the flag is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetPhBkInfoRetrievedResult(TUid aPhonebookUid,
											       TInt aErrorCode)
	{
	LOGPARAMS3(_L8("SetPhBkInfoRetrievedResult(0x%08x, %d)"),
			   aPhonebookUid, aErrorCode);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the PhBkInfo retrieved flag in the phonebook parameters.
	//
	AcquireMutex();
	phonebook->SetPhBkInfoRetrievedResult(aErrorCode);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetPhBkInfoRetrievedResult


/**
 *  Returns the sync mode for the given phonebook which specifies how the
 *  synchroniser should handle a phonebook at startup.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aSyncMode      Filled with the value on successful exit.
 *
 *  @return KErrNone if the mode is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetSyncMode(TUid aPhonebookUid,
									RPhoneBookSession::TPhonebookSyncMode& aSyncMode)
	{
	LOGPARAMS2(_L8("GetSyncMode(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Get the Sync Mode from the phonebook parameters.
	//
	AcquireMutex();
	aSyncMode = phonebook->GetSyncMode();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetSyncMode


/**
 *  Sets the sync mode for the given phonebook which specifies how the
 *  synchroniser should handle a phonebook at startup.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value on.
 *  @param aSyncMode      The value to set.
 *
 *  @return KErrNone if the mode is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetSyncMode(TUid aPhonebookUid,
									RPhoneBookSession::TPhonebookSyncMode aSyncMode)
	{
	LOGPARAMS3(_L8("SetSyncMode(0x%08x, %d)"),
			   aPhonebookUid, (TInt) aSyncMode);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the Sync Mode for the phonebook...
	//
	AcquireMutex();
	RPhoneBookSession::TPhonebookSyncMode  oldSyncMode(phonebook->GetSyncMode());
	
	phonebook->SetSyncMode(aSyncMode);
	result = iIniFile->WriteIniFileSettingsIfNeeded();
	if (result != KErrNone)
		{
		phonebook->SetSyncMode(oldSyncMode);
		}
	ReleaseMutex();
	
	return result;
	} // CPhoneBookManager::SetSyncMode


/**
 *  Returns the sync state for the given phonebook which specifies if the
 *  phonebook has been syncronhised.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aSyncState     Filled with the value on successful exit.
 *
 *  @return KErrNone if the state is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetSyncState(TUid aPhonebookUid,
									 RPhoneBookSession::TSyncState& aSyncState)
	{
	LOGPARAMS2(_L8("GetSyncState(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Get the Sync State from the phonebook parameters.
	//
	AcquireMutex();
	aSyncState = phonebook->GetSyncState();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetSyncState


/**
 *  Sets the sync state for the given phonebook which specifies if the
 *  phonebook has been syncronhised.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value on.
 *  @param aSyncState     The value to set.
 *
 *  @return KErrNone if the state is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetSyncState(TUid aPhonebookUid,
									RPhoneBookSession::TSyncState aSyncState)
	{
	LOGPARAMS3(_L8("SetSyncState(0x%08x, %d)"),
			   aPhonebookUid, (TInt) aSyncState);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the Sync State for the phonebook...
	//
	AcquireMutex();
	RPhoneBookSession::TSyncState  oldSyncState(phonebook->GetSyncState());
	
	phonebook->SetSyncState(aSyncState);
	result = iIniFile->WriteIniFileSettingsIfNeeded();
	if (result != KErrNone)
		{
		phonebook->SetSyncState(oldSyncState);
		}
	ReleaseMutex();
	
	return result;
	} // CPhoneBookManager::SetSyncState


/**
 *  Returns the last sync error for the given phonebook which specifies the last
 *  error seen whist synchronising the phonebook.
 *
 *  @param aPhonebookUid   Phonebook UID to get the value from.
 *  @param aLastSyncError  Filled with the value on successful exit.
 *
 *  @return KErrNone if the last error is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetLastSyncError(TUid aPhonebookUid,
										 TInt& aLastSyncError)
	{
	LOGPARAMS2(_L8("GetLastSyncError(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Acquire the mutex to protect shared data. This must be released
	// before the function returns.
	//
	AcquireMutex();

	//
	// Get the last sync error from the phonebook parameters.
	//
	aLastSyncError = phonebook->GetLastSyncError();

	//
	// Release the mutex now that we have finished.
	//
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetLastSyncError


/**
 *  Sets the last sync error for the given phonebook which specifies the last
 *  error seen whist synchronising the phonebook.
 *
 *  @param aPhonebookUid   Phonebook UID to set the value on.
 *  @param aLastSyncError  The value to set.
 *
 *  @return KErrNone if the last error is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetLastSyncError(TUid aPhonebookUid,
										 TInt aLastSyncError)
	{
	LOGPARAMS3(_L8("SetLastSyncError(0x%08x, %d)"),
			   aPhonebookUid, aLastSyncError);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the last sync error for the phonebook...
	//
	AcquireMutex();
	phonebook->SetLastSyncError(aLastSyncError);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetLastSyncError


/**
 *  Returns the template ID for the given phonebook which specifies the
 *  Contacts record template used as a base for records on this phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aTemplateId    Filled with the value on successful exit.
 *
 *  @return KErrNone if the template ID is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetTemplateId(TUid aPhonebookUid,
									  TContactItemId& aTemplateId)
	{
	LOGPARAMS2(_L8("GetTemplateId(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And return the template ID...
	//
	AcquireMutex();
	aTemplateId = phonebook->GetTemplateId();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetTemplateId


/**
 *  Sets the template ID for the given phonebook which specifies the
 *  Contacts record template used as a base for records on this phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value on.
 *  @param aTemplateId    The value to set.
 *
 *  @return KErrNone if the template ID is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetTemplateId(TUid aPhonebookUid,
									  TContactItemId aTemplateId)
	{
	LOGPARAMS3(_L8("SetTemplateId(0x%08x, 0x%08x)"),
			   aPhonebookUid, aTemplateId);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And set the template ID...
	//
	AcquireMutex();
	TContactItemId  oldTemplateId = phonebook->GetTemplateId();

	phonebook->SetTemplateId(aTemplateId);
	result = iIniFile->WriteIniFileSettingsIfNeeded();
	if (result != KErrNone)
		{
		phonebook->SetTemplateId(oldTemplateId);
		}
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetTemplateId


/**
 *  Returns the group ID for the given phonebook which specifies the
 *  Contacts record group used as an association for records on this
 *  phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aGroupId       Filled with the value on successful exit.
 *
 *  @return KErrNone if the group ID is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetGroupId(TUid aPhonebookUid,
								   TContactItemId& aGroupId)
	{
	LOGPARAMS2(_L8("GetGroupId(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And return the group ID...
	//
	AcquireMutex();
	aGroupId = phonebook->GetGroupId();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetGroupId


/**
 *  Sets the group ID for the given phonebook which specifies the
 *  Contacts record group used as an association for records on this
 *  phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value on.
 *  @param aGroupId       The value to set.
 *
 *  @return KErrNone if the group ID is set, otherwise a system error code.
 */
TInt CPhoneBookManager::SetGroupId(TUid aPhonebookUid,
								   TContactItemId aGroupId)
	{
	LOGPARAMS3(_L8("SetGroupId(0x%08x, 0x%08x)"), aPhonebookUid, aGroupId);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And set the group ID...
	//
	AcquireMutex();
	TContactItemId  oldGroupId = phonebook->GetGroupId();

	phonebook->SetGroupId(aGroupId);
	result = iIniFile->WriteIniFileSettingsIfNeeded();
	if (result != KErrNone)
		{
		phonebook->SetGroupId(oldGroupId);
		}
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetGroupId


/**
 *  Returns the contact fields format for the given phonebook which
 *  specifies the format of contacts on this phonebook.
 *
 *  @param aPhonebookUid   Phonebook UID to get the value from.
 *  @param aContactFields  Filled with the value on successful exit.
 *
 *  @return KErrNone if the contact fields format is retrieved, otherwise a
 *          system error code.
 */
TInt CPhoneBookManager::GetContactFields(TUid aPhonebookUid,
										 RPhoneBookSession::TContactFieldsV3& aContactFields)
	{
	LOGPARAMS2(_L8("GetContactFields(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And return the Contact Fields...
	//
	AcquireMutex();
	aContactFields = phonebook->GetContactFields();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetContactFields


/**
 *  Sets the contact fields format for the given phonebook which
 *  specifies the format of contacts on this phonebook.
 *
 *  @param aPhonebookUid   Phonebook UID to set the value on.
 *  @param aContactFields  The value to set.
 *
 *  @return KErrNone if the contact fields format is retrieved, otherwise a
 *          system error code.
 */
TInt CPhoneBookManager::SetContactFields(TUid aPhonebookUid,
										 const RPhoneBookSession::TContactFieldsV3& aContactFields)
	{
	LOGPARAMS2(_L8("SetContactFields(0x%08x)"), aPhonebookUid);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the Contact Fields...
	//
	AcquireMutex();
	phonebook->SetContactFields(aContactFields);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetContactFields


/**
 *  Returns the phonebook store handle for the given phonebook which
 *  provides access to the ICC phonebook.
 *
 *  @param aPhonebookUid    Phonebook UID to get the handle from.
 *  @param aPhone           Handle to the ETel phone.
 *  @param aPhonebookStore  Filled with the handle on successful exit.
 *
 *  @return KErrNone if the handle is retrieved, otherwise a system error code.
 */
TInt CPhoneBookManager::GetPhoneBookStore(TUid aPhonebookUid,
										  RMobilePhone& aPhone,
										  RMobilePhoneBookStore& aPhonebookStore)
	{
	LOGPARAMS2(_L8("GetPhoneBookStore(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And return the phone store handle...
	//
	AcquireMutex();
	aPhonebookStore = phonebook->GetPhoneBookStore(aPhone);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetPhoneBookStore


/**
 *  Closes the phonebook ICC handle for each phonebook.
 */
TInt CPhoneBookManager::ClosePhoneBookStores()
	{
	LOGPARAMS1(_L8("ClosePhoneBookStores()"));

	//
	// Acquire the mutex before we start...
	//
	AcquireMutex();

	//
	// Now loop through the phonebooks and close each one... 
	//
	TInt  count(iPhonebookList.Count());
	
	for (TInt index = 0;  index < count;  index++)
		{
		iPhonebookList[index]->ClosePhoneBookStore();
		}

	//
	// Release the mutex...
	//
	ReleaseMutex();
		
	return KErrNone;
	} // CPhoneBookManager::ClosePhoneBookStores


/**
 *  Returns the PhBkInfo data for the given phonebook which
 *  specifies information about the phonebook and its contents.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aPhBkInfo      Filled with the value on successful exit.
 *
 *  @return KErrNone if the PhBkInfo data is retrieved, otherwise a
 *          system error code.
 */
TInt CPhoneBookManager::GetPhoneBookInfo(TUid aPhonebookUid,
										 RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo)
	{
	LOGPARAMS2(_L8("GetPhoneBookInfo(0x%08x)"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// And return the Phone Book Info...
	//
	AcquireMutex();
	aPhBkInfo = phonebook->GetPhoneBookInfo();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetPhoneBookInfo


/**
 *  Sets the PhBkInfo data for the given phonebook which
 *  specifies information about the phonebook and its contents.
 *
 *  @param aPhonebookUid  Phonebook UID to set the value to.
 *  @param aPhBkInfo      The value to set.
 *
 *  @return KErrNone if the PhBkInfo data is set, otherwise a system error
 *          code.
 */
TInt CPhoneBookManager::SetPhoneBookInfo(TUid aPhonebookUid,
										 const RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aPhBkInfo)
	{
	LOGPARAMS2(_L8("SetPhoneBookInfo(0x%08x)"), aPhonebookUid);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Set the Phone Book Info Fields...
	//
	AcquireMutex();
	phonebook->SetPhoneBookInfo(aPhBkInfo);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::SetPhoneBookInfo


/**
 *  Adjusted the count of used entries in the PhBkInfo data for a phonebook.
 *  This is the same as performing a get and set combination, but quicker and
 *  more efficent.
 *
 *  @note The data can only be updated if the phone TSY supports it in the
 *        first place.
 *
 *  @param aPhonebookUid  Phonebook UID to adjust the value on.
 *  @param aChange        The modification value (e.g. -1 or 1).
 *
 *  @return KErrNone if the PhBkInfo data is updated, otherwise a system error
 *          code.
 */
TInt CPhoneBookManager::AdjustPhoneBookInfoUsedEntries(TUid aPhonebookUid,
													   TInt aChange)
	{
	LOGPARAMS3(_L8("AdjustPhoneBookInfoUsedEntries(0x%08x, %d)"),
			   aPhonebookUid, aChange);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Adjust the Phone Book Info Used Entries Field...
	//
	AcquireMutex();
	phonebook->AdjustPhoneBookInfoUsedEntries(aChange);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::AdjustPhoneBookInfoUsedEntries


/**
 *  Returns the current size of the Look-Up Table for a particular phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aSize          Filled on successful exit with the table size.
 *
 *  @return KErrNone if the size is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetLookUpTableSize(TUid aPhonebookUid, TInt& aSize)
	{
	LOGPARAMS2(_L8("GetLookUpTableSize(): 0x%08x"), aPhonebookUid);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Get the Look-Up Table size...
	//
	AcquireMutex();
	aSize = phonebook->GetLookUpTableSize();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetLookUpTableSize


/**
 *  Sets the current size of the Look-Up Table for a particular phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to perform the operation on.
 *  @param aSize          The new table size.
 *
 *  @return KErrNone if the size is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::SetLookUpTableSize(TUid aPhonebookUid, TInt aSize)
	{
	LOGPARAMS3(_L8("SetLookUpTableSize(): 0x%08x %d"), aPhonebookUid, aSize);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Change the Look-Up Table size...
	//
	AcquireMutex();
	TRAPD(leaveCode, phonebook->SetLookUpTableSizeL(aSize));
	ReleaseMutex();
	
	return leaveCode;
	} // CPhoneBookManager::SetLookUpTableSize


/**
 *  Clears the Look-Up Table for a particular phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to perform the operation on.
 *
 *  @return KErrNone if the operation is successful, otherwise a system
 *          error code.
 */
TInt CPhoneBookManager::ClearLookUpTable(TUid aPhonebookUid)
	{
	LOGPARAMS2(_L8("ClearLookUpTable(): 0x%08x"), aPhonebookUid);
	
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Clear the Look-Up Table...
	//
	AcquireMutex();
	phonebook->ClearLookUpTable();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::ClearLookUpTable


/**
 *  Check whether an entry with the given Contacts UID exists in the
 *  look-up-table and whether that entry has already been flagged as
 *  being used.
 *
 *  @param aPhonebookUid  Phonebook UID of the Look-Up Table to examine.
 *  @param aContactId     Contacts ICC entry UID
 *
 *  @return KErrNone if entry found, otherwise KErrNotFound.
 */
TInt CPhoneBookManager::IsEntryInTable(TUid aPhonebookUid,
									   TContactItemId aContactId)
	{
	LOGPARAMS3(_L8("IsEntryInTable(): 0x%08x 0x%08x"),
			   aPhonebookUid, aContactId);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to look this up in the Look-Up-Table...
	//
	AcquireMutex();
	result = phonebook->IsEntryInTable(aContactId);
	ReleaseMutex();
	
	return result;
	} // CPhoneBookManager::IsEntryInTable


/**
 *  Update the Contact UID parameter for a phonebook entry in a phonebook's
 *  Look-Up Table. The entry is identified by the slot number.
 *
 *  @param aPhonebookUid  Phonebook UID of the Look-Up Table to update.
 *  @param aSlotNum       Phonebook entry slot number
 *  @param aContactId    The new contacts ICC entry UID.
 *
 *  @return KErrNone if the update is successful, otherwise a system error code.
 */
TInt CPhoneBookManager::UpdateEntryInTable(TUid aPhonebookUid, TInt aSlotNum,
										   TContactItemId aContactId)
	{
	LOGPARAMS4(_L8("UpdateEntryInTable(): 0x%08x %d 0x%08x"),
			   aPhonebookUid, aSlotNum, aContactId);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to edit this entry in the Look-Up-Table...
	//
	AcquireMutex();
	result = phonebook->UpdateEntryInTable(aSlotNum, aContactId);
	ReleaseMutex();
	
	return result;
	} // CPhoneBookManager::UpdateEntryInTable


/**
 *  Update the Contact UID and slot state parameters for a phonebook entry
 *  in a phonebook's Look-Up Table. The entry is identified by the slot number.
 *
 *  @param aPhonebookUid  Phonebook UID of the Look-Up Table to update.
 *  @param aSlotNum       Phonebook entry slot number
 *  @param aContactId     The new contacts ICC entry UID.
 *  @param aSlotState     The new slot state.
 *
 *  @return KErrNone if the update is successful, otherwise a system error code.
 */
TInt CPhoneBookManager::UpdateEntryInTable(TUid aPhonebookUid, TInt aSlotNum,
										   TContactItemId aContactId,
										   TPhonebookSlotState aSlotState)
	{
	LOGPARAMS5(_L8("UpdateEntryInTable(): 0x%08x %d 0x%08x %d"),
			   aPhonebookUid, aSlotNum, aContactId, aSlotState);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to edit this entry in the Look-Up-Table...
	//
	AcquireMutex();
	result = phonebook->UpdateEntryInTable(aSlotNum, aContactId, aSlotState);
	ReleaseMutex();
	
	return result;
	} // CPhoneBookManager::UpdateEntryInTable


/**
 *  Returns the Contact ID for the entry in the Look-Up Table for a particular
 *  phonebook. The entry is identified by slot number.
 *
 *  @param aPhonebookUid   Phonebook UID to get the value from.
 *  @param aSlotNum        Slot number to search.
 *  @param aContactItemId  Filled on successful exit with the Contact ID.
 *
 *  @return KErrNone if the ID is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetContactIdFromSlotNum(TUid aPhonebookUid,
												TInt aSlotNum,
												TContactItemId& aContactItemId)
	{
	LOGPARAMS3(_L8("GetContactIdFromSlotNum(): 0x%08x %d"),
			   aPhonebookUid, aSlotNum);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to get the contact UID from the Look-Up-Table...
	//
	AcquireMutex();
	aContactItemId = phonebook->GetContactIdFromSlotNum(aSlotNum);
	ReleaseMutex();
	
	if (aContactItemId == KNullContactId  ||  aContactItemId == KGoldenTemplateId)
		{
		return KErrNotFound;
		}

	return KErrNone;
	} // CPhoneBookManager::GetContactIdFromSlotNum


/**
 *  Returns the slot number for the entry in the Look-Up Table for a particular
 *  phonebook. The entry is identified by Contact ID.
 *
 *  @param aPhonebookUid   Phonebook UID to get the value from.
 *  @param aContactItemId  The Contact ID to search.
 *  @param aSlotNum        Filled on successful exit with the slot number.
 *
 *  @return KErrNone if the slot number is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetSlotNumFromContactId(TUid aPhonebookUid,
												TContactItemId aContactItemId,
												TInt& aSlotNum)
	{
	LOGPARAMS3(_L8("GetSlotNumFromContactId(): 0x%08x 0x%08x"),
			   aPhonebookUid, aContactItemId);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to get the slot number from the Look-Up-Table...
	//
	AcquireMutex();
	aSlotNum = phonebook->GetSlotNumFromContactId(aContactItemId);
	ReleaseMutex();
	
	if (aSlotNum < 0)
		{
		return KErrNotFound;
		}
	
	return KErrNone;
	} // CPhoneBookManager::GetSlotNumFromContactId


/**
 *  Returns the number of free slots for a particular phonebook.
 *
 *  @param aPhonebookUid    Phonebook UID to get the value from.
 *  @param aNumOfFreeSlots  Filled on successful exit with the number of
 *                          free slots.
 *
 *  @return KErrNone if the number is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetNumFreeSlots(TUid aPhonebookUid,
										TInt& aNumOfFreeSlots)
	{
	LOGPARAMS2(_L8("GetNumFreeSlots(): 0x%08x"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to edit this entry in the Look-Up-Table...
	//
	AcquireMutex();
	aNumOfFreeSlots = phonebook->GetNumFreeSlots();
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetNumFreeSlots


/**
 *  Returns the first free slot for a particular phonebook.
 *
 *  @param aPhonebookUid  Phonebook UID to get the value from.
 *  @param aSlotNum       Filled on successful exit with the first free
 *                        slot number.
 *
 *  @return KErrNone if the number is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetFirstEmptySlot(TUid aPhonebookUid,
										  TInt& aSlotNum)
	{
	LOGPARAMS2(_L8("GetFirstEmptySlot(): 0x%08x"), aPhonebookUid);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to find the first free slot in the Look-Up-Table...
	//
	AcquireMutex();
	result = phonebook->GetFirstEmptySlot();
	ReleaseMutex();
	
	if (result < 0)
		{
		return result;
		}
		
	aSlotNum = result;
	
	return KErrNone;
	} // CPhoneBookManager::GetFirstEmptySlot


/**
 *  Returns a list of slot numbers for a particular phonebook that match a
 *  given slot state.
 *
 *  @param aPhonebookUid  Phonebook UID to get the list from.
 *  @param aSlotState     Slot state to search for.
 *  @param aEntries       Filled on successful exit with the list of slots.
 *
 *  @return KErrNone if the list is returned, otherwise a system error code.
 */
TInt CPhoneBookManager::GetMatchingEntries(TUid aPhonebookUid,
										   TPhonebookSlotState aSlotState,
										   RArray<TInt>& aEntries)
	{
	LOGPARAMS3(_L8("GetMatchingEntries(): 0x%08x %d"), aPhonebookUid, aSlotState);

	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to the slot numbers from the Look-Up-Table...
	//
	AcquireMutex();
	phonebook->GetMatchingEntries(aSlotState, aEntries);
	ReleaseMutex();
	
	return KErrNone;
	} // CPhoneBookManager::GetMatchingEntries


/**
 *  Outputs the Look-Up Table for a particular phonebook to the logging file.
 *
 *  @note This function works only in debug mode.
 *
 *  @param aPhonebookUid  Phonebook UID of of the phonebook to log.
 *
 *  @return KErrNone if the table is logged, otherwise a system error code.
 */
TInt CPhoneBookManager::LogLookUpTable(TUid aPhonebookUid)
	{
	LOGPARAMS2(_L8("LogLookUpTable(): 0x%08x"), aPhonebookUid);
	
#ifdef _DEBUG
	//	
	// Get the pointer to the object containing parameters associated
	// with this phonebook.
	//
	CPhoneBook*  phonebook;
	TInt  result = GetPhonebook(aPhonebookUid, phonebook);

	if (result != KErrNone)
		{
		return result;
		}

	//
	// Call the phonebook to log the Look-Up-Table...
	//
	AcquireMutex();
	phonebook->LogLookUpTable();
	ReleaseMutex();
#else
	(void) aPhonebookUid;
#endif
	
	return KErrNone;
	} // CPhoneBookManager::LogLookUpTable


/**
 *  Returns a pointer to the class containing the parameters for the
 *  phonebook specified by the aPhonebook parameter.
 *
 *  @param aPhonebookUid  The phonebook specified.
 *
 *  @return Standard error code.
 */
TInt CPhoneBookManager::GetPhonebook(TUid aPhonebookUid,
									 CPhoneBook*& aPhoneBookPtr) const
	{
	LOGPARAMS2(_L8("GetPhonebook(0x%08x)"), aPhonebookUid);

	TInt  phonebooks(iPhonebookList.Count());
	
	for (TInt index = 0;  index < phonebooks;  index++)
		{
		CPhoneBook*  phonebook = iPhonebookList[index];
		
		if (phonebook->GetPhonebookUid() == aPhonebookUid)
			{
			aPhoneBookPtr = phonebook;
			return KErrNone;
			}
		}

	return KErrNotSupported;
	} // CPhoneBookManager::GetPhonebook

