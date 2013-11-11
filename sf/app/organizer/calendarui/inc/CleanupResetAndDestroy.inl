/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/


#include "CleanupResetAndDestroy.h"


/**
 *  Creates a TCleanupItem for the specified object.
 *  The cleanup operation is the private static function ResetAndDestroy() of this class.
 *  @param aRef The object for which a TCleanupItem is to be constructed.
 **/
template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
{
    CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
}

/**
 *  The cleanup operation to be performed.
 *  @param aPtr A pointer to the object for which clean up is to be performed. 
 *              The implementation calls ResetAndDestroy() on this object.
 **/
template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
{
    static_cast<T*>( aPtr )->ResetAndDestroy();
}

/**
 *  See header file CleanupResetAndDestroy.h for in-source comment.
 *
 **/
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
{
    CleanupResetAndDestroy<T>::PushL( aRef );
}


// End of File
