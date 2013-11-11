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
// Header file for Phonebook Synchroniser INI File class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __PHONEBOOKINIFILE_H__
#define __PHONEBOOKINIFILE_H__

#include "phbksync.h"
#include "phbksynclog.h"


/**
 *  Maximmum number of phonebooks supported by a Phonebook Sync.
 *
 *  @internalComponent
 */
const TInt KMaxPhonebooks = 5;


class CPhoneBookManager;

/**
 *  This class represents the Phonebook Sync INI file and contains
 *  functions to read and write to the file.
 *
 *  The class uses a caching system to avoid writing the file when
 *  no changes have take place.
 *
 *  @internalComponent
 */
class CPhoneBookIniFile : public CBase
	{
public:
	static CPhoneBookIniFile* NewL(RPointerArray<CPhoneBook>& aPhonebookList);
	~CPhoneBookIniFile();

	TInt WriteIniFileSettingsIfNeeded();

private:
	CPhoneBookIniFile(RPointerArray<CPhoneBook>& aPhonebookList);

	void ConstructL();
	TBool CheckPhonebookParamsForChanges() const;
	void GetIniFileName(RFs& aFs, TDes& aIniFileName) const;
	void WriteIniFileL() const;
	void ReadIniFileL() const;
	void UpdatePhonebookParamsCache();

private:
	RPointerArray<CPhoneBook>&  iPhonebookList;
	TBool  iIsIniFileCacheValid;
	RPhoneBookSession::TPhonebookSyncMode  iIniFileSyncModeCache[KMaxPhonebooks];
	TContactItemId  iIniFileTemplateIdCache[KMaxPhonebooks];
	TContactItemId  iIniFileGroupIdCache[KMaxPhonebooks];
	RMobilePhoneBookStore::TMobilePhoneBookIdentity  iIniFileIdentityCache[KMaxPhonebooks];
	};


#endif // __PHONEBOOKINIFILE_H__
