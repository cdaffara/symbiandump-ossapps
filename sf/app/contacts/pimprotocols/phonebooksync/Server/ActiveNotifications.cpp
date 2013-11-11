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
// Contains implementations for the Active Object based notification
// classes.
// 
//

/**
 @file
 @internalComponent
*/

#include <connect/sbdefs.h>
#include <etelmm.h>
#include <etelsat.h>
#include <satcs.h>

#include "common.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksynclog.h"
#include "phbksyncsvr.h"
#include "SyncEngineSession.h"
#include "SyncEngineServer.h"
#include "phbksyncsess.h"
#include "ActiveNotifications.h"

using namespace conn;


//
// Some timing constants...
//
const TInt  KActiveNotificationInitialDelayInSeconds = 5;
const TInt  KActiveNotificationMaxDelayInSeconds     = 2560;
const TInt  KOneSecondInMS                           = 1000000;


//
// Definition of the locks used by CNotifyLockInfoChange...
//
const TInt  KLockInfoNumberOfLocks = 5;
const RMobilePhone::TMobilePhoneLock  KLockInfoList[KLockInfoNumberOfLocks] =
											{RMobilePhone::ELockICC,
											 RMobilePhone::ELockPin2,
											 RMobilePhone::ELockHiddenKey,
											 RMobilePhone::ELockUSimApp,
											 RMobilePhone::ELockUniversalPin};


/**
 *  Standard constructor.
 *
 *  @param aServer  Pointer to the front-end server.
 */
CActiveNotificationBase::CActiveNotificationBase(CPhoneBookServer& aServer)
  : CActive(EPriorityNormal),
	iIsNotificationInProgress(EFalse),
	iLastDelay(0),
	iServer(aServer),
	iTimer()
	{
	CActiveScheduler::Add(this);
	} // CActiveNotificationBase::CActiveNotificationBase


/**
 *  Standard destructor. Derived classes will need to implement a destructor
 *  too, so that the active notification can be cancelled.
 */
CActiveNotificationBase::~CActiveNotificationBase()
	{
	LOGACTIVE1(_L8("CActiveNotificationBase::~CActiveNotificationBase()"));

	iTimer.Close();
	} // CActiveNotificationBase::~CActiveNotificationBase


/**
 *  This function starts the Active Notification object. Until it is cancelled
 *  the CompleteNotification() and PostNotification() functions will be called
 *  each time the notification triggers.
 *
 *  @return Returns KErrNone if the notification starts, a system error
 *          code otherwise.
 */
TInt CActiveNotificationBase::Start()
	{
	LOGACTIVE1(_L8("CActiveNotificationBase::Start()"));

	//
	// Check if this Active Notifier is running...
	//
	if (IsActive())
		{
		return KErrInUse;
		}
	
	//
	// Create the RTimer...
	//
	TInt  result;
	
	result = iTimer.CreateLocal();
	if (result != KErrNone)
		{
		return result;
		}
		
	//
	// Start the notifications...
	//
	PostNotification();
	iIsNotificationInProgress = ETrue;
	iLastDelay = 0;
	
	return KErrNone;
	} // CActiveNotificationBase::Start


/**
 *  Virtual function, overridden by the derived class, which performs the
 *  task of issuing the notification.
 */
void CActiveNotificationBase::PostNotification()
	{
	LOGACTIVE1(_L8("CActiveNotificationBase::PostNotification()"));

	//
	// To be implemented by the derived class...
	//
	} // CActiveNotificationBase::PostNotification


/**
 *  Virtual function, overridden by the derived class, which performs the
 *  task of completing the notification. Normally this means a function call
 *  is made on the server object to let it know the event has occured.
 *
 *  @param aRetVal  Result of the notification.
 */
void CActiveNotificationBase::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CActiveNotificationBase::CompleteNotification(%d)"),
			   aRetVal);

	//
	// To be implemented by the derived class...
	//
	(void) aRetVal;
	} // CActiveNotificationBase::CompleteNotification


/**
 *  Virtual function, overridden by the derived class, which performs the
 *  task of cancelling the notification.
 */
void CActiveNotificationBase::CancelNotification()
	{
	LOGACTIVE1(_L8("CActiveNotificationBase::CancelNotification()"));

	//
	// To be implemented by the derived class...
	//
	} // CActiveNotificationBase::CancelNotification


/**
 *  Standard Active Object RunL() method to process requests. This function
 *  is called when either the notification request completes or when the timer
 *  delay completes.
 *
 *  The function decides whether to repost the notification, delay and then
 *  repost or to just stop the notification altogether.
 */
