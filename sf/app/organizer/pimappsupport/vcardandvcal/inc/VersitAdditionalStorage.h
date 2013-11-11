// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __VERSITADDITIONALSTORAGE_H__
#define __VERSITADDITIONALSTORAGE_H__

// System includes
#include <e32base.h>

// Classes referenced
class TVersitAdditionalStorageItem;


class CVersitAdditionalStorage : public CBase
/** 
@internalComponent
@released
*/
	{
public:
	static CVersitAdditionalStorage* NewL();
    ~CVersitAdditionalStorage();

private:
    CVersitAdditionalStorage();

public:
    void AddToStorageL(const TAny* aKey, CBase* aValue);
    CBase* FromStorage(const TAny* aKey);
    void FreeStorage(const TAny* aKey);

private:
    TVersitAdditionalStorageItem* ItemByKey(const TAny* aKey, TInt& aIndex);
    static TBool ItemsMatch(const TVersitAdditionalStorageItem& aLeft, const TVersitAdditionalStorageItem& aRight);

private:
    RArray<TVersitAdditionalStorageItem> iStorage;
	};


#endif
