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
// Header file for the Phonebook Synchroniser Server server side Session
// classes.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHBKSYNCSRV_H__
#define __PHBKSYNCSRV_H__

#include <commsdattypesv1_1.h>
#include <e32property.h>
#include <etelsat.h>
#include <f32file.h>

#include "common.h"
#include "SyncEngineClient.h"


/**
 *  Name of the server. The '!' means it is a protected server.
 */
_LIT(PHBKSYNC_SERVER_NAME, "!PhBkSyncServer");


/**
 *  Only bits 5-7 needed for TON.
 */
const TInt KEtsiTonPosition = 0x70;


/**
 *  Following literals are used for the INI file...
 */
_LIT8(KPhBkSyncPhonebook, "Phonebook");
_LIT8(KPhBkSyncHiddenName, "**********");


//
// Forward declarations...
//
class CPhoneBookSession;
class CGetPhoneStoreInfo;
class CNotifySecurityEvent;
class CNotifyLockInfoChange;
class CNotifySATUpdates;
class CNotifyAppInfoChange;
class CNotifyBackupAndRestore;
class CSyncEngineRequest;
class CPhoneBookSyncStarter;


/**
 *  Structure to hold information for Sync State Notifications.
 */
typedef struct
	{
	CPhoneBookSession*  iSession;
	RMessage2  iMessage;
	TUid  iPhonebookUid;
	} TCacheStateNotification;


/**
 *  The Phonebook Synchroniser Server class. This class provides the front end
 *  server implementation which handles all basic (and quick) requests and
 *  forwards the longer requests to the Background Sync Engine.
 *
 *  The class provides all the services required by class CPhoneBookSession.
 */