void CActiveNotificationBase::RunL()
	{
	LOGACTIVE2(_L8("CActiveNotificationBase::RunL(): iStatus=%d"), iStatus.Int());

	//
	// Handle the result of the last request or timer request...
	//
	if (iIsNotificationInProgress)
		{
		TInt  retVal(iStatus.Int()); // Stored as iStatus may change!

		iIsNotificationInProgress = EFalse;
		CompleteNotification(retVal);
		
		//
		// Now work out what to do next. If this was successful then repost,
		// otherwise stop the notification or delay then repost.
		//
		if (retVal == KErrNone)
			{
			//
			// The previous notification was successful so repost...
			//
			LOGACTIVE1(_L8("CActiveNotificationBase::RunL(): Reposting..."));

			PostNotification();
			iIsNotificationInProgress = ETrue;
			iLastDelay = 0;
			}
		else if (retVal == KErrNotFound  ||  retVal == KErrCancel  ||
				 retVal == KErrNotSupported)
			{
			//
			// The previous notification returned an error code that indicates
			// it is not worth reposting.
			//
			LOGACTIVE1(_L8("CActiveNotificationBase::RunL(): Stopping..."));
			}
		else
			{
			//
			// The previous notification returned an error code that suggests
			// we should retry in a while. Delay some seconds.
			//
			// If this is the first delay since a successful notification then
			// delay the initial ammount, otherwise double the delay used last
			// time. If we have exceed the maximum delay then stop.
			//
			LOGACTIVE1(_L8("CActiveNotificationBase::RunL(): Delaying..."));

			if (iLastDelay == 0)
				{
				iLastDelay = KActiveNotificationInitialDelayInSeconds;
				}
			else
				{
				iLastDelay = iLastDelay * 2;
				}

			if (iLastDelay < KActiveNotificationMaxDelayInSeconds)
				{
				LOGACTIVE2(_L8("CActiveNotificationBase::RunL(): Delaying %d seconds"),
						   iLastDelay);

				iTimer.After(iStatus, iLastDelay * KOneSecondInMS);
				SetActive();
				}
			else
				{
				LOGACTIVE1(_L8("CActiveNotificationBase::RunL(): Out of time..."));
				}
			}
		}
	else
		{
		//
		// The timer must have expired (either on time or because of some
		// other event such as timezone change).
		//
		// Retry the notification...
		//
		LOGACTIVE1(_L8("CActiveNotificationBase::RunL(): Reposting again..."));

		PostNotification();
		iIsNotificationInProgress = ETrue;
		}
	} // CActiveNotificationBase::RunL


/**
 *  Standard Active Object RunErrorL() function. This is called if the RunL()
 *  leaves. Hopefully this should not happen!
 *
 *  @param aError  Leave code from the RunL().
 *
 *  @return KErrNone (although it panics first).
 */
