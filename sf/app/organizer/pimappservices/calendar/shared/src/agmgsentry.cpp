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

#include "agmgsentry.h"
#include "agmpanic.h"

#include <s32stor.h>

// TGsChildRefData
EXPORT_C TGsChildRefData::TGsChildRefData(TCalLocalUid aChildId, const TAgnCalendarTime& aRecurId)
	: iChildId(aChildId), iRecurrenceId(aRecurId)
	{
	}
	
EXPORT_C TCalLocalUid TGsChildRefData::ChildId() const
	{
	return iChildId;
	}
	
EXPORT_C const TAgnCalendarTime& TGsChildRefData::RecurrenceId() const
	{
	return iRecurrenceId;
	}


// CGsData
CGsData::CGsData()
	{
	}
	
CGsData::CGsData(TInt aSeq, CCalEntry::TMethod aMethod)
	{
	iSeqNum = aSeq;
	iMethod = aMethod;
	}
	
CGsData::~CGsData()
	{
	delete iGuid;
	}

void CGsData::SetSequenceNumber(TInt aNum)
	{
	iSeqNum = aNum;
	}

TInt CGsData::SequenceNumber() const
	{
	return iSeqNum;
	}

void CGsData::SetMethod(CCalEntry::TMethod aMethod)
	{
	iMethod = aMethod;
	}

CCalEntry::TMethod CGsData::Method() const
	{
	return iMethod;
	}

const TDesC8& CGsData::Guid() const
	{
	if(iGuid)
		{
		return *iGuid;
		}
	return KNullDesC8;
	}
	
void CGsData::SetGuid(HBufC8* aGuid)
	{
	delete iGuid;
	iGuid = aGuid;
	}

void CGsData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iSeqNum);
	aStream.WriteInt8L(iMethod);	
	ExternalizeTypeSpecificDataL(aStream, EFalse);
	}

void CGsData::ExternalizeToBufferL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iSeqNum);
	aStream.WriteInt8L(iMethod);	
	ExternalizeTypeSpecificDataL(aStream, ETrue);
	}

void CGsData::InternalizeL(RReadStream& aStream)
	{
	iSeqNum = aStream.ReadInt32L();
	iMethod = static_cast<CCalEntry::TMethod> (aStream.ReadInt8L());
	InternalizeTypeSpecificDataL(aStream, EFalse); 
	}

void CGsData::InternalizeFromBufferL(RReadStream& aStream)
	{
	iSeqNum = aStream.ReadInt32L();
	iMethod = static_cast<CCalEntry::TMethod> (aStream.ReadInt8L());
	InternalizeTypeSpecificDataL(aStream, ETrue); 
	}
	
// CGsParentEntryData  
void CGsParentData::RemoveChildId(TCalLocalUid aId)
	{
	TInt pos = FindChildId(aId);
	if (pos != KErrNotFound)
		{
		iChildIds->Remove(pos);
		}
	}
	
void CGsParentData::AddChildIdL(const TGsChildRefData& aChildData)
	{
	TInt pos = FindChildId(aChildData.ChildId());
	if (pos == KErrNotFound)
		{
		User::LeaveIfError(iChildIds->Append(aChildData));
		}
	}	

CGsParentData::~CGsParentData()
	{
	if (iChildIds)
		{
		iChildIds->Close();
		}
	delete iChildIds;
	}

CGsParentData* CGsParentData::NewL()
	{
	return new (ELeave) CGsParentData();
	}

CGsParentData* CGsParentData::NewL(HBufC8* aUid, TInt aSeqNum, CCalEntry::TMethod aMethod)
	{
	CGsParentData* self = new (ELeave) CGsParentData(aSeqNum, aMethod);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->SetGuid(aUid); // set GUID last because ownership taken
	CleanupStack::Pop(self);
	return self;	
	}

CGsParentData::CGsParentData(TInt aSeqNum, CCalEntry::TMethod aMethod)
	:CGsData(aSeqNum, aMethod)
	{
	}

void CGsParentData::ConstructL()
	{
	iChildIds = new (ELeave) RArray<TGsChildRefData>();
	}

CGsParentData::CGsParentData()
	{
	}
	
void CGsParentData::ExternalizeTypeSpecificDataL(RWriteStream& aStream, TBool /*aToBuffer*/) const
	{
	aStream.WriteUint16L(Guid().Length());
	aStream << Guid();

	const TInt KCount = iChildIds->Count();
	aStream.WriteInt32L(KCount);
		
	for (TInt i = 0; i < KCount; ++i)
		{
		aStream << (*iChildIds)[i].ChildId();
		aStream << (*iChildIds)[i].RecurrenceId();
		}
	}

