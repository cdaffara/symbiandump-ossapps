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
//

/**
 @file
 @internalComponent
*/

#ifndef _TE_CNTSYNCUTILS_H_
#define _TE_CNTSYNCUTILS_H_

#include <phbksync.h>

class CPhbkIntegrationTestUtility : public CBase
	{
public:
	static CPhbkIntegrationTestUtility* NewL();
	~CPhbkIntegrationTestUtility();
private:
	CPhbkIntegrationTestUtility();
public:
	void ConstructL();
	void CheckGroupMembershipL(CContactICCEntry& aItem, TInt aExpectedMembers);
	void CheckFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry);
	void CheckMinimalFieldCountL(const TUid aPhonebook, CContactICCEntry* aEntry);
	void CheckFieldsL(CContactICCEntry& aItem, TInt aSlotNumber);
	void CheckFieldContentL(CContactItemFieldSet& aFieldset, TFieldType aFieldType, const TDesC& aExpectedContent);
	void ReadContactL(const TUid aPhonebook, TInt aExpectedCount);
	void ReadMinimalContactL(const TUid aPhonebook, TInt aExpectedCount);
	void ChangeNameFieldL(CContactItem& aItem, const TDesC& aModifiedName);
	void ChangeNumberFieldL(CContactItem& aItem, const TDesC& aModifiedNumber);
	void OpenContactL();
	void EditContactL();
	void FindContactsL(const TDesC& aFindString, TFieldType aFieldToSearch, TInt aExpectedMatches);
	void DeleteContactL(const TUid aPhonebook, TInt aIndex);
	void AddContactL(const TDesC& aName, const TDesC& aNumber, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEmail = KNullDesC);
	void AddContactL(const TDesC& aName, const TDesC& aNumber, const TDesC& aSlotNumber, const TUid aPhonebook = KUidIccGlobalAdnPhonebook, const TDesC& aEmail = KNullDesC);
	void AddTextFieldL(TFieldType aFieldType, TUid aMapping, const TDesC& aField, CContactICCEntry& aIccEntry);

public:
	CContactDatabase* iDb;
	RPhoneBookSession iSession;
	};

#endif
