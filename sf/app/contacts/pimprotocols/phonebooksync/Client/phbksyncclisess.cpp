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
// Phonebook Synchroniser API Client side implementation. This file implements
// the methods used to package the client data and pass it to the front-end server
// for processing.
// 
//

/**
 @file
 @publishedAll
 @released
*/

#include "phbksync.h"
#include "common.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"
#include "phbksynclog.h"


/**
 *  Phonebook Synchroniser API version.
 */
const TInt  KPhonebookSyncMajorVersion = 1;
const TInt  KPhonebookSyncMinorVersion = 2;
const TInt  KPhonebookSyncBuildVersion = 1;


/**
 *  Holds any client data that is used in RPhoneBookSession requests. Typically
 *  this is data for asynchronous requests which need to be repackaged. This is
 *  currently just the RPhoneBookSession::WriteContact() APIs.
 *
 *  Its data used in an asyncronous request, that does not fit into a IPC
 *  request. E.g. It is bigger that the 4 32-bit numbers, and is not a
 *  descriptor. It must be put in a descriptor before it can be sent, and
 *  that memory has to be allocated somewhere on the client side. 
 *
 *  The API code also uses this class to ensure BC by keeping the size of R-classes
 *  constant. Future data can be added to this class as required.
 */
class CSyncClientData : public CBase
	{
public:
	static CSyncClientData* NewL();
	void ConstructL();
	CSyncClientData();
	~CSyncClientData();

public:
    //
    // The following fields allow a contact mapped in internal format and other
    // data related to the WriteContact() API call to be streamed across the IPC
    // boundary.
    //
	RPhoneBookSession::TTemplateAndBufferSize  iWrtCntTmpltIdAndBufSize;
	CSyncContactICCEntry*  iWrtCntEntry;
	CBufBase*  iWrtCntEntryBuf;
	TUid  iWrtCntDefaultPhbkUid;
	TPtrC8  iWrtCntTmpltIdAndBufSizePtr;
	TPtr8  iWrtCntSlotNumPtr;
	TPtr8  iWrtCntPhBkIDPtr;
	TPtr8  iWrtCntEntryBufPtr;
	};


/**
 *  Start the server process which lives in its own executable and rendezvous with it.
 *
 *  @return  KErrNone if successful, or an error code if not.
 */
static TInt StartPhBkSyncServer()
	{
	LOGCLIENT1(_L8("StartPhBkSyncServer()"));
	
	//
	// Create a new server process. Simultaneous launching of two such
	// processes should be detected when the second one attempts to
	// create the server object, failing with KErrAlreadyExists.
	//		
	_LIT(KPhBkSyncSvrExeImg, "PHBKSYNCSVREXE.EXE");
	RProcess server;
	
	TInt  ret = server.Create(KPhBkSyncSvrExeImg, KNullDesC);
	if (ret != KErrNone)
		{
		return ret;
		}
	
	//
	// Rendezvous with the server or abort startup...
	//
	TRequestStatus  status;

	server.Rendezvous(status);
	if (status != KRequestPending)
		{
		server.Kill(0);
		}
	else
		{
		server.Resume();
		}
	User::WaitForRequest(status);

	//
	// We can't use the 'exit reason' if the server paniked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone.
	//
	if (server.ExitType() == EExitPanic)
		{
		ret = KErrGeneral;
		}
	else
		{
		ret = status.Int();
		}

	server.Close();

	return ret;
	} // StartPhBkSyncServer


/**
 *  Standard constructor.
 */
EXPORT_C RPhoneBookSession::RPhoneBookSession()
  : iData(NULL)
	{
	// NOP
	} // RPhoneBookSession::RPhoneBookSession


