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
// Contains the implementation for the Active Object class used by the
// Phonebook Synchroniser to delete an ICC Contact.
// 
//

/**
 @file
 @internalComponent
*/

#include "common.h"
#include "Phonebook.h"
#include "PhonebookManager.h"
#include "SyncContactICCEntry.h"
#include "phbksyncsvr.h"
#include "phbksynclog.h"
#include "SyncEngineSession.h"
#include "SyncEngineServer.h"
#include "phbksyncsess.h"
#include "DeleteContactFromICC.h"


/**
 *  Static factory method used to create a CDeleteContactFromICC object.
 *
 *  @param aSession           Reference to the engine session.
 *  @param aPhonebookManager  Reference to the Phonebook Manager which stores
 *                            all the phonebook parameters.
 *  @param aDb                Reference to the database.
 *  @param aPhone             Handle to the phone object.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 */
CDeleteContactFromICC* CDeleteContactFromICC::NewL(CSyncEngineSession& aSession,
												   CPhoneBookManager& aPhonebookManager,
												   CContactDatabase& aDb,
												   RMobilePhone& aPhone,
												   TUid aPhonebookUid,
												   const RMessage2& aClientMessage)
	{
	CDeleteContactFromICC*  self = new(ELeave) CDeleteContactFromICC(aSession,
																	 aPhonebookManager,
																	 aDb,
																	 aPhone,
																	 aPhonebookUid,
																	 aClientMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CDeleteContactFromICC::NewL


/**
 *  Standard constructor.
 *
 *  @param aSession           Reference to the engine session.
 *  @param aPhonebookManager  Reference to the Phonebook Manager which stores
 *                            all the phonebook parameters.
 *  @param aPhone             Handle to the phone object.
 *  @param aDb                Reference to the database.
 *  @param aPhonebookUid      Phonebook UID for the contact to be deleted from.
 *  @param aClientMessage     Handle to the engine message request.
 */
CDeleteContactFromICC::CDeleteContactFromICC(CSyncEngineSession& aSession,
											 CPhoneBookManager& aPhonebookManager,
											 CContactDatabase& aDb,
											 RMobilePhone& aPhone,
											 TUid aPhonebookUid,
											 const RMessage2& aClientMessage)
  : CActive(EPriorityNormal),
	iSession(aSession),
	iPhonebookManager(aPhonebookManager),
	iDb(aDb),
	iPhone(aPhone),
	iPhonebookUid(aPhonebookUid),
	iClientMessage(aClientMessage),
	iState(EDeleteContactFromICCStateIdle),
	iContactId(KNullContactId),
	iSlotNum(0)
	{
	// NOP
	} // CDeleteContactFromICC::CDeleteContactFromICC


/**
 *  Standard destructor. This will cancel any pending active requests.
 */
CDeleteContactFromICC::~CDeleteContactFromICC()
	{
	LOGACTIVE1(_L8("~CDeleteContactFromICC()"));

 	Cancel();
	} // CDeleteContactFromICC::~CDeleteContactFromICC


/**
 *  Second phase constructor.
 */
void CDeleteContactFromICC::ConstructL()
	{
	User::LeaveIfError(iPhonebookManager.GetPhoneBookStore(iPhonebookUid, iPhone, iPhonebookStore));

	CActiveScheduler::Add(this);
	} // CDeleteContactFromICC::ConstructL


/** 
 *  Delete an ICC entry.
 *
 *  @param aContactId  TContactItemId item to delete from the ICC.
 */
void CDeleteContactFromICC::DoIccDelete(TContactItemId aContactId)
	{
	LOGACTIVE2(_L8("CDeleteContactFromICC::DoIccDelete(0x%08x)"), aContactId);

	//
	// Check that we are not in use!
	//
	if (iState != EDeleteContactFromICCStateIdle)
		{
		PhBkSyncPanic(EPhBkSyncPanicDoIccDeleteError);
		}

	//
	// Store the Contact UID for later and find the slot number.
	//
	TInt  result;
	
	iContactId = aContactId;

	result = iPhonebookManager.GetSlotNumFromContactId(iPhonebookUid, aContactId, iSlotNum);
	if (result != KErrNone)
		{
		iSession.Server().CompleteDoIccDelete(result);
		return;
		}

	//
	// Request the slot entry to be deleted from the phonebook store...
	//
	iPhonebookStore.Delete(iStatus, iSlotNum);
	iState = EDeleteContactFromICCStateWaitForDelete;
	SetActive();

	LOGACTIVETIMESTAMP();
	} // CDeleteContactFromICC::DoIccDelete


/** 
 *  Standard Active Object RunL() method to process delete requests.
 */
void CDeleteContactFromICC::RunL()
	{
	LOGACTIVE2(_L8("CDeleteContactFromICC::RunL(): iStatus=%d"),
			   iStatus.Int());
	LOGACTIVETIMESTAMP();

	switch (iState)
		{
		case EDeleteContactFromICCStateWaitForDelete:
			{
			if (iStatus.Int() == KErrNone  ||
				iStatus.Int() == KErrCancel)
				{
				//
				// Update the entry in the table...
				//
				TInt  result;
				
				result = iPhonebookManager.UpdateEntryInTable(iPhonebookUid, iSlotNum,
															  KNullContactId, ESlotEmpty);
 				if (result != KErrNone)
 					{
 					PhBkSyncPanic(EPhBkSyncPanicUpdateEntryInTableError);
 					}

				//
				// Decrement the number of used entries for this phonebook...
				//
				result = iPhonebookManager.AdjustPhoneBookInfoUsedEntries(iPhonebookUid, -1);
 				if (result != KErrNone)
 					{
 					PhBkSyncPanic(EPhBkSyncPanicUpdateEntryInTableError);
 					}
 				}

			//
			// Return to the idle state and complete the request...
			//
			iState = EDeleteContactFromICCStateIdle;
			iSession.Server().CompleteDoIccDelete(iStatus.Int());
			}
			break;

		case EDeleteContactFromICCStateWaitForCancel:
			{
			//
			// Return to the idle state and complete the request...
			//
			iState = EDeleteContactFromICCStateIdle;
			iSession.Server().CompleteDoIccDelete(iStatus.Int());
			}
			break;

		default:
			{
			PhBkSyncPanic(EPhBkSyncPanicDoIccDeleteError);
			}
			break;
		}
	} // CDeleteContactFromICC::RunL


/** 
 *  Standard Active Object DoCancel method called when the objects Cancel()
 *  method is called.
 */
void CDeleteContactFromICC::DoCancel()
	{
	LOGACTIVE1(_L8("CDeleteContactFromICC::DoCancel()"));

 	if (iState == EDeleteContactFromICCStateWaitForDelete)
		{
		iPhonebookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
 		iState = EDeleteContactFromICCStateWaitForCancel;
		}
	} // CDeleteContactFromICC::DoCancel


/**
 *  Standard Active Object RunError method called when the objects RunL()
 *  method leaves.
 *
 *  Hopefully this method should never be called.
 *
 *  @param aError  Leave code from the RunL().
 *
 *  @return  KErrNone is returned although the server will panic first.
 */
TInt CDeleteContactFromICC::RunError(TInt aError)
	{
#ifdef _DEBUG
	LOGACTIVE2(_L8("CDeleteContactFromICC::RunError(%d)"), aError);
#else
	(void) aError;
#endif

	PhBkSyncPanic(EPhBkSyncPanicUnexpectedLeave);

	return KErrNone;	
	} // CDeleteContactFromICC::RunError

