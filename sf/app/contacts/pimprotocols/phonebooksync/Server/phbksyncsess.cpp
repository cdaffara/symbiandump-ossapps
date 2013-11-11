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
// Contains method implementations for CPhonebookSession.
// 
//

/**
 @file
 @internalComponent
*/

#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"
#include "phbksyncsess.h"
#include "phbksynclog.h"


#ifdef _DEBUG
/**
 *  Debug text names for the Phonebook Sync IPC Requests.
 */
const char*  KPhonebookSyncRequestNames[26] = {
		"ESyncDoSynchronisation", "ESyncGetContactFormat", "ESyncGetSyncMode",
		"ESyncSetSyncMode", "ESyncDeleteCntFromICC", "ESyncWriteCntToICC",
		"ESyncValidateContact", "ESyncUpdateLookupTable",
		"ESyncGetPhoneBookId", "ESyncGetCacheState", "ESyncDbgMarkHeap",
		"ESyncDbgCheckHeap", "ESyncDbgMarkEnd", "ESyncDbgFailNext",
		"ESyncDoSynchronisationCancel", "ESyncDeleteCntFromICCCancel",
		"ESyncWriteCntToICCCancel", "ESyncNotifyCacheStateChange",
		"ESyncNotifyCacheStateChangeCancel", "ESyncGetLastSyncError",
		"ESyncGetNumSlots", "ESyncGetNumFreeSlots", "ESyncGetFreeSlots",
		"ESyncGetSlotId", "ESyncFlushInterfaceQueue", "ESyncShutdownServer"};
#endif


/**
 *  Second phase constructor for sessions. Called by the CServer2 framework
 *  when a session is created (e.g. a connection is made to the server).
 */
void CPhoneBookSession::CreateL()
	{
	LOGSESSION1(_L8("CreateL()"));

	Server().AddSessionL(this);
	} // CPhoneBookSession::CreateL()


/**
 *  Destructor for session classes. When this is called it indicates that
 *  a session is closing its connection with the server.
 */
CPhoneBookSession::~CPhoneBookSession()
	{
	LOGSESSION1(_L8("~CPhoneBookSession()"));

	Server().DropSession(this);
	} // CPhoneBookSession::~CPhoneBookSession()


/**
 *  Handle message requests for this session. Leaving is handled by
 *  CPhoneBookServer::RunError() which reports the error code to the client.
 *
 *  @param aMessage  RMessage2 reference which encapsulates a client request.
 */
