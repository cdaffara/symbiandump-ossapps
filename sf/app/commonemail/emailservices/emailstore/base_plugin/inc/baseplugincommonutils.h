/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Utilities.
*
*/



#ifndef __COMMONUTILS_H__
#define __COMMONUTILS_H__

#include <e32base.h>

//off imsutils.
template <class T> class CleanupResetAndDestroyClose
    {
public:
    inline static void PushL(T& aRef);
private:
    static void Close(TAny *aPtr);
    };

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef);

template <class T> inline void CleanupResetAndDestroyClose<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&Close, &aRef));
    }

template <class T> void CleanupResetAndDestroyClose<T>::Close(TAny *aPtr)
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    static_cast<T*>(aPtr)->Close();
    }

template <class T> inline void CleanupResetAndDestroyClosePushL(T& aRef)
    {
    CleanupResetAndDestroyClose<T>::PushL(aRef);
    }

/**
* Cleanup support for owning pointer arrays
*/

template<class T>
class CleanupResetAndDestroy
{
private:
    inline static void ResetAndDestroy( TAny *aPtr )
    {
    reinterpret_cast<T*>( aPtr )->ResetAndDestroy();
    };
public:
    inline static void PushL( T& aItem )
    { 
    TCleanupItem item( &CleanupResetAndDestroy::ResetAndDestroy, &aItem );
    CleanupStack::PushL( item );
    };
} ;

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }
#endif