/**
 *  Connects the client to the Phonebook Synchroniser Server. 
 *
 *  This must be used before any of the other methods listed in this API
 *  section. The first client to call this method will cause the initiation of
 *  the Phonebook Synchroniser Server within its own executable process.
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::Connect()
	{
	LOGCLIENT1(_L8("RPhoneBookSession::Connect()"));
	
	__ASSERT_ALWAYS(iData == NULL,
					PhBkSyncPanic(EPhBkSyncPanicHandleNotClosed));

	//
	// Create a session with the server, but if it doesn't exist then start it and
	// then create a session.
	//
	TInt  result = CreateSession(PHBKSYNC_SERVER_NAME,
								 TVersion(KPhonebookSyncMajorVersion,
	                					  KPhonebookSyncMinorVersion,
	                					  KPhonebookSyncBuildVersion));
	if (result == KErrNotFound  ||  result == KErrServerTerminated)
		{
		result = StartPhBkSyncServer();
		
		if(result == KErrNone)
			{
			result = CreateSession(PHBKSYNC_SERVER_NAME,
								   TVersion(KPhonebookSyncMajorVersion,
	                					    KPhonebookSyncMinorVersion,
	                					    KPhonebookSyncBuildVersion));
			}
		}	
		
	if (result == KErrNone)
		{
		TRAP(result, iData = CSyncClientData::NewL());
		}

	//
	// If the creation of the session fails clean up session data...
	//
	if (result != KErrNone)
		{
		Close();
		}

	return result;
	} // RPhoneBookSession::Connect


/**
 *  Closes the client's session with the Phonebook Synchroniser Server. 
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::Close()
	{
	LOGCLIENT1(_L8("RPhoneBookSession::Close()"));

	RSessionBase::Close();

	delete iData;
	iData = NULL;
	} // RPhoneBookSession::Close


/**
 *  Returns the current version of the Phonebook Synchroniser Server.
 *
 *  @return The version of the Phonebook Synchroniser Server. 
 *
 *  @capability None
 */
EXPORT_C TVersion RPhoneBookSession::Version() const
	{
	LOGCLIENT1(_L8("RPhoneBookSession::Version()"));

	return(TVersion(KPhonebookSyncMajorVersion,
	                KPhonebookSyncMinorVersion,
	                KPhonebookSyncBuildVersion));
	} // RPhoneBookSession::Version


/**
 *  Requests the shutdown of the server when the last client disconnects.
 *  There is no support for immediate shutdown functionality. This API call
 *  can only be executed if the server is compiled as a debug release.
 *  
 *  @param aConditional  If true, subsequent connection requests from any client
 *                       will be honoured delaying shutdown until they disconnect.
 *                       If untrue, all subsequent connect requests will be denied.
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability ReadUserData
 *  @capability WriteUserData 
 */
EXPORT_C TInt RPhoneBookSession::ShutdownServer(TBool aConditional)
	{	
	LOGCLIENT1(_L8("RPhoneBookSession::ShutdownServer()"));

	return SendReceive(ESyncShutdownServer, TIpcArgs(aConditional));
	} // RPhoneBookSession::ShutdownServer


/**
 *  Executes a manual synchronisation of the Global/GSM ADN phonebook entries.
 *
 *  @param aReqStatus  On completion, KErrNone if successful, a system-wide error 
 *                     code if not. 
 *
 *  @capability ReadUserData
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::DoSynchronisation(TRequestStatus& aReqStatus)
	{
	DoSynchronisation(aReqStatus, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::DoSynchronisation


/**
 *  Instructs the Phonebook Synchroniser Server to start a manual synchronisation
 *  of the ICC phonebook specified by the aPhonebook parameter.
 *
 *  @param aReqStatus     On completion, KErrNone if successful, a system-wide
 *						  error code if not. 
 *  @param aPhonebookUid  TUid of the ICC phonebook to be synchronised.
 *
 *  @capability ReadUserData
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::DoSynchronisation(TRequestStatus& aReqStatus,
												   TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::DoSynchronisation"));
	
	SendReceive(ESyncDoSynchronisation, TIpcArgs(aPhonebookUid.iUid), aReqStatus);
	} // RPhoneBookSession::DoSynchronisation


/**
 *  Instructs the Phonebook Synchroniser Server to verify the access to the ICC
 *  contact item stored in the Contacts database. The server is responsible for
 *  mapping the UID to the phonebook this item belongs to.
 *
 *  @param aOperation  Holds the type of check, search, edit, read.
 *  @param aId         The contact model UID of the record stored in the database.
 *
 *  @return KErrNone if operation was successful, otherwise return error
 *
 *  @capability ReadUserData
 */
EXPORT_C TInt RPhoneBookSession::ValidateContact(MContactSynchroniser::TValidateOperation aOperation, 
												 TContactItemId aId)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::ValidateContact"));

	return SendReceive(ESyncValidateContact, TIpcArgs(aId, aOperation));
	} // RPhoneBookSession::ValidateContact