void CPhoneBookSession::ServiceL(const RMessage2& aMessage)

	{
	LOGSESSION4(_L8("ServiceL(): Session=0x%08x IPC=%d (%s)"),
	           this, aMessage.Function(),
	           KPhonebookSyncRequestNames[aMessage.Function()]);
	LOGSESSIONTIMESTAMP();
	
	switch (aMessage.Function())
		{
		case ESyncDoSynchronisation:
			{
			DoSynchronisationL(aMessage);
			}
			break;

		case ESyncGetContactFormat:
			{
			GetContactFormatL(aMessage);
			}
			break;

		case ESyncGetSyncMode:
			{
			GetSyncModeL(aMessage);
			}
			break;

		case ESyncSetSyncMode:
			{
			SetSyncModeL(aMessage);
			}
			break;

		case ESyncDeleteCntFromICC:
			{
			DeleteCntFromICCL(aMessage);
			}
			break;

		case ESyncWriteCntToICC:
			{
			WriteCntToICCL(aMessage);
			}
			break;

		case ESyncValidateContact:
			{
			ValidateContactL(aMessage);
			}
			break;

		case ESyncUpdateLookupTable:
			{
			UpdateLookupTableL(aMessage);
			}
			break;

		case ESyncGetPhoneBookId:
			{
			GetPhoneBookIdL(aMessage);
			}
			break;

		case ESyncGetCacheState:
			{
			GetCacheStateL(aMessage);
			}
			break;

		case ESyncDbgMarkHeap:
			{
			DbgMarkHeapL(aMessage);
			}
			break;

		case ESyncDbgCheckHeap:
			{
			DbgCheckHeapL(aMessage);
			}
			break;

		case ESyncDbgMarkEnd:
			{
			DbgMarkEndL(aMessage);
			}
			break;

		case ESyncDbgFailNext:
			{
			DbgFailNextL(aMessage);
			}
			break;

		case ESyncDoSynchronisationCancel:
			{
			DoSynchronisationCancelL(aMessage);
			}
			break;

		case ESyncDeleteCntFromICCCancel:
			{
			DeleteCntFromICCCancelL(aMessage);
			}
			break;

		case ESyncWriteCntToICCCancel:
			{
			WriteCntToICCCancelL(aMessage);
			}
			break;

		case ESyncNotifyCacheStateChange:
			{
			NotifyCacheStateChangeL(aMessage);
			}
			break;

		case ESyncNotifyCacheStateChangeCancel:
			{
			NotifyCacheStateChangeCancelL(aMessage);
			}
			break;

		case ESyncGetLastSyncError:
			{
			GetLastSyncErrorL(aMessage);
			}
			break;

		case ESyncGetNumSlots:
			{
			GetNumSlotsL(aMessage);
			}
			break;

		case ESyncGetNumFreeSlots:
			{
			GetNumFreeSlotsL(aMessage);
			}
			break;

		case ESyncGetFreeSlots:
			{
			GetFreeSlotsL(aMessage);
			}
			break;

		case ESyncGetSlotId:
			{
			GetSlotIdL(aMessage);
			}
			break;

		case ESyncFlushInterfaceQueue:
			{
			CompleteRequest(aMessage, KErrNone);
			}
			break;

		case ESyncShutdownServer:
			{
			ShutdownServerL(aMessage);
			}
			break;

		default:
			{
			aMessage.Panic(PHBKSYNC_SERVER_NAME,
						   EPhBkSyncPanicIllegalFunction);
			}
			break;
		}
	} // CPhoneBookSession::ServiceL()


/**
 *  Completes a client request. This function provides a single point of
 *  message completion which benefits debugging and maintenance.
 *
 *  @param aMessage  The RMessage2 client request.
 *  @param aResult   Result of the request.
 */
void CPhoneBookSession::CompleteRequest(const RMessage2& aMessage,
										TInt aResult) const
	{
	LOGSESSION5(_L8("CompleteRequest(): Session=0x%08x "
					"IPC=%d (%s) Result=%d"), this, aMessage.Function(),
	           KPhonebookSyncRequestNames[aMessage.Function()], aResult);
	LOGSESSIONTIMESTAMP();

	if (aMessage.IsNull() == EFalse)
		{
	    aMessage.Complete(aResult);
		}
    } // CPhoneBookSession::CompleteRequest()


