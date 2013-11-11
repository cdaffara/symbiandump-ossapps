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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONXMLSTACK_H__
#define __SCONXMLSTACK_H__

// -----------------------------------------------------------------------------
// CNSmlStack - Stack for WBXML / XML elements
// ----------------------------------------------------------------------------

template <class T>
class CXMLStack  : public CBase
	{
public:
	static inline CXMLStack* NewL();
	inline ~CXMLStack();
        
        /**
        *  Removes latest item from the stack
        * @param none
        * @return none
        */
	inline T* Pop();
	
	/**
        *  Return the latest item from the stack
        * @param none
        * @return the item
        */
	inline T* Top();
	
	/**
        *  Adds an item to the stack
        * @param none
        * @return none
        */
	inline void PushL( T* aItem );
	
	/**
        * Returns the number of items in the stack
        * @param none
        * @return number of items as TInt
        */
	inline TInt Count();
	
	/**
        *  Resets the stack
        * @param none
        * @return none
        */
	inline void Reset();
	
	/**
        *  Resets and destroys the stack
        * @param none
        * @return none
        */
	inline void ResetAndDestroy();


protected:
	inline CXMLStack<T>();

private:
	RPointerArray<T>* iStack;
	};

#include "sconxmlstack.inl"

#endif // __SCONXMLSTACK_H__
