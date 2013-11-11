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

#include "agmids.h"

#include "agmpanic.h"
#include <s32stor.h>

// ----------------------------- TAgnEntryId -----------------------------------

EXPORT_C TAgnEntryId::TAgnEntryId()
/**
@internalAll
*/
	{
	SetStreamId(KNullStreamId);
	}

EXPORT_C TAgnEntryId::TAgnEntryId(const TStreamId& aStreamId,TUint aPartialId)
/**
@internalAll
*/
	{
	SetStreamId(aStreamId);
	SetPartialId(aPartialId);
	}

EXPORT_C TBool TAgnEntryId::operator==(const TAgnEntryId& aId) const
/** 
Compares two IDs for equivalence.

@internalAll
@param aId The ID to compare with this one.
@return True if aId is equal to this ID. False if they are different. 
*/
	{	
	return (iId==aId.iId); 
	}

EXPORT_C TBool TAgnEntryId::operator!=(const TAgnEntryId& aId) const
/** 
Compares two IDs for inequality.

@internalAll
@param aId The ID to compare with this one.
@return True if aId is different from this ID. False if they are the same. 
*/
	{ 
	return (iId!=aId.iId); 
	}

EXPORT_C TUint TAgnEntryId::PartialId() const
	{
	TUint id = ((iId & (~KMaskStreamIdValue)) >> KShiftStreamIdValue); 
	__ASSERT_DEBUG(id < EMaxNumEntriesPerStream, Panic(EAgmErrInvalidPartialId)); // 
	return (id);
	}

void TAgnEntryId::SetStreamId(const TStreamId& aId)
	{
	TUint partialId = iId & (~KMaskStreamIdValue);
	iId = partialId | aId.Value();
	}

void TAgnEntryId::SetPartialId(TUint aId)
	{
	__ASSERT_DEBUG(aId < EMaxNumEntriesPerStream, Panic(EAgmErrPartialIdOverFlow));

	TUint streamId = iId & KMaskStreamIdValue;	
	aId <<= KShiftStreamIdValue;
	iId = streamId | aId;
	}

EXPORT_C TBool TAgnEntryId::IsNullId() const
/** 
Tests whether the ID value has been set. 

If not set, its value is zero.

@internalAll
@return True if the ID value is zero. False if not. 
*/
	{ 
	return (StreamId()==KNullStreamId); 
	}

EXPORT_C void TAgnEntryId::InternalizeL(RReadStream& aStream)
/** 
Internalises the ID from a read stream. 

The presence of this function means that the standard templated 
operator>>() is available to internalise objects of this class.

@internalAll
@param aStream Stream from which the ID should be internalised. 
*/
	{	
	aStream >> iId;
	}

EXPORT_C void TAgnEntryId::ExternalizeL(RWriteStream& aStream) const
/** 
Externalises the ID to a write stream. 

The presence of this function means that the standard templated 
operator<<() is available to externalise objects of this class.

@internalAll
@param aStream Stream to which the ID should be externalised. 
*/
	{
	aStream << iId;
	}

// -------------------------- TAgnInstanceId --------------------------------------	 

EXPORT_C TBool TAgnInstanceId::operator==(const TAgnInstanceId& aId) const
/** Compares two instance IDs for equality.

@internalAll
@param aId The instance ID to compare with this one.
@return True if aId is the same as this ID. False if they are different. */
	{ 
	return (iId==aId.iId && iDate==aId.iDate); 
	}

EXPORT_C TBool TAgnInstanceId::operator!=(const TAgnInstanceId& aId) const
/** Compares two instance IDs for inequality.

@internalAll
@param aId The instance ID to compare with this one.
@return True if aId is different from this ID. False if they are the same. 
*/
	{ 
	return (iId!=aId.iId || iDate!=aId.iDate); 
	}

EXPORT_C void TAgnInstanceId::InternalizeL(RReadStream& aStream)
/** Internalises an instance ID from a read stream. 

The presence of this function means that the standard templated operator>>() 
is available to internalise objects of this class.

@internalAll
@param aStream Stream from which the object should be internalized */
	{
	aStream >> iId;
	aStream >> iDate;
	}

EXPORT_C void TAgnInstanceId::ExternalizeL(RWriteStream& aStream) const
/** Externalises an instance ID to a write stream. 

The presence of this function means that the standard templated operator<<() 
is available to externalise objects of this class.

@internalAll
@param aStream Stream to which the object should be externalised. */
	{
	aStream << iId;
	aStream << iDate;
	}

EXPORT_C const TAgnCalendarTime& TAgnInstanceId::Date() const
	{
	return iDate;
	}

EXPORT_C void TAgnInstanceId::SetDate(const TAgnCalendarTime& aDate)
/** Sets the date component of the instance ID.

@internalAll
@param aDate The new date. 
*/
	{
	iDate = aDate;
	}

EXPORT_C void TAgnInstanceId::SetEntryId(const TAgnEntryId& aEntryId)
	{
	iId = aEntryId.Value();
	}
EXPORT_C void TAgnInstance::InternalizeL(RReadStream& aStream)
/** Internalises an instance ID from a read stream. 

The presence of this function means that the standard templated operator>>() 
is available to internalise objects of this class.

@internalAll
@param aStream Stream from which the object should be internalized */
	{
	aStream >> iId;
	iCollectionId = aStream.ReadUint8L();
	}

EXPORT_C void TAgnInstance::ExternalizeL(RWriteStream& aStream) const
/** Externalises an instance ID to a write stream. 

The presence of this function means that the standard templated operator<<() 
is available to externalise objects of this class.

@internalAll
@param aStream Stream to which the object should be externalised. */
	{
	aStream << iId;
	aStream.WriteUint8L(iCollectionId);
	}