/**
 *  Requests a synchronisation from the server. This is an asynchronous
 *  request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DoSynchronisationL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	Server().DoSynchronisationL(*this, aMessage, phonebookUid);
	} // CPhoneBookSession::DoSynchronisationL()


/**
 *  Requests the supported phonebook field formats from the server. This is a
 *  synchronous request which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetContactFormatL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	//
	// Get the contact format from the server...
	//
	RPhoneBookSession::TContactFieldsV3  contactFieldsV3;
	TInt  result;
	
	result = Server().GetContactFormatL(phonebookUid, contactFieldsV3);

	//
	// If the contact format was retrieved from the server, then write it to
	// the client.
	//
	if (result == KErrNone)
		{
		RPhoneBookSession::TContactFieldsV1::TContactFieldsExtensionId  extensionId;
		
		extensionId = static_cast<RPhoneBookSession::TContactFieldsV1::TContactFieldsExtensionId>(aMessage.Int1());
		if (extensionId == RPhoneBookSession::TContactFieldsV1::KPhBkSyncTContactFieldsV1)
			{
			RPhoneBookSession::TContactFieldsV1  contactFieldsV1;
			RPhoneBookSession::TContactFieldsV1Pckg  contactFieldsV1Pckg(contactFieldsV1);
			
			contactFieldsV1 = static_cast<RPhoneBookSession::TContactFieldsV1>(contactFieldsV3);

			aMessage.WriteL(2, contactFieldsV1Pckg);
			}
		else if (extensionId == RPhoneBookSession::TContactFieldsV1::KPhBkSyncTContactFieldsV2)
			{
			RPhoneBookSession::TContactFieldsV2  contactFieldsV2;
			RPhoneBookSession::TContactFieldsV2Pckg  contactFieldsV2Pckg(contactFieldsV2);
			
			contactFieldsV2 = static_cast<RPhoneBookSession::TContactFieldsV2>(contactFieldsV3);

			aMessage.WriteL(2, contactFieldsV2Pckg);
			}
		else
			{
			RPhoneBookSession::TContactFieldsV3Pckg  contactFieldsV3Pckg(contactFieldsV3);

			aMessage.WriteL(2, contactFieldsV3Pckg);
			}
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetContactFormatL()


/**
 *  Requests the current setting for the Synchronisation Mode from the server.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetSyncModeL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	//
	// Get the sync mode from the server...
	//
	RPhoneBookSession::TPhonebookSyncMode  syncMode;
	TInt  result;
	
	result = Server().GetSyncModeL(phonebookUid, syncMode);

	//
	// If the sync mode was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<RPhoneBookSession::TPhonebookSyncMode>  syncModePckg(syncMode);

		aMessage.WriteL(1, syncModePckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetSyncModeL()


/**
 *  Requests the server to set the sync mode. This is a synchronous request
 *  which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::SetSyncModeL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	//
	// Set the sync mode via the server...
	//
	RPhoneBookSession::TPhonebookSyncMode  syncMode = static_cast<RPhoneBookSession::TPhonebookSyncMode>(aMessage.Int1());
	TInt  result;
	
	result = Server().SetSyncModeL(phonebookUid, syncMode);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::SetSyncModeL()


/**
 *  Requests the server to delete a contact from the ICC. This is an
 *  asynchronous request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DeleteCntFromICCL(const RMessage2& aMessage)
	{
	TContactItemId  contactItemId = aMessage.Int0();
	
	Server().DeleteCntFromICCL(*this, aMessage, contactItemId);
	} // CPhoneBookSession::DeleteCntFromICCL()


/**
 *  Requests the server to write a contact to the ICC. This is an asynchronous
 *  request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::WriteCntToICCL(const RMessage2& aMessage)
	{
	//	
	// Extract the request parameters from aMessage. These are the template ID,
	// buffer size, slot number and ICC Entry pointer.
	//
	RPhoneBookSession::TTemplateAndBufferSize  templateIdAndBufferSize;
	TInt  slotNum;
	TPtr8 iccEntryPtr(NULL, 0);
	TPckg<RPhoneBookSession::TTemplateAndBufferSize>  templateIdAndBufferSizePckg(templateIdAndBufferSize);
	TPckg<TInt>  slotNumPckg(slotNum);

	aMessage.ReadL(0, templateIdAndBufferSizePckg); 
	aMessage.ReadL(1, slotNumPckg);

	//
	// Pass the request to the server for further processing.
	//
	Server().WriteCntToICCL(*this, aMessage, slotNum,
	                        templateIdAndBufferSize.templateId);
	} // CPhoneBookSession::WriteCntToICCL()


/**
 *  Requests the server to validate a contact. This is an asynchronous
 *  request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::ValidateContactL(const RMessage2& aMessage)
	{
	TContactItemId  contactItemId = aMessage.Int0();
	MContactSynchroniser::TValidateOperation  validateOp = static_cast<MContactSynchroniser::TValidateOperation>(aMessage.Int1());
	TInt  result;
	
	result = Server().ValidateContactL(contactItemId, validateOp);
	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::ValidateContactL()


/**
 *  Requests the server to update its Look Up Table with new contact
 *  information. This is a synchronous request which will be completed when
 *  the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::UpdateLookupTableL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TContactItemId  contactItemId = aMessage.Int1();
	TInt  index = aMessage.Int2();
	TInt  result;
	
	result = Server().UpdateLookupTableL(phonebookUid, contactItemId, index);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::UpdateLookupTableL()


/**
 *  Requests the Phonebook template or group ID from the server for a
 *  particular phonebook. This is a synchronous request which will be
 *  completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetPhoneBookIdL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	RPhoneBookSession::TSyncIdType  syncIdType = static_cast<RPhoneBookSession::TSyncIdType>(aMessage.Int1());
	TContactItemId  contactItemId;
	TInt  result;
	
	result = Server().GetPhoneBookIdL(phonebookUid, syncIdType, contactItemId);

	//
	// If the phonebook ID was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<TContactItemId>  contactItemIdPckg(contactItemId);

		aMessage.WriteL(2, contactItemIdPckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetPhoneBookIdL()


/**
 *  Requests the current cache state for a phonebook from the server.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetCacheStateL(const RMessage2& aMessage)
	{
	TUid phonebookUid = TUid::Uid(aMessage.Int0());
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = Server().GetCacheStateL(phonebookUid, syncState);

	//
	// If the phonebook ID was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckgC<RPhoneBookSession::TSyncState>  syncStatePckg(syncState);
		
		aMessage.WriteL(1, syncStatePckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetCacheStateL()


/**
 *  Requests the server to mark the start of checking the server's and
 *  engine's heap. This function only works in debug releases and is a
 *  synchronous request which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DbgMarkHeapL(const RMessage2& aMessage)
	{
	TInt  count = aMessage.Int0();
	TInt  result;
	
	result = Server().DbgMarkHeap();

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DbgMarkHeapL()


/**
 *  Requests the server to check that the number of allocated cells at the
 *  current nested level on the server's and engine's heaps are the same as
 *  the specified value. This function only works for debug builds and is a
 *  synchronous request which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DbgCheckHeapL(const RMessage2& aMessage)
	{
	TInt  count = aMessage.Int0();
	TInt  result;
	
	result = Server().DbgCheckHeap(count);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DbgCheckHeapL()


/**
 *  Requests the server to mark the end of checking the server's and
 *  engines's heap. This function must match an earlier call to
 *  DbgMarkHeap() and only functions on debug releases. This is a
 *  synchronous request which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DbgMarkEndL(const RMessage2& aMessage)
	{
	TInt  count = aMessage.Int0();
	TInt  result;
	
	result = Server().DbgMarkEnd(count);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DbgMarkEndL()


/**
 *  Simulates heap allocation failure for the sever and engine.
 *  The failure occurs on the next call to new or any of the functions which 
 *  allocate memory from the heap. This is defined only for debug builds and
 *  is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DbgFailNextL(const RMessage2& aMessage)
	{
	TInt  count = aMessage.Int0();
	TInt  result;
	
	result = Server().DbgFailNext(count);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DbgFailNextL()


/**
 *  Requests the server to cancel a previous synchronisation request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DoSynchronisationCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().DoSynchronisationCancelL(*this, phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DoSynchronisationCancelL()


/**
 *  Requests the server to cancel a previous delete contact request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::DeleteCntFromICCCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().DeleteCntFromICCCancelL(*this, phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::DeleteCntFromICCCancelL()


/**
 *  Requests the server to cancel a previous write contact request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::WriteCntToICCCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().WriteCntToICCCancelL(*this, phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::WriteCntToICCCancelL()
	

/**
 *  Requests the server to post a notification of cache state change.
 *  This is an asynchronous request which will be completed when the
 *  cache state changes or the request is canceled.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::NotifyCacheStateChangeL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	Server().NotifyCacheStateChangeL(*this, aMessage, phonebookUid);
	} // CPhoneBookSession::NotifyCacheStateChangeL()
	

/**
 *  Requests the server to cancel a previous notify cache state change request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::NotifyCacheStateChangeCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().NotifyCacheStateChangeCancelL(*this, phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::NotifyCacheStateChangeCancelL()
	

/**
 *  Requests the last sync error for a phonebook from the server.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetLastSyncErrorL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  lastSyncError;
	TInt  result;
	
	result = Server().GetLastSyncErrorL(phonebookUid, lastSyncError);

	//
	// If the last sync error was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<TInt> lastSyncErrorPckg(lastSyncError);
		
		aMessage.WriteL(1, lastSyncErrorPckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetLastSyncErrorL()
	

/**
 *  Requests the current number of slots in a phonebook from the server.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetNumSlotsL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  numSlots;
	TInt  result;
	
	result = Server().GetNumSlotsL(phonebookUid, numSlots);

	//
	// If the number of slots was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<TInt> numSlotsPckg(numSlots);
		
		aMessage.WriteL(1, numSlotsPckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetNumSlotsL()
	

/**
 *  Requests the current number of free slots in a phonebook from the server.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetNumFreeSlotsL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  numFreeSlots, result;
	
	result = Server().GetNumFreeSlotsL(phonebookUid, numFreeSlots);

	//
	// If the number of free slots was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<TInt> numFreeSlotsPckg(numFreeSlots);
		
		aMessage.WriteL(1, numFreeSlotsPckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetNumFreeSlotsL()
	

/**
 *  Requests a list of the current free slots from the server. This is a
 *  synchronous request which will be completed when the procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetFreeSlotsL(const RMessage2& aMessage)
	{
	TUid phonebookUid = TUid::Uid(aMessage.Int0());

	//
	// Get the list of free slots from the server...
	//
	RArray<TInt>  freeSlots;
	CleanupClosePushL(freeSlots);
	TInt result;

	result = Server().GetFreeSlotsL(phonebookUid, freeSlots);

	//
	// If the number of free slots was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TInt  numSlots = freeSlots.Count();
		HBufC8*  transferBuf = HBufC8::NewLC(sizeof(TInt) * numSlots);
		TPtr8  transferBufPtr(transferBuf->Des());

		RDesWriteStream  writeStream(transferBufPtr);
		CleanupClosePushL(writeStream);

		for (TInt slot = 0;  slot < numSlots;  slot++)
			{
			writeStream.WriteInt32L(freeSlots[slot]);
			}

		transferBufPtr.SetLength(sizeof(TInt) * numSlots);
		aMessage.WriteL(1, *transferBuf);

		CleanupStack::PopAndDestroy(2, transferBuf); // writeStream & transferBuf
		}

	CleanupStack::PopAndDestroy(&freeSlots);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetFreeSlotsL()
	

/**
 *  Requests the contact ID of the slot in a particular phonebook from the
 *  server. This is a synchronous request which will be completed when the
 *  procedure returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::GetSlotIdL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  slotNum = aMessage.Int1();
	TContactItemId  contactItemId;
	TInt  result;
	
	result = Server().GetSlotIdL(phonebookUid, slotNum, contactItemId);

	//
	// If the slot ID was retrieved then write it back to the client.
	//
	if (result == KErrNone)
		{
		TPckg<TContactItemId>  contactItemIdPckg(contactItemId);
		
		aMessage.WriteL(2, contactItemIdPckg);
		}

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::GetSlotIdL()
	

/**
 *  Requests the server to shut down when it no longer has any connected
 *  sessions. This procedure is only premitted in debug builds for security
 *  reasons (e.g. to prevent a denial of service attack) and is provided
 *  for testing purposes. This is a synchronous request which will be
 *  completed when the procedure returns. The server will shutdown when the
 *  last session disconnects.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CPhoneBookSession::ShutdownServerL(const RMessage2& aMessage)
	{
	TBool  conditional = aMessage.Int0();
	TInt  result;
	
	result = Server().ShutdownServer(conditional);
	
	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CPhoneBookSession::ShutdownServerL()

