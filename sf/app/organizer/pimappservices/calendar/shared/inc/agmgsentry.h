// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMGSENTRY_H__
#define __AGMGSENTRY_H__

#include "agmcalendartime.h"
#include "calentry.h"

class CAgnEntry;

/**
Class representing a child entry's unique id and its recurrence id. Held by parent classes.
@internalComponent
@released
*/	
NONSHARABLE_CLASS(TGsChildRefData)
	{
public:

	IMPORT_C TGsChildRefData(TCalLocalUid aChildId, const TAgnCalendarTime& aRecurId);
	IMPORT_C TCalLocalUid ChildId() const;
	IMPORT_C const TAgnCalendarTime& RecurrenceId() const;
	
private:
	TCalLocalUid 	 	iChildId;        
	TAgnCalendarTime	iRecurrenceId;    	
	};
	

/**
Abstract base class for storing group scheduling data.
Parent entries contain data about all their child entries (modifying entries) - this is a list of local IDs.
Child entries contain data about how they modiy their parent entry - their recurrence ID and range.
Both types of entries contain a GUID, sequence number and method.

@internalComponent
@released
*/
NONSHARABLE_CLASS (CGsData) : public CBase  
	{
public:

	enum TGsDataType
		{
		EParent,
		EChild
		};
		
	virtual ~CGsData();

	// These methods are common to child and parent entries - parent entries have a default (NULL) recurrence ID
	// and default recurrence range of 'this only'
	virtual TGsDataType GsDataType() const = 0;
	virtual void CopyDataL(const CGsData& aSource) = 0;
	virtual TAgnCalendarTime RecurrenceId() const = 0;
	virtual CalCommon::TRecurrenceRange RecurrenceRange() const = 0;

	void SetSequenceNumber(TInt aNum);
	TInt SequenceNumber() const;

	void SetMethod(CCalEntry::TMethod aMethod);
	CCalEntry::TMethod Method() const;

	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);

	void ExternalizeToBufferL(RWriteStream& aStream) const;
	void InternalizeFromBufferL(RReadStream& aStream);
	
	const TDesC8& Guid() const;
 	void SetGuid(HBufC8* aGuid); // takes ownership
	
protected:
	CGsData(TInt aSeq, CCalEntry::TMethod aMethod); 
	CGsData();

private:
	virtual void InternalizeTypeSpecificDataL(RReadStream& aStream, TBool aFromBuffer)  = 0;
	virtual void ExternalizeTypeSpecificDataL(RWriteStream& aStream, TBool aToBuffer) const = 0;

private:
	HBufC8* 				iGuid; 
	TInt					iSeqNum;
	CCalEntry::TMethod  	iMethod; 
	};
 
/**
Class representing a child entry's group scheduling data.
@internalComponent
@released
*/ 
NONSHARABLE_CLASS (CGsChildData) : public CGsData
	{
public:	
    
    // GsChild methods
    static CGsChildData* NewL(); //Used by CAgnEntry before internalise the object from the stream
  	static CGsChildData* NewL(HBufC8* aUid, TInt aSeq, CCalEntry::TMethod aMethod, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRecurrenceRange);
  	
	TCalLocalUid ParentId() const;
	void SetParentId(TCalLocalUid aParentId);
	void SetRecurrenceId(const TAgnCalendarTime& aRecId);
	void SetRecurrenceRange(CalCommon::TRecurrenceRange aRecurrenceRange);

	// From CGsEntryData
	CGsData::TGsDataType GsDataType() const;
	void CopyDataL(const CGsData& aSource);
	TAgnCalendarTime RecurrenceId() const;
	CalCommon::TRecurrenceRange RecurrenceRange() const;
	
private:
	CGsChildData();
	CGsChildData(TInt aSeq, CCalEntry::TMethod aMethod, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRecurrenceRange);

	// From CGsEntryData
	void ExternalizeTypeSpecificDataL(RWriteStream& aStream, TBool aToBuffer) const;
	void InternalizeTypeSpecificDataL(RReadStream& aStream, TBool aFromBuffer);

private:
	TAgnCalendarTime			iRecurrenceId;
	CalCommon::TRecurrenceRange iRecurrenceRange;	
	TCalLocalUid 	 			iParentId;    		
	};
	
/**
Class representing a parent entry's group scheduling data.
@internalComponent
@released
*/ 
NONSHARABLE_CLASS (CGsParentData) : public CGsData  
	{
public:
 	static CGsParentData* NewL();
 	static CGsParentData* NewL(HBufC8* aUid, TInt aSeqNum, CCalEntry::TMethod aMethod);   
    ~CGsParentData();
    
	const RArray<TGsChildRefData>& ChildIds() const;
 	void RemoveChildId(TCalLocalUid aId);
	void AddChildIdL(const TGsChildRefData& aChildData);

	// From CGsEntryData
	CGsData::TGsDataType GsDataType() const;
	void CopyDataL(const CGsData& aSource);
	TAgnCalendarTime RecurrenceId() const;
	CalCommon::TRecurrenceRange RecurrenceRange() const;

private:
	CGsParentData(TInt aSeqNum, CCalEntry::TMethod aMethod);
	CGsParentData();
	void ConstructL();
    TInt FindChildId(TCalLocalUid aChildId) const;

	// From CGsEntryData
	void ExternalizeTypeSpecificDataL(RWriteStream& aStream, TBool aToBuffer) const;
	void InternalizeTypeSpecificDataL(RReadStream& aStream, TBool aFromBuffer);
  
private:
	RArray<TGsChildRefData>* iChildIds;
	}; 	

#endif // __AGMGSENTRY_H__

