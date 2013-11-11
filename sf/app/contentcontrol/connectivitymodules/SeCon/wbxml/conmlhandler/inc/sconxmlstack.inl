/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONXMLSTACK_INL__
#define __SCONXMLSTACK_INL__


// ------------------------------------------------------------------------------------------------
// CXMLStack
// ------------------------------------------------------------------------------------------------
template<class T>
inline CXMLStack<T>::CXMLStack()
	{
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline CXMLStack<T>::~CXMLStack()
	{
	iStack->ResetAndDestroy();
	delete iStack;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline CXMLStack<T>* CXMLStack<T>::NewL()
	{
	CXMLStack<T>* self = new (ELeave) CXMLStack<T>();
	CleanupStack::PushL(self);
	self->iStack = new (ELeave) RPointerArray<T>();
	CleanupStack::Pop(self);
	return self;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline T* CXMLStack<T>::Pop()
	{
	T* temp = iStack->operator[](iStack->Count() - 1);
	iStack->Remove(iStack->Count() - 1);
	return temp;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline T* CXMLStack<T>::Top()
	{
	if( Count() > 0 )
		{
		return iStack->operator[](iStack->Count() - 1);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CXMLStack<T>::PushL( T* aItem )
	{
	iStack->AppendL(aItem);
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline TInt CXMLStack<T>::Count()
	{
	return iStack->Count();
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CXMLStack<T>::Reset()
	{
	iStack->Reset();
	}

// ------------------------------------------------------------------------------------------------
template<class T>
inline void CXMLStack<T>::ResetAndDestroy()
	{
	iStack->ResetAndDestroy();
	}

#endif  // __SCONXMLSTACK_INL__