TInt CActiveNotificationBase::RunError(TInt aError)
	{
#ifdef _DEBUG
	LOGACTIVE2(_L8("CActiveNotificationBase::RunError(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);

	return KErrNone;	
	} // CActiveNotificationBase::RunError


/**
 *  Standard Active Object DoCancel method. This is called when Cancel()
 *  is called on the object and a request is active.
 */
void CActiveNotificationBase::DoCancel()
	{
	LOGACTIVE1(_L8("CActiveNotificationBase::DoCancel()"));

	if (iIsNotificationInProgress)
		{
		CancelNotification();
		}
	else
		{
		iTimer.Cancel();
		}
	} // CActiveNotificationBase::DoCancel




/**
 *  Static factory method used to create a CGetPhoneStoreInfo object.
 *
 *  @param aServer            Reference to the front-end server object.
 *  @param aPhonebookManager  Reference to the Phonebook Manager object.
 *  @param aPhone             Handle to the phone object.
 *
 *  @return  Pointer to the created CNotifySecurityEvent object, or NULL.
 */
CGetPhoneStoreInfo* CGetPhoneStoreInfo::NewL(CPhoneBookServer& aServer,
											 CPhoneBookManager& aPhonebookManager,
											 RMobilePhone& aPhone)
	{
	CGetPhoneStoreInfo* self = new(ELeave) CGetPhoneStoreInfo(aServer,
															  aPhonebookManager,
															  aPhone);

	return self;
	} // CGetPhoneStoreInfo::NewL


/**
 *  Standard constructor.
 *
 *  @param aServer            Reference to the front-end server object.
 *  @param aPhonebookManager  Reference to the Phonebook Manager object.
 *  @param aPhone             Handle to the phone object.
 */
CGetPhoneStoreInfo::CGetPhoneStoreInfo(CPhoneBookServer& aServer,
									   CPhoneBookManager& aPhonebookManager,
									   RMobilePhone& aPhone)
  : CActiveNotificationBase(aServer),
    iPhonebookManager(aPhonebookManager),
    iPhone(aPhone),
    iPhonebookUid(KUidIccPhonebookNotSpecified),
    iPhonebookCount(-1),
    iStoreInfoPckgV1(iStoreInfoV1),
    iStoreInfoPckgV2(iStoreInfoV2),
    iStoreInfoPckgV5(iStoreInfoV5)
	{
	SelectNextPhonebook();
	} // CGetPhoneStoreInfo::CGetPhoneStoreInfo


/**
 *  Standard destructor.
 */
CGetPhoneStoreInfo::~CGetPhoneStoreInfo()
	{
	LOGACTIVE1(_L8("CGetPhoneStoreInfo::~CGetPhoneStoreInfo()"));

	Cancel();
	iPhonebooksToRefresh.Close();
	} // CGetPhoneStoreInfo::~CGetPhoneStoreInfo


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the GetPhoneStoreInfo request.
 */
void CGetPhoneStoreInfo::PostNotification()
	{
	LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification()"));

	//
	// Check if a phonebook remains to have it's info retrieved...
	//
	if (iPhonebookUid != KUidIccPhonebookNotSpecified)
		{
		//
		// Work out the store name...
		//
		if (iPhonebookUid == KUidIccGlobalAdnPhonebook)
			{
			LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification(): Getting ADN."));
			iStore.iStoreName = KETelIccAdnPhoneBook;
			iStore.iMode = KEtelGsmPhoneBookType;
			}
		else if (iPhonebookUid == KUidIccGlobalSdnPhonebook)
			{
			LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification(): Getting SDN."));
			iStore.iStoreName = KETelIccSdnPhoneBook;
			iStore.iMode = KEtelGsmPhoneBookType;
			}
		else if (iPhonebookUid == KUidIccGlobalLndPhonebook)
			{
			LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification(): Getting LND."));
			iStore.iStoreName = KETelIccLndPhoneBook;
			iStore.iMode = KEtelGsmPhoneBookType;
			}
		else if (iPhonebookUid == KUidUsimAppAdnPhonebook)
			{
			LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification(): Getting USIM ADN."));
			iStore.iStoreName = KETelIccAdnPhoneBook;
			iStore.iMode = KEtelUSimPhoneBookType;
			}
		else if (iPhonebookUid == KUidIccGlobalFdnPhonebook)
			{
			LOGACTIVE1(_L8("CGetPhoneStoreInfo::PostNotification(): Getting FDN."));
			iStore.iStoreName = KETelIccFdnPhoneBook;
			iStore.iMode = KEtelGsmPhoneBookType;
			}
		else
			{
			PhBkSyncPanic(EPhBkSyncPanicTooManyPhonebooks);
			}

		//
		// Request the phone store info...
		//
		if (Server().IsV5Tsy())
			{
			iPhone.GetPhoneStoreInfo(iStatus, iStoreInfoPckgV5,
									 iStore.iStoreName, iStore.iMode);
			}
		else if (Server().IsV2Tsy())
			{
			iPhone.GetPhoneStoreInfo(iStatus, iStoreInfoPckgV2,
									 iStore.iStoreName, iStore.iMode);
			}
		else
			{
			iPhone.GetPhoneStoreInfo(iStatus, iStoreInfoPckgV1,
									 iStore.iStoreName);
			}

		SetActive();
		}
	} // CGetPhoneStoreInfo::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the GetPhoneStoreInfo request.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CGetPhoneStoreInfo::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CGetPhoneStoreInfo::CompleteNotification(%d)"), aRetVal);

	//
	// Was this successful???
	//
	if (aRetVal == KErrNone)
		{
		//
		// PhBkSync cannot deal with phonebooks with no identity as it
		// causes later problems with the INI file parsing. So we take
		// care of it here before was pass this information to the server.
		//
		if (Server().IsV5Tsy())
			{
			if (iStoreInfoV5.iIdentity.Length() == 0)
				{
				iStoreInfoV5.iIdentity.Copy(KPhonebookIdUnknown);
				}
				
			Server().CompleteGetPhoneStoreInfo(aRetVal, iStoreInfoV5,
											   iPhonebookUid);	
			}
		else if (Server().IsV2Tsy())
			{
			if (iStoreInfoV2.iIdentity.Length() == 0)
				{
				iStoreInfoV2.iIdentity.Copy(KPhonebookIdUnknown);
				}

			Server().CompleteGetPhoneStoreInfo(aRetVal, 
											   static_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV5&>(iStoreInfoV2),
											   iPhonebookUid);
			}
		else
			{
			if (iStoreInfoV1.iIdentity.Length() == 0)
				{
				iStoreInfoV1.iIdentity.Copy(KPhonebookIdUnknown);
				}

			Server().CompleteGetPhoneStoreInfo(aRetVal,
											   static_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV5&>(iStoreInfoV1),
											   iPhonebookUid);
			}
			
		if (iAutoSyncRequired)
			{
			Server().QueueAutoSyncRequest(iPhonebookUid);
			}

		SelectNextPhonebook();
		}
	else if (aRetVal == KErrNotFound  ||  aRetVal == KErrCancel  ||
		     aRetVal == KErrNotSupported)
		{
		Server().CompleteGetPhoneStoreInfo(aRetVal, iStoreInfoV5,
										   iPhonebookUid);

		//
		// This will not be retried, so move onto the next phonebook...
		//
		SelectNextPhonebook();
		PostNotification();
		}
	else
		{
		//
		// This failure will be retried by the CActiveNotificationBase...
		//
		}
	} // CGetPhoneStoreInfo::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the GetPhoneStoreInfo request.
 */
void CGetPhoneStoreInfo::CancelNotification()
	{
	LOGACTIVE1(_L8("CGetPhoneStoreInfo::CancelNotification()"));

	iPhone.CancelAsyncRequest(EMobilePhoneGetPhoneStoreInfo);
	} // CGetPhoneStoreInfo::CancelNotification


/**
 *  This function triggers a GetPhoneStoreInfo request followed by a sync request.
 *  It is called when a SAT refresh occurs.
 *
 *  @param aPhonebookUid  UID of the ICC phonebook to perform the operation on.
 */
void  CGetPhoneStoreInfo::QueueGetInfoAndSync(TUid aPhonebookUid)
	{
	LOGACTIVE2(_L8("CGetPhoneStoreInfo::QueueGetInfoAndSync(0x%08x)"), aPhonebookUid);

	//
	// Store the UID to refresh if it is not in the list and not in progress.
	//
	if (iPhonebooksToRefresh.Find(aPhonebookUid) == KErrNotFound)
		{
		if (iPhonebooksToRefresh.Append(aPhonebookUid) != KErrNone)
			{
			//
			// We cannot store this request for later, so best thing is
			// is queue a resync now.
			//
			Server().QueueAutoSyncRequest(aPhonebookUid);
			return;
			}
		}

	//
	// If the Active Object is currently running, the request will be performed when
	// the remaining phonebooks are completed. Otherwise post the notification now.
	//
	if (!IsActive())
		{
		SelectNextPhonebook();
		PostNotification();
		iIsNotificationInProgress = ETrue;
		iLastDelay = 0;
		}
	} // CGetPhoneStoreInfo::QueueGetInfoAndSync


/**
 *  Selects the next phonebook to get the store info for. If no phonebooks
 *  are left then the iPhonebookUid is set to KUidIccPhonebookNotSpecified.
 */
void CGetPhoneStoreInfo::SelectNextPhonebook()
	{
	LOGACTIVE1(_L8("CGetPhoneStoreInfo::SelectNextPhonebook()"));

	iPhonebookCount++;
	
	if (iPhonebookCount < iPhonebookManager.GetPhonebookCount())
		{
		//
		// Get the next phonebook UID from the list of phonebooks...
		//
		LOGACTIVE2(_L8("CGetPhoneStoreInfo::SelectNextPhonebook(): Phonebook %d)"),
				   iPhonebookCount);

		TInt  result;
		
		iAutoSyncRequired = EFalse;

		result = iPhonebookManager.GetPhonebookUid(iPhonebookCount, iPhonebookUid);
		if (result != KErrNone)
			{
			//
			// Not much we can do here!
			//
			LOGACTIVE2(_L8("CGetPhoneStoreInfo::SelectNextPhonebook(): Error %d!"),
					   result);
			SelectNextPhonebook();
			}
		}
	else if (iPhonebooksToRefresh.Count() > 0)
		{
		//
		// Get the next phonebook UID from the list of requested refreshes...
		//
		LOGACTIVE2(_L8("CGetPhoneStoreInfo::SelectNextPhonebook(): Refresh=0x%08x)"),
				   iPhonebooksToRefresh[0]);

		iPhonebookUid     = iPhonebooksToRefresh[0];
		iAutoSyncRequired = ETrue;
		iPhonebooksToRefresh.Remove(0);
		}
	else
		{
		//
		// We are finished...
		//
		LOGACTIVE1(_L8("CGetPhoneStoreInfo::SelectNextPhonebook(): Finished!"));

		iPhonebookUid     = KUidIccPhonebookNotSpecified;
		iAutoSyncRequired = EFalse;
		}
	} // CGetPhoneStoreInfo::SelectNextPhonebook


/**
 *  Standard constructor.  Member variables are initialised.
 *
 *  @param aServer  Pointer to the server object.
 *  @param aPhone   The phone containing the ICC that the server uses.
 */
CNotifyLockInfoChange::CNotifyLockInfoChange(CPhoneBookServer& aServer,
											 RMobilePhone& aPhone)
  : CActiveNotificationBase(aServer),
    iPhone(aPhone),
    iLockInfosReceived(0),
    iLockInfoPckg(iLockInfo)
	{
	iLockInfo.iSetting = RMobilePhone::ELockSetUnknown;
	iLockInfo.iStatus  = RMobilePhone::EStatusLockUnknown;
	} // CNotifyLockInfoChange::CNotifyLockInfoChange


/**
 *  Destructor.  Will cancel any pending requests if needed.
 */
CNotifyLockInfoChange::~CNotifyLockInfoChange()
	{
	LOGACTIVE1(_L8("CNotifyLockInfoChange::~CNotifyLockInfoChange()"));

	Cancel();
	} // CNotifyLockInfoChange::~CNotifyLockInfoChange


/**
 *  Factory function for creating a CNotifyLockInfoChange object.
 *
 *  @param aServer  Pointer to the server object.
 *  @param aPhone   The phone containing the ICC that the server uses.
 */
CNotifyLockInfoChange* CNotifyLockInfoChange::NewL(CPhoneBookServer& aServer,
												   RMobilePhone& aPhone)
	{
	CNotifyLockInfoChange*  self = new(ELeave) CNotifyLockInfoChange(aServer,
																	 aPhone);

	return self;
	} // CNotifyLockInfoChange::NewL


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the GetLockInfo or NotifyLockInfoChange request.
 */
void CNotifyLockInfoChange::PostNotification()
	{
	LOGACTIVE1(_L8("CNotifyLockInfoChange::PostNotification()"));

	//
	// Either post a GetLockInfo request for the next lock in the list or a
	// notify request...
	//
	if (iLockInfosReceived < KLockInfoNumberOfLocks)
		{
		iLock = KLockInfoList[iLockInfosReceived];
		iPhone.GetLockInfo(iStatus, iLock, iLockInfoPckg);
		}
	else
		{
		iPhone.NotifyLockInfoChange(iStatus, iLock, iLockInfoPckg);
		}

	SetActive();
	} // CNotifyLockInfoChange::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the GetLockInfo or NotifyLockInfoChange request.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CNotifyLockInfoChange::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CNotifyLockInfoChange::CompleteNotification(%d)"), aRetVal);

	//
	// Was this successful???
	//
	if (aRetVal == KErrNone)
		{
		Server().CompleteNotifyLockInfoChange(aRetVal, iLock, iLockInfo);

		if (iLockInfosReceived < KLockInfoNumberOfLocks)
			{
			iLockInfosReceived++;
			}
		}
	else if (aRetVal == KErrNotFound  ||  aRetVal == KErrCancel  ||
		     aRetVal == KErrNotSupported)
		{
		//
		// This will not be retried, so move onto the next lock if we are
		// still doing the initial list, or otherwise stop...
		//
		if (iLockInfosReceived < KLockInfoNumberOfLocks)
			{
			iLockInfosReceived++;
			PostNotification();
			}
		}
	else
		{
		//
		// This failure will be retried by the CActiveNotificationBase...
		//
		}
	} // CNotifyLockInfoChange::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the GetLockInfo or NotifyLockInfoChange request.
 */