/**
 *  Writes the contact item to the Global/GSM ADN phonebook. This can be used
 *  to edit an existing contact or to create a new contact.
 *
 *  @param aReqStatus    On completion, KErrNone if successful, a system-wide error 
 *                       code if not.
 *  @param aContactItem  The contact item to write to the ICC card.
 *  @param aSlotNumber   The slot number to write to contact into. If set to
 *                       KSyncIndexNotSupplied the Phonebook Synchroniser Server
 *                       will use the first empty slot and aSlotNumber will contain
 *                       the slot number used on completion.
 *
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::WriteContact(TRequestStatus& aReqStatus,
                                              CContactICCEntry& aContactItem, 
											  TInt& aSlotNumber)
	{
	__ASSERT_ALWAYS(iData != NULL,
					PhBkSyncPanic(EPhBkSyncPanicNullHandle));

	//
	// The dummy phonebook ID value is used as we are the old API and must communicate
	// the ADN ID to the server. However the server may write it back and thus it must
	// be a valid memory address after this function finishes (which is before the async
	// request will complete).
	//
	iData->iWrtCntDefaultPhbkUid = KUidIccGlobalAdnPhonebook;
	
	WriteContact(aReqStatus, aContactItem, aSlotNumber,
	             iData->iWrtCntDefaultPhbkUid);
	} // RPhoneBookSession::WriteContact
	

/**
 *  Write the contact item supplied in the aContactItem parameter to the ICC
 *  phonebook specified by aPhonebookUid.
 *
 *  This method is used to edit an existing or write new entry to the ICC phonebook.
 *  When editing an existing entry the slot number will be supplied in aContactItem,
 *  whereas for new entries the new slot number will be returned in the aSlotNumber
 *  parameter. Similarly for new entries the phonebook UID will be returned in the
 *  aPhonebookUid parameter.
 *
 *  @param aReqStatus     On completion, KErrNone if successful, a system-wide error 
 *                        code if not.
 *  @param aContactItem   Contacts ICC entry.
 *  @param aPhonebookUid  UID of the ICC phonebook.
 *  @param aSlotNumber    Allocated slot number.
 *
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::WriteContact(TRequestStatus& aReqStatus, CContactICCEntry& aContactItem, 
											  TInt& aSlotNumber, TUid& aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::WriteContact()"));

	//
	// Check that the sync client data is available and free any previous
	// buffer.
	//
	__ASSERT_ALWAYS(iData != NULL,
					PhBkSyncPanic(EPhBkSyncPanicNullHandle));

	//
	// Convert the entry to internal format used by PhBkSync. This is needed
	// as use of a Contact Database entry can be dependant on a transaction
	// being open, therefore an independent structure is used. Also a
	// CContactICCEntry is not streamable whereas the internal format is.
	//
	TRAPD(err, iData->iWrtCntEntry->InitialiseFromContactICCEntryL(aContactItem));
	if (err != KErrNone)
		{
		TRequestStatus*  status = &aReqStatus; 
		User::RequestComplete(status, err);
		return;
		}

	if (aSlotNumber != KSyncIndexNotSupplied)
		{
		iData->iWrtCntEntry->iSlotNum = aSlotNumber;
		}

#ifdef _DEBUG
	iData->iWrtCntEntry->LogSyncContactICCEntry();
#endif

	//
	// Stream the entry into a single buffer. This will allow the entry to be
	// read accoss the IPC boundary.
	//
	// Note: The iWrtCntEntryBuf buffer is allocated by StoreL().
	//
	if (iData->iWrtCntEntryBuf)
		{
		delete iData->iWrtCntEntryBuf;  
		iData->iWrtCntEntryBuf = NULL;
		}

	TRAP(err, iData->iWrtCntEntryBuf = iData->iWrtCntEntry->StoreL());
	if (err != KErrNone)
		{
		TRequestStatus*  status = &aReqStatus; 
		User::RequestComplete(status, err);
		return;
		}

	//
	// Setup the pointers for the transfer of the contact across the IPC
	// boundary.
	//
	iData->iWrtCntTmpltIdAndBufSize.bufferSize = iData->iWrtCntEntryBuf->Size();
	iData->iWrtCntTmpltIdAndBufSize.templateId = iData->iWrtCntEntry->iTemplateId;

	iData->iWrtCntTmpltIdAndBufSizePtr.Set(reinterpret_cast<const TText8*>(&iData->iWrtCntTmpltIdAndBufSize),
										   sizeof(RPhoneBookSession::TTemplateAndBufferSize));
	iData->iWrtCntSlotNumPtr.Set(reinterpret_cast<TText8*>(&aSlotNumber),
								 sizeof(aSlotNumber), sizeof(aSlotNumber));
	iData->iWrtCntPhBkIDPtr.Set(reinterpret_cast<TText8*>(&aPhonebookUid),
								sizeof(aPhonebookUid), sizeof(aPhonebookUid));
	iData->iWrtCntEntryBufPtr.Set(iData->iWrtCntEntryBuf->Ptr(0));

	TIpcArgs  args(&iData->iWrtCntTmpltIdAndBufSizePtr,
	               &iData->iWrtCntSlotNumPtr,
	               &iData->iWrtCntPhBkIDPtr,
	               &iData->iWrtCntEntryBufPtr);

	SendReceive(ESyncWriteCntToICC, args, aReqStatus);

	//
	// This function uses the session client data space to store encoded
	// parameters before they are sent to the front-end server.  This means
	// that there is a small risk that if another call to WriteContact is made
	// before this one completes, then the data maybe corrupted.
	//
	// To prevent this we issue a synchronous request here. This will ensure
	// that at a minimum the request is read by the server and pre-processed
	// such that the data and pointer locations are read and stored inside the
	// server.
	//
	// This leaves only a risk that if the requests are ADN single phonebook
	// API versions, then the dummy Phonebook ID may be overwritten. This is
	// of course not a problem as the dummy value is not used.
	//
	SendReceive(ESyncFlushInterfaceQueue, TIpcArgs());
	} // RPhoneBookSession::WriteContact


/**
 *  Deletes the contact item from the ICC card.
 *
 *  @param aReqStatus  On completion, KErrNone if successful, a system-wide error 
 *                     code if not.
 *  @param aId         The contact item to delete. 
 *
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::DeleteContact(TRequestStatus& aReqStatus, TContactItemId aId)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::DeleteContact()"));

	SendReceive(ESyncDeleteCntFromICC, TIpcArgs(aId), aReqStatus);
	} // RPhoneBookSession::DeleteContact


/**
 *  Returns information about the supported fields within an ICC phonebook entry. 
 *
 *  The TContactFieldFormat type defines the information available for each field 
 *  and TContactFieldsV1 is a collection of this information for all supported 
 *  field types. The client can use the field information to tailor its UI so 
 *  that user can only enter fields with the maximum number of supported characters.
 *
 *  The only fields supported by GSM ADN phonebook are the name, number and ICC 
 *  slot number, so these are the only fields included in TContactFieldsV1.
 *
 *  TContactFieldsV2 is used to contain the new USIM phonebook fields.
 *
 *  TContactFieldsV3 is an extension of TContactFieldsV2 and contains additional
 *  information on the fields supported by the USIM phonebooks.
 *
 *  @param aContactFields  On return, the packaged TContactFieldsV1/V2/V3
 *                         is filled with the supported field information.
 *
 *  @return KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetContactFormat(TDes8& aContactFields)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetContactFormat()"));

	TUid  phonebook(KUidIccPhonebookNotSpecified);

	TContactFieldsV1Pckg*  contactFieldsV1Pckg = static_cast<TContactFieldsV1Pckg*>(&aContactFields);
	TContactFieldsV1&  contactFieldsV1 = (*contactFieldsV1Pckg)();
	TContactFieldsV1::TContactFieldsExtensionId  extensionId = static_cast<TContactFieldsV1::TContactFieldsExtensionId>(contactFieldsV1.ExtensionId());
	
	if (extensionId == TContactFieldsV1::KPhBkSyncTContactFieldsV2)
		{
		TContactFieldsV2Pckg*  contactFieldsV2Pckg = static_cast<TContactFieldsV2Pckg*>(&aContactFields);
		TContactFieldsV2&  contactFieldsV2 = (*contactFieldsV2Pckg)();
		phonebook = contactFieldsV2.iPhonebook;
		}
	else if (extensionId == TContactFieldsV1::KPhBkSyncTContactFieldsV3)
		{
		TContactFieldsV3Pckg*  contactFieldsV3Pckg = static_cast<TContactFieldsV3Pckg*>(&aContactFields);
		TContactFieldsV3&  contactFieldsV3 = (*contactFieldsV3Pckg)();
		phonebook = contactFieldsV3.iPhonebook;
		}
	else
		{
		//
		// Phonebook for TContactFieldsV1 is set to Global ADN, as the same
		// information is set for all phonebooks.
		//
		phonebook = KUidIccGlobalAdnPhonebook;
		}

	return SendReceive(ESyncGetContactFormat, TIpcArgs(phonebook.iUid, extensionId, 
	                                                   &aContactFields)); 
	} // RPhoneBookSession::GetContactFormat


/**
 *  Returns the current setting for the synchronisation mode of the Global/GSM ADN
 *  phonebook, used by the Phonebook Synchroniser Server at start-up to determine
 *  if the phonebook should be automatically synchronised.
 *
 *  @param aSyncMode  On return, the synchronisation mode.
 *
 *  @return KErrNone if the setting is successfully returned in aSyncMode, or
 *          an error code if the setting does not exist or can not be accessed. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetSyncMode(TPhonebookSyncMode& aSyncMode)
	{
	return GetSyncMode(aSyncMode, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetSyncMode


/**
 *  Returns the current setting for the Synchonisation Mode of the ICC phonebook
 *  specified by aPhonebookUid and used by the Phonebook Synchroniser Server at
 *  start-up to determine if the phonebook should be automatically synchronised.
 *
 *  @param aSyncMode      On return Will contain the current synchronisation mode.
 *  @param aPhonebookUid  TUid of the ICC phonebook
 *
 *  @return KErrNone if operation was successful, otherwise return error.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetSyncMode(TPhonebookSyncMode& aSyncMode,
                                             TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetSyncMode()"));

	TPckg<TPhonebookSyncMode>  syncModePtr(aSyncMode);

	return SendReceive(ESyncGetSyncMode, TIpcArgs(aPhonebookUid.iUid, &syncModePtr));
	} // RPhoneBookSession::GetSyncMode


/**
 *  Sets a new value for the synchronisation mode of the Global/GSM ADN phonebook, 
 *  used by the Phonebook Synchroniser Server at start-up to determine its
 *  synchronisation policy.
 *
 *  If the mode is changing to EAutoCurrentIcc and no sync has yet been performed,
 *  then an automatic sync will be performed.
 *
 *  @param aSyncMode  The new synchronisation mode.
 *
 *  @return KErrNone if the setting specified by aSyncMode is successfully changed, 
 *          or an error code if the setting does not exist or can not be accessed.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::SetSyncMode(TPhonebookSyncMode aSyncMode)
	{
	return SetSyncMode(aSyncMode, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::SetSyncMode


/**
 *  Set a new value for the Synchonisation Mode of the ICC phonebook specified by aPhonebookUid,
 *  used by the Phonebook Synchroniser Server at start-up to determine its synchronisation 
 *  policy.
 *
 *  @param aSyncMode      Current synchronisation mode
 *  @param aPhonebookUid  TUid of the ICC phonebook
 *
 *  @return KErrNone if operation was successful, otherwise return error
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::SetSyncMode(TPhonebookSyncMode aSyncMode,
                                             TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::SetSyncMode()"));
	
	return SendReceive(ESyncSetSyncMode, TIpcArgs(aPhonebookUid.iUid,aSyncMode));
	} // RPhoneBookSession::SetSyncMode


/**
 *  Returns the requested UID for the ID type belonging to the Global/GSM ADN
 *  phonebook. This is the ID type spedified by aIdType, which can be either the
 *  template ID or the group ID for an ICC entry belonging to the Global/GSM ADN
 *  phonebook, depending on the type supplied by aIdType.
 *
 *  @param aId      On return, the contact ID.
 *  @param aIdType  The type of ID requested.
 *
 *  @return KErrNone, if the UID is successfully returned or an error code if the 
 *          UID does not exist or can not be accessed. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetPhoneBookId(TContactItemId& aId, TSyncIdType aIdType)
	{
	return GetPhoneBookId(aId, aIdType, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetPhoneBookId


/**
 *  Return either the current group or template ID (depending on the type supplied
 *  by aIdType) for the ICC entries belonging to the ICC phonebook specified by the
 *  aPhonebookUid parameter.
 *
 *  @param aId            Contacts UID
 *  @param aPhonebookUid  TUid of the ICC phonebook
 *  @param aIdType        Type of Contacts UID
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetPhoneBookId(TContactItemId& aId, TSyncIdType aIdType,
											    TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetPhoneBookId()"));

	TPckg<TContactItemId>  contactItemIdPtr(aId);

	return SendReceive(ESyncGetPhoneBookId,
	                   TIpcArgs(aPhonebookUid.iUid, aIdType, &contactItemIdPtr));
	} // RPhoneBookSession::GetPhoneBookId


/**
 *  Updates the Phonebook Synchroniser's look-up table with the new contact ID in
 *  the Global/GSM ADN phonebook.
 *
 *  This function should be called when a new entry has been added to the Contacts
 *  Database by a client other than the Phonebook Synchroniser Server, so the server
 *  can know what the UID of the newly added contact was.
 *
 *  The Phonebook Synchroniser plug-in should extract the appropriate details from
 *  the item (e.g. item UID and slot number) and call the Phonebook server to update
 *  the look-up tables.
 *
 *  @param aNewUid   The contact use for the update.
 *  @param aIccSlot  The ICC slot number to match the Contacts UID.
 *
 *  @return KErrNone if the look-up table update succeeded, or an error code (such 
 *          as KErrArgument) if the operation failed for that ICC slot number. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::UpdateLookuptable(TContactItemId aNewUid, TInt aIccSlot)
	{
	return UpdateLookuptable(aNewUid, aIccSlot, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::UpdateLookuptable


/**
 *  Updates the Phonebook Synchroniser's look-up table with the new contact ID in
 *  the ICC phonebook specified by the aPhonebookUid parameter
 *
 *  This function should be called when a new entry has been added to the Contacts
 *  Database by a client other than the Phonebook Synchroniser Server, so the server
 *  can know what the UID of the newly added contact was.
 *
 *  The Phonebook Synchroniser plug-in should extract the appropriate details from
 *  the item (e.g. item UID and slot number) and call the Phonebook server to update
 *  the look-up tables.
 *
 *  @param aNewUid        Contact UID number unique per entry
 *  @param aIccSlot       ICC slot value
 *  @param aPhonebookUid  Type of ICC phonebook
 *
 *  @return KErrNone if operation was successful, otherwise return error
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::UpdateLookuptable(TContactItemId aNewUid, TInt aIccSlot,
												   TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::UpdateLookuptable()"));

	return SendReceive(ESyncUpdateLookupTable,
	                   TIpcArgs(aPhonebookUid.iUid, aNewUid, aIccSlot));
	} // RPhoneBookSession::UpdateLookuptable


/**
 *  Returns the current state of the Phonebook Synchroniser server cache for
 *  the Global/GSM ADN phonebook.
 *
 * @param aState  On return, the state of the phonebook synchroniser cache.
 *
 * @return KErrNone if the state is successfully retrieved, or an error code if 
 *         the server is unable to process the request. 
 *
 * @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetPhoneBookCacheState(TSyncState& aState)
	{
	return GetPhoneBookCacheState(aState, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetPhoneBookCacheState


/**
 *  Return the current phonebook server cache state for the ICC phonebook
 *  specified by the aPhonebook parameter.
 *
 *  @param aState         Current state of the phonebook cache
 *  @param aPhonebookUid  Type of ICC phonebook
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetPhoneBookCacheState(TSyncState& aState,
														TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetPhoneBookCacheState()"));

	TPckg<TSyncState>  statePtr(aState);
	
	return SendReceive(ESyncGetCacheState, TIpcArgs(aPhonebookUid.iUid, &statePtr));
	} // RPhoneBookSession::GetPhoneBookCacheState


/**
 *  Cancels a previously issued asynchronous request on the Global/GSM ADN phonebook. 
 *
 *  @param aReqToCancel  The request to be cancelled.
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::CancelRequest(TPhonebookSyncRequestCancel aReqToCancel)
	{
	CancelRequest(aReqToCancel, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::CancelRequest


/**
 *  Cancels a previously issued asynchronous request. The request to be cancelled
 *  is specified by the aReqToCancel parameter and the ICC phonebook for which the
 *  request needs to be cancelled is specified by the aPhonebookUid parameter.
 *
 *  @param aReqToCancel   Request to be cancelled.
 *  @param aPhonebookUid  UID of the ICC phonebook.
 *
 *  @capability ReadUserData
 *  @capability WriteUserData
 */
