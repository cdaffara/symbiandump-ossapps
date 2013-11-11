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
// Phonebook Synchroniser API header file.
// Describes the Phonebook Synchroniser API for use by clients of the
// Phonebook Synchroniser Server.
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __PHBKSYNC_H__
#define __PHBKSYNC_H__

#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>

/**
 *  Index not supplied.
 */
const TInt KSyncIndexNotSupplied=-1;


//
// TUid Phonebook Types used by Contacts Model 
//
#define KUidIccPhonebookNotSpecifiedValue    0x101F7874
#define KUidIccGlobalAdnPhonebookValue       0x101F7875
#define KUidIccGlobalSdnPhonebookValue       0x101F7876
#define KUidIccGlobalLndPhonebookValue       0x101F7877
#define KUidUsimAppAdnPhonebookValue         0x101F7878
#define KUidIccGlobalFdnPhonebookValue       0x101FB2AF

const TUid  KUidIccPhonebookNotSpecified = {KUidIccPhonebookNotSpecifiedValue};
const TUid  KUidIccGlobalAdnPhonebook    = {KUidIccGlobalAdnPhonebookValue};
const TUid  KUidIccGlobalSdnPhonebook    = {KUidIccGlobalSdnPhonebookValue};
const TUid  KUidIccGlobalLndPhonebook    = {KUidIccGlobalLndPhonebookValue};
const TUid  KUidUsimAppAdnPhonebook      = {KUidUsimAppAdnPhonebookValue};
const TUid  KUidIccGlobalFdnPhonebook    = {KUidIccGlobalFdnPhonebookValue};


/**
 *  Types of cancel requests for the asynchronous server requests.
 *
 *  @see RPhoneBookSession::CancelRequest
 */
enum TPhonebookSyncRequestCancel
	{
	/** Cancel the asynchronous RPhoneBookSession::DoSynchronisation() request */
	EDoSynchronisationCancelClient,

	/** Cancel the asynchronous RPhoneBookSession::DeleteContact() request */
	ESyncDeleteCntFromICCCancelClient,

	/** Cancel the asynchronous RPhoneBookSession::WriteContact() request */
	ESyncWriteCntToICCCancelClient,

	/** Cancel the asynchronous RPhoneBookSession::NotifyPhBkCacheStateChange() request */
	ESyncNotifyCacheStateChangeCancelClient
	};


/**
 *  Class to hold the contact field formats.
 *
 *  @publishedAll
 *  @released
 */
class TContactFieldFormat
	{
public:
	TContactFieldFormat(TInt aLength, TInt aCount, TBool aDisplayed);

public:
	/** The maximum length (i.e. number of characters) of the field. If the field is 
	not a string type this attribute should be ignored. */
	TInt  iLength;

	/** The maximum number of instances of this field within an item. This is set to 
	-1 if this is unlimited. */
	TInt  iCount;

	/** Indicates whether this is displayed or hidden. */
	TBool iDisplayed;
	};


//
// Forward declare the internal data storage class.
//
class CSyncClientData;

/**
 *  Provides the CPhoneBookSyncPlugin client access to a session with the
 *  Phonebook Synchronizer Server. 
 *
 *  A handle to an RPhoneBookSession enables the plug-in class to perform
 *  IPC communications with the server.
 *
 *  @publishedAll
 *  @released
 */
