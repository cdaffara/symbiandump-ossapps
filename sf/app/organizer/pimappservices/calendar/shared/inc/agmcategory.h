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


#ifndef __AGMCATEGORY_H__
#define __AGMCATEGORY_H__

#include "caluser.h"
#include "calcategory.h"

//
// CAgnAttendee

class CAgnCategory;
class RWriteStream;
class RReadStream;

// CAgnCategory
//
NONSHARABLE_CLASS(CAgnCategory): public CBase
/** A category associated with an agenda entry. 

The category's type is held as a TAgnCategoryType value. The set 
of possible categories is as defined in the vCalendar 1.0 standard, 
with the addition of an 'Extended' category type. 

The Extended category type allows for user-defined categories. An extended 
name is needed for an extended category. The name can be assigned a descriptor 
of any length.

Any number of categories can be specified for an entry. CCalEntry provides 
member functions for adding, deleting and fetching categories for an entry. 
CCalCategoryManager's category support functions enable you to find the categories 
used in a file and to get a list of entries filtered by their category. 

These properties are identical to those accessible through the CCalCategory interface.

@internalAll
@released
*/
	{
public:
	// construct / destruct functions
	IMPORT_C static CAgnCategory* NewL(CCalCategory::TCalCategoryType aCategory);
	IMPORT_C static CAgnCategory* NewL(const TDesC& aExtendedName);
	IMPORT_C static CAgnCategory* NewL(RReadStream& aStream);
	IMPORT_C static CAgnCategory* NewL(const CAgnCategory& aCategory);
	~CAgnCategory();

	// Get functions
	inline CCalCategory::TCalCategoryType Category() const;
	IMPORT_C const TDesC& ExtendedCategoryName() const;

	// Persistence
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	CAgnCategory();
	void CopyL(const CAgnCategory& aCategory);
	void ConstructL(CCalCategory::TCalCategoryType aCategory,const TDesC& aExtendedName);
	static CAgnCategory* CreateCategoryL(CCalCategory::TCalCategoryType aCategory,const TDesC& aExtendedName);
private:
	HBufC*				iExtendedCategoryName;
	CCalCategory::TCalCategoryType	iCategoryType;
	};

// Inline functions
CCalCategory::TCalCategoryType CAgnCategory::Category() const
/** Gets the category type.

@return The category type. */
	{
	return iCategoryType;
	}

#endif