EXPORT_C void RPhoneBookSession::CancelRequest(TPhonebookSyncRequestCancel aReqToCancel,
											   TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::CancelRequest()"));

	switch (aReqToCancel)
		{
		case EDoSynchronisationCancelClient:
			{
			SendReceive(ESyncDoSynchronisationCancel, TIpcArgs(aPhonebookUid.iUid));
			}
			break;

		case ESyncDeleteCntFromICCCancelClient:
			{
			SendReceive(ESyncDeleteCntFromICCCancel, TIpcArgs(aPhonebookUid.iUid));
			}
			break;

		case ESyncWriteCntToICCCancelClient:
			{
			SendReceive(ESyncWriteCntToICCCancel, TIpcArgs(aPhonebookUid.iUid));
			}
			break;

		case ESyncNotifyCacheStateChangeCancelClient:
			{
			SendReceive(ESyncNotifyCacheStateChangeCancel, TIpcArgs(aPhonebookUid.iUid));
			}
			break;

		default:
			{
			PhBkSyncPanic(EPhBkSyncPanicCancelSyncReqError);
			}
			break;
		}
	} // RPhoneBookSession::CancelRequest


/**
 *  Returns the last server cache sync error for the Global/GSM ADN phonebook. 
 *
 *  @param aError  Last server cache sync error
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetLastSyncError(TInt& aError)
	{
	return GetLastSyncError(aError, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetLastSyncError


/**
 *  Return the last server cache sync error for the requested phonebook.
 *
 *  @param aError         Last server cache sync error.
 *  @param aPhonebookUid  UID of the ICC phonebook.
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetLastSyncError(TInt& aError,
												  TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetLastSyncError()"));

	TPckg<TInt>  errorPtr(aError);

	return SendReceive(ESyncGetLastSyncError,
	                   TIpcArgs(aPhonebookUid.iUid, &errorPtr));
	} // RPhoneBookSession::GetLastSyncError


/**
 *  Registers the notification for a change in the Global/GSM ADN phonebook cache
 *  state.
 *
 *  @note The client should call GetPhoneBookCacheState() in order to get the new 
 *        state of the synchroniser.
 *
 *  @param aReqStatus  On completion, KErrNone if successful, a system-wide error 
 *                     code if not.
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::NotifyPhBkCacheStateChange(TRequestStatus& aReqStatus)
	{
	NotifyPhBkCacheStateChange(aReqStatus, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::NotifyPhBkCacheStateChange


/**
 *  Registers the notification for a change in the Phone book cache state.
 *
 *  @note  The client should call GetPhoneBookCacheState() in order to get the new 
 *  state of the synchroniser.
 *
 *  @param aReqStatus     On completion, KErrNone if successful, a system-wide error 
 *                        code if not.
 *  @param aPhonebookUid  UID of the ICC phonebook.
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::NotifyPhBkCacheStateChange(TRequestStatus& aReqStatus,
                                                            TUid aPhonebookUid)
	{
	LOGCLIENT1(_L8("RPhoneBookSession::NotifyPhBkCacheStateChange()"));

	SendReceive(ESyncNotifyCacheStateChange, TIpcArgs(aPhonebookUid.iUid), aReqStatus);
	} // RPhoneBookSession::NotifyPhBkCacheStateChange


/**
 *  Gets the total number of slots on the Global/GSM ADN phonebook.
 *
 *  @param aNumSlots  The number of slots.
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetNumSlots(TInt& aNumSlots) const
    {
	return GetNumSlots(aNumSlots, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetNumSlots


/**
 *  Gets the total number of slots on the specified ICC phonebook.
 *
 *  @param aNumSlots      The number of slots.
 *  @param aPhonebookUid  Type of ICC phonebook
 *
 *  @return  KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetNumSlots(TInt& aNumSlots,
											 TUid aPhonebookUid) const
    {
	LOGCLIENT1(_L8("RPhoneBookSession::GetNumSlots()"));

	TPckg<TInt>  numSlotsPtr(aNumSlots);

	return SendReceive(ESyncGetNumSlots, TIpcArgs(aPhonebookUid.iUid, &numSlotsPtr));
	} // RPhoneBookSession::GetNumSlots


/**
 *  Get the slots that are free in the Global/GSM ADN phonebook.
 *
 *  @param aFreeSlots  On return, an array of slots numbers that are free.
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::GetFreeSlotsL(RArray<TInt>& aFreeSlots) const
	{
	GetFreeSlotsL(aFreeSlots, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetFreeSlotsL


/**
 *  Get the slots that are free in the requested phonebook.
 *
 *  @param aFreeSlots     On return, an array of slots numbers that are free.
 *  @param aPhonebookUid  Type of ICC phonebook
 *
 *  @capability None
 */
