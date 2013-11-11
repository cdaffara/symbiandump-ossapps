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
// Implementation of the CPhoneBookServer class which is the front-end server,
// responsible for processing all short and quick requests, and for passing
// longer requests to the engine.
// 
//

/**
 @file
 @internalComponent
*/

#include <connect/sbdefs.h>
#include <commsdattypesv1_1.h>
#include <cdblen.h>

#include "common.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksynclog.h"
#include "phbksyncsvr.h"
#include "phbksyncsess.h"
#include "SyncEngineRequest.h"
#include "ActiveNotifications.h"


using namespace CommsDat;


/**
 * Number of retries for connecting to Etel.
 */
const TInt KMaxConnectToEtelRetries = 3;


//
//  Definition of iPolicy dictating the capability checking for phbksyncsvr...
//
const TUint iRangeCount = 10;

const TInt CPhoneBookServer::iRanges[iRangeCount] = 
	{
	0,		//range is 0
	1,		//range is 1-3 inclusive
	4,		//range is 4-5 inclusive
	6,		//range is 6
	7,		//range is 7-13 inclusive
	14,		//range is 14
	15,		//range is 15-16 inclusive
	17,		//range is 17-24 inclusive
	25,		//range is 25 inclusive
	26,		//range is 26-KMaxTInt inclusive
	};

const TUint8 CPhoneBookServer::iElementsIndex[iRangeCount] = 
	{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	CPolicyServer::ENotSupported,
	};

const CPolicyServer::TPolicyElement CPhoneBookServer::iElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 0:  range 0 - 0
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 1:  range 1 - 3
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 2:  range 4 - 5
	{ _INIT_SECURITY_POLICY_C1( ECapabilityReadUserData), CPolicyServer::EFailClient},	// policy 3:  range 6 - 6
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 4:  range 7 - 13
	{ _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 5:  range 14 - 14
	{ _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData), CPolicyServer::EFailClient},	// policy 6:  range 15 - 16
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient},	// policy 7:  range 17 - 24
#ifdef _DEBUG
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient}, // policy 8: range 25
#else
	{ _INIT_SECURITY_POLICY_FAIL}, // policy 8: range 25
#endif
	};

const CPolicyServer::TPolicy CPhoneBookServer::iPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	iRangeCount,
	iRanges,
	iElementsIndex,
	iElements
	};


/**
 *  Static factory method used to create a CPhoneBookServer object.
 *
 *  @return  Pointer to the created CPhoneBookServer object, or NULL.
 */
