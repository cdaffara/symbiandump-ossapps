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
// Implementation of the Background Sync Engine client interface.
// 
//

/**
 @file
 @internalComponent
*/

#include "SyncEngineClient.h"
#include "common.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "SyncEngineServer.h"
#include "phbksynclog.h"


/**
 *  Size of the Background Sync Engine thread stack size.
 */
const TInt KSyncEngineStackSize(0x3000);


/**
 *  Performs all server initialisation, in particular creation of the
 *  scheduler object, the enigne server and then runs the scheduler
 *  (which starts the engine server).
 *
 *  @param aPhoneBookManager  Reference to the phonebook manager class used
 *                            by the front-end server.
 */
static void RunEngineL(CPhoneBookManager& aPhoneBookManager)
	{
	LOGENGINE1(_L8("RunEngineL()"));

	//
	// Create a new Sync Engine Server...
	//
	CSyncEngineServer*  engine = CSyncEngineServer::NewL(aPhoneBookManager);
	CleanupStack::PushL(engine);
	
	//
	// Initialisation complete, now signal the client thread...
	//
	LOGENGINE1(_L8("RunEngineL(): Meeting Rendezvous..."));
	RThread::Rendezvous(KErrNone);

	//
	// Run the server...
	//
	LOGENGINE1(_L8("RunEngineL(): Starting server..."));
	CSyncEngineScheduler::Start();
	
	CleanupStack::PopAndDestroy(engine);
	} // RunEngineL


/**
 *  Thread function for starting the engine. This function basically just
 *  installs an Active Scheduler, a clean up stack and then calls the
 *  RunEngineL() leaving function.
 *
 *  @param aPtr  TAny pointer to the phonebook manager class used
 *               by the front-end server.
 */
static TInt RunEngineThread(TAny* aPtr)
	{
	CPhoneBookManager*  phonebookManager = static_cast<CPhoneBookManager*>(aPtr);

	LOGENGINE1(_L8("RunEngineThread()"));

	//
	// Create an Active Scheduler...
	//
	CActiveScheduler*  scheduler = new CActiveScheduler();
	if (scheduler == NULL)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);

	//
	// Create a clean up stack...
	//
	CTrapCleanup*  cleanup = CTrapCleanup::New();
	
	TRAPD(leaveCode, RunEngineL(*phonebookManager));

	delete cleanup;	
	delete scheduler;

	return leaveCode;
	} // RunEngineThread


/**
 *  Start the engine thread.
 */
TInt RSyncEngineSession::StartPhBkSyncEngine(CPhoneBookManager* aPhoneBookManager)
	{
	LOGENGINE1(_L8("RSyncEngineSession::StartPhBkSyncEngine()"));

	//
	// Create the Sync Engine Thread (using the same heap as this thread) at
	// EPriorityAbsoluteBackground priority, ...
	//
	TInt  ret;
	
	ret = iEngineThread.Create(PHBKSYNC_ENGINE_NAME, RunEngineThread,
	                           KSyncEngineStackSize, NULL, aPhoneBookManager);
	if (ret != KErrNone)
		{
		return ret;
		}

	iEngineThread.SetPriority(EPriorityAbsoluteBackground);
		
	//
	// Logon to the engine thread. This will be used later to ensure the
	// thread shuts down when closed.
	//
	iEngineThread.Logon(iEngineThreadStatus);
	if (iEngineThreadStatus != KRequestPending)
		{
		iEngineThread.Kill(0);
		iEngineThread.Close();

		User::WaitForRequest(iEngineThreadStatus);

		return iEngineThreadStatus.Int();
		}

	//
	// Rendezvous with the engine or abort the engine start...
	//
	TRequestStatus  rendezvousStatus;

	iEngineThread.Rendezvous(rendezvousStatus);
	if (rendezvousStatus != KRequestPending)
		{
		iEngineThread.Kill(0);
		iEngineThread.Close();

		User::WaitForRequest(rendezvousStatus);
		User::WaitForRequest(iEngineThreadStatus);

		return rendezvousStatus.Int();
		}

	//
	// The thread is ready to go...
	//
	iEngineThread.Resume();
	User::WaitForRequest(rendezvousStatus);
	if (rendezvousStatus != KErrNone)
		{
		iEngineThread.Kill(0);
		iEngineThread.Close();

		User::WaitForRequest(iEngineThreadStatus);

		return rendezvousStatus.Int();
		}

	return KErrNone;
	} // RSyncEngineSession::StartPhBkSyncEngine


/**
 *  Standard constructor.
 */
RSyncEngineSession::RSyncEngineSession()
	{
	iEngineThread.SetHandle(0);
	} // RSyncEngineSession::RSyncEngineSession


/**
 *  Connects the client to the Phonebook Synchronizer Server. 
 *
 *  This must be used before any of the other methods listed in 
 *  this API section. The first client to call this method will 
 *  cause the initiation of the Phonebook Synchronizer Server 
 *  within its own executable process.
 *
 *  @param aPhoneBookManager  Pointer to the phonebook manager class used
 *                            by the front-end server.
 *
 *  @return KErrNone if successful, a system-wide error code if not. 
 *
 *  @capability None
 */
TInt RSyncEngineSession::Connect(CPhoneBookManager* aPhoneBookManager)
	{
	LOGENGINE1(_L8("RSyncEngineSession::Connect()"));
	
	//
	// Create a session with the server, but if it doesn't exist then start it and
	// then create a session.
	//
	TInt  result = CreateSession(PHBKSYNC_ENGINE_NAME, TVersion());
	if (result == KErrNotFound  ||  result == KErrServerTerminated)
		{
		result = StartPhBkSyncEngine(aPhoneBookManager);
		
		if(result == KErrNone)
			{
			result = CreateSession(PHBKSYNC_ENGINE_NAME, TVersion());
			}
		}	
		
	//
	// If the creation of the session fails then close it to clean up
	// any allocated member data or running threads...
	//
	if (result != KErrNone)
		{
		Close();
		}

	return result;
	} // RSyncEngineSession::Connect