class CPhoneBookServer : public CPolicyServer
	{
	friend class CPhoneBookSyncStarter;
		
private:
	/**
	 *  Configuration level of the server.
	 */
	enum TServerConfiguration
		{
		/** Not configured for use */
		EServerConfigurationNone = 0,
		/** Not configured, configuring to Idle configuration */
		EServerConfigurationConfiguringToIdle = 1,
		/** Configured only to support existing session closures */
		EServerConfigurationIdle = 2,
		/** Idle config, configuring to full running configuration */
		EServerConfigurationConfiguringToFull = 3,
		/** Full running configuration */
		EServerConfigurationFull = 4
		};

public:
	static CPhoneBookServer* NewL();

	void HandleBackupOrRestoreStarting();
	void HandleBackupOrRestoreComplete();

	void AddSessionL(CPhoneBookSession* aSession);
	void DropSession(CPhoneBookSession* aSession);

	void DoSynchronisationL(CPhoneBookSession& aSession,
							const RMessage2& aMessage,
							TUid aPhonebookUid);
	TInt GetContactFormatL(TUid aPhonebookUid,
						   RPhoneBookSession::TContactFieldsV3& aContactFields);
	TInt GetSyncModeL(TUid aPhonebookUid,
					  RPhoneBookSession::TPhonebookSyncMode& aSyncMode);
	TInt SetSyncModeL(TUid aPhonebookUid,
					  RPhoneBookSession::TPhonebookSyncMode aSyncMode);
	void DeleteCntFromICCL(CPhoneBookSession& aSession,
						   const RMessage2& aMessage,
						   TContactItemId  aContactId);
	void WriteCntToICCL(CPhoneBookSession& aSession,
						const RMessage2& aMessage,
						TInt aSlotNum, TContactItemId aTemplateId);
	TInt ValidateContactL(TContactItemId aContactId,
						  MContactSynchroniser::TValidateOperation aValidateOp);
	TInt UpdateLookupTableL(TUid aPhonebookUid, TContactItemId aContactId,
							TInt aSlotNum);
	TInt GetPhoneBookIdL(TUid aPhonebookUid,
						 RPhoneBookSession::TSyncIdType aSyncIdType,
						 TContactItemId& aContactItemId);
	TInt GetCacheStateL(TUid aPhonebookUid,
						RPhoneBookSession::TSyncState& aSyncState);
	TInt DbgMarkHeap() const;
	TInt DbgCheckHeap(TInt aCount) const;
	TInt DbgMarkEnd(TInt aCount) const;
	TInt DbgFailNext(TInt aCount) const;
	TInt DoSynchronisationCancelL(CPhoneBookSession& aSession,
								  TUid aPhonebookUid);
	TInt DeleteCntFromICCCancelL(CPhoneBookSession& aSession,
								 TUid aPhonebookUid);
	TInt WriteCntToICCCancelL(CPhoneBookSession& aSession, TUid aPhonebookUid);
	void NotifyCacheStateChangeL(CPhoneBookSession& aSession,
								 const RMessage2& aMessage,
								 TUid aPhonebookUid);
	TInt NotifyCacheStateChangeCancelL(CPhoneBookSession& aSession,
							 		   TUid aPhonebookUid);
	TInt GetLastSyncErrorL(TUid aPhonebookUid, TInt& aLastSyncError);
	TInt GetNumSlotsL(TUid aPhonebookUid, TInt& aNumSlots);
	TInt GetNumFreeSlotsL(TUid aPhonebookUid, TInt& aNumFreeSlots);
	TInt GetFreeSlotsL(TUid aPhonebookUid, RArray<TInt>& aFreeSlots);
	TInt GetSlotIdL(TUid aPhonebookUid, TInt aSlotNum,
					TContactItemId& aContactItemId);
	TInt ShutdownServer(TBool aConditional);

	void CompleteNotifyStateChange(TUid aPhonebookUid);
	void CompleteGetPhoneStoreInfo(TInt aRetVal, RMobilePhoneBookStore::TMobilePhoneBookInfoV5& aStoreInfo,
		                           TUid aPhonebook);
	void CompleteNotifySecurityEvent(TInt aRetVal, RMobilePhone::TMobilePhoneSecurityEvent aSecurityEvent);
	void CompleteNotifyLockInfoChange(TInt aRetVal, RMobilePhone::TMobilePhoneLock& aLock,
                                      RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo);
	void CompleteNotifySATUpdates(TInt aRetVal, RSat::TRefreshType aRefreshType, 
		                           RArray<TUid>& aPhonebookList);
	void CompleteNotifyAppInfoChange(TInt aRetVal, TBool aInitialValue,
									 RMobilePhone::TAID& aActiveUsimAID);

	inline TUint32 ICCCaps();
	inline TBool IsV2Tsy();
	inline TBool IsV5Tsy();
	inline TBool IsV2Sat();
	inline TBool IsUsimAppTsy();

	void QueueEngineRequestL(TPhonebookSyncRequest aPhonebookSyncRequest,
							 TUid aPhonebookUid,
							 TContactItemId aContactId,
							 CPhoneBookSession* aClientSession,
							 const RMessage2& aClientMessage);
	void StartNextEngineRequest();
	void CompleteEngineRequest(CSyncEngineRequest& aEngineRequest);
	void CancelEngineRequest(TPhonebookSyncRequest aPhonebookSyncRequest,
							 TUid aPhonebookUid,
							 TContactItemId aContactId,
							 CPhoneBookSession* aClientSession);
	TBool IsEngineRequestQueued(TPhonebookSyncRequest aPhonebookSyncRequest,
								TUid aPhonebookUid,
								TBool aIncludeUserRequests,
								TBool aIncludeActiveRequests);
	void QueueAutoSyncRequest(TUid aPhonebookUid);

private:
	CPhoneBookServer();
	~CPhoneBookServer();

	void ConstructL();

	void ConnectToEtelL();
	void ConfigureServerL(TServerConfiguration aLevel);
	void AttemptToConfigureServerL(TServerConfiguration aLevel);
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2&/*aMessage*/) const;
	TInt RunError(TInt aError);


	TServerConfiguration  iServerConfigLevel;

	RTelServer  iEtelServer;
	TUint32  iICCCaps;
	RMobilePhone iPhone;
	RSat iSat;
	TBool  iIsV2Tsy;
	TBool  iIsV5Tsy;
	TBool  iIsV2SAT;
	TBool  iIsUsimAppTsy;

	TBool iIsSatSupported;

	CGetPhoneStoreInfo*  iGetPhoneStoreInfo;
	CNotifyLockInfoChange*  iNotifyLockInfoChange;
	CNotifySecurityEvent*  iGetSecurityEvent;
	CNotifySATUpdates*  iNotifySatUpdates;
	CNotifyAppInfoChange*  iNotifyAppInfoChange;
	CNotifyBackupAndRestore*  iNotifyBackupAndRestore;
	CPhoneBookSyncStarter*  iPhoneBookSyncStarter;

	RPointerArray<CPhoneBookSession>  iSessionArray;

	CPhoneBookManager*  iPhonebookManager;

	RMobilePhone::TAID  iActiveUsimAID;

	//
	// Variables to control shutdown of the server...
	//
	TBool  iShouldShutdownServer;
	TBool  iShouldShutdownUnconditionally;

	//
	// Connection to the Background Sync Engine...
	//
	RSyncEngineSession  iSyncEngine;
	RPointerArray<CSyncEngineRequest>  iSyncEngineRequestArray;
	
	//
	// List of notification requests and on which phonebook UIDs.
	//
	RArray<TCacheStateNotification>  iCacheStateNotificationArray;

	//
	// Policy server information...
	//
	static const TInt  iRanges[];
	static const TUint8  iElementsIndex[];
	static const CPolicyServer::TPolicyElement  iElements[];
	static const CPolicyServer::TPolicy  iPolicy;
	};  


