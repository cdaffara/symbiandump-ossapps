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
// Header file for the Active Object based class which synchronises the
// Contacts DB with the ICC.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCCONTACTWITHICC_H__
#define __SYNCCONTACTWITHICC_H__

#include <e32base.h>
#include <mpbutil.h>
#include <etelmm.h>
#include <etelsat.h>
#include <satcs.h>

#include "phbksyncsvr.h"
#include "SyncEngineServer.h"


/**
 *  This class is responsible for implementing the state machine required
 *  to perform the synchronisation of the Contacts DB with the ICC.
 */
class CSyncContactsWithICC : public CActive
	{
private:
	enum TSyncContactsWithICCState
		{
		ESyncContactsWithICCStateIdle,
		ESyncContactsWithICCStateWaitForRead,
		ESyncContactsWithICCStateRemoveUnconfirmedEntries,
		ESyncContactsWithICCStateWaitForCancel
		};

public:
	static CSyncContactsWithICC* NewL(CSyncEngineSession& aSession, 
		                              CPhoneBookManager& aPhonebookManager,
		                              CContactDatabase& aDb,
		                              RMobilePhone& aPhone,
									  TUid aPhonebookType,
									  const RMessage2& aClientMessage);
	~CSyncContactsWithICC();

	void SyncContactsWithICCL();
	void DecodeICCAndStoreToDbL(TInt& aIndex, TInt& aReceivedEntries);

	inline TUid PhonebookUid() const;
	inline const RMessage2& ClientMessage() const;

	void DoCancel();

protected:
	TInt RunError(TInt aError);

private:
	CSyncContactsWithICC(CSyncEngineSession& aSession,
						 CPhoneBookManager& aPhonebookManager,
						 CContactDatabase& aDb,
						 RMobilePhone& aPhone,
						 TUid aPhonebookUid,
						 const RMessage2& aClientMessage);
	void ConstructL();
	void RunL();

	void ReadICCEntries();
	void YieldToOtherActiveObjects();
	RMobilePhone::TMobileTON ConvertTypeOfNumber(TUint8 aValue) const;

private:
	CSyncEngineSession&  iSession;
	CPhoneBookManager&  iPhonebookManager;
	CContactDatabase&  iDb;
	RMobilePhone&  iPhone;
	TUid  iPhonebookUid;
	RMessage2  iClientMessage;

	TSyncContactsWithICCState  iState;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5  iPhBkInfo;
	RMobilePhoneBookStore  iPhonebookStore;
	HBufC8*  iPhonebookData;
	TPtr8  iPhonebookDataPtr;
	TInt  iReceivedEntries;
	TInt  iNumOfEntries;
	TInt  iSlotNum;
	TInt  iDbAccessRetryCount;
	TInt  iICCReadRetryCount;
	RTimer  iTimer;
	RArray<TInt>  iUnconfirmedEntries;
	};


/**
 *  Return the ICC phonebook UID. 
 *
 *  @return The UID of the phonebook being used by this Active Object.
 */
inline TUid CSyncContactsWithICC::PhonebookUid() const
	{
	return iPhonebookUid;
	} // CSyncContactsWithICC::PhonebookUid


/**
 *  Return the client message handle. 
 *
 *  @return  A reference to the client message request being used by this
 *  Active Object.
 */
inline const RMessage2& CSyncContactsWithICC::ClientMessage() const
	{
	return iClientMessage;
	} // CSyncContactsWithICC::ClientMessage


#endif // __SYNCCONTACTWITHICC_H__
