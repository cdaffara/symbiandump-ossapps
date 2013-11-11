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
* Description:  Map implementation inlines.
*
*/


template <class Tkey, class Tvalue>
inline RMap<Tkey, Tvalue>::RMap(const TLinearOrder<Tkey>& anOrder)
:iOrder(anOrder)
{
}

template <class Tkey, class Tvalue>
inline void RMap<Tkey, Tvalue>::ResetAndDestroy()
{
	iKeys.ResetAndDestroy();
	iValues.ResetAndDestroy();
}

template <class Tkey, class Tvalue>
inline void RMap<Tkey, Tvalue>::Close()
{
	// ResetAndDestroy();
	iKeys.Close();
	iValues.Close();
}

template <class Tkey, class Tvalue>
inline void RMap<Tkey, Tvalue>::InsertL(const Tkey* aKey, const Tvalue* aValue)
{
	TInt pos = Find(*aKey);
	if (pos == KErrNotFound)
	{
		User::LeaveIfError(iKeys.InsertInOrder(aKey, iOrder));
		pos = Find(*aKey);
		User::LeaveIfError(iValues.Insert(aValue, pos));
	} else
	{
		User::LeaveIfError(iKeys.InsertInOrderAllowRepeats(aKey, iOrder));
		
		while (*(iKeys[pos]) == *aKey)
		{
			pos++;
		}
		
		//our position should be
		pos--;
		
		User::LeaveIfError(iValues.Insert(aValue, pos));
	}
}

template <class Tkey, class Tvalue>
inline const Tvalue* RMap<Tkey, Tvalue>::InsertReplaceL(const Tkey* aKey, const Tvalue* aValue)
{
	TInt pos = Find(*aKey);
	if (pos == KErrNotFound)
	{
		User::LeaveIfError(iKeys.InsertInOrder(aKey, iOrder));
		pos = Find(*aKey);
		User::LeaveIfError(iValues.Insert(aValue, pos));
		return aValue;
	} else
	{
		Tvalue* v = iValues[pos];
		iValues[pos] = const_cast<Tvalue*>(aValue);
		return v;
	}
}

template <class Tkey, class Tvalue>
inline TInt RMap<Tkey, Tvalue>::Find(const Tkey& aKey) const
{
	TInt pos = KErrNotFound;
	for (TInt i = 0; i < iKeys.Count(); i++)
	{
		if (*(iKeys[i]) == aKey)
		{
			pos = i;
			break;
		}
	}

	return pos;
}


template <class Tkey, class Tvalue>
inline TBool RMap<Tkey, Tvalue>::ContainsKey(const Tkey& aKey) const
{
	return (Find(aKey) != KErrNotFound);
}

template <class Tkey, class Tvalue>
inline TBool RMap<Tkey, Tvalue>::ContainsValue(const Tvalue& aValue) const
{
	TInt pos = KErrNotFound;
	for (TInt i = 0; i < iValues.Count(); i++)
	{
		if (*(iValues[i]) == aValue)
		{
			pos = i;
			break;
		}
	}
  return (pos != KErrNotFound);
}


template <class Tkey, class Tvalue>
inline Tvalue& RMap<Tkey, Tvalue>::GetValueL(const Tkey& aKey) const
{
	TInt pos = Find(aKey);
	
	if (pos == KErrNotFound)
	{
		User::Leave(KErrNotFound);
	}
	
	return *(iValues[pos]);
}

template <class Tkey, class Tvalue>
inline Tkey& RMap<Tkey, Tvalue>::GetKeyL(const Tvalue& aValue) const
{
	TInt pos = KErrNotFound;
	for (TInt i = 0; i < iValues.Count(); i++)
	{
		if (*(iValues[i]) == aValue)
		{
			pos = i;
			break;
		}
	}
	
	if (pos == KErrNotFound)
	{
		User::Leave(KErrNotFound);
	}
	
	return *(iKeys[pos]);
}