void CNotifyLockInfoChange::CancelNotification()
	{
	LOGACTIVE1(_L8("CNotifyLockInfoChange::CancelNotification()"));

	if (iLockInfosReceived < KLockInfoNumberOfLocks)
		{
		iPhone.CancelAsyncRequest(EMobilePhoneGetLockInfo);
		}
	else
		{
		iPhone.CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange);
		}
	} // CNotifyLockInfoChange::CancelNotification




/**
 *  Static factory method used to create a CNotifySecurityEvent object.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aPhone   Handle to the phone object.
 *
 *  @return  Pointer to the created CNotifySecurityEvent object, or NULL.
 */
CNotifySecurityEvent* CNotifySecurityEvent::NewL(CPhoneBookServer& aServer,
												 RMobilePhone& aPhone)
	{
	CNotifySecurityEvent* self = new(ELeave) CNotifySecurityEvent(aServer,
																  aPhone);

	return self;
	} // CPhoneBookNotifySecurityEvent::NewL


/**
 *  Standard constructor.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aPhone   Handle to the phone object.
 */
CNotifySecurityEvent::CNotifySecurityEvent(CPhoneBookServer& aServer,
										   RMobilePhone& aPhone)
  : CActiveNotificationBase(aServer),
    iSecurityEvent(RMobilePhone::ESPCVerified),
    iPhone(aPhone)
	{
	// NOP
	}


