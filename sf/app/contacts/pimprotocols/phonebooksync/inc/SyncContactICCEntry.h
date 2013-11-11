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
// Header file for Phonebook Synchroniser internal ICC Entry representation
// class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNCCONTACTICCENTRY_H__
#define __SYNCCONTACTICCENTRY_H__


#include <s32mem.h>
#include "phbksync.h"
#include <etelmm.h>


/**
 *  This class is used to store the contacts phonebook data (supplied in
 *  CContactICCEntry) in the Phonebook Server internal format. This class
 *  is needed as CContactICCEntry also contains some data used only by the
 *  Contacts Model and not needed by the Synchroniser at all. Additonally this
 *  class supports streaming.
 */
class CSyncContactICCEntry : public CBase
	{
public:
	enum 
		{
		KMaxPhBkNameSize = 241
		};

	typedef TBuf16<KMaxPhBkNameSize> TSyncEntryName;
	typedef TBuf16<RMobilePhone::KMaxMobileTelNumberSize> TSyncEntryNumber;

	class TSyncAdditionalNumber
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;

		TSyncEntryNumber  iNumber;		 // Additional number 
		TSyncEntryName  iNumberString;	 // Alpha string associated with additional number 
		RMobilePhone::TMobileTON  iTON;  // TON associated with additional number
		};

public:
	static CSyncContactICCEntry* NewL();
	~CSyncContactICCEntry();

	CBufBase* StoreL();
	void StoreL(TDes8& aDes);
	void RestoreL(const TDesC8& aBuf);

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

	void InitialiseFromContactICCEntryL(const CContactICCEntry& aContactItem);
	void Reset();

#ifdef _DEBUG
	void LogSyncContactICCEntry() const;
#endif

private:
	CSyncContactICCEntry();
	void ConstructL();

public:
	TSyncEntryName  iName;        // First representation of name
	TSyncEntryName  iSecondName;  // Second representation of name
	TSyncEntryNumber iNumber;
	CArrayFixFlat<TSyncAdditionalNumber>*  iNumberList;
	CArrayFixFlat<TSyncEntryName>*  iGroupList;
	CArrayFixFlat<TSyncEntryName>*  iEmailList;
	TInt  iSlotNum;
	
	//
	// Type-Of-Number associated with iNumber...
	//
	RMobilePhone::TMobileTON  iTON;

	TContactItemId  iContactId;
	TUid  iPhonebookUid;
	TBool  iIsHidden;
	
	//
	// This is needed for new entries only, so that server can map this group
	// ID to phonebook UID. This phonebook UID will be returned back to client.
	//
	TContactItemId  iTemplateId;
	};

#endif // __SYNCCONTACTICCENTRY_H__