template <class Tkey, class Tvalue>
inline void RMap<Tkey, Tvalue>::RemoveL(const Tkey& aKey)
{
	TInt pos = Find(aKey);
	if (pos == KErrNotFound)
	{
		User::Leave(KErrNotFound);
	}
	
	iKeys.Remove(pos);
	iValues.Remove(pos);
	iKeys.Compress();
	iValues.Compress();
}

template <class Tkey, class Tvalue>
inline void RMap<Tkey, Tvalue>::RemoveAtL(TInt anIndex, Tkey*& aKey, Tvalue*& aValue)
{
	if (anIndex >= iKeys.Count())
	{
		User::Leave(KErrNotFound);
	}
	
	*aKey = *(iKeys[anIndex]);	
	*aValue = *(iValues[anIndex]);	
	
	iKeys.Remove(anIndex);
	iValues.Remove(anIndex);
	iKeys.Compress();
	iValues.Compress();
}

template <class Tkey, class Tvalue>
inline RSafePointerArray<Tkey>& RMap<Tkey, Tvalue>::Keys()
{
	return iKeys;
}

template <class Tkey, class Tvalue>
inline RSafePointerArray<Tvalue>& RMap<Tkey, Tvalue>::Values()
{
	return iValues;
}
//key by index
template <class Tkey, class Tvalue>
inline Tkey& RMap<Tkey, Tvalue>::KeyAt(TInt anIndex)
{
	return *(iKeys[anIndex]);
}

template <class Tkey, class Tvalue>
inline const Tkey& RMap<Tkey, Tvalue>::KeyAt(TInt anIndex) const
{
	return *(iKeys[anIndex]);
}
//value by index
template <class Tkey, class Tvalue>
inline Tvalue& RMap<Tkey, Tvalue>::ValueAt(TInt anIndex)
{
	return *(iValues[anIndex]);
}

template <class Tkey, class Tvalue>
inline const Tvalue& RMap<Tkey, Tvalue>::ValueAt(TInt anIndex) const
{
	return *(iValues[anIndex]);
}

template <class Tkey, class Tvalue>
inline TInt RMap<Tkey, Tvalue>::Count() const
{
	return iKeys.Count();
}