/**
 *  Standard destructor.
 */
CNotifySecurityEvent::~CNotifySecurityEvent()
	{
	LOGACTIVE1(_L8("CNotifySecurityEvent::~CNotifySecurityEvent()"));

	Cancel();
	} // CNotifySecurityEvent::CancelNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the Security Event notification.
 */
void CNotifySecurityEvent::PostNotification()
	{
	LOGACTIVE1(_L8("CNotifySecurityEvent::PostNotification()"));

	iPhone.NotifySecurityEvent(iStatus, iSecurityEvent);
	SetActive();
	} // CNotifySecurityEvent::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the Security Event notification.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CNotifySecurityEvent::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CNotifySecurityEvent::CompleteNotification(%d)"), aRetVal);

	Server().CompleteNotifySecurityEvent(aRetVal, iSecurityEvent);
	} // CNotifySecurityEvent::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the Security Event notification.
 */
void CNotifySecurityEvent::CancelNotification()
	{
	LOGACTIVE1(_L8("CNotifySecurityEvent::CancelNotification()"));

	iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);
	} // CNotifySecurityEvent::CancelNotification




/**
 *  Static factory method used to create a CNotifySATUpdates object.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aSat     Handle to the SAT object.
 *
 *  @return  Pointer to the created CNotifySATUpdates object, or NULL.
 */
