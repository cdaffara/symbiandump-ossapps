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

#ifndef __CALCATEGORYMANAGER_H__
#define __CALCATEGORYMANAGER_H__

#include <e32base.h>

class MCalProgressCallBack;
class CCalCategory;
class CCalEntry;
class CCalCategoryManagerImpl;
class CCalSession;

/** A category manager used to manipulate the categories held by the Calendar store. 

There are 12 built-in category types which are specified by CCalCategory::TCalCategoryType. 
In addition, clients can create as many of their own categories as they like by giving a name to the category.
This class is used along with the class CCalCategory to manipulate the categories in the file, for instance to 
add, delete and get a list of entries filtered by their category. 

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalCategoryManager) : public CBase
    {
public:

	IMPORT_C static CCalCategoryManager* NewL(CCalSession& aSession);
	IMPORT_C ~CCalCategoryManager();

	IMPORT_C TInt CategoryCountL() const;
	IMPORT_C CCalCategory* CategoryL(TInt aIndex) const;
	IMPORT_C void AddCategoryL(const CCalCategory& aCategory);
	IMPORT_C void FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries, MCalProgressCallBack& aProgressCallBack);
	IMPORT_C void FilterCategoryL(const CCalCategory& aCategory, RPointerArray<CCalEntry>& aEntries);
	IMPORT_C void DeleteCategoryL(const CCalCategory& aCategory, MCalProgressCallBack& aProgressCallBack);
	
private:
	CCalCategoryManager();
	void ConstructL(CCalSession& aSession);

private:
	CCalCategoryManagerImpl* iCCalCategoryManagerImpl;
	};
	
#endif // __CALCATEGORYMANAGER_H__
