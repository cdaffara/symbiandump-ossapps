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

#ifndef __AGMSORTCRITERIA_H_
#define __AGMSORTCRITERIA_H_

#include <calcommon.h>
#include <calentry.h>

class RWriteStream;
class RReadStream;
class TAgnSortCriterion;

NONSHARABLE_CLASS(CAgnSortCriteria) : public CBase
	{
public:
	IMPORT_C static CAgnSortCriteria* NewL();
	IMPORT_C ~CAgnSortCriteria();
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;

	IMPORT_C TInt AppendCriterionL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir);	
	IMPORT_C TInt Count() const;
	IMPORT_C const TAgnSortCriterion& AtL(TInt aIndex) const;
	IMPORT_C void SetEntryTypeOrderL(const RArray<CCalEntry::TType>& aTypeOrder);
	IMPORT_C const RArray<CCalEntry::TType>& EntryTypeOrder() const;
	
private:
	CAgnSortCriteria();
	
    RArray<TAgnSortCriterion> iSortCriteria;
	RArray<CCalEntry::TType> iEntryTypeOrder;
	};

class TAgnSortCriterion
	{
public:
	TAgnSortCriterion(CalCommon::TCalSortAttribute iSortAttribute, CalCommon::TCalSortDirection iSortDirection);
	
    CalCommon::TCalSortAttribute iSortAttribute;
	CalCommon::TCalSortDirection iSortDirection;
	};

#endif /*__AGMSORTCRITERIA_H_*/
