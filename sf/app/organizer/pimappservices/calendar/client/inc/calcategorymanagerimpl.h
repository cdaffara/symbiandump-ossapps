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

#ifndef __CALCATEGORYMANAGERIMPL_H__
#define __CALCATEGORYMANAGERIMPL_H__

#include <e32base.h>

class CCalEntry;
class CCalSession;
class CCalSessionImpl;
class CCalCategory;
class MCalProgressCallBack;
class TAgnEntryId;
/**
@internalComponent
*/
NONSHARABLE_CLASS(CCalCategoryManagerImpl) : public CBase
	{
public:
	CCalCategoryManagerImpl(CCalSession& aSession);
	~CCalCategoryManagerImpl();

	TInt CategoryCountL() const;
	CCalCategory* CategoryL(TInt aIndex) const;
	void AddCategoryL(const CCalCategory& aCategory);
	void FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries, MCalProgressCallBack* aProgressCallBack);
	void DeleteCategoryL(const CCalCategory& aCategory, MCalProgressCallBack& aProgressCallBack);
	static void GetFilteredEntryL(CArrayFixSeg<TAgnEntryId>& aEntryId, RPointerArray<CCalEntry>& aEntries, CCalSessionImpl& aCalSessionImpl);

private:
	CCalSession& iCalSession;
	};

#endif
