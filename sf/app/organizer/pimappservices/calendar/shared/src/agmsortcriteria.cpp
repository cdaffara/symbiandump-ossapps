// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>
#include "agmsortcriteria.h"

/** CAgnSortCriteria NewL function.
@internalComponent
@prototype
@capability None
*/
EXPORT_C CAgnSortCriteria* CAgnSortCriteria::NewL()
	{
	CAgnSortCriteria* self = new (ELeave) CAgnSortCriteria();
	return self;
	}

CAgnSortCriteria::CAgnSortCriteria()
	{
	}

/** Destructor.
@internalComponent
@prototype
@capability None
*/
EXPORT_C CAgnSortCriteria::~CAgnSortCriteria()
	{
	iSortCriteria.Close();
	iEntryTypeOrder.Close();
	}

/** Internalize sort criteria from a stream.
@param aReadStream The read stream storing the sort criteria 
@pre A read stream storing sort criteria has been settled up
@post Sort criteria stored in the stream has been retored.
@internalComponent
@prototype
@capability None
*/
EXPORT_C void CAgnSortCriteria::InternalizeL(RReadStream& aReadStream)
	{
	iSortCriteria.Reset();
	iEntryTypeOrder.Reset();
	
	const TInt KCriteriaCount = aReadStream.ReadInt8L();
	for(TInt count = 0; count < KCriteriaCount; count++)
		{
		CalCommon::TCalSortAttribute attr = static_cast<CalCommon::TCalSortAttribute>(aReadStream.ReadInt8L()); 
		CalCommon::TCalSortDirection dir = static_cast<CalCommon::TCalSortDirection>(aReadStream.ReadInt8L());
		iSortCriteria.AppendL(TAgnSortCriterion(attr, dir));
		}
	
	const TInt KTypeOrderCount = aReadStream.ReadInt8L();
	for(TInt count = 0; count < KTypeOrderCount; count++)
		{
		CCalEntry::TType type = static_cast<CCalEntry::TType>(aReadStream.ReadInt8L());
		iEntryTypeOrder.AppendL(type);
		}
	}

/** Externalize sort criteria to a stream.
@param aReadStream The write stream to which sort criteria export
@pre A write stream has been created
@post Sort criteria has been exported into the write stream.
@internalComponent
@prototype
@capability None
*/
EXPORT_C void CAgnSortCriteria::ExternalizeL(RWriteStream& aWriteStream) const
	{
	const TInt KCriteriaCount = iSortCriteria.Count();
	aWriteStream.WriteInt8L(KCriteriaCount);
	for(TInt count = 0; count < KCriteriaCount; count++)
		{
		aWriteStream.WriteInt8L(iSortCriteria[count].iSortAttribute);
		aWriteStream.WriteInt8L(iSortCriteria[count].iSortDirection);
		}
	
	const TInt KTypeOrderCount = iEntryTypeOrder.Count();
	aWriteStream.WriteInt8L(KTypeOrderCount);
	for(TInt count = 0; count < KTypeOrderCount; count++)
		{
		aWriteStream.WriteInt8L(iEntryTypeOrder[count]);
		}
	}

/** Append sort criterion to the end of sort criteria list.
@param aAttribute Sort attribute of the sort criterion
@param aAttribute Sort direction of the sort criterion
@pre None
@post the new sort criterion has been added into the sort criteria list.
@publishedAll
@prototype
@capability None
*/
EXPORT_C TInt CAgnSortCriteria::AppendCriterionL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir)
	{
	iSortCriteria.AppendL(TAgnSortCriterion(aAttr, aDir));
	return iSortCriteria.Count() - 1; 
	}

/** Get the number of sort criteria.
@return The number of sort criteria 
@pre None
@post None.
@internalComponent
@prototype
@capability None
*/
EXPORT_C TInt CAgnSortCriteria::Count() const
	{
	return iSortCriteria.Count();
	}

/** Get the sort criterion at specified index.
@param aIndex The index of wanted sort criterion  
@return Reference to the sort criterion 
@pre None
@post None.
@internalComponent
@prototype
@capability None
*/
EXPORT_C const TAgnSortCriterion& CAgnSortCriteria::AtL(TInt aIndex) const
	{
	if((aIndex < 0) || (aIndex > iSortCriteria.Count()))
		{
		//avoid the panic mightly raised by iSortCriteria[aIndex] 
		User::Leave(KErrArgument);
		}
	
	return iSortCriteria[aIndex];
	}

/** Set the sort orders which are used to sort instances type.
@param aTypeOrder Reference to an array of CCalEntry::TType  
@pre None
@post The sort order of entry types have been updated.
@internalComponent
@prototype
@capability None
*/
EXPORT_C void CAgnSortCriteria::SetEntryTypeOrderL(const RArray<CCalEntry::TType>& aTypeOrder)
	{
	const TInt KCount(aTypeOrder.Count());
	iEntryTypeOrder.Reset();
	for(TInt count = 0; count < KCount; count++)
		{
		iEntryTypeOrder.AppendL(aTypeOrder[count]);
		}
	}

/** Get the sort order which are used to sort entry type.
@return Reference to the array of CCalEntry::TType stored in the sort criteria 
@pre None
@post None.
@internalComponent
@prototype
@capability None
*/
EXPORT_C const RArray<CCalEntry::TType>& CAgnSortCriteria::EntryTypeOrder() const
	{
	return iEntryTypeOrder;
	}

TAgnSortCriterion::TAgnSortCriterion(CalCommon::TCalSortAttribute aSortAttribute, CalCommon::TCalSortDirection aSortDirection)
	: iSortAttribute(aSortAttribute), iSortDirection(aSortDirection)
	{
	}