EXPORT_C void RPhoneBookSession::GetFreeSlotsL(RArray<TInt>& aFreeSlots,
											   TUid aPhonebookUid) const
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetFreeSlotsL"));

	aFreeSlots.Reset();

	//
	// Get the number of free slots available...
	//
	TInt  numFreeSlots;
	TPckg<TInt>  numFreeSlotsPtr(numFreeSlots);
	
	TIpcArgs args(aPhonebookUid.iUid,&numFreeSlotsPtr);
	User::LeaveIfError(SendReceive(ESyncGetNumFreeSlots,
	                               TIpcArgs(aPhonebookUid.iUid, &numFreeSlotsPtr)));

	//
	// Now read back the data about each slot...
	//
	HBufC8*  buf = HBufC8::NewLC(sizeof(TInt) * numFreeSlots);
	TPtr8  bufPtr(buf->Des());
	
	User::LeaveIfError(SendReceive(ESyncGetFreeSlots,
	                               TIpcArgs(aPhonebookUid.iUid, &bufPtr)));

	RDesReadStream  readStream(*buf);
	CleanupClosePushL(readStream);
	
	for (TInt slot =0;  slot < numFreeSlots;  slot++)
		{
		User::LeaveIfError(aFreeSlots.Append(readStream.ReadInt32L()));
		}

	CleanupStack::PopAndDestroy(2, buf); // readStream, buf
	} // RPhoneBookSession::GetFreeSlotsL