/*
//for test purposes
class RDesMap: public RMap<TDesC, TDesC>
{
public:
	RDesMap(const TLinearOrder<TDesC>& anOrder)
	:RMap(anOrder)
	{
	}

	void ResetAndDestroy()
	{
		RMap::ResetAndDestroy();
	}
	
	void InsertL(const TDesC* aKey, const TDesC* aValue)
	{	
		RMap::InsertL(aKey, aValue);
	}
	
	const TDesC* InsertReplaceL(const TDesC* aKey, const TDesC* aValue)
	{	
		return RMap::InsertReplaceL(aKey, aValue);
	}

	TInt Find(const TDesC& aKey)
	{	
		return RMap::Find(aKey);
	}

	TBool ContainsKey(const TDesC& aKey)
	{	
		return RMap::ContainsKey(aKey);
	}
	
	void GetValueL(const TDesC& aKey)
	{	
		RMap::GetValueL(aKey);
	}
	
	void GetKeyL(const TDesC& aValue)
	{	
		RMap::GetKeyL(aValue);
	}
	
	void RemoveL(const TDesC& aKey)
	{	
		RMap::RemoveL(aKey);
	}
	
	RSafePointerArray<TDesC>& Keys()
	{	
		return RMap::Keys();
	}
	
	RSafePointerArray<TDesC>& Values()
	{	
		return RMap::Values();
	}
	
	TDesC& KeyAt(TInt anIndex)
	{
		return RMap::KeyAt(anIndex);
	}
	
	const TDesC& KeyAt(TInt anIndex) const
	{
		return RMap::KeyAt(anIndex);
	}
	
	TDesC& ValueAt(TInt anIndex)
	{
		return RMap::ValueAt(anIndex);
	}
	
	const TDesC& ValueAt(TInt anIndex) const
	{
		return RMap::ValueAt(anIndex);
	}
	
	TInt Count()
	{	
		return RMap::Count();
	}
	
	static void InsertEntryL(RDesMap& aMap, const TDesC& aKey,const TDesC& aValue)
	{
		HBufC* k = HBufC::NewL(aKey.Length());
		*k = aKey;
		HBufC* v = HBufC::NewL(aValue.Length());
		*v = aValue;
		
		aMap.InsertL(k, v);
		//MGLOG2(*k, *v);
	}
	
	static void InsertEntryReplaceL(RDesMap& aMap, const TDesC& aKey,const TDesC& aValue)
	{
		HBufC* k = HBufC::NewL(aKey.Length());
		*k = aKey;
		HBufC* v = HBufC::NewL(aValue.Length());
		*v = aValue;
		
		const HBufC* result = static_cast<const HBufC*>(aMap.InsertReplaceL(k, v));
		if (result != v) 
		{
			//MGLOG2(_L("Deleted"), *result);
			delete result;
		}			

		//MGLOG2(*k, *v);
	}
	
	static void DoTest()
	{
		TLinearOrder<TDesC> order(RDesMap::CompareAlphabetDes);
		RDesMap map(order);
		InsertEntryL(map, _L("bbb"), _L("My name is Mud"));
		InsertEntryL(map, _L("aaa"), _L("Hello big brother"));
		InsertEntryL(map, _L("aaa"), _L("Hello big brother 1"));
		InsertEntryL(map, _L("ddd"), _L("Ala bala nica"));
		InsertEntryL(map, _L("aaa"), _L("Hello big brother 2"));
		InsertEntryL(map, _L("cc"), _L("Ala bala"));
		InsertEntryL(map, _L("x"), _L("Ala bala nica turska panica"));
		InsertEntryL(map, _L("fff"), _L("Ala bala nica turska"));

		//MGLOG1(_L("")); //new line

		for (TInt i = 0; i<map.Count(); i++)		
		{
			//MGLOG2(*(map.Keys()[i]), *(map.Values()[i]));			
		}
		
		map.ResetAndDestroy();

		//MGLOG1(_L("")); //new line
		InsertEntryReplaceL(map, _L("bbb"), _L("My name is Mud"));
		InsertEntryReplaceL(map, _L("aaa"), _L("Hello big brother"));
		InsertEntryReplaceL(map, _L("aaa"), _L("Hello big brother 1"));
		InsertEntryReplaceL(map, _L("ddd"), _L("Ala bala nica"));
		InsertEntryReplaceL(map, _L("aaa"), _L("Hello big brother 2"));
		InsertEntryReplaceL(map, _L("cc"), _L("Ala bala"));
		InsertEntryReplaceL(map, _L("x"), _L("Ala bala nica turska panica"));
		InsertEntryReplaceL(map, _L("fff"), _L("Ala bala nica turska"));

		//MGLOG1(_L("")); //new line

		for (TInt i = 0; i<map.Count(); i++)		
		{
			//MGLOG2(*(map.Keys()[i]), *(map.Values()[i]));			
		}
	}

};


//for test purposes
class RIntMap: public RMap<TInt64, TInt64>
{
public:
	RIntMap(const TLinearOrder<TInt64>& anOrder)
	:RMap(anOrder)
	{
	}

	void ResetAndDestroy()
	{
		RMap::ResetAndDestroy();
	}
	
	void InsertL(const TInt64* aKey, const TInt64* aValue)
	{	
		RMap::InsertL(aKey, aValue);
	}
	
	const TInt64* InsertReplaceL(const TInt64* aKey, const TInt64* aValue)
	{	
		return RMap::InsertReplaceL(aKey, aValue);
	}

	TInt Find(const TInt64& aKey)
	{	
		return RMap::Find(aKey);
	}

	TBool ContainsKey(const TInt64& aKey)
	{	
		return RMap::ContainsKey(aKey);
	}
	
	TInt64 GetValueL(const TInt64& aKey)
	{	
		return RMap::GetValueL(aKey);
	}
	
	TInt64 GetKeyL(const TInt64& aValue)
	{	
		return RMap::GetKeyL(aValue);
	}
	
	void RemoveL(const TInt64& aKey)
	{	
		RMap::RemoveL(aKey);
	}
	
	RSafePointerArray<TInt64>& Keys()
	{	
		return RMap::Keys();
	}
	
	RSafePointerArray<TInt64>& Values()
	{	
		return RMap::Values();
	}
	
	TInt64& KeyAt(TInt anIndex)
	{
		return RMap::KeyAt(anIndex);
	}
	
	const TInt64& KeyAt(TInt anIndex) const
	{
		return RMap::KeyAt(anIndex);
	}
	
	TInt64& ValueAt(TInt anIndex)
	{
		return RMap::ValueAt(anIndex);
	}
	
	const TInt64& ValueAt(TInt anIndex) const
	{
		return RMap::ValueAt(anIndex);
	}
	
	TInt Count()
	{	
		return RMap::Count();
	}
	
	static void InsertEntryL(RIntMap& aMap, const TInt aKey,const TInt aValue)
	{
		TBuf<16> k;
		k.Num(aKey);
		
		TBuf<16> v;
		v.Num(aValue);
		
		TInt64* k64 = new TInt64;
		*k64 = aKey;
		TInt64* v64 = new TInt64;
		*v64 = aValue;
		
		aMap.InsertL(k64, v64);
		//MGLOG2(k, v);
	}
	
	static void InsertEntryReplaceL(RIntMap& aMap, const TInt aKey,const TInt aValue)
	{
		TBuf<16> k;
		k.Num(aKey);
		
		TBuf<16> v;
		v.Num(aValue);
		
		TInt64* k64 = new TInt64;
		*k64 = aKey;
		TInt64* v64 = new TInt64;
		*v64 = aValue;
		
		const TInt64* result = static_cast<const TInt64*>(aMap.InsertReplaceL(k64, v64));
		if (result != v64) 
		{
			//MGLOG2(_L("Deleted"), (*result).GetTInt());
			delete result;
		}			

		//MGLOG2(k, v);
	}
	
	static void DoTest()
	{
		TLinearOrder<TInt64> order(RMap::CompareInt);
		RIntMap map(order);
		InsertEntryL(map, 2, 325);
		InsertEntryL(map, 1, 226);
		InsertEntryL(map, 1, 227);
		InsertEntryL(map, 4, 6623);
		InsertEntryL(map, 1, 228);
		InsertEntryL(map, 3, 6534);
		InsertEntryL(map, 9, 7443);
		InsertEntryL(map, 7, 8643);

		//MGLOG1(_L("")); //new line

		for (TInt i = 0; i<map.Count(); i++)		
		{
			TBuf<16> k;
			k.Num(*(map.Keys()[i]));
			
			TBuf<16> v;
			v.Num(*(map.Values()[i]));
			//MGLOG2(k, v);			
		}
		
		map.ResetAndDestroy();

		//MGLOG1(_L("")); //new line
		InsertEntryReplaceL(map, 2, 325);
		InsertEntryReplaceL(map, 1, 226);
		InsertEntryReplaceL(map, 1, 227);
		InsertEntryReplaceL(map, 4, 6623);
		InsertEntryReplaceL(map, 1, 228);
		InsertEntryReplaceL(map, 3, 6534);
		InsertEntryReplaceL(map, 9, 7443);
		InsertEntryReplaceL(map, 7, 8643);

		//MGLOG1(_L("")); //new line

		for (TInt i = 0; i<map.Count(); i++)		
		{
			TBuf<16> k;
			k.Num(*(map.Keys()[i]));
			
			TBuf<16> v;
			v.Num(*(map.Values()[i]));
			//MGLOG2(k, v);			
		}
	}

};
*/
