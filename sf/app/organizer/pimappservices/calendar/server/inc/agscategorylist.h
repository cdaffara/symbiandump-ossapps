// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSCATEGORYLIST_H__
#define __AGSCATEGORYLIST_H__

#include <calcategory.h>

class CAgnCategory;
class CAgnEntry;
class CAgnServFile;
class RReadStream;
class RWriteStream;

//
//	CAgnCategoryList
//
//	Represents a list of categories within entries in an Agenda file
//

class CAgnCategoryList	:	public CBase
	{
public:
	static CAgnCategoryList* NewL(CAgnServFile& aAgnServFile);
	~CAgnCategoryList();

public:
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);

public:
	void AddCategoryL(const TDesC& aCategory);
	void AddCategoryL(CAgnCategory& aCategory);
	void DeleteCategoryL(TInt aIndex);
	const CAgnCategory& Category(TInt aIndex) const;

	void AddEntryL(CAgnEntry& aEntry);
	TInt FindCategoryIndex(CAgnCategory& aCategory);
	void UpdateCategoryListInFileL();
	void RollBackL();
	CAgnServFile& AgnServFile();
	void CreateCategoryBackupL();
	void DestroyCategoryBackup();

public:
	inline TInt CategoryCount() const;
	inline TPtrC CategoryOldName() const;

private:
	CAgnCategoryList(CAgnServFile& aAgnServFile);
	void ConstructL();

private:
	// Do exact comparison if aCompareCase=ETrue otherwise do a folding comparison
	TBool ListContains(const TDesC& aName, TInt& aIndex, TBool aCompareCase=ETrue);
	
private:
	RPointerArray<CAgnCategory>*	iCategories;
	CAgnServFile&					iAgnServFile;
	RPointerArray<CAgnCategory>*	iCategoriesBuffer;
	HBufC* 							iCatOldName;
	};

// inline functions
TInt CAgnCategoryList::CategoryCount() const	{ return iCategories->Count(); }
TPtrC CAgnCategoryList::CategoryOldName() const { return *iCatOldName;}

#endif
