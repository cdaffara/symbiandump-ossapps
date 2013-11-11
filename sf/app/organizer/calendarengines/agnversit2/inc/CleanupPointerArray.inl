/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   A class for simplifying cleanup of RPointerArrays
*
*/



/**
Pushes an RPointerArray onto the cleanup stack. When popped with PopAndDestroy,
the array will be reset with ResetAndDestroy.
@param aRef The RPointerArray to be cleaned up.
@internalTechnology
*/
template <class T>	
inline void TCleanupPointerArray<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&ResetAndDestroy, &aRef));
	}
	
/**
Takes a pointer to an RPointerArray and calls ResetAndDestroy on it.
@param aPtr The RPointerArray
@internalTechnology
*/
template <class T>
inline void TCleanupPointerArray<T>::ResetAndDestroy(TAny* aPtr)
	{
	reinterpret_cast<T *>(aPtr)->ResetAndDestroy();
	}

/**
Pushes an RPointerArray onto the cleanup stack. When popped with PopAndDestroy,
the array will be reset with ResetAndDestroy.
@param aRef The RPointerArray to be cleaned up.
@internalTechnology
*/
template <class T>
inline void CleanupPointerArrayPushL(T& aRef)
	{
	TCleanupPointerArray<T>::PushL(aRef);
	}

// End of file.