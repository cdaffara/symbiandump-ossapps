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

#include "VersitAdditionalStorage.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "versit_internal.h"
#endif

// User includes
#include <versit.h>

// Constants
const TInt KVersitAdditionalStorageArrayGranularity = 2;

class TVersitAdditionalStorageItem
/** 
@internalComponent
@released
*/
    {
public:
    inline TVersitAdditionalStorageItem(const TAny* aKey, CBase* aValue)
    :   iKey(aKey), iValue(aValue)
        {
        }

public:
    const TAny* iKey;
    CBase* iValue;
    };




CVersitAdditionalStorage::CVersitAdditionalStorage()
:   iStorage(KVersitAdditionalStorageArrayGranularity)
    {
    }


CVersitAdditionalStorage::~CVersitAdditionalStorage()
    {
    const TInt count = iStorage.Count();
    for(TInt i=0; i<count; i++)
        {
        TVersitAdditionalStorageItem& item = iStorage[i];
        delete item.iValue;
        }
    iStorage.Close();
    }


CVersitAdditionalStorage* CVersitAdditionalStorage::NewL()
    {
    return new(ELeave) CVersitAdditionalStorage();
    }


void CVersitAdditionalStorage::AddToStorageL(const TAny* aKey, CBase* aValue)
    {
    __ASSERT_ALWAYS(aValue != NULL, Panic(EVersitPanicNullValueAssignedToAdditionalStorageSlot));

    // Check if an additional value has already been stored for this item
    CBase* value = FromStorage(aKey);
    __ASSERT_ALWAYS(value == NULL, Panic(EVersitPanicAdditionalStorageSlotAlreadyInUse));

    // Create new item
    TVersitAdditionalStorageItem newItem(aKey, aValue);
    User::LeaveIfError(iStorage.Append(newItem));
    }


CBase* CVersitAdditionalStorage::FromStorage(const TAny* aKey)
    {
    CBase* value = NULL;
    TInt index = KErrNotFound;
    TVersitAdditionalStorageItem* item = ItemByKey(aKey, index);
    if  (item)
        {
        value = item->iValue;
        }
    return value;
    }

void CVersitAdditionalStorage::FreeStorage(const TAny* aKey)
    {
    TInt index = KErrNotFound;
    TVersitAdditionalStorageItem* item = ItemByKey(aKey, index);
    if  (item)
        {
        delete item->iValue;
        iStorage.Remove(index);
        }
    }

TVersitAdditionalStorageItem* CVersitAdditionalStorage::ItemByKey(const TAny* aKey, TInt& aIndex)
    {
    TVersitAdditionalStorageItem* foundItem = NULL;
    TVersitAdditionalStorageItem searchFor(aKey, NULL);
    TIdentityRelation<TVersitAdditionalStorageItem> relation(ItemsMatch);
    //
    aIndex = iStorage.Find(searchFor, relation);
    if  (aIndex >= 0)
        {
        foundItem = &iStorage[aIndex];
        }
    //
    return foundItem;
    }

TBool CVersitAdditionalStorage::ItemsMatch(const TVersitAdditionalStorageItem& aLeft, const TVersitAdditionalStorageItem& aRight)
    {
    return aLeft.iKey == aRight.iKey;
    }
