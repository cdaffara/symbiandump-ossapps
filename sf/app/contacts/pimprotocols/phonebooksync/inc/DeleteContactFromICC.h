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
// Header file for the Active Object responsible for deleting an ICC Contact.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __DELETECONTACTFROMICC_H__
#define __DELETECONTACTFROMICC_H__

#include <e32base.h>
#include <mpbutil.h>

#include "SyncEngineServer.h"


/**
 *  This class is responsible for implementing the state machine required to delete
 *  a phonebook entry from ICC.
 */
class CDeleteContactFromICC : public CActive
	{
private:
	enum TDeleteContactFromICCState
		{
		EDeleteContactFromICCStateIdle,
		EDeleteContactFromICCStateWaitForDelete,
		EDeleteContactFromICCStateWaitForCancel
		};

public:
	static CDeleteContactFromICC* NewL(CSyncEngineSession& aSession,
									   CPhoneBookManager& aPhonebookManager,
									   CContactDatabase& aDb,
									   RMobilePhone& aPhone,
									   TUid aPhonebookUid,
									   const RMessage2& aClientMessage);
	~CDeleteContactFromICC();
	void ConstructL();

	void DoIccDelete(TContactItemId aContactId);

	inline TUid PhonebookUid() const;
	inline const RMessage2& ClientMessage() const;
	
	void DoCancel();

protected:
	void RunL();
	TInt RunError(TInt aError);

private:
	CDeleteContactFromICC(CSyncEngineSession& aSession,
						  CPhoneBookManager& aPhonebookManager,
						  CContactDatabase& aDb,
						  RMobilePhone& aPhone,
						  TUid aPhonebookUid,
						  const RMessage2& aClientMessage);

	CSyncEngineSession&  iSession;
	CPhoneBookManager&  iPhonebookManager;
	CContactDatabase&  iDb;
	RMobilePhone&  iPhone;
	TUid  iPhonebookUid;
	RMessage2  iClientMessage;

	RMobilePhoneBookStore  iPhonebookStore;
	TDeleteContactFromICCState  iState;
	TContactItemId  iContactId;
	TInt  iSlotNum;
	};


/**
 *  Return the ICC phonebook UID. 
 *
 *  @return The UID of the phonebook being used by this Active Object.
 */
inline TUid CDeleteContactFromICC::PhonebookUid() const
	{
	return iPhonebookUid;
	} // CDeleteContactFromICC::PhonebookUid


/**
 *  Return the client message handle. 
 *
 *  @return  A reference to the client message request being used by this
 *  Active Object.
 */
inline const RMessage2& CDeleteContactFromICC::ClientMessage() const
	{
	return iClientMessage;
	} // CDeleteContactFromICC::ClientMessage


#endif // __DELETECONTACTFROMICC_H__
