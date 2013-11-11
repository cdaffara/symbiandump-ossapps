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
// Implementation of the Background Sync Engine session class. This class is
// responsible for passing the requests that have come from the main
// front-end server's engine handle to the main engine class.
// 
//

/**
 @file
 @internalComponent
*/

#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "SyncEngineServer.h"
#include "SyncEngineSession.h"
#include "phbksynclog.h"


#ifdef _DEBUG
/**
 *  Debug text names to the Phonebook Sync IPC Requests.
 */
const char*  KEnginePhonebookSyncRequestNames[26] = {
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
 *  Second phase construct for sessions. Called by the CServer2 framework
 *  when a session is created (e.g. a connection is made to the server).
 */
void CSyncEngineSession::CreateL()
	{
	LOGENGINE1(_L8("CreateL()"));

	Server().AddSessionL(this);
	} // CSyncEngineSession::CreateL()


/**
 *  Destructor from session classes. When this is called it indicates that
 *  a session is closing its connection with the server.
 */
CSyncEngineSession::~CSyncEngineSession()
	{
	LOGENGINE1(_L8("~CSyncEngineSession()"));

	Server().DropSession(this);
	} // CSyncEngineSession::~CSyncEngineSession()


/**
 *  Handle message requests for this session. Leaving is handled by
 *  CPhoneBookServer::RunError() which reports the error code to the client.
 *
 *  @param aMessage  RMessage2 reference which encapsulates a client request.
 */
void CSyncEngineSession::ServiceL(const RMessage2& aMessage)

	{
	LOGENGINE4(_L8("ServiceL(): Session=0x%08x IPC=%d (%s)"),
	           this, aMessage.Function(),
	           KEnginePhonebookSyncRequestNames[aMessage.Function()]);
	LOGENGINETIMESTAMP();
	
	switch (aMessage.Function())
		{
		case ESyncDoSynchronisation:
			{
			DoSynchronisationL(aMessage);
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

		default:
			{
			CompleteRequest(aMessage, KErrNotSupported);
			}
			break;
		}
	} // CSyncEngineSession::ServiceL()


/**
 *  Completes a client request. This function provides a single point of
 *  message completion which benefits debugging and maintenance.
 *
 *  @param aMessage  The RMessage2 client request.
 *  @param aResult   Result of the request.
 */
void CSyncEngineSession::CompleteRequest(const RMessage2& aMessage,
										TInt aResult) const
	{
	LOGENGINE5(_L8("CSyncEngineSession::CompleteRequest: Session=0x%08x "
					"IPC=%d (%s) Result=%d"), this, aMessage.Function(),
	           KEnginePhonebookSyncRequestNames[aMessage.Function()], aResult);
	LOGENGINETIMESTAMP();

	if (aMessage.IsNull() == EFalse)
		{
	    aMessage.Complete(aResult);
		}
    } // CSyncEngineSession::CompleteRequest()


/**
 *  Requests a synchronisation from the server. This is an asynchronous
 *  request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::DoSynchronisationL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());

	Server().DoSynchronisationL(aMessage, phonebookUid);
	} // CSyncEngineSession::DoSynchronisationL()


/**
 *  Requests the server to delete a contact from the ICC. This is an
 *  asynchronous request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::DeleteCntFromICCL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TContactItemId  contactItemId = aMessage.Int1();
	
	Server().DeleteCntFromICCL(aMessage, phonebookUid, contactItemId);
	} // CSyncEngineSession::DeleteCntFromICCL()


/**
 *  Requests the server to write a contact to the ICC. This is an asynchronous
 *  request.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::WriteCntToICCL(const RMessage2& aMessage)
	{
	//	
	// Extract the request parameters from aMessage. These are the template ID,
	// buffer size, slot number and ICC Entry pointer.
	//
	RPhoneBookSession::TTemplateAndBufferSize  templateIdAndBufferSize;
	TPtr8 iccEntryPtr(NULL, 0);
	TPckg<RPhoneBookSession::TTemplateAndBufferSize>  templateIdAndBufferSizePckg(templateIdAndBufferSize);

	aMessage.ReadL(0, templateIdAndBufferSizePckg); 

	//
	// Pass the request to the server for further processing.
	//
	Server().WriteCntToICCL(aMessage, templateIdAndBufferSize.templateId,
	                        templateIdAndBufferSize.bufferSize);
	} // CSyncEngineSession::WriteCntToICCL()


/**
 *  Requests the server to cancel a previous synchronisation request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::DoSynchronisationCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().DoSynchronisationCancelL(phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CSyncEngineSession::DoSynchronisationCancelL()


/**
 *  Requests the server to cancel a previous delete contact request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::DeleteCntFromICCCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().DeleteCntFromICCCancelL(phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CSyncEngineSession::DeleteCntFromICCCancelL()


/**
 *  Requests the server to cancel a previous write contact request.
 *  This is a synchronous request which will be completed when the procedure
 *  returns.
 *
 *  @param aMessage  RMessage2 client request.
 */
void CSyncEngineSession::WriteCntToICCCancelL(const RMessage2& aMessage)
	{
	TUid  phonebookUid = TUid::Uid(aMessage.Int0());
	TInt  result;
	
	result = Server().WriteCntToICCCancelL(phonebookUid);

	//
	// Complete the request...
	//
	CompleteRequest(aMessage, result);
	} // CSyncEngineSession::WriteCntToICCCancelL()
	