/**
 *  Closes the client's session with the Phonebook Synchronizer Server. 
 *
 *  @capability None
 */
void RSyncEngineSession::Close()
	{
	LOGENGINE1(_L8("RSyncEngineSession::Close()"));

	RSessionBase::Close();

	//
	// Ensure the engine thread shuts down before we return...
	//
	if (iEngineThread.Handle() != 0)
		{
		User::WaitForRequest(iEngineThreadStatus);
		iEngineThread.Close();
		}
	} // RSyncEngineSession::Close


/**
 *  Instructs the engine to start a manual synchronisation of the ICC
 *  phonebook specified by the aPhonebookUid parameter.
 *
 *  @param aReqStatus     On completion, KErrNone if successful, a
 *                        system-wide error code if not. 
 *  @param aPhonebookUid  TUid of the ICC phonebook to be synchronised.
 *
 *  @capability ReadUserData
 *  @capability WriteUserData
 */
void RSyncEngineSession::DoSynchronisation(TRequestStatus& aReqStatus,
										   TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("RSyncEngineSession::DoSynchronisation(): 0x%08x"),
			   aPhonebookUid);
	
	SendReceive(ESyncDoSynchronisation, TIpcArgs(aPhonebookUid.iUid), aReqStatus);
	} // RSyncEngineSession::DoSynchronisation


/**
 *  Cancels an existing Syncronisation request.
 *
 *  @param aPhonebookUid  TUid of the ICC phonebook to cancel.
 *
 *  @capability ReadUserData
 *  @capability WriteUserData
 */
void RSyncEngineSession::DoSynchronisationCancel(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("RSyncEngineSession::DoSynchronisationCancel(): 0x%08x"),
			   aPhonebookUid);

	SendReceive(ESyncDoSynchronisationCancel, TIpcArgs(aPhonebookUid.iUid));
	} // RSyncEngineSession::DoSynchronisationCancel


/**
 *  Write the contact item to an ICC phonebook. The parameters for this
 *  function are left in the original packaged state from the front-end
 *  server's client.
 *
 *  @param aReqStatus                   On completion, KErrNone if successful,
 *										a system-wide error code if not.
 *  @param aTemplateIdAndBufferSizePtr  Packaged pointer to the template and
 *                                      buffer size.
 *  @param aSlotNumberPtr  				Packaged pointer to the slot number.
 *  @param aPhoneBookUidPtr    			Packaged pointer to the phonebook UID.
 *  @param aICCEntryBufPtr    			Packaged pointer to the ICC Entry.
 *
 *  @capability WriteUserData
 */
void RSyncEngineSession::WriteContact(TRequestStatus& aReqStatus,
									  TPtrC8& aTemplateIdAndBufferSizePtr,
									  TPtr8& aSlotNumberPtr,
									  TPtr8& aPhoneBookUidPtr,
									  TPtr8& aICCEntryBufPtr)
	{
	LOGENGINE1(_L8("RSyncEngineSession::WriteContact()"));

	TIpcArgs  args(&aTemplateIdAndBufferSizePtr, &aSlotNumberPtr,
	               &aPhoneBookUidPtr, &aICCEntryBufPtr);

	SendReceive(ESyncWriteCntToICC, args, aReqStatus);
	} // RSyncEngineSession::WriteContact


/**
 *  Cancels an existing WriteContact request.
 *
 *  @param aPhonebookUid  TUid of the ICC phonebook to cancel.
 *
 *  @capability WriteUserData
 */
void RSyncEngineSession::WriteContactCancel(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("RSyncEngineSession::WriteContactCancel(): 0x%08x"),
			   aPhonebookUid);

	SendReceive(ESyncWriteCntToICCCancel, TIpcArgs(aPhonebookUid.iUid));
	} // RSyncEngineSession::WriteContactCancel


/**
 *  Deletes a contact item from the ICC.
 *
 *  @param aReqStatus     On completion, KErrNone if successful, a system-wide
 *					      error code if not.
 *  @param aPhonebookUid  UID of the ICC phonebook to delete from.
 *  @param aId            The contact item to delete. 
 *
 *  @capability WriteUserData
 */
void RSyncEngineSession::DeleteContact(TRequestStatus& aReqStatus,
									   TUid aPhonebookUid,
									   TContactItemId aId)
	{
	LOGENGINE3(_L8("RSyncEngineSession::DeleteContact(): 0x%08x"),
			   aPhonebookUid, aId);

	SendReceive(ESyncDeleteCntFromICC, TIpcArgs(aPhonebookUid.iUid, aId),
				aReqStatus);
	} // RSyncEngineSession::DeleteContact


/**
 *  Cancels an existing WriteContact request.
 *
 *  @param aPhonebookUid  TUid of the ICC phonebook to cancel.
 *
 *  @capability WriteUserData
 */
void RSyncEngineSession::DeleteContactCancel(TUid aPhonebookUid)
	{
	LOGENGINE2(_L8("RSyncEngineSession::DeleteContactCancel(): 0x%08x"),
			   aPhonebookUid);

	SendReceive(ESyncDeleteCntFromICCCancel, TIpcArgs(aPhonebookUid.iUid));
	} // RSyncEngineSession::DeleteContactCancel

