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

#ifndef __CALCATEGORYIMPL_H__
#define __CALCATEGORYIMPL_H__

#include "calcategory.h"

class CAgnCategory;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CCalCategoryImpl) : public CBase
	{
public:
	static CCalCategoryImpl* NewL(CCalCategory::TCalCategoryType aCategory);
	static CCalCategoryImpl* NewL(const TDesC& aExtendedName);
	static CCalCategoryImpl* NewL(CAgnCategory* aCategory);

	~CCalCategoryImpl();

	CAgnCategory* AgnCategory() const;
	void SetOwnAgnCategory(TBool aOwnCategory);
	CCalCategory::TCalCategoryType CategoryType() const;
	const TDesC& ExtendedCategoryName() const;

private:
	CCalCategoryImpl();
	CCalCategoryImpl(CAgnCategory* aCategory);
	void ConstructL(CCalCategory::TCalCategoryType aCategory);
	void ConstructL(const TDesC& aExtendedName);

private:
	CAgnCategory* iCategory;
	TBool iOwnCategory;
	};

#endif // __CALCATEGORYIMPL_H__

