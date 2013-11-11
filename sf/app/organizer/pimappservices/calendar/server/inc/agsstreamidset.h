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

#ifndef __AGSSTREAMIDSET_H__
#define __AGSSTREAMIDSET_H__

#include "agsver.h"
#include <s32std.h>

class CAgnCalendarConverter;

//------------------------------ referenced classes --------------------------

class CAgnStreamIdSet : public CBase
/**
@internalAll
@released
*/
	{
public:

	static CAgnStreamIdSet* NewL();
	virtual ~CAgnStreamIdSet();
	void AddL(TStreamId aId);
	void DeleteL(TStreamId aId);
	inline TInt Count() const;
	TStreamId CreateL(CStreamStore& aStore);
	void LoadL(CStreamStore& aStore,TStreamId aStreamId);
	void SaveL(CStreamStore& aStore) const;
	void ResetIteratorToStart();
	TBool Next();
	TBool At(TStreamId& aItem) const;
	TInt Find(TStreamId aItem,TInt& aPos) const;
	void RollbackL();
	void CommitL(CStreamStore& aStore);
	void Reset();
private:
	enum { EGranularity = 16, ERollbackGranularity = 2 };
	TStreamId					iStreamId;
	CArrayFixFlat<TStreamId>*	iArray;
	TInt						iPos;
	CArrayFixFlat<TStreamId>*	iAddRollbackArray;
	CArrayFixFlat<TStreamId>*	iDeleteRollbackArray;
	};


class CAgnModelStreamIdSet : public CBase
/**
@internalAll
@released
*/
	{
public:
	friend class CAgnCalendarConverter; // to allow stream IDs to be loaded from file converter
	
	static CAgnModelStreamIdSet* NewL();
	virtual ~CAgnModelStreamIdSet();										
	inline const CAgnStreamIdSet& EntryStreamIdSet() const;
	inline CAgnStreamIdSet& EntryStreamIdSet();
	inline const TStreamId& EntrySetStreamId() const;
	inline const TStreamId& NextLocalUidValueStreamId() const;
	inline const TStreamId& EntryManagerStreamId() const;
	inline const TStreamId& FileInformationStreamId() const;
	inline TAgnVersion Version() const;
	void SaveL(CStreamStore& aStore,TStreamId aStreamId) const;
	TStreamId CreateL(CStreamStore& aStore,const TAgnVersion& aVersion);
	void LoadL(CStreamStore& aStore,TStreamId aStreamId);
	void LoadL(CStreamStore& aStore,TStreamId aStreamId, CAgnCalendarConverter& aCalendarConverter);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	void RollbackL();
	void CommitL(CStreamStore& aStore);
	void Reset();
	void ChangeFileVersionL(CStreamStore& aStore,TStreamId aStreamId, TAgnVersion& aNewVersion);
	
protected:
	void ConstructL();
	
private:
	CAgnStreamIdSet*	iEntryStreamIdSet;	
	TAgnVersion			iVersion;				
	TStreamId			iEntrySetStreamId;	
	TStreamId			iNextLocalUidValueStreamId; 
	TStreamId			iEntryManagerStreamId;
	TStreamId			iFileInformationStreamId;
	};

#include "agsstreamidset.inl"

#endif
