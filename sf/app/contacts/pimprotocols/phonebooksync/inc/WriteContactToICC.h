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
// Header file for the Active Object responsible for writting to the ICC.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __WRITECONTACTTOICC_H__
#define __WRITECONTACTTOICC_H__

#include <e32base.h>
#include <mpbutil.h>
#include <etelmm.h>
#include <etelsat.h>
#include <satcs.h>

#include "phbksyncsvr.h"
#include "SyncEngineServer.h"


/**
 *  This class is responsible for implementing the state machine required to write 
 *  phonebook entries to ICC.
 */
class CWriteContactToICC : public CActive
	{
private:
	enum TPBSyncIccWrite
		{
		EPBSyncIccWriteIdle,
		EPBSyncIccWriteWaitForIccWrite,
		EPBSyncIccWriteWaitForCancel,
		EPBSyncIccWriteDeleteOldSlot
		};

	enum TTypeOfWrite
		{
		EWriteAddNew,
		EWriteEdit,
		EWriteEditSlotNumber
		};

public:
	static CWriteContactToICC* NewL(CSyncEngineSession& aSession,
									TInt aBufferSize,
									CPhoneBookManager& aPhonebookManager,
									RMobilePhone& aPhone,
									TUid aPhonebookUid,
									const RMessage2& aClientMessage);
	~CWriteContactToICC();

	void DoIccWriteL();

	inline TUid PhonebookUid() const;
	inline const RMessage2& ClientMessage() const;
	inline TInt SlotNum() const;

	void DoCancel();

protected:
	void RunL();
	TInt RunError(TInt aError);

private:
	CWriteContactToICC(CSyncEngineSession& aSession,
					   CPhoneBookManager& aPhonebookManager,
					   RMobilePhone& aPhone,
					   TUid aPhonebookUid,
					   const RMessage2& aClientMessage);
	void ConstructL(TInt aBufferSize);

	TUint8 ConvertTonNpiByte(RMobilePhone::TMobileTON aTon);
	void EncodeICCEntryL();


	CSyncEngineSession&  iSession;
	CPhoneBookManager&  iPhonebookManager;
	RMobilePhone&  iPhone;
	TUid iPhonebookUid;
	RMessage2 iClientMessage;

	CBufBase*  iIccEntryBuf;
	TPtr8  iIccEntryPtr;
	CSyncContactICCEntry*  iIccEntry;

	HBufC8*  iPhonebookData;
	TPtr8  iPhonebookDataPtr;
	TInt  iOldSlot;

	TTypeOfWrite  iTypeOfWrite;
	TPBSyncIccWrite  iIccWriteState;
	RMobilePhoneBookStore  iPhonebookStore;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV5  iPhBkInfoV5;
	CPhoneBookBuffer*  iPhonebookBuffer;
	TBool  iIsUsedSlot;
	};


/**
 *  Return the ICC phonebook UID. 
 *
 *  @return The UID of the phonebook being used by this Active Object.
 */
inline TUid CWriteContactToICC::PhonebookUid() const
	{
	return iPhonebookUid;
	} // CWriteContactToICC::PhonebookUid


/**
 *  Return the client message handle. 
 *
 *  @return  A reference to the client message request being used by this
 *           Active Object.
 */
inline const RMessage2& CWriteContactToICC::ClientMessage() const
	{
	return iClientMessage;
	} // CWriteContactToICC::ClientMessage


/**
 *  Return the slot number of the newly written entry. 
 *
 *  @return The slot number.
 */
inline TInt CWriteContactToICC::SlotNum() const
	{
	return iIccEntry->iSlotNum;
	} // CWriteContactToICC::SlotNum


#endif // __WRITECONTACTTOICC_H__