CNotifySATUpdates* CNotifySATUpdates::NewL(CPhoneBookServer& aServer,
										   RSat& aSat)
	{
	CNotifySATUpdates*  self = new(ELeave) CNotifySATUpdates(aServer, aSat);

	return self;
	} // CPhoneBookNotifySATUpdates::NewL


/**
 *  Standard constructor.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aSat     Handle to the SAT object.
 */
CNotifySATUpdates::CNotifySATUpdates(CPhoneBookServer& aServer,
									 RSat& aSat)
  : CActiveNotificationBase(aServer),
    iRefreshPckgV1(iRefreshV1),
    iRefreshPckgV2(iRefreshV2),
    iSat(aSat)
	{
	// NOP
	} // CNotifySATUpdates::CNotifySATUpdates


/**
 *  Standard destructor.
 */
CNotifySATUpdates::~CNotifySATUpdates()
	{
	LOGACTIVE1(_L8("CNotifySATUpdates::~CNotifySATUpdates()"));

	Cancel();
	} // CNotifySATUpdates::~CNotifySATUpdates


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the SAT Update notification.
 */
void CNotifySATUpdates::PostNotification()
	{
	LOGACTIVE1(_L8("CNotifySATUpdates::PostNotification()"));

	if (Server().IsV2Sat())
		{
		iSat.NotifyRefreshPCmd(iStatus, iRefreshPckgV2); 
		}
	else
		{
		iSat.NotifyRefreshPCmd(iStatus, iRefreshPckgV1); 
		}

	SetActive();
	} // CNotifySATUpdates::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the SAT Update notification.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CNotifySATUpdates::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CNotifySATUpdates::CompleteNotification(%d)"), aRetVal);

	//
	// Work out the affected phonebooks and whether an update is required...
	//
	RSat::TRefreshType  refreshType(RSat::ERefreshTypeNotSet);
	RArray<TUid>  phonebookList;

	if (aRetVal == KErrNone)
		{
		if (Server().IsV2Sat())
			{
			for (TInt index = 0;  index < iRefreshV2.iFileList.Length();  index++)
				{
				if (iRefreshV2.iFileList[index] == RSat::KAdnEf  ||
					iRefreshV2.iFileList[index] == RSat::KExt1Ef)
					{
					phonebookList.Append(KUidIccGlobalAdnPhonebook);
					}
				else if (iRefreshV2.iFileList[index] == RSat::KSdnEf)
					{
					phonebookList.Append(KUidIccGlobalSdnPhonebook);
					}
				else if (iRefreshV2.iFileList[index] == RSat::KLndEf)
					{
					phonebookList.Append(KUidIccGlobalLndPhonebook);
					}
				else if (iRefreshV2.iFileList[index] == RSat::KGmsi  ||
						 iRefreshV2.iFileList[index] == RSat::KDirEf ||
						 iRefreshV2.iFileList[index] == RSat::KHiddenKeyEf)
					{
					phonebookList.Append(KUidUsimAppAdnPhonebook);
					}
				else if (iRefreshV2.iFileList[index] == RSat::KFdnEf)
					{
					phonebookList.Append(KUidIccGlobalFdnPhonebook);
					}
				}

			refreshType = iRefreshV2.iType;
			}
		else
			{
			for (TInt index = 0;  index < iRefreshV1.iFileList.Length();  index++)
				{
				if (iRefreshV1.iFileList[index] == RSat::KAdnEf  ||
					iRefreshV1.iFileList[index] == RSat::KExt1Ef)
					{
					phonebookList.Append(KUidIccGlobalAdnPhonebook);
					}
				}

			refreshType = iRefreshV1.iType;
			}
		}

	//
	// Pass the information to the server...
	//
	Server().CompleteNotifySATUpdates(aRetVal, refreshType, phonebookList);

	phonebookList.Close();
	} // CNotifySATUpdates::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the SAT Update notification.
 */
