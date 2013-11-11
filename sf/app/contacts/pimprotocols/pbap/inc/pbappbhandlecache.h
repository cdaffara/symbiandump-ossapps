// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef PBAPPBHANDLECACHE_H
#define PBAPPBHANDLECACHE_H

#include <e32std.h>
#include <badesca.h>
#include <cntdef.h>
#include <cntdbobs.h>

//constants
const TInt KOwnCardHandle = 0;

//forward declarations
class CContactDatabase;
class CContactChangeNotifier;


class MPbHandleCacheObserver
	{
public:
	virtual void HandleCacheChanged()=0;
	};
	

class TPbHandleAndId
	{
public:
	inline TPbHandleAndId(TInt aHandle, TContactItemId aContactId) : iHandle(aHandle), iContactId(aContactId) {};
	
public:
	TInt iHandle;
	TContactItemId iContactId;
	};

				
class CPbapPbHandleCache : public CBase, public MContactDbObserver
	{
public:
	static CPbapPbHandleCache* NewL(CContactDatabase& aDatabase, MPbHandleCacheObserver& aObserver);
	~CPbapPbHandleCache();

public:
	TInt HandleAtL(TUint aIndex) const;
	TContactItemId ContactIdAtL(TUint aIndex) const;
	TInt FindHandle(TInt aHandle) const;
	TInt FindContactId(TContactItemId aContactId) const;	
	inline TInt Count() const;	

public:
	static TInt CompareHandles(const TPbHandleAndId& aFirst, const TPbHandleAndId& aSecond);
	static TBool MatchContactIds(const TPbHandleAndId& aFirst, const TPbHandleAndId& aSecond);

private:
	CPbapPbHandleCache(CContactDatabase& aDatabase, MPbHandleCacheObserver& aObserver);
	void ConstructL();
	void CreateHandleListL();

	void HandleContactAddedL(TContactItemId aContactId);
	void HandleContactDeleted(TContactItemId aContactId);
	void HandleOwnCardChangedL();
	void HandleOwnCardDeleted();
	TInt NextAvailableHandleL() const;
		
private: 
	//from MContactDbObserver	
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	
private:
	CContactDatabase& iDatabase;
	MPbHandleCacheObserver& iObserver;
	CContactChangeNotifier* iChangeNotifier;

	RArray<TPbHandleAndId> iHandleList;
	
	};

inline TInt CPbapPbHandleCache::Count() const
	{
	return iHandleList.Count();
	}
		
#endif // PBAPPBHANDLECACHE_H
