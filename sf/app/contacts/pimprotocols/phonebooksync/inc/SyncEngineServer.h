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
// Header file for the Background Sync Engine server. The engine provides
// the ability to perform Phonebook Sync's long running operations (e.g. Sync,
// write contact and delete contact).
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCENGINESERVER_H__
#define __SYNCENGINESERVER_H__

#include <commsdattypesv1_1.h>
#include <e32property.h>
#include <etelsat.h>
#include <f32file.h>

#include "common.h"


/**
 *  Name of the engine. The '!' means it is a protected server.
 */
_LIT(PHBKSYNC_ENGINE_NAME, "!PhBkSyncEngine");


//
// Template field lengths...
//
const TInt KTemplateFieldLength  = 16; 
const TInt KTemplateNumberLength = 8; 


//
// Phonebook data size limits.
//
const TInt KPBDataClientHeap = 900;
const TInt KPBDataClientBuf  = 512;


/**
 *  Number of retries to open, create or write to the DB.
 */
const TInt KMaxDbAccessRetryCount = 3;


class CSyncEngineSession;
class CSyncContactsWithICC;
class CWriteContactToICC;
class CDeleteContactFromICC;
class CPhoneBookSyncEngineStarter;


/**
 *  Background Sync Engine server class.
 */
class CSyncEngineServer : public CPolicyServer
	{
	friend class CPhoneBookSyncEngineStarter;
		
public:
	static CSyncEngineServer* NewL(CPhoneBookManager& aPhoneBookManager);

	void AddSessionL(CSyncEngineSession* aSession);
	void DropSession(CSyncEngineSession* aSession);

	void DoSynchronisationL(const RMessage2& aMessage,
							TUid aPhonebookUid);
	void DeleteCntFromICCL(const RMessage2& aMessage,
						   TUid aPhonebookUid, TContactItemId aContactId);
	void WriteCntToICCL(const RMessage2& aMessage,
						TContactItemId aTemplateId, TInt aBufferSize);
	TInt DoSynchronisationCancelL(TUid aPhonebookUid);
	TInt DeleteCntFromICCCancelL(TUid aPhonebookUid);
	TInt WriteCntToICCCancelL(TUid aPhonebookUid);

	TContactItemId WriteICCContactToDBL(CSyncContactICCEntry& aICCEntry);
	void RollbackIccTransaction();
	void CommitIccTransactionL();

	void CompleteDoSync(TInt aRetVal);
	void CompleteWriteContactToICC(TInt aRetVal);
	void CompleteDoIccDelete(TInt aRetVal);

	inline TBool IsV2Tsy();

private:
	CSyncEngineServer(CPhoneBookManager& aPhoneBookManager);
	~CSyncEngineServer();

	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2&/*aMessage*/) const;

	void ConfigureEngineL();
	void UnconfigureEngineL();
	void ConnectToEtelL();

	TInt RunError(TInt aError);
	CContactItemField* AddTextFieldToIccContactL(TStorageType aType, TFieldType aFieldType, TUid aMapping, TDesC& aField, CContactICCEntry* aIccEntry, TInt aCount);
	void CreateTemplateIdL(TUid aPhonebookUid, TUint32 aICCCaps);
	void CreateGroupIdL(TUid aPhonebookUid);
	void CreatePhoneBookIdsL();
	void AssignAlphaTagsToFieldTypeL(const TDesC16 &aTextTag, CContactItemField *apCurrField);
 	TBool EntriesIdenticalL(CSyncContactICCEntry& aICCEntry, TContactItemId& aId);
	void RemoveAllContactsForPhoneBookL(TUid aPhonebookUid);

	RTelServer  iEtelServer;
	TUint32  iICCCaps;
	RMobilePhone  iPhone;
	CContactDatabase*  iDb;

	CPhoneBookSyncEngineStarter*  iPhoneBookSyncEngineStarter;
	
	CSyncEngineSession*  iConnectedSession;
	CPhoneBookManager&  iPhonebookManager;

	TBool  iIsInTransaction;
	TBool  iIsV2Tsy;
	
	//
	// Pointers to the worker Active Objects...
	//
	CSyncContactsWithICC*  iSyncContactsWithICC;
	CWriteContactToICC*  iWriteContactToICC;
	CDeleteContactFromICC*  iDeleteContactFromICC;

	//
	// Policy Server Tables (naturally the same as CPhoneBookServer)...
	//
	static const TInt iRanges[];
	static const TUint8 iElementsIndex[];
	static const CPolicyServer::TPolicyElement iElements[];
	static const CPolicyServer::TPolicy iPolicy;
	};  


/**
 *  Simple Active Object to start the configuration of the engine.
 *  This is used to break any deadlock between CntModel starting
 *  PhoneBookSync and PhoneBookSync starting CntModel.
 */
class CPhoneBookSyncEngineStarter : public CAsyncOneShot
	{
public:
	CPhoneBookSyncEngineStarter(CSyncEngineServer& aEngine);

private:
	virtual void RunL();
	
	CSyncEngineServer&  iEngine;
	};


/**
 *  Specialised Active Scheduler to cleanly handle leaves of the Active
 *  Objects.
 */
class CSyncEngineScheduler : public CActiveScheduler
	{
public:
	static CSyncEngineScheduler* New();
	void Error(TInt aError) const;
	};


/**
 *  Indicates whether V2 functionality is supported by the TSY.
 *
 *  @return Boolean indicating whether V2 functionality is supported by the
 *          phone.
 */
inline TBool CSyncEngineServer::IsV2Tsy()
	{
	return iIsV2Tsy;
	} // CSyncEngineServer::IsV2Tsy


#endif // __SYNCENGINESERVER_H__

