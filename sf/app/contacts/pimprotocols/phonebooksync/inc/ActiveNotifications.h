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
// Header file for Active Object based notification classes.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __ACTIVENOTIFICATIONS_H__
#define __ACTIVENOTIFICATIONS_H__

#include <etelmm.h>

#include "phbksyncsvr.h"
#include "SyncEngineServer.h"


/**
 *  This class is the base class for all the Acitve Notifications.
 */
class CActiveNotificationBase : public CActive
	{
public:
	~CActiveNotificationBase();

	TInt Start();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

protected:
	CActiveNotificationBase(CPhoneBookServer& aServer);

	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
	inline CPhoneBookServer& Server() const { return iServer; };

	TBool  iIsNotificationInProgress;
	TInt  iLastDelay;

private:
	CPhoneBookServer&  iServer;
	RTimer  iTimer;
	};


/**
 *  This class is responsible for implementing the initial request of
 *  the phone store information (at the RMobilePhone level) for all
 *  supported phonebook stores from the ICC.
 *
 *  It is also responsible for triggering refresh synchronisations
 *  which require a GetPhoneStoreInfo request to be carried out before
 *  a sync request.
 */
class CGetPhoneStoreInfo : public CActiveNotificationBase
	{
public:
	static CGetPhoneStoreInfo* NewL(CPhoneBookServer& aServer,
									CPhoneBookManager& aPhonebookManager,
									RMobilePhone& aPhone);
	~CGetPhoneStoreInfo();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();
	
	void QueueGetInfoAndSync(TUid aPhonebookUid);

private:
	CGetPhoneStoreInfo(CPhoneBookServer& aServer,
					   CPhoneBookManager& aPhonebookManager,
					   RMobilePhone& aPhone);

	void SelectNextPhonebook();

	CPhoneBookManager&  iPhonebookManager;
	RMobilePhone&  iPhone;
	TUid  iPhonebookUid;
	TBool  iAutoSyncRequired;
	TInt  iPhonebookCount;
	RMobilePhone::TPhoneStoreNameAndIccType  iStore;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1      iStoreInfoV1;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg  iStoreInfoPckgV1;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV2      iStoreInfoV2;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV2Pckg  iStoreInfoPckgV2;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5	   iStoreInfoV5;
 	RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg  iStoreInfoPckgV5;
 	RArray<TUid>  iPhonebooksToRefresh;
	};


/**
 *  Responsible for implementing the initial request and notification
 *  of any phone lock state or lock info change events.
 */
class CNotifyLockInfoChange : public CActiveNotificationBase
	{
public:
	static CNotifyLockInfoChange* NewL(CPhoneBookServer& iServer,
									   RMobilePhone& aPhone);
	~CNotifyLockInfoChange();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

private:
	CNotifyLockInfoChange(CPhoneBookServer& iServer,
						  RMobilePhone& aPhone);

	RMobilePhone&  iPhone;
	TInt  iLockInfosReceived;
	RMobilePhone::TMobilePhoneLock  iLock;
	RMobilePhone::TMobilePhoneLockInfoV1  iLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg  iLockInfoPckg;
	};


/**
 *  This class is responsible for implementing the state machine required to be
 *  notified of any phone security events.
 */
class CNotifySecurityEvent : public CActiveNotificationBase
	{
public:
	static CNotifySecurityEvent* NewL(CPhoneBookServer& aServer, RMobilePhone& aPhone);
	~CNotifySecurityEvent();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

private:
	CNotifySecurityEvent(CPhoneBookServer& aServer, RMobilePhone& aPhone);

	RMobilePhone::TMobilePhoneSecurityEvent  iSecurityEvent;
	RMobilePhone&  iPhone;
	};


/**
 *  This class is responsible for implementing the notification of any
 *  ETel SAT updates.
 */
class CNotifySATUpdates : public CActiveNotificationBase
	{
public:
	static CNotifySATUpdates* NewL(CPhoneBookServer& aServer, RSat& aSat);
	~CNotifySATUpdates();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

private:
	CNotifySATUpdates(CPhoneBookServer& aServer, RSat& aSat);

	RSat::TRefreshV1  iRefreshV1;
	RSat::TRefreshV1Pckg  iRefreshPckgV1;
	RSat::TRefreshV2  iRefreshV2;
	RSat::TRefreshV2Pckg  iRefreshPckgV2;
	RSat&  iSat;
	};


/**
 *  This class is responsible for implementing the notification of the
 *  USIM Application information change.
 */ 
class CNotifyAppInfoChange : public CActiveNotificationBase
	{
public:
	static CNotifyAppInfoChange* NewL(CPhoneBookServer&aServer,
									  RMobilePhone& aPhone);
	~CNotifyAppInfoChange();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

private:
	CNotifyAppInfoChange(CPhoneBookServer& aServer, RMobilePhone& aPhone);

	RMobilePhone&  iPhone;
	TBool  iReceivedInitialValue;
	TInt  iAppCount;
	RMobilePhone::TAID  iActiveAID;
	};


/**
 *  This class is responsible for implementing the secure backup and restore
 *  operation notification.
*/ 
class CNotifyBackupAndRestore : public CActiveNotificationBase
	{
public:
	static CNotifyBackupAndRestore* NewL(CPhoneBookServer& aServer);
	~CNotifyBackupAndRestore();

	virtual void PostNotification();
	virtual void CompleteNotification(TInt aRetVal);
	virtual void CancelNotification();

private:
	CNotifyBackupAndRestore(CPhoneBookServer& aServer);

	void ConstructL();

	RProperty iBackupProperty;
	};



#endif // __ACTIVENOTIFICATIONS_H__