/**
 *  Simple Active Object to start the configuration of the server.
 *  This is used to break any deadlock between CntModel starting
 *  PhoneBookSync and PhoneBookSync starting CntModel.
 */
class CPhoneBookSyncStarter : public CAsyncOneShot
	{
public:
	CPhoneBookSyncStarter(CPhoneBookServer& aServer);

private:
	virtual void RunL();
	
	CPhoneBookServer&  iServer;
	};


/**
 *  Specialised Active Scheduler to cleanly handle leaves of the Active
 *  Objects.
 */
class CPhoneBookSyncScheduler : public CActiveScheduler
	{
public:
	static CPhoneBookSyncScheduler* NewL();
	void Error(TInt aError) const;
	};


//
// Old unsupported PhBkSync server thread.
//
IMPORT_C TInt PhBkSyncServerThread(TAny* aArg);


/**
 *  Indicates whether V2 functionality is supported by the TSY. 
 *
 *  @return Boolean indicating whether V2 functionality is supported by the phone. 
 */
inline TBool CPhoneBookServer::IsV2Tsy()
	{
	return iIsV2Tsy;
	} // CPhoneBookServer::IsV2Tsy

	
/**
 *  Indicates whether V5 functionality is supported by the TSY. 
 *
 *  @return Boolean indicating whether V5 functionality is supported by the phone. 
 */
inline TBool CPhoneBookServer::IsV5Tsy()
	{
	return iIsV5Tsy;
	} // CPhoneBookServer::IsV5Tsy		


/**
 *  Indicates whether V2 SAT is supported by the TSY. 
 *
 *  @return Boolean indicating whether V2 SAT is supported 
 */
inline TBool CPhoneBookServer::IsV2Sat()
	{
	return iIsV2SAT;
	} // CPhoneBookServer::IsV2Sat


/**
 *  Indicates whether USIM App is supported by the TSY. 
 *
 *  @return Boolean indicating whether USIM App is supported 
 */
inline TBool CPhoneBookServer::IsUsimAppTsy()
	{
	return iIsUsimAppTsy;
	} // CPhoneBookServer::IsUsimAppTsy


#endif // __PHBKSYNCSRV_H__

