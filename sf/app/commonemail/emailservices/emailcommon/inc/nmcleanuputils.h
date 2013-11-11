/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: General utilities for CleanupStack usage
*
*/

#ifndef NMCLEANUPUTILS_H
#define NMCLEANUPUTILS_H

/**
 * Cleanup support for pointer arrays
 * @since S60 v10.1
   @code
       RPointerArray<HBufC> descArray;
       CleanupResetAndRelease<HBufC>::PushL( descArray );

       // <here some code that might leave>

       CleanupStack::PopAndDestroy();
   @endcode
 */
template<class T>
NONSHARABLE_CLASS( CleanupResetAndDestroy )
{
public:
    /**
     * Cleanup for elements containing Release() method
     */
    inline static void PushL( RPointerArray<T>& aItem ){
        TCleanupItem item( &CleanupResetAndDestroy<T>::ResetAndDestroy, &aItem );
        CleanupStack::PushL( item );
    };

    /**
     * Releases array elements and resets array.
     */
    inline static void ResetAndDestroy( TAny* aPtr ) {
        RPointerArray<T>* array = reinterpret_cast<RPointerArray<T>* >( aPtr );
        array->ResetAndDestroy();
    };
};

#endif // NMCLEANUPUTILS_H
