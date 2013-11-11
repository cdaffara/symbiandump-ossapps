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

#ifndef __AGMIDS_H__
#define __AGMIDS_H__

#include "agmcalendartime.h"

#include <s32std.h>

class RReadStream;
class RWriteStream;
class TAgnInstanceId;

NONSHARABLE_CLASS(TAgnEntryId)
/** The base class for agenda entry and instance ID types.
Internally, the ID is stored as a 32-bit unsigned integer. 

This is made up of a stream ID which stores the stream containing the entry, and a partial ID,
which gives the position of the entry within that stream. There can be up to EMaxNumEntriesPerStream
entries in a stream (16).

@internalAll
@released
*/
	{
public:
	enum {EMaxNumEntriesPerStream = 16};
	
	IMPORT_C TAgnEntryId();
	IMPORT_C TAgnEntryId(const TStreamId& aStreamId,TUint aPartialId);
	IMPORT_C TBool operator==(const TAgnEntryId& aId) const;
	IMPORT_C TBool operator!=(const TAgnEntryId& aId) const;
	inline TStreamId StreamId() const;
	void SetStreamId(const TStreamId& aId);
	IMPORT_C TUint PartialId() const;
	inline TUint Value() const;
	IMPORT_C TBool IsNullId() const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
private:
	void SetPartialId(TUint aId);
protected:
	TUint32 iId;
	};

NONSHARABLE_CLASS(TAgnInstanceId) : public TAgnEntryId
/** Identifies an instance of an entry in the agenda instance model (CAgnModel), 
using an entry ID (TAgnEntryId) and an agenda date (TTime). 
@internalAll
@released
*/
	{
public:
	IMPORT_C TBool operator==(const TAgnInstanceId& aId) const;
	IMPORT_C TBool operator!=(const TAgnInstanceId& aId) const;
	IMPORT_C const TAgnCalendarTime& Date() const;
	IMPORT_C void SetDate(const TAgnCalendarTime& aDate);
	IMPORT_C void SetEntryId(const TAgnEntryId& aId);
	
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

protected:
	TAgnCalendarTime iDate;
	};

class TAgnInstance
	{
public:
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;

public:
	TAgnInstanceId iId;
	TUint8 iCollectionId;
	}; 
#include "agmids.inl"

#endif