/**
 *  Get the Contacts UID of a particular slot in the Global/GSM ADN phonebook.
 *
 *  @param aSlot  The slot number on which the request is made.
 *  @param aId    The contact Id of the slot number.
 *
 *  @return KErrNotFound if the UID is not found.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetSlotId(TInt aSlot, TContactItemId& aId) const
	{
	return GetSlotId(aSlot, aId, KUidIccGlobalAdnPhonebook);
	} // RPhoneBookSession::GetSlotId


/**
 *  Get the Contacts UID of a particular slot in the requested phonebook.
 *
 *  @param aSlot          The slot number on which the request is made.
 *  @param aId            The contact Id of the slot number.
 *  @param aPhonebookUid  Type of ICC phonebook
 *
 *  @return KErrNotFound if the UID is not found.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::GetSlotId(TInt aSlot, TContactItemId& aId,
										   TUid aPhonebookUid) const
	{
	LOGCLIENT1(_L8("RPhoneBookSession::GetSlotId()"));
	
	TPckg<TContactItemId>  contactItemPtr(aId);
	
	return SendReceive(ESyncGetSlotId,
	                   TIpcArgs(aPhonebookUid.iUid,aSlot, &contactItemPtr));
	} // RPhoneBookSession::GetSlotId


/**
 *  Set a heap mark in the Phonebook Sync Server and Background Sync Engine
 *  threads.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::__DbgMarkHeap()
	{
	TIpcArgs args(TIpcArgs::ENothing);

	return SendReceive(ESyncDbgMarkHeap, args);
	} // RPhoneBookSession::__DbgMarkHeap


/**
 *  Performs a heap mark check in the Phonebook Sync Server and Background
 *  Sync Engine threads.
 *
 *  @param aCount  The number of heap cells expected to be allocated at
 *                 the current nest level.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::__DbgCheckHeap(TInt aCount)
	{
	TIpcArgs args(aCount);

	return SendReceive(ESyncDbgCheckHeap, args);
	} // RPhoneBookSession::__DbgCheckHeap


/**
 *  Perfom a heap mark end check in the Phonebook Sync Server and Background
 *  Sync Engine threads.
 *
 *  @param aCount  The number of heap cells expected to remain allocated
 *                 at the current nest level.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::__DbgMarkEnd(TInt aCount)
	{
	TIpcArgs args(aCount);

	return SendReceive(ESyncDbgMarkEnd, args);
	} // RPhoneBookSession::__DbgMarkEnd


/**
 *  Set a heap fail next condition in the Phonebook Sync Server and
 *  Background Sync Engine threads.
 *
 *  @param aCount  Determines when the allocation will fail.
 *
 *  @capability None
 */