class RPhoneBookSession : public RSessionBase
	{
public:
	/**
	 *  Structure holding the template ID and buffer size. The client has
	 *  no need to use this structure.
	 *
	 *  @deprecated
	 */
	struct TTemplateAndBufferSize
		{
		TContactItemId  templateId;
		TInt  bufferSize;
		};

	/**
	 *  The synchronisation mode for phonebooks when the server starts.
	 */
	enum TPhonebookSyncMode
		{
		/** Automatic synchronisation will be performed. */
		EAutoCurrentIcc,

		/** Automatic synchronisation will only be performed if the current ICC is
		    the same as the last ICC to be synchonised. */
		EAutoSameIcc,

		/** Manual synchronisation will be performed when requested. */
		EManual
		};

	/**
	 *  The phonebook cache state.
	 */
	enum TSyncState
		{
		/** The server has not yet populated the Contacts Database. */
		EUnsynchronised,

		/** The server has finished populating the Contacts Database. */
		ECacheValid,

		/** The server encountered an error populating the Contacts Database. */
		EErrorDuringSync
		};

	/**
	 *  Specifies format of phonebook fields within an ICC phonebook entry.
	 *
	 *  @publishedAll
	 *  @released
	 */
	class TContactFieldsV1  
		{
	public:
		/**
		 *  Enumeration to describe the version ID of the Phonebook Synchronizer.
		 */
		enum TContactFieldsExtensionId
			{
			/** Phonebook Synchronizer version 1 */
			KPhBkSyncTContactFieldsV1,

			/** Phonebook Synchronizer version 2 */
			KPhBkSyncTContactFieldsV2,

			/** Phonebook Synchronizer version 3 */
			KPhBkSyncTContactFieldsV3
			};

		/**
		 *  Returns the Phonebook Synchronizer API extension number of the class.
		 *
		 *  @return  The version ID from TContactFieldsExtensionId.
		 */
		IMPORT_C TInt ExtensionId() const;

		/**
		 *  Standard constructor.
		 */
		IMPORT_C TContactFieldsV1();

	public:
		/** The format of the name field within an ICC phonebook entry. */
		TContactFieldFormat  iNameField;

		/** The format of the number field within an ICC phonebook entry. */
		TContactFieldFormat  iNumberField;

		/** The format of the ICC slot number field within an ICC phonebook entry. */
		TContactFieldFormat  iIccSlotField;

	protected:
		TInt  iExtensionId;
		};

	/**
	 *  A packaged instance of TContactFieldsV1.
	 */
	typedef TPckg<TContactFieldsV1>  TContactFieldsV1Pckg;

	/**
	 *  Specifies format of phonebook fields within an UICC phonebook entry
	 *
	 *  @publishedAll
	 *  @released
	 */
	class TContactFieldsV2 : public TContactFieldsV1 
		{
	public:
		IMPORT_C TContactFieldsV2();

	public:
		/** The format of any additional number fields within an ICC phonebook entry. */
		TContactFieldFormat  iAdditionalNumString;

		/** The format of group field within an ICC phonebook entry. */
		TContactFieldFormat  iGroupField;

		/** The format of email field within an ICC phonebook entry. */
		TContactFieldFormat  iEmailField;

		/** The UID of the phonebook. */
		TUid  iPhonebook;
		};

	/**
	 *  A packaged instance of TContactFieldsV2.
	 */
	typedef TPckg<TContactFieldsV2>  TContactFieldsV2Pckg;


	/**
	 *  Specifies additional format information for phonebook fields within a
	 *  UICC phonebook entry.
	 *
	 *  @publishedAll
	 *  @released
	 */
	class TContactFieldsV3 : public TContactFieldsV2 
		{
	public:
		IMPORT_C TContactFieldsV3();

	public:
		/** The format of an addition number's text field within an ICC phonebook entry. */
		TContactFieldFormat  iAdditionalNumAlphaString;

		/** The format of the second name field within an ICC phonebook entry. */
		TContactFieldFormat  iSecondNameField;
		};

	/**
	 *  A packaged instance of TContactFieldsV3.
	 */
	typedef TPckg<TContactFieldsV3>  TContactFieldsV3Pckg;


	/**
	 *  The phonebook ID type requested from a phonebook. There are two types of IDs
	 *  (group IDs and template IDs).
	 *
	 *  @see RPhoneBookSession::GetPhoneBookId()
	 */
	enum TSyncIdType
		{
		/** Used to allocate a new entry to the right group in the Contacts Database. */
		ESyncGroupId,

		/** Used to define new ICC entries in the Contacts Database. */
		ESyncTemplateId
		};

public:
	IMPORT_C RPhoneBookSession();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	IMPORT_C TVersion Version() const;
	IMPORT_C TInt ShutdownServer (TBool aUnconditionally);

	//
	// Single (ADN) phonebook support and functions not requiring a phonebook UID.
	//
	IMPORT_C void DoSynchronisation(TRequestStatus& aReqStatus);
	IMPORT_C TInt ValidateContact(MContactSynchroniser::TValidateOperation aOperation, TContactItemId aId);
	IMPORT_C void WriteContact(TRequestStatus& aReqStatus, CContactICCEntry& aContactItem, 
							   TInt& aSlotNumber);
	IMPORT_C void DeleteContact(TRequestStatus& aReqStatus, TContactItemId aId);
	IMPORT_C TInt GetPhoneBookId(TContactItemId& aId, TSyncIdType aIdType);
	IMPORT_C TInt UpdateLookuptable(TContactItemId aNewUid, TInt aIccSlot);
	IMPORT_C TInt GetContactFormat(TDes8& aContactFields);
	IMPORT_C TInt GetSyncMode(TPhonebookSyncMode& aSyncMode);
	IMPORT_C TInt SetSyncMode(TPhonebookSyncMode aSyncMode);
	IMPORT_C TInt GetPhoneBookCacheState(TSyncState& aState);
	IMPORT_C void CancelRequest(TPhonebookSyncRequestCancel aReqToCancel);
	IMPORT_C TInt GetLastSyncError(TInt& aError);
	IMPORT_C void NotifyPhBkCacheStateChange(TRequestStatus& aStatus);
	IMPORT_C TInt GetNumSlots(TInt& aNumSlots) const;
	IMPORT_C void GetFreeSlotsL(RArray<TInt>& aFreeSlots) const;
	IMPORT_C TInt GetSlotId(TInt aSlot, TContactItemId& aId) const;

	//
	// Multiple phonebooks support. These should be used in preference to the above
	// ADN specific versions.
	//
	IMPORT_C void DoSynchronisation(TRequestStatus& aReqStatus, TUid aPhonebookUid);
	IMPORT_C void WriteContact(TRequestStatus& aReqStatus, CContactICCEntry& aContactItem, 
							   TInt& aSlotNumber, TUid& aPhonebookUid);
	IMPORT_C TInt GetPhoneBookId(TContactItemId& aId, TSyncIdType aIdType, TUid aPhonebookUid);
	IMPORT_C TInt UpdateLookuptable(TContactItemId aNewUid, TInt aIccSlot, TUid aPhonebookUid);
	IMPORT_C TInt GetSyncMode(TPhonebookSyncMode& aSyncMode, TUid aPhonebookId);
	IMPORT_C TInt SetSyncMode(TPhonebookSyncMode aSyncMode, TUid aPhonebookId);
	IMPORT_C TInt GetPhoneBookCacheState(TSyncState& aState, TUid aPhonebookId);
	IMPORT_C void CancelRequest(TPhonebookSyncRequestCancel aReqToCancel, TUid aPhonebookId);
	IMPORT_C TInt GetLastSyncError(TInt& aError, TUid aPhonebookId);
	IMPORT_C void NotifyPhBkCacheStateChange(TRequestStatus& aStatus, TUid aPhonebookId);
	IMPORT_C TInt GetNumSlots(TInt& aNumSlots, TUid aPhonebookId) const;
	IMPORT_C void GetFreeSlotsL(RArray<TInt>& aFreeSlots, TUid aPhonebookId) const;
	IMPORT_C TInt GetSlotId(TInt aSlot, TContactItemId& aId, TUid aPhonebookId) const;

	//
	// Memory checking functionality for Debug builds only.
	//
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);

private:
	RPhoneBookSession(const RPhoneBookSession& aSession);

private:
	/* Temporary data used by the session. */
	CSyncClientData*  iData;
	};

#endif
