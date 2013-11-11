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
* Description:  Map implementation header.
*
*/


#ifndef __MAP_H__
#define __MAP_H__

/****************************************************************
This template class represent a hash map
USE ONLY POINTERS TO CLASSES!!!
INSERTION TRANSFERS THE OWNERSHIP OF THE INSERTED POINTER!!!

For example the correct interger key and value insertion is:
static void InsertEntryL(RMap<TInt64, TInt64>& aMap, const TInt aKey,const TInt aValue)
{
	TInt64* k64 = new TInt64;
	*k64 = aKey;
	TInt64* v64 = new TInt64;
	*v64 = aValue;
		
	aMap.InsertL(k64, v64);
}
****************************************************************/

#include "SafePointerArray.h"

template <class Tkey, class Tvalue>
class RMap
{
public:
	inline RMap(const TLinearOrder<Tkey>& anOrder);
	inline void ResetAndDestroy();
	inline void Close();
	
	//insert related functions
	inline void InsertL(const Tkey* aKey, const Tvalue* aValue);
	inline const Tvalue* InsertReplaceL(const Tkey* aKey, const Tvalue* aValue);

	//check if there is an element with this key
	inline TBool ContainsKey(const Tkey& aKey) const;
	inline TBool ContainsValue(const Tvalue& aValue) const;
	inline TInt Find(const Tkey& aKey) const;

	//value by key
	inline Tvalue& GetValueL(const Tkey& aKey) const;
	//key by value
	inline Tkey& GetKeyL(const Tvalue& aValue) const;

	//remove the element with this key
	inline void RemoveL(const Tkey& aKey);
	inline void RemoveAtL(TInt anIndex, Tkey*& aKey, Tvalue*& aValue);

	RSafePointerArray<Tkey>& Keys();
	RSafePointerArray<Tvalue>& Values();

	//key by index
	inline Tkey& KeyAt(TInt anIndex);
	inline const Tkey& KeyAt(TInt anIndex) const;
	//value by index
	inline Tvalue& ValueAt(TInt anIndex);
	inline const Tvalue& ValueAt(TInt anIndex) const;

	//length
	inline TInt Count() const;

	static TInt CompareAlphabetDes(const TDesC& aFirst,const TDesC& aSecond)
	{
		if (aFirst < aSecond)
		{
			return -1;
		}

		if (aFirst > aSecond)
		{
			return 1;
		}
		
		return 0;
	}
	
	static TInt CompareAlphabetDes8(const TDesC8& aFirst,const TDesC8& aSecond)
	{
		if (aFirst < aSecond)
		{
			return -1;
		}

		if (aFirst > aSecond)
		{
			return 1;
		}
		
		return 0;
	}
	
	static TInt CompareInt(const TInt64& aFirst,const TInt64& aSecond)
	{
		if (aFirst < aSecond)
		{
			return -1;
		}

		if (aFirst > aSecond)
		{
			return 1;
		}
		
		return 0;
	}
protected:
	TLinearOrder<Tkey> iOrder;
	RSafePointerArray<Tkey> iKeys;
	RSafePointerArray<Tvalue> iValues;
};
/*
template <class T>
class CMap: public CArrayFixFlat<T>
{
public:
	CMap(const TKeyArrayFix& aKey);
	inline void Reset();
	
	//insert related functions
	inline void InsertL(const T& anEntry);
	inline Tvalue InsertReplaceL(const T& anEntry);
	
	//check if there is an element with this key
	TBool ContainsKey(const Tkey& aKey);

	//value by key
	Tvalue& GetValueL(const Tkey& aKey);
	//key by value
	Tkey& GetKeyL(const Tvalue& aValue);

	//remove the element with this key
	void RemoveL(const Tkey& aKey);

	RArray<Tkey>& Keys();
	RArray<Tvalue>& Values();

	//key by index
	Tkey& KeyAt(TInt anIndex);
	const Tkey& KeyAt(TInt anIndex) const;
	//value by index
	Tvalue& ValueAt(TInt anIndex);
	const Tvalue& ValueAt(TInt anIndex) const;

	//length
	inline TInt Count() const;
	
protected:
	TKeyArrayFix& iKey;
};

*/

#include "Map.inl"

#endif //__MAP_H__