EXPORT_C TInt RPhoneBookSession::__DbgFailNext(TInt aCount)
	{
	TIpcArgs args(aCount);

	return SendReceive(ESyncDbgFailNext, args);
	} // RPhoneBookSession::__DbgFailNext


/**
 *  This method creates the RPhoneBookSession CPhoneBookSyncData object which is
 *  used to store temporary data required by some asynchonous requests. 
 *
 *  @return Pointer to newly created CPhoneBookSyncData object.
 */
CSyncClientData* CSyncClientData::NewL()
	{
	CSyncClientData*  syncClientData = new (ELeave) CSyncClientData();
	CleanupStack::PushL(syncClientData);
	syncClientData->ConstructL();
	CleanupStack::Pop(syncClientData);

	return syncClientData;
	} // CSyncClientData::NewL


/**
 *  Second phase constructor.
 */
void CSyncClientData::ConstructL()
	{
	iWrtCntEntry = CSyncContactICCEntry::NewL();
	} // CSyncClientData::ConstructL


/**
 *  Default constructor.
 */
CSyncClientData::CSyncClientData()
  : iWrtCntTmpltIdAndBufSizePtr(NULL, 0),
    iWrtCntSlotNumPtr(NULL, 0),
    iWrtCntPhBkIDPtr(NULL, 0),
    iWrtCntEntryBufPtr(NULL, 0)
	{
	// NOP
	} // CSyncClientData::CSyncClientData
	

/**
 *  Destructor.
 */
CSyncClientData::~CSyncClientData()
	{
	delete iWrtCntEntry;
	delete iWrtCntEntryBuf;
	} // CSyncClientData::~CSyncClientData


