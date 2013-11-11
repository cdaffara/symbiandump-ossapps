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
* Description:  RPointerArray with remove + memory freeing.
*
*/

#ifndef __SAFEPOINTERARRAY_H__
#define __SAFEPOINTERARRAY_H__

#include <e32std.h>

template <class T>
class RSafePointerArray : public RPointerArray<T>
{
public:
	void Remove(TInt aIndex)
		{
		T* pointer = RPointerArray<T>::operator[](aIndex);
		RPointerArray<T>::Remove(aIndex);
		delete pointer;
		}
};

#endif //__SAFEPOINTERARRAY_H__
