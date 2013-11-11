// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSINSTANCEITERATOR_H__
#define __AGSINSTANCEITERATOR_H__

#include <e32std.h>
#include <e32base.h>
#include <calentry.h>

class TFindInstanceParams;
class TAgnInstanceId;
class CAgsInstanceSorter;
class CAgnEntryModel;
class CAgnSortCriteria;
class CAgsInstanceSorter;
class CAgnServFileMgr;
class TAgnSortInstance;
class TAgnInstance;
/**
 *  CAgsInstanceIterator
 * 
 */
class CAgsInstanceIterator : public CBase
	{
public:
	// Constructors and destructor
	~CAgsInstanceIterator();

	static CAgsInstanceIterator* NewLC(TInt aIteratorId, TFindInstanceParams* aFindParams, CAgnSortCriteria* aSortCriteria, const RArray<CCalEntry::TType>& aDefaultTypeOrder, RArray<TInt64>& aFileIds, CAgnServFileMgr& aFileManager);

	void NextInstancesL(RArray<TAgnInstance>& aInstances, TInt aStartIndex);
	void PreviousInstancesL(RArray<TAgnInstance>& aInstances, TInt aLastIndex);
	TInt CountL();
	TInt LocateIndexL(TAgnInstance& aInstanceId);
	
	inline TInt IteratorId() const;
	
private:
	CAgsInstanceIterator(TInt aIteratorId, CAgnServFileMgr&	aFileManager);
	void ConstructL(CAgnSortCriteria* aSortCriteria, const RArray<CCalEntry::TType>& aDefaultTypeOrder, TFindInstanceParams* aFindParams, RArray<TInt64>& aFileIds);
	
	TBool IsCached(TInt aIndex, TBool aPrevious) const;
	CArrayFixSeg<TAgnSortInstance>* FindSortedInstanceLC();
	void AddInstanceL(TAgnSortInstance& aSortInstance, RArray<TAgnInstance>& aInstances);

private:	
	RPointerArray<CAgnEntryModel> iModels;
	TInt iIteratorId;
	TInt iInstanceCount;
	TUint iCachedOffset;
	
	CAgsInstanceSorter* iSorter;
	TFindInstanceParams* iFindParams;
	RArray<TAgnInstance> iCachedInstances;
	CAgnServFileMgr&	iFileManager;
	};

/**
 *  CAgsInstanceIteratorMgr
 * 
 */
class CAgsInstanceIteratorMgr : public CBase
	{
public:
	// Constructors and destructor
	~CAgsInstanceIteratorMgr();
	
	static CAgsInstanceIteratorMgr* NewL();
	
	TInt CreateIteratorL(TFindInstanceParams* aFindParams, CAgnSortCriteria* aSortCriteria, RArray<TInt64>& aFileIds, CAgnServFileMgr& aFileManager);
	void DestroyIteratorL(TInt aIteratorId);
	void NextInstancesL(TInt aIteratorId, RArray<TAgnInstance>& aInstances, TInt aStartIndex);
	void PreviousInstancesL(TInt aIteratorId, RArray<TAgnInstance>& aInstances, TInt aLastIndex);
	TInt CountL(TInt aIteratorId);
	TInt LocateIndexL(TInt aIteratorId, TAgnInstance& aInstanceId);

private:
	CAgsInstanceIteratorMgr();
	void ConstructL();
	TInt FindIteratorById(TInt aIteratorId) const;	

private:
	RPointerArray<CAgsInstanceIterator> iIteratorList;
	RArray<CCalEntry::TType> iDefaultTypeOrder;	
	TInt iNextAvailableId;
	};

inline TInt CAgsInstanceIterator::IteratorId() const
	{
	return iIteratorId;
	}

#endif // __AGSINSTANCEITERATOR_H__
