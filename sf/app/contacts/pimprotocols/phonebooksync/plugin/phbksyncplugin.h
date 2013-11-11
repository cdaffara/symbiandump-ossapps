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
// Header file for the Phonebook Synchroniser Plug-in.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHBKSYNCPLUGIN_H__
#define __PHBKSYNCPLUGIN_H__

#include <e32std.h>
#include <cntsync.h>
#include <cntitem.h>

#include "phbksync.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntsyncecom.h>
#endif
/**
 *  Concrete implementation of Contacts Phonebook Synchroniser.
 *
 *  @internalComponent
 */
class CPhoneBookSyncPlugin : public CContactSynchroniser
	{
public: //From CContactSynchroniser
	TInt ValidateContact(TValidateOperation aOp, TContactItemId aId);
	TInt ValidateWriteContact(CContactICCEntry& aContactItem);
	TInt DeleteContact(TContactItemId aId);
	void Release();
	void UpdatePostWriteL(const CContactICCEntry& aContactItem);
	TContactItemId ICCTemplateIdL(TUid aPhonebookUid);
	TContactItemId GroupIdL(TUid aPhonebookUid);
	void NotifySyncStateChange(TRequestStatus& aStatus, TUid aPhonebookUid);
	void CancelNotifyRequest(TUid aPhonebookUid);
	TBool IsSynchronisedL(TUid aPhonebookUid);
	TInt PhonebookList(RArray<TUid>& aPhonebookList);

	static CPhoneBookSyncPlugin* New();
	static CPhoneBookSyncPlugin* NewL();
private:
	void ConstructL();
	TInt ICCSlotNumber(const CContactICCEntry& aContactItem) const;
	TUid ICCPhonebookL(CContactICCEntry& aContactItem) const;
	void AddFieldValueL(CContactICCEntry& aContactItem, TFieldType aFieldType, TDesC& aField);

	RPhoneBookSession iSession;

#ifndef SYMBIAN_CNTMODEL_V2
	TBool iInServerThread;
#endif // SYMBIAN_CNTMODEL_V2
	};

#endif // __PHBKSYNCPLUGIN_H__