void CNotifySATUpdates::CancelNotification()
	{
	LOGACTIVE1(_L8("CNotifySATUpdates::CancelNotification()"));

	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);
	} // CNotifySATUpdates::CancelNotification




/**
 *  Static factory method used to create a CNotifyAppInfoChange object.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aPhone   Handle to the ETel Phone object.
 *
 *  @return  Pointer to the created CNotifyAppInfoChange object, or NULL.
 */
CNotifyAppInfoChange* CNotifyAppInfoChange::NewL(CPhoneBookServer& aServer,
												 RMobilePhone& aPhone)
	{
	CNotifyAppInfoChange*  self = new(ELeave) CNotifyAppInfoChange(aServer,
																   aPhone);

	return self;
	} // CNotifyAppInfoChange::NewL


/**
 *  Standard constructor.
 *
 *  @param aServer  Reference to the front-end server object.
 *  @param aPhone   Handle to the ETel Phone object.
 */
CNotifyAppInfoChange::CNotifyAppInfoChange(CPhoneBookServer& aServer,
										   RMobilePhone& aPhone)
  : CActiveNotificationBase(aServer),
    iPhone(aPhone),
    iReceivedInitialValue(EFalse),
    iAppCount(0)
	{
	// NOP
	} // CNotifyAppInfoChange::CNotifyAppInfoChange


/**
 *  Standard destructor.
 */
CNotifyAppInfoChange::~CNotifyAppInfoChange()
	{
	LOGACTIVE1(_L8("CNotifyAppInfoChange::~CNotifyAppInfoChange()"));

	Cancel();
	} // CNotifyAppInfoChange::~CNotifyAppInfoChange


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the SAT Update notification.
 */
void CNotifyAppInfoChange::PostNotification()
	{
	LOGACTIVE1(_L8("CNotifyAppInfoChange::PostNotification()"));

	if (iReceivedInitialValue == EFalse)
		{
		iPhone.EnumerateUSimApplications(iStatus, iAppCount, iActiveAID);
		}
	else
		{
		iPhone.NotifyUSimApplicationsInfoChange(iStatus, iAppCount, iActiveAID);
		}

	SetActive();
	} // CNotifyAppInfoChange::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the SAT Update notification.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CNotifyAppInfoChange::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CNotifyAppInfoChange::CompleteNotification(%d)"), aRetVal);

	if (iReceivedInitialValue == EFalse)
		{
		Server().CompleteNotifyAppInfoChange(aRetVal, ETrue, iActiveAID);
		iReceivedInitialValue = ETrue;
		}
	else
		{
		Server().CompleteNotifyAppInfoChange(aRetVal, EFalse, iActiveAID);
		}
	} // CNotifyAppInfoChange::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the SAT Update notification.
 */
