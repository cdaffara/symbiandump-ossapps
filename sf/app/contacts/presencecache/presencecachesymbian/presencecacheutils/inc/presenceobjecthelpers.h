/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Templated algorithms to simplify object handling
*
*/

#ifndef PRESENCEOBJECTHELPERS_H
#define PRESENCEOBJECTHELPERS_H

#include <s32mem.h>

class MPresenceBuddyInfo2;

// const TInt KObjectBufGranurality = 100;


class TPresenceArrayPacker
    {
public:
    /**
     * Pack array of buffers to buffer.
     * @param aBufferArray Array of buffers to pack.
     * @return Buffer containing buffers.
     */
    IMPORT_C static HBufC8* PackArrayL( const RPointerArray< MPresenceBuddyInfo2 >& aBufferArray );
    
    /**
     * Unpack array of buffers to array.
     * @param aBufferArray. Target array for new buffers. Ownhersip of items in array is transferred to caller.
     * @param aPack Packet containing array of buffers.
     */
    IMPORT_C static void UnPackArrayL( RPointerArray< MPresenceBuddyInfo2 >& aBufferArray, const TDesC8& aPack  );
    };

class PresenceCacheBuddyUtils
    {
public:
    /**
     * Reset buddy info expiry time stamp
     */
    IMPORT_C static void ResetBuddyInfoTimeStamp(MPresenceBuddyInfo2& aBuddy);

    /**
     * Check is buddy info expired
     */
    IMPORT_C static TBool IsBuddyInfoExpired(const MPresenceBuddyInfo2& aBuddy);
private:
    // Hide constructor and destructor
    PresenceCacheBuddyUtils();
    ~PresenceCacheBuddyUtils();
    };

// CleanupStack helpers for item owning RPointerArrays
template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef)
        { CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef)); }
private:
    inline static void ResetAndDestroy(TAny *aPtr)
        { static_cast<T*>(aPtr)->ResetAndDestroy(); }
    };

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
    { CleanupResetAndDestroy<T>::PushL(aRef); }
#endif // PRESENCEOBJECTHELPERS_H