void CGsParentData::InternalizeTypeSpecificDataL(RReadStream& aStream, TBool /*aFromBuffer*/)
	{
	TUint16 length = aStream.ReadUint16L();
	HBufC8* guid = HBufC8::NewL(aStream,length);
	SetGuid(guid);

	iChildIds = new (ELeave) RArray<TGsChildRefData>();

	TAgnCalendarTime recurId;
    const TInt KCount = aStream.ReadInt32L(); 
    
    for (TInt i = 0; i < KCount; ++i)
		{
		TCalLocalUid childId = aStream.ReadUint32L();	
		aStream >> recurId;
		
		TGsChildRefData childData(childId, recurId);
		iChildIds->AppendL(childData);
		}
 	}
	
TInt CGsParentData::FindChildId(TCalLocalUid aChildId) const
	{
	const TInt KCount = iChildIds->Count();
	for (TInt i = 0; i < KCount; ++i)
		{
		if ((*iChildIds)[i].ChildId() == aChildId)
			{
			return i;
			}
		}
	return KErrNotFound;
	}
	
const RArray<TGsChildRefData>& CGsParentData::ChildIds() const
	{
	return *iChildIds;
	}
	
CGsData::TGsDataType CGsParentData::GsDataType() const
	{
	return CGsData::EParent;
	}
	
void CGsParentData::CopyDataL(const CGsData& aSource)
	{
	if (aSource.GsDataType() == CGsData::EParent)
		{
		iChildIds->Reset();
		
		const RArray<TGsChildRefData>& sourceChild = static_cast <const CGsParentData&> (aSource).ChildIds();
		
		const TInt KCount = sourceChild.Count();
		for (TInt ii = 0; ii<KCount; ++ii)
			{
			iChildIds->AppendL((sourceChild)[ii]);
			}
		}
	}
	
TAgnCalendarTime CGsParentData::RecurrenceId() const
	{
	return TAgnCalendarTime();
	}

CalCommon::TRecurrenceRange CGsParentData::RecurrenceRange() const
	{
	return CalCommon::EThisOnly;
	}
	
// CGsChildEntryData
CGsData::TGsDataType CGsChildData::GsDataType() const
	{
	return CGsData::EChild;
	}
	
void CGsChildData::CopyDataL(const CGsData& aSource)
	{
	if (aSource.GsDataType() == CGsData::EChild)
		{
		iParentId = static_cast <const CGsChildData&>(aSource).ParentId();
		}
	}
	
void CGsChildData::ExternalizeTypeSpecificDataL(RWriteStream& aStream, TBool aToBuffer) const
	{
	aStream << iParentId;
	if(aToBuffer)
		{
		if (Guid().Length() > 0)
			{
			aStream.WriteUint16L(Guid().Length());
			aStream << Guid();
			}
		else
			{
			aStream.WriteUint16L(0);
			}
		aStream << iRecurrenceId;
		aStream.WriteUint8L(iRecurrenceRange);
		}
	
	}

void CGsChildData::InternalizeTypeSpecificDataL(RReadStream& aStream, TBool aFromBuffer)
	{
	aStream >> iParentId; 
	if(aFromBuffer)
		{
		TUint16 length = aStream.ReadUint16L();
		HBufC8* guid = NULL;
		if(length>0)
			{
			guid = HBufC8::NewL(aStream,length);
			}
		SetGuid(guid);
		aStream >> iRecurrenceId;
		iRecurrenceRange = static_cast<CalCommon::TRecurrenceRange>(aStream.ReadUint8L());
		}
	}

CGsChildData* CGsChildData::NewL()
	{
	return new (ELeave) CGsChildData();;
	}

CGsChildData* CGsChildData::NewL(HBufC8* aUid, TInt aSeq, CCalEntry::TMethod aMethod, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRecurrenceRange)
	{
	CGsChildData* self = new (ELeave) CGsChildData(aSeq, aMethod, aRecurrenceId, aRecurrenceRange );
	self->SetGuid(aUid); // set GUID last because ownership taken
	return self;
	}

CGsChildData::CGsChildData(TInt aSeq, CCalEntry::TMethod aMethod, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRecurrenceRange)
	:CGsData(aSeq, aMethod)
	{
	iRecurrenceId = aRecurrenceId;
	iRecurrenceRange = aRecurrenceRange;
	}

CGsChildData::CGsChildData()
	{
	}

void CGsChildData::SetParentId(TCalLocalUid aParentId)
	{
	iParentId = aParentId;
	}
	
TCalLocalUid CGsChildData::ParentId() const
	{
	return iParentId;
	}
	
TAgnCalendarTime CGsChildData::RecurrenceId() const
	{
	return iRecurrenceId;
	}

void CGsChildData::SetRecurrenceId(const TAgnCalendarTime& aRecurrenceId)
	{
	iRecurrenceId = aRecurrenceId;
	}
	
CalCommon::TRecurrenceRange CGsChildData::RecurrenceRange() const
	{
	return iRecurrenceRange;
	}

void CGsChildData::SetRecurrenceRange(CalCommon::TRecurrenceRange aRecurrenceRange)
	{
	iRecurrenceRange = aRecurrenceRange;
	}
	