void CNotifyAppInfoChange::CancelNotification()
	{
	LOGACTIVE1(_L8("CNotifySATUpdates::CancelNotification()"));

	if (iReceivedInitialValue == EFalse)
		{
		iPhone.CancelAsyncRequest(EMobilePhoneEnumerateUSimApplications);
		}
	else
		{
		iPhone.CancelAsyncRequest(EMobilePhoneNotifyUSimApplicationsInfoChange);
		}
	} // CNotifyAppInfoChange::CancelNotification




/**
 *  Static factory method used to create a CNotifyBackupAndRestore object.
 *
 *  @param aServer  Reference to the front-end server object.
 *
 *  @return  Pointer to the created CNotifyBackupAndRestore object, or NULL.
 */
CNotifyBackupAndRestore* CNotifyBackupAndRestore::NewL(CPhoneBookServer& aServer)
 	{
 	CNotifyBackupAndRestore*  self = new(ELeave) CNotifyBackupAndRestore(aServer);
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop(self);

 	return self;
 	} // CBackupAndRestore::NewL
 
 
/**
 *  Standard constructor.
 *
 *  @param aServer  Reference to the front-end server object.
 */
CNotifyBackupAndRestore::CNotifyBackupAndRestore(CPhoneBookServer& aServer)
  : CActiveNotificationBase(aServer)
 	{
 	// NOP
 	} // CNotifyBackupAndRestore::CNotifyBackupAndRestore
 
 
/**
 *  Second phase constructor.
 */
void CNotifyBackupAndRestore::ConstructL()
 	{
	User::LeaveIfError(iBackupProperty.Attach(KUidSystemCategory,
											  KUidBackupRestoreKey));
 	} // CNotifyBackupAndRestore::ConstructL
 
 
/**
 *  Standard destructor.
 */
CNotifyBackupAndRestore::~CNotifyBackupAndRestore()
 	{
	LOGACTIVE1(_L8("CNotifyBackupAndRestore::~CNotifyBackupAndRestore()"));

 	Cancel();

 	iBackupProperty.Close();
 	} // CNotifyBackupAndRestore::~CNotifyBackupAndRestore


/**
 *  Virtual method called by the CActiveNotificationBase class to post
 *  the Secure Backup notification.
 */
void CNotifyBackupAndRestore::PostNotification()
	{
	LOGACTIVE1(_L8("CNotifyBackupAndRestore::PostNotification()"));

 	iBackupProperty.Subscribe(iStatus);
	SetActive();
	} // CNotifyBackupAndRestore::PostNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to complete
 *  the Secure Backup notification.
 *
 *  @param aRetVal  Result of the notification request.
 */
void CNotifyBackupAndRestore::CompleteNotification(TInt aRetVal)
	{
	LOGACTIVE2(_L8("CNotifyBackupAndRestore::CompleteNotification(%d)"), aRetVal);

	if (aRetVal == KErrNone)
		{
		//
		// Get the property value.  The last byte contains backup and restore
		// data of interest to us. Re-subscribe straight away!
		//
	 	TInt  backupRestoreFlag;

	 	iBackupProperty.Get(backupRestoreFlag);
	 	
		LOGACTIVE2(_L8("CNotifyBackupAndRestore::RunL() backupRestoreFlag=0x%08x"),
				   backupRestoreFlag);
		
		//
		// Handle the property value...
		//
		if (backupRestoreFlag & EBURNormal)
			{
			Server().HandleBackupOrRestoreComplete();
			}
		else if (backupRestoreFlag & EBURBackupFull  ||
				 backupRestoreFlag & EBURBackupPartial  ||
				 backupRestoreFlag & EBURRestoreFull  ||
				 backupRestoreFlag & EBURRestorePartial)
			{
			Server().HandleBackupOrRestoreStarting();
			}
		}
	} // CNotifyBackupAndRestore::CompleteNotification


/**
 *  Virtual method called by the CActiveNotificationBase class to cancel
 *  the Secure Backup notification.
 */
void CNotifyBackupAndRestore::CancelNotification()
	{
	LOGACTIVE1(_L8("CNotifyBackupAndRestore::CancelNotification()"));

	iBackupProperty.Cancel();
	} // CNotifyBackupAndRestore::CancelNotification