CPhoneBookServer* CPhoneBookServer::NewL()
	{
	LOGSERVER1(_L8("NewL()"));
	CPhoneBookServer* self = new (ELeave) CPhoneBookServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CPhoneBookServer::NewL


/**
 *  Standard constructor.
 */
CPhoneBookServer::CPhoneBookServer()
	: CPolicyServer(EPriorityNormal, iPolicy, ESharableSessions), 
	iServerConfigLevel(EServerConfigurationNone),
	iICCCaps(RMobilePhone::KCapsSimAccessSupported), 
	iPhonebookManager(NULL),
	iShouldShutdownServer(EFalse),
	iShouldShutdownUnconditionally(EFalse)
	{
	__DECLARE_NAME(_L("CPhoneBookServer"));
	} // CPhoneBookServer::CPhoneBookServer


/**
 *  Second phase constructor. Ensures the server is created and ready to run.
 */
void CPhoneBookServer::ConstructL()
	{
	StartL(PHBKSYNC_SERVER_NAME);
	} // CPhoneBookServer::ConstructL


/**
 *  Destructor.
 */
CPhoneBookServer::~CPhoneBookServer()
	{
	//
	// Stop the server from starting if it has initiated a startup.
	//
	delete iPhoneBookSyncStarter;
	iPhoneBookSyncStarter = NULL;
	
	//
	// Unconfigure the server if needed...
	//
	TRAP_IGNORE(ConfigureServerL(EServerConfigurationNone));

	iCacheStateNotificationArray.Reset();
	iSyncEngineRequestArray.ResetAndDestroy();
	iSessionArray.Reset();
	} // CPhoneBookServer::~CPhoneBookServer


/**
 *  Attempts to configure or unconfigure the server as specified. If this
 *  function leaves, the configuration state will be invalid. This function
 *  is therefore a private function and callers should use ConfigureServerL()
 *  instead which protects against such problems.
 *  
 *  @param aLevel  The desired configuration level to initialise the server to.
 */
void CPhoneBookServer::AttemptToConfigureServerL(TServerConfiguration aLevel)
	{
	LOGSERVER2(_L8("AttemptToConfigureServerL() to level %d"), aLevel);

	//
	// Are we configuring or unconfiguring?
	//
	if (iServerConfigLevel < aLevel)
		{
		//
		// Configure from level 'None' to 'Idle'...
		//
		if (iServerConfigLevel == EServerConfigurationNone)
			{
			iServerConfigLevel = EServerConfigurationConfiguringToIdle;
			LOGSERVER1(_L8("Configuration changed from 'None' to 'ConfiguringToIdle'."));

			iNotifyBackupAndRestore = CNotifyBackupAndRestore::NewL(*this);
			User::LeaveIfError(iNotifyBackupAndRestore->Start());

			iServerConfigLevel = EServerConfigurationIdle;
			LOGSERVER1(_L8("Configuration changed from 'ConfiguringToIdle' to 'Idle'."));
			}

		//
		// Configure from level 'Idle' to 'Full'...
		//
		if (iServerConfigLevel < aLevel  &&
			iServerConfigLevel == EServerConfigurationIdle)
			{
			iServerConfigLevel = EServerConfigurationConfiguringToFull;
			LOGSERVER1(_L8("Configuration changed from 'Idle' to 'ConfiguringToFull'."));

			//
			// Connect to ETel...
			//
			TInt  etelErr(KErrNone), retryCount;

   			for (retryCount = 0;  retryCount < KMaxConnectToEtelRetries;  retryCount++)
   				{
   				TRAP(etelErr, ConnectToEtelL());	//This can leave due to a denied access 										
													//to CommDb or Etel
   				if (etelErr == KErrNone)
					{
   					break;
					}

				if (retryCount < KMaxConnectToEtelRetries - 1)
					{
   					User::After(1000000);
					}
   				}
   			User::LeaveIfError(etelErr);

			//
			// Setup the phonebook parameters and create the Phonebook Manager.
			// This will also read the settings from the INI file.
			//
			if (iPhone.GetIccAccessCaps(iICCCaps) != KErrNone)
				{
				iICCCaps = RMobilePhone::KCapsSimAccessSupported;
				}

			iPhonebookManager = CPhoneBookManager::NewL(iICCCaps);

			//
			// Initialise and start the Active Objects...
			//
			iGetPhoneStoreInfo = CGetPhoneStoreInfo::NewL(*this,
														  *iPhonebookManager,
														  iPhone);
			User::LeaveIfError(iGetPhoneStoreInfo->Start());

			iNotifyLockInfoChange = CNotifyLockInfoChange::NewL(*this, iPhone);
			User::LeaveIfError(iNotifyLockInfoChange->Start());

			iGetSecurityEvent = CNotifySecurityEvent::NewL(*this, iPhone);
			User::LeaveIfError(iGetSecurityEvent->Start());

			if (IsUsimAppTsy()  &&
				(iICCCaps & RMobilePhone::KCapsUSimAccessSupported))
				{
				iNotifyAppInfoChange = CNotifyAppInfoChange::NewL(*this, iPhone);
				User::LeaveIfError(iNotifyAppInfoChange->Start());
				}

			if (iIsSatSupported)
				{
				iNotifySatUpdates = CNotifySATUpdates::NewL(*this, iSat);
	 			User::LeaveIfError(iNotifySatUpdates->Start());
				}

			//
			// Start the Sync Engine...
			//
			User::LeaveIfError(iSyncEngine.Connect(iPhonebookManager));

			iServerConfigLevel = EServerConfigurationFull;
			LOGSERVER1(_L8("Configuration changed from 'ConfiguringToFull' to 'Full'."));
			}
		}
	else if (iServerConfigLevel > aLevel)
		{
		//
		// Unconfigure from level 'Full' to 'Idle'...
		//
		if (iServerConfigLevel == EServerConfigurationFull  ||
			iServerConfigLevel == EServerConfigurationConfiguringToFull)
			{
			iCacheStateNotificationArray.Reset();
			iSyncEngineRequestArray.ResetAndDestroy();

			iSyncEngine.Close();

			delete iNotifySatUpdates;
			iNotifySatUpdates = NULL;

			delete iGetSecurityEvent;
			iGetSecurityEvent = NULL;

			delete iNotifyLockInfoChange;
			iNotifyLockInfoChange = NULL;

			delete iGetPhoneStoreInfo;
			iGetPhoneStoreInfo = NULL;

			delete iPhonebookManager;
			iPhonebookManager = NULL;

			delete iNotifyAppInfoChange;
			iNotifyAppInfoChange = NULL;

			iSat.Close();
			iPhone.Close();
			iEtelServer.Close();
				
			iServerConfigLevel = EServerConfigurationIdle;
			LOGSERVER1(_L8("Configuration changed to 'Idle'."));
			}
		//
		// Unconfigure from level 'Idle' to 'None'...
		//
		if (iServerConfigLevel > aLevel  &&
			(iServerConfigLevel == EServerConfigurationIdle  ||
			 iServerConfigLevel == EServerConfigurationConfiguringToIdle))
			{
			delete iNotifyBackupAndRestore;
			iNotifyBackupAndRestore = NULL;

			iServerConfigLevel = EServerConfigurationNone;
			LOGSERVER1(_L8("Configuration changed to 'None'."));
			}
		}
	} // CPhoneBookServer::AttemptToConfigureServerL


/**
 *  Configures or unconfigures the server as specified.
 *  
 *  @param aLevel  The desired configuration level to initialise the server to.
 */
void CPhoneBookServer::ConfigureServerL(TServerConfiguration aLevel)
	{
	//
	// Check parameter is valid...
	//
	if (aLevel != EServerConfigurationNone  &&
		aLevel != EServerConfigurationIdle  &&
		aLevel != EServerConfigurationFull)
		{
		User::Leave(KErrArgument);
		}

	//
	// Save the current configuration level in case AttemptToConfigureServerL()
	// leaves.
	//
	TServerConfiguration  oldConfigLevel = iServerConfigLevel;

	TRAPD(configErr, AttemptToConfigureServerL(aLevel));

	//
	// If the attempt to configure the server failed, then reset the config
	// state and leave.
	//
	if (configErr != KErrNone)
		{
		LOGSERVER2(_L8("ConfigureServerL: Could not config server (Error %d)."),
			     configErr);

		TRAPD(unconfigErr, AttemptToConfigureServerL(oldConfigLevel));
		if (unconfigErr != KErrNone)
			{
			LOGSERVER2(_L8("ConfigureServerL: Could not unconfig server either (Error %d)."),
				     unconfigErr);
			}

		iServerConfigLevel = oldConfigLevel;

		User::Leave(configErr);
		}
	} // CPhoneBookServer::ConfigureServerL


/**
 *  This method is called by the system to indicate that a backup or restore has
 *  started.
 */
void CPhoneBookServer::HandleBackupOrRestoreStarting()
	{
	LOGSERVER1(_L8("HandleBackupOrRestoreStarting()"));

	TRAPD(err, ConfigureServerL(EServerConfigurationIdle));
	if (err != KErrNone)
		{
		LOGSERVER2(_L8("ConfigureServerL() failed with error %d."), err);
		}
	} // CPhoneBookServer::HandleBackupOrRestoreStarting


/**
 *  This method is called by the system to indicate that a backup or restore has
 *  completed.
 */
void CPhoneBookServer::HandleBackupOrRestoreComplete()
	{
	LOGSERVER1(_L8("HandleBackupOrRestoreComplete()"));

	TRAPD(err, ConfigureServerL(EServerConfigurationFull));
	if (err != KErrNone)
		{
		LOGSERVER2(_L8("ConfigureServerL() failed with error %d."), err);
		}
	} // CPhoneBookServer::HandleBackupOrRestoreComplete


/**
 *  Create a new client session.
 *
 *  @note You can not create a new session if the server is shuting down
 *        unconditionally.
 */
CSession2* CPhoneBookServer::NewSessionL(const TVersion&, const RMessage2& /*aMessage*/) const
	{
	LOGSERVER1(_L8("NewSessionL"));

	if (iShouldShutdownUnconditionally)
		{
		User::Leave(KErrPermissionDenied);
		}

	return new(ELeave) CPhoneBookSession();
	} // CPhoneBookServer::NewSessionL


/**
 *  Called by the session class when it is being created.
 *
 *  @param aSession  Server side session.
 */
void CPhoneBookServer::AddSessionL(CPhoneBookSession* aSession)
	{
	LOGSERVER1(_L8("AddSession"));

	//
	// Store this session in the list of sessions...
	//
	iSessionArray.Append(aSession);

	//
	// Queue an Active Object to configure the server straight after this
	// session is created.
	//
	if (iPhoneBookSyncStarter == NULL  &&
	    iServerConfigLevel == EServerConfigurationNone)
		{
		iPhoneBookSyncStarter = new (ELeave) CPhoneBookSyncStarter(*this);
		iPhoneBookSyncStarter->Call();
		}
	} // CPhoneBookServer::AddSessionL


/**
 *  Called by the session class when it is being destroyed.
 *
 *  @param aSession  Server side session.
 */
void CPhoneBookServer::DropSession(CPhoneBookSession* aSession)
	{
	LOGSERVER1(_L8("DropSession"));

	//
	// Remove this session from the session array list...
	//
	TInt  position;
	
	position = iSessionArray.Find(aSession);
 	if (position != KErrNotFound) 
		{
 		iSessionArray.Remove(position);
 		
 		// 
 		// Also remove any outstanding notifications owned by this session
 		//
		for (TInt index = 0;  index < iCacheStateNotificationArray.Count();  index++)
			{
			TCacheStateNotification  notifyRequest = iCacheStateNotificationArray[index];

			if (notifyRequest.iSession == aSession)
				{
				//
				// If it is in the list then just remove and complete it since session has been closed...
				//
				aSession->CompleteRequest(notifyRequest.iMessage, KErrDisconnected);
				iCacheStateNotificationArray.Remove(index);
				index--;
				}
			} 		
		}

	//
	// If we are shuting down then unconfigure and stop...
	//
	if (iSessionArray.Count() == 0  &&  iShouldShutdownServer)
		{
		TRAP_IGNORE(ConfigureServerL(EServerConfigurationNone));
		CActiveScheduler::Stop();
		}
	} // CPhoneBookServer::DropSession


/**
 *  Requests a synchronisation from the engine. This is an asynchronous
 *  request that will result in an engine request being posted to the engine.
 *
 *  @param aSession       A reference to the client session.
 *  @param aMessage       A reference to the client request.
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 */
void CPhoneBookServer::DoSynchronisationL(CPhoneBookSession& aSession,
										  const RMessage2& aMessage,
										  TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("DoSynchronisationL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		aSession.CompleteRequest(aMessage, KErrNotReady);
		return;
		}

	//
	// Validate the phonebook UID to ensure sync requests can only be posted
	// for valid phonebooks. Although no harm would come of sending the engine
	// a sync request with an invalid UID (it will check it as well) we can
	// easily check it now, and reduce the load on the engine.
	//
	if (iPhonebookManager->ValidatePhonebookUid(aPhonebookUid) != KErrNone)
		{
		aSession.CompleteRequest(aMessage, KErrNotSupported);
		return;
		}
	
	//
	// Check a sync request is not in progress or queued for this phonebook...
	//
	if (IsEngineRequestQueued(ESyncDoSynchronisation, aPhonebookUid, ETrue, ETrue))
		{
		aSession.CompleteRequest(aMessage, KErrInUse);
		return;
		}

	//
	// Queue the request to the engine...
	//
	QueueEngineRequestL(ESyncDoSynchronisation, aPhonebookUid, KNullContactId,
						&aSession, aMessage);
	} // CPhoneBookServer::DoSynchronisationL
	
	
/**
 *  Requests the supported phonebook field formats for the specified phonebook.
 *
 *  @param aPhonebookUid   UID of the ICC phonebook to perform the operation on.
 *  @param aContactFields  The returned contact fields.
 *
 *  @return KErrNone if successful, a system-wide error code if not.
 */
TInt CPhoneBookServer::GetContactFormatL(TUid aPhonebookUid,
										 RPhoneBookSession::TContactFieldsV3& aContactFields)
	{
	LOGSERVER2(_L8("GetContactFormatL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return(KErrNotReady);
		}

	//	
	// Get the Contact Fields from the phonebook via the Phonebook Manager.
	//
	TInt result = iPhonebookManager->GetContactFields(aPhonebookUid,
													  aContactFields);
	
	return(result);
	} // CPhoneBookServer::GetContactFormatL


/**
 *  Return the current setting for the Synchronisation Mode for the specified
 *  phonebook. The request will complete with KErrNone if the setting is
 *  successfully returned.
 * 
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 *  @param aSyncMode      The returned sync mode.
 *
 *  @return KErrNone if successful, a system-wide error code if not.
 */
TInt CPhoneBookServer::GetSyncModeL(TUid aPhonebookUid,
									RPhoneBookSession::TPhonebookSyncMode& aSyncMode)
	{
	LOGSERVER2(_L8("GetSyncModeL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return(KErrNotReady);
		}

	//	
	// Get the Sync Mode from the Phonebook via the Phonebook Manager.
	//
	TInt  result = iPhonebookManager->GetSyncMode(aPhonebookUid, aSyncMode);
	
	return result;
	} // CPhoneBookServer::GetSyncModeL


/**
 *  Set the Synchronisation Mode for the specified phonebook.
 *
 *  If the mode is changing to EAutoCurrentIcc and no sync has yet been performed,
 *  then an automatic sync will be performed.
 * 
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 *  @param aSyncMode      The new sync mode.
 *
 *  @return KErrNone if successful, a system-wide error code if not.
 */
TInt CPhoneBookServer::SetSyncModeL(TUid aPhonebookUid,
									RPhoneBookSession::TPhonebookSyncMode aSyncMode)
	{
	LOGSERVER2(_L8("SetSyncModeL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return(KErrNotReady);
		}

	//
	// If the new sync mode is EAutoCurrentIcc then get the current Sync
	// Mode and Sync State. This will determine if an AutoSync should be
	// queued following the change.
	//
	TBool  autoSyncNeeded(EFalse);
	TInt  result(KErrUnknown);

	if (aSyncMode == RPhoneBookSession::EAutoCurrentIcc)
		{
		//
		// Get the current sync mode...
		//
		RPhoneBookSession::TPhonebookSyncMode  oldSyncMode;

		result = iPhonebookManager->GetSyncMode(aPhonebookUid,
												oldSyncMode);
		if (result != KErrNone)
			{
			return result;
			}
		
		//
		// Get the current sync state...
		//
		RPhoneBookSession::TSyncState  syncState;

		result = iPhonebookManager->GetSyncState(aPhonebookUid,
												 syncState);
		if (result != KErrNone)
			{
			return result;
			}
		
		//
		// Find out if the phonebook info has been retrieved...
		//
		TInt  phBkInfoRetrievedResult;
		
		result = iPhonebookManager->GetPhBkInfoRetrievedResult(aPhonebookUid,
														       phBkInfoRetrievedResult);
		if (result != KErrNone)
			{
			return result;
			}
		
		//
		// If the sync mode is changing, the phonebook was previously
		// unsynchronised and the phonebook info is retrieved then
		// an auto-sync will be needed.
		//
		if (oldSyncMode != RPhoneBookSession::EAutoCurrentIcc  &&
			syncState == RPhoneBookSession::EUnsynchronised  &&
			phBkInfoRetrievedResult != KErrNotReady)
			{
			autoSyncNeeded = ETrue;
			}
		}

	//
	// Set the Sync Mode in the Phonebook via the Phonebook Manager.
	//
	result = iPhonebookManager->SetSyncMode(aPhonebookUid, aSyncMode);

	//
	// Queue an auto sync if needed...
	//
	if (autoSyncNeeded)
		{
		QueueAutoSyncRequest(aPhonebookUid);
		}

	return result;
	} // CPhoneBookServer::SetSyncModeL


/**
 *  Delete an entry specified by aContactId from the ICC phonebook store
 *  it exists in.  This is an asynchronous request that will result in an
 *  engine request being posted to the engine.
 *
 *  If a DoSynchronisation or an existing DeleteCntFromICC request has already
 *  been posted for the same phonebook, KErrInUse will be returned.
 *
 *  @param aSession    A reference to the client session.
 *  @param aMessage    A reference to the client request.
 *  @param aContactId  ID of the contact to delete.
 */
void CPhoneBookServer::DeleteCntFromICCL(CPhoneBookSession& aSession,
										 const RMessage2& aMessage,
										 TContactItemId aContactId)
	{
	LOGSERVER2(_L8("DeleteCntFromICCL(0%d)"), aContactId);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		aSession.CompleteRequest(aMessage, KErrNotReady);
		return;
		}

	//
	// Get the phonebook UID from the contact UID.
	//
	TUid phonebookUid = iPhonebookManager->GetPhonebookUidFromContactId(aContactId);

	if (phonebookUid == KUidIccPhonebookNotSpecified)
		{
		aSession.CompleteRequest(aMessage, KErrArgument);
		return;
		}

	//
	// Check a sync or delete request is not in progress or queued for this
	// phonebook...
	//
	if (IsEngineRequestQueued(ESyncDoSynchronisation, phonebookUid, ETrue, ETrue)  ||
		IsEngineRequestQueued(ESyncDeleteCntFromICC, phonebookUid, ETrue, ETrue))
		{
		aSession.CompleteRequest(aMessage, KErrInUse);
		return;
		}

	QueueEngineRequestL(ESyncDeleteCntFromICC, phonebookUid, aContactId,
						&aSession, aMessage);
	} // CPhoneBookServer::DeleteCntFromICCL


/**
 *  Write an entry to a specified ICC phonebook store. This is an asynchronous
 *  request that will result in an engine request being posted to the engine.
 *
 *  If a DoSynchronisation or WriteCntToICCL request has already been posted
 *  for the same phonebook then KErrInUse will be returned.
 *
 *  @param aSession     A reference to the client session.
 *  @param aMessage     A reference to the client request.
 *  @param aSlotNum     The desired slot number or KSyncIndexNotSupplied.
 *  @param aTemplateId  ID of the contact template to base the entry on.
 */
void CPhoneBookServer::WriteCntToICCL(CPhoneBookSession& aSession,
									  const RMessage2& aMessage,
									  TInt aSlotNum,
									  TContactItemId aTemplateId)
	{
	LOGSERVER2(_L8("WriteCntToICCL(%d)"), aTemplateId);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		aSession.CompleteRequest(aMessage, KErrNotReady);
		return;
		}
	
	//
	// Check a sync or write request is not in progress or queued for this
	// phonebook...
	//
	TUid  phonebookUid = iPhonebookManager->GetPhonebookUidFromTemplateId(aTemplateId);

	if (IsEngineRequestQueued(ESyncDoSynchronisation, phonebookUid, ETrue, ETrue)  ||
		IsEngineRequestQueued(ESyncWriteCntToICC, phonebookUid, ETrue, ETrue))
		{
		aSession.CompleteRequest(aMessage, KErrInUse);
		return;
		}

	//
	// This request can only proceed if the cache is valid. Since no sync
	// requests are pending or in progress, we can check this now.
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(phonebookUid, syncState);
	if (result != KErrNone)
		{
		aSession.CompleteRequest(aMessage, result);
		return;
		}
	
	if (syncState != RPhoneBookSession::ECacheValid)
		{
		aSession.CompleteRequest(aMessage, KErrNotReady);
		return;
		}

	//
	// If a slot number is supplied, check it is valid. Again we can do this
	// now as it won't change...
	//
	if (aSlotNum != KSyncIndexNotSupplied)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
		TInt  result;
		
		result = iPhonebookManager->GetPhoneBookInfo(phonebookUid, phBkInfo);
		if (result != KErrNone)
			{
			aSession.CompleteRequest(aMessage, result);
			return;
			}

		if (aSlotNum < 1  ||  aSlotNum > phBkInfo.iTotalEntries)
			{
			aSession.CompleteRequest(aMessage, KErrArgument);
			return;
			}
		}

	//
	// Queue the request for the engine...
	//
	QueueEngineRequestL(ESyncWriteCntToICC, phonebookUid, KNullContactId,
						&aSession, aMessage);
	} // CPhoneBookServer::WriteCntToICCL


/**
 *  Check whether an ICC entry is allowed to be read/write/edit on behalf of
 *  the client. The request will complete with KErrNone if the operation could
 *  be performed.
 * 
 *  @param aContactId   Contact Item ID
 *  @param aValidateOp  Type of operation that needs to be validated
 *
 *  @return KErrNone if validation was successful, otherwise returns an error.
 */
TInt CPhoneBookServer::ValidateContactL(TContactItemId aContactId,
									    MContactSynchroniser::TValidateOperation aValidateOp)
	{
	LOGSERVER2(_L8("ValidateContactL(%d)"), aContactId);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// Get the contact UID from the phonebook...
	//
	TUid  phonebookUid = iPhonebookManager->GetPhonebookUidFromContactId(aContactId);

	if (phonebookUid == KUidIccPhonebookNotSpecified)
		{
		return KErrNotFound;
		}

	//
	// This request can only proceed if the cache is valid...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(phonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}
	
	if (syncState != RPhoneBookSession::ECacheValid)
		{
		return KErrNotReady;
		}

	//
	// First check whether phonebook can be accessed, i.e. whether ICC is
	// pin-locked/blocked...
	//
	if (iPhonebookManager->IsPin1Valid() == EFalse  ||
		(phonebookUid == KUidUsimAppAdnPhonebook  &&
		 iPhonebookManager->IsUsimAppPinValid() == EFalse))
		{
		return KErrAccessDenied;
		}

	//
	// Check the available types of access...
	//
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
	
	result = iPhonebookManager->GetPhoneBookInfo(phonebookUid, phBkInfo);
	if (result != KErrNone)
		{
		return result;
		}

	switch (aValidateOp)
		{
		case MContactSynchroniser::ERead:
		case MContactSynchroniser::ESearch:
			{
			if (phBkInfo.iCaps & RMobilePhoneStore::KCapsReadAccess == 0)
				{
				return KErrAccessDenied;
				}
			}
			break;

		case MContactSynchroniser::EEdit:
			{
			if (phBkInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess == 0)
				{
				return KErrAccessDenied;
				}
			
			 //
			 // For FDN Phonebook, can only edit if PIN2 is set
			 //
			 if (phonebookUid == KUidIccGlobalFdnPhonebook  &&
			 	 iPhonebookManager->IsPin2Valid() == EFalse)
				{
				return KErrAccessDenied;
				}
			}	
			break;

		default:
			{
			return(KErrNotSupported);
			}
		}

	//
	// Finally, check whether the entry exists in the Look-Up Table...
	//
	return iPhonebookManager->IsEntryInTable(phonebookUid, aContactId);
	} // CPhoneBookServer::ValidateContactL


/**
 *  Update an entry in the Phonebook Sync's look-up table for the specified
 *  phonebook by adding the Contacts ID supplied by the client (Contacts Model)
 *  into the specified slot number.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 *  @param aContactId     Contact Item ID
 *  @param aSlotNum       The slot number of the entry.
 *
 *  @return KErrNone if the table was updated successfully, otherwise returns an error.
 */
TInt CPhoneBookServer::UpdateLookupTableL(TUid aPhonebookUid,
										  TContactItemId aContactId,
									      TInt aSlotNum)
	{
	LOGSERVER2(_L8("UpdateLookupTableL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// This request can only proceed if the cache is valid...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}

	if (syncState != RPhoneBookSession::ECacheValid)
		{
		return KErrNotReady;
		}

	//
	// Update the entry in the loop-up-table...
	//
	result = iPhonebookManager->UpdateEntryInTable(aPhonebookUid,
												   aSlotNum, aContactId);

	return result;
	} // CPhoneBookServer::UpdateLookupTableL


/**
 *  Return the ICC entry template or group ID for a specified phonebook.
 *
 *  @param aPhonebookUid   UID of the ICC phonebook to perform the operation on.
 *  @param aSyncIdType     Either ESyncTemplateId or ESyncGroupId.
 *  @param aContactItemId  Returned contact ID.
 *
 *  @return KErrNone if the ID was returned successfully, otherwise returns an error.
 */
TInt CPhoneBookServer::GetPhoneBookIdL(TUid aPhonebookUid,
								       RPhoneBookSession::TSyncIdType aSyncIdType,
									   TContactItemId& aContactItemId)
	{
	LOGSERVER2(_L8("GetPhoneBookIdL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}
	
	//
	// Get the required contact ID...
	//
	TInt  result(KErrArgument);
	
	if (aSyncIdType == RPhoneBookSession::ESyncTemplateId)
		{
		result = iPhonebookManager->GetTemplateId(aPhonebookUid,
												  aContactItemId);
		}
	else if (aSyncIdType == RPhoneBookSession::ESyncGroupId)
		{
		result = iPhonebookManager->GetGroupId(aPhonebookUid,
											   aContactItemId);
		}

	return(result);
	} // CPhoneBookServer::GetPhoneBookIdL


/**
 *  Return the current phonebook server cache state.
 * 
 *  @param aPhonebookUid   UID of the ICC phonebook to perform the operation on.
 *  @param aContactItemId  Returned cache state.
 *
 *  @return KErrNone if the cache state was returned successfully, otherwise returns
 *          an error.
 */
TInt CPhoneBookServer::GetCacheStateL(TUid aPhonebookUid,
								      RPhoneBookSession::TSyncState& aSyncState)
	{
	LOGSERVER2(_L8("GetCacheStateL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// Check if the phonebook is supported...
	//
	TInt  result, phBkInfoRetrievedResult;
	
	result = iPhonebookManager->GetPhBkInfoRetrievedResult(aPhonebookUid,
														   phBkInfoRetrievedResult);
	if (result != KErrNone)
		{
		return result;
		}
	
	if (phBkInfoRetrievedResult != KErrNone)
		{
		return phBkInfoRetrievedResult;
		}

	return iPhonebookManager->GetSyncState(aPhonebookUid, aSyncState);
	} // CPhoneBookServer::GetCacheStateL


/**
 *  Marks the start of checking the server's and engine's heap. 
 *  This function only works in debug releases.
 *
 *  Calls to this function can be nested but each call must be matched by
 *  corresponding DbgMarkEnd().
 *
 *  @return  KErrNone.
 */
TInt CPhoneBookServer::DbgMarkHeap() const
	{
#ifdef _DEBUG
	__UHEAP_MARK;
#endif

	return(KErrNone);
	} // CPhoneBookServer::DbgMarkHeap


/**
 *  Checks that the number of allocated cells at the current nested level on
 *  the server's and engine's heaps are the same as the specified value.
 *  This function only works for debug builds.
 *
 *  @param aCount  The number of heap cells expected to be allocated at
 *                 the current nest level.
 *
 *  @return  KErrNone.
 */
TInt CPhoneBookServer::DbgCheckHeap(TInt aCount) const
	{
#ifdef _DEBUG 
	__UHEAP_CHECK(aCount);
#else
	(void) aCount;
#endif

	return(KErrNone);
	} // CPhoneBookServer::DbgCheckHeap


/**
 *  Marks the end of checking the current server's and engine's heap. 
 *
 *  The function expects aCount heap cells to remain allocated at the
 *  current nest level. This function must match an earlier call to
 *  DbgMarkHeap() and only functions on debug releases.
 *
 *  @param aCount  The number of heap cells expected to remain allocated
 *                 at the current nest level.
 *
 *  @return  KErrNone.
 */
TInt CPhoneBookServer::DbgMarkEnd(TInt aCount) const
	{
#ifdef _DEBUG
	__UHEAP_MARKENDC(aCount);
#else
	(void) aCount;
#endif

	return(KErrNone);
	} // CPhoneBookServer::DbgMarkEnd


/**
 *  Simulates heap allocation failure for the sever and engine.
 *
 *  The failure occurs on the next call to new or any of the functions which 
 *  allocate memory from the heap. This is defined only for debug builds.
 *
 *  @param aCount  Determines when the allocation will fail.
 *
 *  @return  KErrNone.
 */
TInt CPhoneBookServer::DbgFailNext(TInt aCount) const
	{
#ifdef _DEBUG
	if (aCount == 0)
		{
		__UHEAP_RESET;
		}
	else
		{
		__UHEAP_FAILNEXT(aCount);
		}
#else
	(void) aCount;
#endif

	return(KErrNone);
	} // CPhoneBookServer::DbgFailNext


/**
 *  Cancel a previous synchronisation request.
 *
 *  @param aSession       Reference to the client's session.
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the sync request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CPhoneBookServer::DoSynchronisationCancelL(CPhoneBookSession& aSession,
												TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("DoSynchronisationCancelL(): 0x%08x"), aPhonebookUid);

	CancelEngineRequest(ESyncDoSynchronisation, aPhonebookUid,
						KNullContactId, &aSession);

	return(KErrNone);
	} // CPhoneBookServer::DoSynchronisationCancelL


/**
 *  Cancel a previous delete contact request.
 *
 *  @param aSession       Reference to the client's session.
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the delete request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CPhoneBookServer::DeleteCntFromICCCancelL(CPhoneBookSession& aSession,
												TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("DeleteCntFromICCCancelL(0x%08x)"), aPhonebookUid);

	CancelEngineRequest(ESyncDeleteCntFromICC, aPhonebookUid,
						KNullContactId, &aSession);

	return(KErrNone);
	} // CPhoneBookServer::DeleteCntFromICCCancelL


/**
 *  Cancel a previous write contact request.
 *
 *  @param aSession       Reference to the client's session.
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the write request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CPhoneBookServer::WriteCntToICCCancelL(CPhoneBookSession& aSession,
											TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("WriteCntToICCCancelL(0x%08x)"), aPhonebookUid);

	CancelEngineRequest(ESyncDoSynchronisation, aPhonebookUid,
						KNullContactId, &aSession);

	return(KErrNone);
	} // CPhoneBookServer::WriteCntToICCCancelL


/**
 *  Stores the current message for later completion when the cache state of the
 *  specified phonebook changes.
 *
 *  @param aSession       Reference to the client's session.
 *  @param aMessage       Reference to the client's request.
 *  @param aPhonebookUid  UID of the ICC phonebook to monitor.
 */
void CPhoneBookServer::NotifyCacheStateChangeL(CPhoneBookSession& aSession,
											   const RMessage2& aMessage,
											   TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("NotifyCacheStateChangeL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		aSession.CompleteRequest(aMessage, KErrNotReady);
		return;
		}

	//	
	// Validate the phonebook UID to ensure notifications can only be posted
	// for valid phonebooks.
	//
	TInt  result;

	result = iPhonebookManager->ValidatePhonebookUid(aPhonebookUid);
	if (result != KErrNone)
		{
		aSession.CompleteRequest(aMessage, result);
		return;
		}

	//
	// Create Notification Request Object and add to the array...
	//
	TCacheStateNotification  notifyRequest;

	notifyRequest.iSession      = &aSession;
	notifyRequest.iMessage      = aMessage;
	notifyRequest.iPhonebookUid = aPhonebookUid;

	result = iCacheStateNotificationArray.Append(notifyRequest);
	if (result != KErrNone)
		{
		aSession.CompleteRequest(aMessage, result);
		return;
		}
	} // CPhoneBookServer::NotifyCacheStateChangeL


/**
 *  Cancel a previous notify state change request.
 *
 *  @param aSession       Reference to the client's session.
 *  @param aPhonebookUid  UID of the ICC phonebook to cancel the notify request.
 *
 *  @return KErrNone if the request was cancelled, otherwise returns an error.
 */
TInt CPhoneBookServer::NotifyCacheStateChangeCancelL(CPhoneBookSession& aSession,
													 TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("NotifyCacheStateChangeCancelL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}
	
	//
	// Cancel the notification by completing the request with KErrCancel...
	//
	for (TInt index = 0;  index < iCacheStateNotificationArray.Count();  index++)
		{
		TCacheStateNotification  notifyRequest = iCacheStateNotificationArray[index];

		if (notifyRequest.iSession == &aSession  &&
			notifyRequest.iPhonebookUid == aPhonebookUid)
			{
			//
			// If it is in the list then cancel it, and remove it...
			//
			aSession.CompleteRequest(notifyRequest.iMessage, KErrCancel);

			iCacheStateNotificationArray.Remove(index);
			index--;
			}
		}

	return(KErrNone);
	} // CPhoneBookServer::NotifyCacheStateChangeCancelL


/**
 *  Returns the last error found during synchronisation on the specified phonebook.
 *
 *  @param aPhonebookUid   UID of the ICC phonebook to examine.
 *  @param aLastSyncError  Returned value of the last error.
 *
 *  @return KErrNone if the last error was returned, otherwise returns an error.
 */
TInt CPhoneBookServer::GetLastSyncErrorL(TUid aPhonebookUid,
										 TInt& aLastSyncError)
	{
	LOGSERVER2(_L8("GetLastSyncErrorL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// This request can only proceed if a sync request has been actioned...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}
	
	if (syncState == RPhoneBookSession::EUnsynchronised)
		{
		return KErrNotReady;
		}

	//
	// Get the last sync error form the phonebook parameters.
	//
	iPhonebookManager->GetLastSyncError(aPhonebookUid, aLastSyncError);

	return KErrNone;
	} // CPhoneBookServer::GetLastSyncErrorL


/**
 *  Returns the total number of slots for a specified phonebook.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to examine.
 *  @param aNumSlots      Returned value of the number of slots.
 *
 *  @return KErrNone if the value was returned, otherwise returns an error.
 */
TInt CPhoneBookServer::GetNumSlotsL(TUid aPhonebookUid, TInt& aNumSlots)
	{
	LOGSERVER2(_L8("GetNumSlotsL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// Make sure we do not complete with the default value...
	//
	TInt  phBkInfoRetrievedResult;
	TInt  result;

	result = iPhonebookManager->GetPhBkInfoRetrievedResult(aPhonebookUid,
														   phBkInfoRetrievedResult);
	if (result != KErrNone)
		{
		return result;
		}

	if (phBkInfoRetrievedResult != KErrNone)
		{
		return phBkInfoRetrievedResult;
		}

	//
	// This information can be obtained from the Phonebook Info data
	// (TMobilePhoneBookInfoV5) or the RMobilePhoneStore data (which populates)
	// the Look-Up Table size. Since the first option is not always supported,
	// it's easiest (and quickest) to return the Loop-Up Table size...
	//
	result = iPhonebookManager->GetLookUpTableSize(aPhonebookUid, aNumSlots);
	
	return result;
	} // CPhoneBookServer::GetNumSlotsL


/**
 *  Return number of free slots for a specified phonebook.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to examine.
 *  @param aNumFreeSlots  Returned value of the number of free slots.
 *
 *  @return KErrNone if the value was returned, otherwise returns an error.
 */
TInt CPhoneBookServer::GetNumFreeSlotsL(TUid aPhonebookUid,
										TInt& aNumFreeSlots)
	{
	LOGSERVER2(_L8("GetNumFreeSlotsL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// Get the current sync state for this phonebook...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}

	//
	// If the phone book is synchronised then take the value from the LUT.
	// Otherwise, take the value from the phone info structure. However, 
	// the phone may not support returning this information.
	//
	if (syncState == RPhoneBookSession::ECacheValid)
		{
		result = iPhonebookManager->GetNumFreeSlots(aPhonebookUid, aNumFreeSlots);
		if (result != KErrNone)
			{
			return result;
			}
		}
	else
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5  phBkInfo;
		
		result = iPhonebookManager->GetPhoneBookInfo(aPhonebookUid, phBkInfo);
	
		if (phBkInfo.iTotalEntries < 0  ||  phBkInfo.iUsedEntries < 0)
			{
			//
			// The TSY does not support this information.
			//
			return KErrUnknown;
			}
			
		aNumFreeSlots = phBkInfo.iTotalEntries - phBkInfo.iUsedEntries;
		}
	
	return KErrNone;
	} // CPhoneBookServer::GetNumFreeSlotsL


/**
 *  Return an array of free slot numbers for a specified phonebook.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to examine.
 *  @param aFreeSlots     Returned list of free slots.
 *
 *  @return KErrNone if the list was returned, otherwise returns an error.
 */
TInt CPhoneBookServer::GetFreeSlotsL(TUid aPhonebookUid,
									 RArray<TInt>& aFreeSlots)
	{
	LOGSERVER2(_L8("GetFreeSlotsL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// This request can only proceed if the cache is valid...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}

	if (syncState != RPhoneBookSession::ECacheValid)
		{
		return KErrNotReady;
		}

	//
	// Return the list of free slots...
	//
	result = iPhonebookManager->GetMatchingEntries(aPhonebookUid, ESlotEmpty,
												   aFreeSlots);
	
	return result;
	} // CPhoneBookServer::GetFreeSlotsL
	

/**
 *  Return the contact ID assigned to an entry at a given slot in a specified
 *  phonebook.
 *
 *  @param aPhonebookUid   UID of the ICC phonebook to examine.
 *  @param aSlotNum        Slot number to examine.
 *  @param aContactItemId  Returned contact ID.
 *
 *  @return KErrNone if the contact ID was returned, otherwise returns an error.
 */
TInt CPhoneBookServer::GetSlotIdL(TUid aPhonebookUid, TInt aSlotNum,
								  TContactItemId& aContactItemId)
	{
	LOGSERVER2(_L8("GetSlotIdL(0x%08x)"), aPhonebookUid);

	//
	// Is the server configured for use?
	//
	if (iServerConfigLevel != EServerConfigurationFull)
		{
		return KErrNotReady;
		}

	//
	// This request can only proceed if the cache is valid...
	//
	RPhoneBookSession::TSyncState  syncState;
	TInt  result;
	
	result = iPhonebookManager->GetSyncState(aPhonebookUid, syncState);
	if (result != KErrNone)
		{
		return result;
		}

	if (syncState != RPhoneBookSession::ECacheValid)
		{
		return KErrNotReady;
		}

	//
	// Get the ID from the phonebook's Look Up Table...
	//
	result = iPhonebookManager->GetContactIdFromSlotNum(aPhonebookUid,
														aSlotNum,
														aContactItemId);
	
	return result;
	} // CPhoneBookServer::GetSlotIdL


/**
 *  Requests the server to shut down when it no longer has any connected
 *  sessions. This procedure is only premitted in debug builds and is provided
 *  for testing purposes.
 *
 *  The server will shutdown when the last session disconnects.
 *
 *  @param aUnconditionally  If True it allows future connects to the server up
 *                           until the last session disconnects.
 *
 *  @return KErrNone if the shutdown request was accepted, otherwise returns
 *          an error.
 */
TInt CPhoneBookServer::ShutdownServer(TBool aUnconditionally)
	{
	LOGSERVER1(_L8("ShutdownServer()"));

	iShouldShutdownServer          = ETrue;
	iShouldShutdownUnconditionally = aUnconditionally;

	//
	// Unconfigure the server now. Otherwise the engine may be holding a
	// handle on the Contacts DB, which could be holding a handle on the
	// PhBkSync plugin, which could be holding the last session on PhBkSync
	// and prevent the server from unconfiguring the engine, etc.
	//
	TRAP_IGNORE(ConfigureServerL(EServerConfigurationNone));
	
	return(KErrNone);
	} // CPhoneBookServer::ShutdownServer


/**
 *  If a notification request is outstanding for the specified phonebook it is
 *  completed.
 *
 *  @param aPhonebookUid  The phonebook for which the sync state has changed.
 */
void CPhoneBookServer::CompleteNotifyStateChange(TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("CompleteNotifyStateChange(): 0x%08x"), aPhonebookUid);

	for (TInt index = 0;  index < iCacheStateNotificationArray.Count();  index++)
		{
		TCacheStateNotification  notifyRequest = iCacheStateNotificationArray[index];

		if (notifyRequest.iPhonebookUid == aPhonebookUid)
			{
			//
			// If it is in the list then  complete the message and then remove
			// from the list.
			//
			notifyRequest.iSession->CompleteRequest(notifyRequest.iMessage,
												    KErrNone);

			iCacheStateNotificationArray.Remove(index);
			index--;
			}
		}
	} // CPhoneBookServer::CompleteNotifyStateChange


/**
 *  Connect to the ETel Sever, obtains the name of the currently selected TSY and
 *  then loads the TSY.
 */
void CPhoneBookServer::ConnectToEtelL()
	{
	LOGSERVER1(_L8("Connect to ETel")); 
	// Obtain the name of the currently selected TSY 	
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* db = CMDBSession::NewLC(KCDVersion1_2);
#else
	CMDBSession* db = CMDBSession::NewLC(KCDVersion1_1);
#endif
	
	CMDBField<TUint32>* globalSettingField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingField);

	globalSettingField->SetRecordId(1);
	globalSettingField->LoadL(*db);
	TUint32 modemId = *globalSettingField;

	CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
	CleanupStack::PushL(tsyField);

	tsyField->SetRecordId(modemId);
	TRAPD(err, tsyField->LoadL(*db));
	if (err != KErrNone)
		{
		LOGSERVER1(_L8("Unable to get default TSY"));
		}
	User::LeaveIfError(err);

	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	tsyName = *tsyField;

	CleanupStack::PopAndDestroy(3, db); // db, tsyField & globalSettingField
	
	TInt ret(iEtelServer.Connect()); // First connect to the ETel server
	if(ret==KErrNone)
		{
		LOGSERVER1(_L8("Loading TSY")); // Now load the TSY 
		ret=iEtelServer.LoadPhoneModule(tsyName);
		RTelServer::TPhoneInfo phoneInfo;
		if(ret==KErrNone)
			{
			// Determine if TSY supports V2 functionality
			// in event of a problem here assume that it does not
			ret = iEtelServer.IsSupportedByModule(tsyName, KETelExtMultimodeV2, iIsV2Tsy);
			if (ret != KErrNone)
				{
				iIsV2Tsy = EFalse;
				}

			// Determine if TSY supports V5 functionality
 			// in event of a problem here assume that it does not
 			ret = iEtelServer.IsSupportedByModule(tsyName, KEtelExtMultimodeV5, iIsV5Tsy);
 			if (ret != KErrNone)
 				{
 				iIsV5Tsy = EFalse;
 				}

			// Determine if TSY supports USIM Apps
			// in event of a problem here assume that it does not
			ret = iEtelServer.IsSupportedByModule(tsyName, KEtelFuncMobileUSIMApplications, iIsUsimAppTsy);
			if (ret != KErrNone)
				{
				iIsUsimAppTsy = EFalse;
				}

			TInt phoneIndex(0);
			iEtelServer.EnumeratePhones(phoneIndex); // Get total number of phones supported by all currently loaded TSY modules
			while(phoneIndex-->0)
				{
				TName searchTsyName;
				// Check whether this phone belongs to loaded TSY
				if((iEtelServer.GetTsyName(phoneIndex,searchTsyName)==KErrNone) && (searchTsyName.CompareF(tsyName)==KErrNone)) 
					break;
				}
			iEtelServer.GetPhoneInfo(phoneIndex,phoneInfo); // Get information associated with specified phone

			LOGSERVER1(_L8("Open the phone"));
			ret = iPhone.Open(iEtelServer,phoneInfo.iName); // Open and intialise the phone
			if (ret==KErrNone)
				{
				RPhone::TStatus phoneStatus;
				iPhone.GetStatus(phoneStatus); 
				if(phoneStatus.iMode==RPhone::EModeUnknown) // Check whether phone has already been initialised
					{	
					ret=iPhone.Initialise();
					
					if(ret!=KErrNone)
						{	
						LOGSERVER2(_L8("Phone initialisation failed - closing phone (ret=%d)"), ret);
						iPhone.Close();
						}
					}

				if(ret==KErrNone) // Do not even try to open SAT if phone not successfully initialised
					{
					TInt retSat=iSat.Open(iPhone);
					if(retSat==KErrNone)
						{
						iIsSatSupported=ETrue; // SAT supported by TSY so proceed with SAT processing
						// Determine if TSY supports V2 SAT - in event of a problem assume that it does not
						ret = iEtelServer.IsSupportedByModule(tsyName, KETelExtSatV2, iIsV2SAT);
						if (ret != KErrNone)
							{
							iIsV2SAT = EFalse;
							}
						}
					else
						{
						LOGSERVER2(_L8("Could not start ETel SAT (retSat=%d)"), retSat);
						iIsSatSupported = EFalse;
						}
					}
				}
			}
		else
			{
			LOGSERVER2(_L8("Could not load the TSY (ret=%d)"), ret);
			}
		}
	else
		{
		LOGSERVER2(_L8("Could not connect to ETel (ret=%d)"), ret);
		}

	User::LeaveIfError(ret);
	} // CPhoneBookServer::ConnectToEtelL


/**
 *  Complete an outstanding GetPhoneStoreInfo request.
 *
 *  @note  Any errors will cause the phonebook setup to be ignored. This puts
 *         the phonebook in limbo as it is supported but not usable.
 *
 *  @param aRetVal        Result of the request.
 *  @param aStoreInfo     Phonebook store information containing the phonebook ID.
 *  @param aPhonebookUid  Type of the ICC phonebook used internally by the server 
 */
void CPhoneBookServer::CompleteGetPhoneStoreInfo(TInt aRetVal, 
											     RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aStoreInfo, 
					 							 TUid aPhonebookUid)
	{
	LOGSERVER4(_L8("CompleteGetPhoneStoreInfo(): retVal=%d phonebook=0x%08x "
				   "Identity=\"%S\""), aRetVal, aPhonebookUid,
				   &aStoreInfo.iIdentity);

	if (aRetVal == KErrNone)
		{
		//
		// First log the structure as the contents have been the cause of a
		// number of defects so far...
		//		
#ifdef _DEBUG
		TBuf8<RMobilePhone::KMaxMobileNameSize>  nameIn8bit;
		nameIn8bit.Copy(aStoreInfo.iName);
#endif

		LOGSERVER2(_L8("PhoneBookInfo: iExtensionId=%d"), aStoreInfo.ExtensionId());
		LOGSERVER2(_L8("PhoneBookInfo: iType=%d"), aStoreInfo.iType);
		LOGSERVER2(_L8("PhoneBookInfo: iTotalEntries=%d"), aStoreInfo.iTotalEntries);
		LOGSERVER2(_L8("PhoneBookInfo: iUsedEntries=%d"), aStoreInfo.iUsedEntries);
		LOGSERVER2(_L8("PhoneBookInfo: iCaps=0x%08x"), aStoreInfo.iCaps);
		LOGSERVER2(_L8("PhoneBookInfo: iName=\"%S\""), &nameIn8bit);

		if (aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV1  ||
			aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV2  ||
			aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV5)
			{
			LOGSERVER2(_L8("PhoneBookInfo: iMaxNumLength=%d"), aStoreInfo.iMaxNumLength);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxTextLength=%d"), aStoreInfo.iMaxTextLength);
			LOGSERVER2(_L8("PhoneBookInfo: iLocation=%d"), (TInt) aStoreInfo.iLocation);
			LOGSERVER2(_L8("PhoneBookInfo: iChangeCounter=%d"), (TInt) aStoreInfo.iChangeCounter);
			LOGSERVER2(_L8("PhoneBookInfo: iIdentity=\"%S\""), &aStoreInfo.iIdentity);
			}
		
		if (aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV2  ||
			aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV5)
			{
#ifdef _DEBUG
			TBuf8<KMaxName>  phBkModeIn8bit;
			phBkModeIn8bit.Copy(aStoreInfo.iPhBkMode);
#endif

			LOGSERVER2(_L8("PhoneBookInfo: iPhBkMode=\"%S\""), &phBkModeIn8bit);
			}
		
		if (aStoreInfo.ExtensionId() == RMobilePhoneBookStore::KETelMobilePhonebookStoreV5)
			{
			LOGSERVER2(_L8("PhoneBookInfo: iMaxSecondNames=%d"), aStoreInfo.iMaxSecondNames);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxTextLengthSecondName=%d"), aStoreInfo.iMaxTextLengthSecondName);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxAdditionalNumbers=%d"), aStoreInfo.iMaxAdditionalNumbers);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxNumLengthAdditionalNumber=%d"), aStoreInfo.iMaxNumLengthAdditionalNumber);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxTextLengthAdditionalNumber=%d"), aStoreInfo.iMaxTextLengthAdditionalNumber);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxGroupNames=%d"), aStoreInfo.iMaxGroupNames);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxTextLengthGroupName=%d"), aStoreInfo.iMaxTextLengthGroupName);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxEmailAddr=%d"), aStoreInfo.iMaxEmailAddr);
			LOGSERVER2(_L8("PhoneBookInfo: iMaxTextLengthEmailAddr=%d"), aStoreInfo.iMaxTextLengthEmailAddr);
			}
		
		//
		// Should a TSY return iTotalEntries as 0 for ADN then we are in big trouble and
		// no sync will be possible. It is an error in the TSY so we must panic.
		//
		if (aPhonebookUid == KUidIccGlobalAdnPhonebook  &&
			aStoreInfo.iTotalEntries == 0)
			{
			PhBkSyncPanic(EPhBkSyncPanicGetPhoneStoreInfoError);
			}
		
		//
		// Create the default contacts field structure for this phonebook.
		// These are default settings for all phonebooks regardless of type
		// of ICC.
		//
		RPhoneBookSession::TContactFieldsV3  contactFieldsV3;
		TInt  result;
	
		contactFieldsV3.iNameField.iLength    = aStoreInfo.iMaxTextLength;
		contactFieldsV3.iNameField.iCount     = 1;
		contactFieldsV3.iNameField.iDisplayed = ETrue;

		contactFieldsV3.iNumberField.iLength    = aStoreInfo.iMaxNumLength;
		contactFieldsV3.iNumberField.iCount     = 1;
		contactFieldsV3.iNumberField.iDisplayed = ETrue;
	
		contactFieldsV3.iIccSlotField.iLength    = -1;
		contactFieldsV3.iIccSlotField.iCount     = 1;
		contactFieldsV3.iIccSlotField.iDisplayed = ETrue;
	
		contactFieldsV3.iPhonebook = aPhonebookUid;

		if (iICCCaps & RMobilePhone::KCapsUSimAccessSupported)
			{
			//
			// Set up the 3G ICC contact settings.  There are additional fields
			// for ADN and USIM App phonebooks.
			//
			if (aPhonebookUid == KUidIccGlobalAdnPhonebook  || 
				aPhonebookUid == KUidUsimAppAdnPhonebook)
				{
				if (aStoreInfo.ExtensionId()==RMobilePhoneBookStore::KETelMobilePhonebookStoreV5)
					{
			 		contactFieldsV3.iAdditionalNumString.iLength    = aStoreInfo.iMaxNumLengthAdditionalNumber;
			 		contactFieldsV3.iAdditionalNumString.iCount     = aStoreInfo.iMaxAdditionalNumbers;
			 		contactFieldsV3.iAdditionalNumString.iDisplayed = ETrue;
			 
			 		contactFieldsV3.iAdditionalNumAlphaString.iLength    = aStoreInfo.iMaxTextLengthAdditionalNumber;
			 		contactFieldsV3.iAdditionalNumAlphaString.iCount     = aStoreInfo.iMaxAdditionalNumbers;
			 		contactFieldsV3.iAdditionalNumAlphaString.iDisplayed = ETrue;
			 
			 		contactFieldsV3.iGroupField.iLength    = aStoreInfo.iMaxTextLengthGroupName;
			 		contactFieldsV3.iGroupField.iCount     = aStoreInfo.iMaxGroupNames;
			 		contactFieldsV3.iGroupField.iDisplayed = ETrue;
			 
			 		contactFieldsV3.iEmailField.iLength    = aStoreInfo.iMaxTextLengthEmailAddr;
			 		contactFieldsV3.iEmailField.iCount     = aStoreInfo.iMaxEmailAddr;
			 		contactFieldsV3.iEmailField.iDisplayed = ETrue;
			 
			 		contactFieldsV3.iSecondNameField.iLength    = aStoreInfo.iMaxTextLengthSecondName;
			 		contactFieldsV3.iSecondNameField.iCount     = aStoreInfo.iMaxSecondNames;
			 		contactFieldsV3.iSecondNameField.iDisplayed = ETrue;
			 		}
			 	else
			 		{
					contactFieldsV3.iAdditionalNumString.iLength    = aStoreInfo.iMaxTextLength;
					contactFieldsV3.iAdditionalNumString.iCount     = -1;
					contactFieldsV3.iAdditionalNumString.iDisplayed = ETrue;

					contactFieldsV3.iAdditionalNumAlphaString.iLength    = aStoreInfo.iMaxTextLength;
					contactFieldsV3.iAdditionalNumAlphaString.iCount     = -1;
					contactFieldsV3.iAdditionalNumAlphaString.iDisplayed = ETrue;

					contactFieldsV3.iGroupField.iLength    = aStoreInfo.iMaxTextLength;
					contactFieldsV3.iGroupField.iCount     = -1;
					contactFieldsV3.iGroupField.iDisplayed = ETrue;

					contactFieldsV3.iEmailField.iLength    = aStoreInfo.iMaxTextLength;
					contactFieldsV3.iEmailField.iCount     = -1;
					contactFieldsV3.iEmailField.iDisplayed = ETrue;			 		

					contactFieldsV3.iSecondNameField.iLength    = aStoreInfo.iMaxTextLength;
					contactFieldsV3.iSecondNameField.iCount     = -1;
					contactFieldsV3.iSecondNameField.iDisplayed = ETrue;
			 		}
				}
			}
		
		result = iPhonebookManager->SetContactFields(aPhonebookUid, contactFieldsV3);
		if (result != KErrNone)
			{
			return;
			}

		//
		// Initialise the Look Up Table...
		//
		result = iPhonebookManager->SetLookUpTableSize(aPhonebookUid, aStoreInfo.iTotalEntries);
		if (result != KErrNone)
			{
			return;
			}

		//
		// Get the current sync mode. This is used later and also to decide if
		// we need to get the previous phonebook info...
		//
		RPhoneBookSession::TPhonebookSyncMode  syncMode;
		
		result = iPhonebookManager->GetSyncMode(aPhonebookUid, syncMode);
		if (result != KErrNone)
			{
			return;
			}
		
		//
		// Get the previous phonebook info if we need it...
		//
		RMobilePhoneBookStore::TMobilePhoneBookInfoV5  oldPhBkInfo;

		if (syncMode == RPhoneBookSession::EAutoSameIcc)
			{
			result = iPhonebookManager->GetPhoneBookInfo(aPhonebookUid, oldPhBkInfo);
			if (result != KErrNone)
				{
				return;
				}
			}
		
		//
		// Store the new phonebook info...
		//
		iPhonebookManager->SetPhoneBookInfo(aPhonebookUid, aStoreInfo);
		iPhonebookManager->SetPhBkInfoRetrievedResult(aPhonebookUid, aRetVal);
		
		//
		// If the phonebook is in auto-sync mode then queue a sync request...
		//
		if (syncMode == RPhoneBookSession::EAutoCurrentIcc  ||
		    (syncMode == RPhoneBookSession::EAutoSameIcc  &&  aStoreInfo.iIdentity == oldPhBkInfo.iIdentity))
			{
			QueueAutoSyncRequest(aPhonebookUid);
			}
		}
	else
		{
		//
		// The phonebook is not available or supported.
		//
		iPhonebookManager->SetPhBkInfoRetrievedResult(aPhonebookUid, aRetVal);

		//
		// If the sync mode is auto-sync current then perform an auto sync in
		// case any entries exist in the database from previous syncs. The
		// sync will fail, but the engine will silently clear the entries out.
		//
		RPhoneBookSession::TPhonebookSyncMode  syncMode;
		TInt  result;
		
		result = iPhonebookManager->GetSyncMode(aPhonebookUid, syncMode);
		if (result == KErrNone  &&
		    syncMode == RPhoneBookSession::EAutoCurrentIcc)
			{
			QueueAutoSyncRequest(aPhonebookUid);
			}

		}
	} // CPhoneBookServer::CompleteGetPhoneStoreInfo


/**
 *  Complete an outstanding NotifySecurityEvent request. The Active Notification Base
 *  class is responsible for reposting the notification.
 *
 *  @param aRetVal         Result of the notification request.
 *  @param aSecurityEvent  Type of Security Event received.
 */
void CPhoneBookServer::CompleteNotifySecurityEvent(TInt aRetVal,
												   RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent)
	{
	if (aRetVal == KErrNone)
		{
#ifdef _DEBUG
		const char*  KEventNames[23] = {"ENoICCFound", "EICCTerminated",
									    "EPin1Required", "EPuk1Required",
									    "EPin2Required", "EPuk2Required",
									    "EPhonePasswordRequired",
									    "ESPCRequired", "EPin1Verified",
									    "EPin2Verified", "EPuk1Verified",
									    "EPuk2Verified",
									    "EPhonePasswordVerified", "ESPCVerified",
									    "EHiddenKeyRequired", "EHiddenKeyVerified",
									    "EUSIMAppPinRequired", "EUSIMAppPinVerified",
									    "ESecondUSIMAppPinRequired",
									    "ESecondUSIMAppPinVerified",
									    "EUniversalPinRequired",
									    "EUniversalPinVerified",
									    "ESPCChanged"};
#endif

		LOGSERVER2(_L8("CompleteNotifySecurityEvent(): %s."), KEventNames[aSecurityEvent]);

		iPhonebookManager->RecordSecurityEvent(aSecurityEvent);
		}
	} // CPhoneBookServer::CompleteNotifySecurityEvent


/**
 *  Complete an outstanding NotifyLockInfoChange request. The Active Notification Base
 *  class is responsible for reposting the notification.
 *
 *  @param aRetVal    Result of the notification request.
 *  @param aLock      Mobile phone lock.
 *  @param aLockInfo  Status of the changed lock.
 */
void CPhoneBookServer::CompleteNotifyLockInfoChange(TInt aRetVal,
													RMobilePhone::TMobilePhoneLock& aLock,
		                                            RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo)
	{
	if (aRetVal == KErrNone)
		{
#ifdef _DEBUG
		const char*  KStatusNames[4] = {"unknown", "locked", "unlocked", "blocked"};
		const char*  KSettingNames[4] = {"unknown", "enabled", "disabled", "replaced"};
#endif

		if (aLock == RMobilePhone::ELockICC)
			{
			LOGSERVER3(_L8("Lock Info: PIN1 lock is %s and %s."),
					 KStatusNames[aLockInfo.iStatus], KSettingNames[aLockInfo.iSetting]);
			iPhonebookManager->SetPin1LockStatus(aLockInfo.iStatus);

			//
			// If the status has become unlocked, then ensure all phonebooks that
			// have previously attempted to be sync'd (and failed) are sync'd now.
			//
			if (aLockInfo.iStatus == RMobilePhone::EStatusUnlocked)
				{
				TInt  phonebookCount(iPhonebookManager->GetPhonebookCount());

				for(TInt phonebook = 0;  phonebook < phonebookCount;  phonebook++)
					{
					TUid  phonebookUid;
					TInt  result = iPhonebookManager->GetPhonebookUid(phonebook, phonebookUid);
					
					if (result == KErrNone)
						{
						RPhoneBookSession::TSyncState  syncState;

						result = iPhonebookManager->GetSyncState(phonebookUid, syncState);
			
						if (result == KErrNone  &&
						    syncState != RPhoneBookSession::EUnsynchronised)
							{
							iPhonebookManager->SetSyncState(phonebookUid,
							                                RPhoneBookSession::EUnsynchronised);
							QueueAutoSyncRequest(phonebookUid);
							}
						}
					}
				}
			}
		else if (aLock == RMobilePhone::ELockPin2)
			{
			LOGSERVER3(_L8("Lock Info: PIN2 lock is %s and %s."),
					 KSettingNames[aLockInfo.iStatus], KStatusNames[aLockInfo.iSetting]);
			iPhonebookManager->SetPin2LockStatus(aLockInfo.iStatus);
			}
		else if (aLock == RMobilePhone::ELockHiddenKey)
			{
			LOGSERVER3(_L8("Lock Info: Hidden key lock is %s and %s."),
					 KSettingNames[aLockInfo.iStatus], KStatusNames[aLockInfo.iSetting]);
			iPhonebookManager->SetHiddenKeyLockStatus(aLockInfo.iStatus);

			if (aLockInfo.iStatus != RMobilePhone::EStatusLockUnknown)
				{
				//
				// If the USIM ADN phonebook is currently synchronised, or
				// being synchronised, then re-sync it.
				//
				RPhoneBookSession::TSyncState  syncState;
				TInt  result;
			
				result = iPhonebookManager->GetSyncState(KUidUsimAppAdnPhonebook,
														 syncState);
			
				if ((result == KErrNone  &&
				    syncState != RPhoneBookSession::EUnsynchronised)  ||
				    IsEngineRequestQueued(ESyncDoSynchronisation,
										  KUidUsimAppAdnPhonebook,
										  ETrue, ETrue))
					{
					QueueAutoSyncRequest(KUidUsimAppAdnPhonebook);
					}
				}
			}
		else if (aLock == RMobilePhone::ELockUSimApp)
			{
			LOGSERVER3(_L8("Lock Info: USim App lock is %s and %s."),
					 KSettingNames[aLockInfo.iStatus], KStatusNames[aLockInfo.iSetting]);
			iPhonebookManager->SetUsimAppLockStatus(aLockInfo.iStatus);
			}
		else if (aLock == RMobilePhone::ELockUniversalPin)
			{
			LOGSERVER3(_L8("Lock Info: USim Universal PIN is %s and %s."),
					 KSettingNames[aLockInfo.iStatus], KStatusNames[aLockInfo.iSetting]);
			iPhonebookManager->SetUsimUniversalPinLockStatus(aLockInfo.iStatus);
			}
		else
			{
			LOGSERVER3(_L8("Lock Info: Unknown lock is %s (%s)."),
					 KStatusNames[aLockInfo.iStatus], KSettingNames[aLockInfo.iSetting]);
			}
		}
	} // CPhoneBookServer::CompleteNotifyLockInfoChange


/**
 *  Complete an outstanding NotifySATUpdates request. The Active Notification Base
 *  class is responsible for reposting the notification.
 *
 *  @param aRetVal         Result of the notification request.
 *  @param aRefreshType    Type of SAT refresh.
 *  @param aPhonebookList  List of phonebooks affected the update. 
 */
void CPhoneBookServer::CompleteNotifySATUpdates(TInt aRetVal,
												RSat::TRefreshType aRefreshType,
												RArray<TUid>& aPhonebookList)
	{
	LOGSERVER2(_L8("CompleteNotifySATUpdates(): %d"), aRetVal);

	if (aRetVal == KErrNone)
		{
		//
		// Is a full re-synchronisation needed???
		//
		if (aRefreshType == RSat::EFileChangeNotification  || 
			aRefreshType == RSat::ESimInitFileChangeNotification)
			{
			//
			// Set cache state to unsynchronised for all affected phonebooks
			// from phonebook list and queue new sync requests...
			//
			TInt  phonebookCount(aPhonebookList.Count());

			for(TInt phonebook = 0;  phonebook < phonebookCount;  phonebook++)
				{
				TUid  phonebookUid = aPhonebookList[phonebook];
				
				iPhonebookManager->SetSyncState(phonebookUid, RPhoneBookSession::EUnsynchronised);
				CompleteNotifyStateChange(phonebookUid);
				iGetPhoneStoreInfo->QueueGetInfoAndSync(phonebookUid);
				}
			}
		else if (aRefreshType == RSat::ESimInitFullFileChangeNotification)
			{
			//
			// Set cache state to unsynchronised for all phonebooks and
			// then sync them all...
			//
			TInt phonebookCount(iPhonebookManager->GetPhonebookCount());
			TUid phonebookUid;

			for(TInt phonebook = 0; phonebook < phonebookCount; phonebook++)
				{
				TInt  err = iPhonebookManager->GetPhonebookUid(phonebook, phonebookUid);
				
				if (err == KErrNone)
					{
					iPhonebookManager->SetSyncState(phonebookUid,RPhoneBookSession::EUnsynchronised);
					CompleteNotifyStateChange(phonebookUid);
					iGetPhoneStoreInfo->QueueGetInfoAndSync(phonebookUid);
					}
				}
			}
		}
	} // CPhoneBookServer::CompleteNotifySATUpdates


/**
 *  Complete an outstanding NotifyAppInfoChange request by passing the AID of
 *  the currently active  USIM application in the aActiveUsimAID parameter. The
 *  Active Notification Base class is responsible for reposting the notification.
 *
 *  If the Active App has changed and the USIM App phonebook is synchronised (or
 *  has attempted to be synchronised) this a re-synchornisation is performed.
 *
 *  @param aRetVal         Result of the notification request.
 *  @param aInitialValue   True if this is the first value, false if it is an update.
 *  @param aActiveUsimAID  The AID of the currently active USIM Application.
 */
void CPhoneBookServer::CompleteNotifyAppInfoChange(TInt aRetVal,
												   TBool aInitialValue,
												   RMobilePhone::TAID& aActiveUsimAID)
	{
	LOGSERVER2(_L8("CompleteNotifyAppInfoChange(): aRetVal=%d"), aRetVal);

	if (aRetVal == KErrNone)
		{
		LOGSERVER2(_L8("CompleteNotifyAppInfoChange(): aActiveUsimAID=\"%S\""),
				   &aActiveUsimAID);

		//
		// Is this the initial value or an update?
		//
		if (aInitialValue)
			{
			iActiveUsimAID = aActiveUsimAID;
			}
		else
			{
			//
			// The App may have been updated, so see if it has changed...
			//
			if (iActiveUsimAID.CompareF(aActiveUsimAID) != 0)
				{
				//
				// If the USIM ADN phonebook is currently synchronised, or being
				// synchronised then re-sync it.
				//
				RPhoneBookSession::TSyncState  syncState;
				TInt  result;

				result = iPhonebookManager->GetSyncState(KUidUsimAppAdnPhonebook,
														 syncState);

				if ((result == KErrNone  &&
				    syncState != RPhoneBookSession::EUnsynchronised)  ||
				    IsEngineRequestQueued(ESyncDoSynchronisation,
										  KUidUsimAppAdnPhonebook,
										  ETrue, ETrue))
					{
					QueueAutoSyncRequest(KUidUsimAppAdnPhonebook);
					}

				iActiveUsimAID = aActiveUsimAID;
				}
			}
		}
	} // CPhoneBookServer::CompleteNotifyAppInfoChange


/**
 *  Standard Active Object RunError() method, called when the RunL() method
 *  leaves, which will be when the CPhoneBookSession::ServiceL() leaves.
 *
 *  Find the current message and complete it before restarting the server.
 *
 *  @param aError  Leave code from CPhoneBookSession::ServiceL().
 *
 *  @return KErrNone
 */
TInt CPhoneBookServer::RunError(TInt aError)
	{
	LOGSERVER2(_L8("RunError %d"), aError);

	//
	// Complete the request with the available error code.
	//
	if (Message().IsNull() == EFalse)
		{
		Message().Complete(aError);
		}

	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	//
	ReStart();

	return KErrNone;
	} // CPhoneBookServer::RunError


/**
 *  Queues an engine request. The request will be sent to the engine when the
 *  engine is not busy.
 *
 *  @param aPhonebookSyncRequest  Function request to queue.
 *  @param aPhonebookUid          Phonebook to perform the request on.
 *  @param aContactId             Optional Contact ID parameter.
 *  @param aClientSession         Pointer to the client session, or NULL.
 *  @param aClientMessage         The client message request.
 */
void CPhoneBookServer::QueueEngineRequestL(TPhonebookSyncRequest aPhonebookSyncRequest,
										   TUid aPhonebookUid,
										   TContactItemId aContactId,
										   CPhoneBookSession* aClientSession,
										   const RMessage2& aClientMessage)
	{
	LOGSERVER5(_L8("QueueEngineRequest(): IPC=%d Phonebook=0x%08x Contact=0x%08x "
				   "Session=0x%08x"),
			   aPhonebookSyncRequest, aPhonebookUid, aContactId,
			   (TInt) aClientSession);

	//
	// Create a new request and append it to the list of requests...
	//
	CSyncEngineRequest*  request = CSyncEngineRequest::NewL(*this, iSyncEngine,
															aPhonebookSyncRequest,
															aPhonebookUid, aContactId,
															aClientSession, aClientMessage);
	CleanupStack::PushL(request);
	iSyncEngineRequestArray.AppendL(request);
	CleanupStack::Pop(request);
	
	//
	// Ensure that the request runs if nothing else is waiting...
	//
	StartNextEngineRequest();
	} // CPhoneBookServer::QueueEngineRequest


/**
 *  Start the next engine request if one is available and the engine is not
 *  busy.
 */
void CPhoneBookServer::StartNextEngineRequest()
	{
	LOGSERVER1(_L8("StartNextEngineRequest()"));

	//
	// If the next request in the queue is not running then start it...
	//
	if (iSyncEngineRequestArray.Count() > 0  &&
	    iSyncEngineRequestArray[0]->IsActive() == EFalse)
		{
		iSyncEngineRequestArray[0]->StartRequest();
		}
	} // CPhoneBookServer::StartNextEngineRequest


/**
 *  Completes an Engine Request. This method is called be the Engine Request
 *  itself when it finishes. This will cause the request to be removed from
 *  the queue, and a new request to be started if one is available.
 *
 *  @param aEngineRequest  Reference to the Engine Request.
 */
void CPhoneBookServer::CompleteEngineRequest(CSyncEngineRequest& aEngineRequest)
	{
	LOGSERVER4(_L8("CompleteEngineRequest(): IPC=%d Phonebook=0x%08x "
				   "Session=0x%08x"),
			   aEngineRequest.PhonebookSyncRequest(),
			   aEngineRequest.PhonebookUid(),
			   (TInt) aEngineRequest.ClientSession());

	//
	// Remove this request from the list...
	//
	TInt  position = iSyncEngineRequestArray.Find(&aEngineRequest);
	
	if (position != KErrNotFound)
		{
		iSyncEngineRequestArray.Remove(position);
		}
	
	//
	// Start any queued requests that can now begin since the engine finished
	// the last request...
	//
	StartNextEngineRequest();
	} // CPhoneBookServer::CompleteEngineRequest


/**
 *  Cancel a previous engine request. A request that is in progress is cancelled,
 *  while a request that has not reached the engine will be deleted from the queue.
 *
 *  @param aPhonebookSyncRequest  Function request to cancel.
 *  @param aPhonebookUid          Phonebook UID used in the request.
 *  @param aContactId             Optional Contact ID parameter.
 *  @param aClientSession         Pointer to the client session, or NULL.
 */
void CPhoneBookServer::CancelEngineRequest(TPhonebookSyncRequest aPhonebookSyncRequest,
										   TUid aPhonebookUid,
										   TContactItemId aContactId,
										   CPhoneBookSession* aClientSession)
	{
	LOGSERVER5(_L8("CancelEngineRequest(): IPC=%d Phonebook=0x%08x Contact=0x%08x "
				   "Session=0x%08x"), aPhonebookSyncRequest, aPhonebookUid,
			   aContactId, (TInt) aClientSession);

	//
	// Search through the list of engine requests and if we find this request,
	// then cancel it.
	//
	TInt  requestCount(iSyncEngineRequestArray.Count());

	for (TInt index = 0;  index < requestCount;  index++)
		{
		CSyncEngineRequest*  request = iSyncEngineRequestArray[index];
		
		if (request->PhonebookSyncRequest() == aPhonebookSyncRequest  &&
			request->PhonebookUid() == aPhonebookUid  &&
			request->ContactId() == aContactId  &&
			request->ClientSession() == aClientSession)
			{
			if (request->IsActive())
				{
				//
				// The request is running, so cancel it.
				//
				request->DoCancel();
				}
			else
				{
				//
				// The request never ran so we need to pretend it was cancelled.
				//
				request->iStatus = KErrCancel;
				TRAP_IGNORE(request->CompleteClientRequestL());
				}

			break;
			}
		}
	} // CPhoneBookServer::CompleteEngineRequest


/**
 *  Checks if a previous engine request is queued.
 *
 *  @param aPhonebookSyncRequest   Function request to look for.
 *  @param aPhonebookUid           Phonebook UID used in the request.
 *  @param aIncludeUserRequests    If true then requests from the client
 *                                 should be included in the search.
 *  @param aIncludeActiveRequests  If true then currently executing requests
 *                                 should be included in the search.
 */
TBool CPhoneBookServer::IsEngineRequestQueued(TPhonebookSyncRequest aPhonebookSyncRequest,
											  TUid aPhonebookUid,
											  TBool aIncludeUserRequests,
											  TBool aIncludeActiveRequests)
	{
	LOGSERVER5(_L8("IsEngineRequestQueued(): %d 0x%08x %d %d"),
			   aPhonebookSyncRequest, aPhonebookUid, aIncludeUserRequests,
			   aIncludeActiveRequests);
	
	//
	// Search through the list of engine requests and looking for the request,
	// including active and user requests if required.
	//
	TInt  requestCount(iSyncEngineRequestArray.Count());

	for (TInt index = 0;  index < requestCount;  index++)
		{
		CSyncEngineRequest*  request = iSyncEngineRequestArray[index];
		
		if (request->PhonebookSyncRequest() == aPhonebookSyncRequest  &&
			request->PhonebookUid() == aPhonebookUid  &&
			(aIncludeUserRequests  ||  request->ClientSession() == NULL)  &&
			(aIncludeActiveRequests  ||  request->IsActive() == EFalse))
			{
			LOGSERVER1(_L8("IsEngineRequestQueued(): Found"));
			return ETrue;
			}
		}

	LOGSERVER1(_L8("IsEngineRequestQueued(): Not found"));
	return EFalse;
	} // CPhoneBookServer::IsEngineRequestQueued


/**
 *  Queue an automatic synchronisation to the engine.
 *
 *  @param aPhonebookUid   Phonebook to synchronise.
 */
void CPhoneBookServer::QueueAutoSyncRequest(TUid aPhonebookUid)
	{
	LOGSERVER2(_L8("QueueAutoSyncRequest(): 0x%08x"), aPhonebookUid);

	//
	// First check if a request has already been made for an auto-sync on the
	// same phonebook (excluding user requests and requests in progress).
	// If a request already exists then we can ignore this request.
	//
	if (IsEngineRequestQueued(ESyncDoSynchronisation, aPhonebookUid,
							  EFalse, EFalse))
		{
		return;
		}

	//
	// Queue an engine request for a synchronisation...
	//
	TRAPD(err, QueueEngineRequestL(ESyncDoSynchronisation, aPhonebookUid,
						           KNullContactId, NULL, RMessage2()));
	if (err != KErrNone)
		{
		//
		// The request could not be queued!!!
		//
		LOGSERVER2(_L8("QueueAutoSyncRequest(): Error %d"), err);

		iPhonebookManager->SetLastSyncError(aPhonebookUid, err);
		iPhonebookManager->SetSyncState(aPhonebookUid, RPhoneBookSession::EErrorDuringSync);

		CompleteNotifyStateChange(aPhonebookUid);
		}
	} // CPhoneBookServer::QueueAutoSyncRequest


/**
 *  Factory method for producing a CPhoneBookSyncScheduler object. This creates
 *  and installs the Active Scheduler object.
 *
 *  @return Returns a pointer to the CPhoneBookSyncScheduler object or NULL.
 */
CPhoneBookSyncScheduler* CPhoneBookSyncScheduler::NewL()
	{
	LOGSERVER1(_L8("CPhoneBookSyncScheduler::NewL()"));

	CPhoneBookSyncScheduler*  self = new(ELeave) CPhoneBookSyncScheduler();
	CPhoneBookSyncScheduler::Install(self);

	return self;
	} // CPhoneBookSyncScheduler::NewL


/**
 *  Standard Active Scheduler Error() method. This is called if any of the
 *  RunErrorL() functions leave. Hopefully this should not happen!
 *
 *  @param aError  Leave code from the RunErrorL() function.
 */
void CPhoneBookSyncScheduler::Error(TInt aError) const
	{
#ifdef _DEBUG
	LOGSERVER2(_L8("CPhoneBookSyncScheduler::Error %d"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);
	} // CPhoneBookSyncScheduler::Error


/**
 *  Standard constructor.
 *
 *  @param aServer  Reference to the main server.
 */
CPhoneBookSyncStarter::CPhoneBookSyncStarter(CPhoneBookServer& aServer)
  : CAsyncOneShot(EPriorityHigh),
    iServer(aServer)
	{
	// NOP
	} // CPhoneBookSyncStarter::CPhoneBookSyncStarter


/**
 *  RunL() for the starter object. This configures the server fully or
 *  shuts it down.
 */
void CPhoneBookSyncStarter::RunL()
	{
	LOGSERVER2(_L8("CPhoneBookSyncStarter::RunL(): iStatus=%d."), iStatus.Int());

	//
	// Configure the server...
	//
	TRAPD(configErr, iServer.ConfigureServerL(CPhoneBookServer::EServerConfigurationFull));
	if (configErr != KErrNone)
		{
		LOGSERVER2(_L8("ConfigureServerL() failed with error %d."), configErr);

		//
		// Shutdown the server in this case, so it can be restarted later.
		//
		TRAP_IGNORE(iServer.ConfigureServerL(CPhoneBookServer::EServerConfigurationNone));
		CActiveScheduler::Stop();
		}
	} // CPhoneBookSyncStarter::RunL


/**
 *  Perform all server initialisation, in particular creation of the
 *  scheduler and server and then run the scheduler.
 */
static void RunServerL()
	{
	//
	// Naming the server thread after the server helps to debug panics.
	//
	User::LeaveIfError(User::RenameThread(PHBKSYNC_SERVER_NAME));

	//	
	// Create a new Active Scheduler...
	//
	CPhoneBookSyncScheduler*  scheduler = CPhoneBookSyncScheduler::NewL();
	CleanupStack::PushL(scheduler);	
	
	//
	// Create a new PhoneBookServer...
	//
	CPhoneBookServer*  server = CPhoneBookServer::NewL();
	CleanupStack::PushL(server);
	
	//
	// Initialisation complete, now signal the client thread...
	//
	LOGSERVER1(_L8("RunServerL(): Meeting Rendezvous..."));
	RProcess::Rendezvous(KErrNone);

	//
	// Run the server...
	//
	LOGSERVER1(_L8("RunServerL(): Starting server..."));
	CPhoneBookSyncScheduler::Start();
	
	CleanupStack::PopAndDestroy(2, scheduler);
	} // RunServerL


/**
 *  Server process entry-point.
 *
 *  @return  KErrNone or a standard Symbian error code.
 */
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt ret(KErrNone);
	if (cleanup)
		{
		TRAP(ret,RunServerL());
		delete cleanup;
		}
	else
		{
		ret = KErrNoMemory;
		}

	__UHEAP_MARKEND;
	return ret;
	} // E32Main

