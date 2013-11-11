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

#ifndef __AGSSORT_H__
#define __AGSSORT_H__

#include <e32base.h>

class TAgnDaySortKey : public TKeyArrayFix
	{
public:
	TAgnDaySortKey(const TTime& aTargetDate, const TTime& aUndatedTodoTime);
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	void SetTargetDate(const TTime& aTargetDate);
protected:
	TInt OrderByType(TUint aLeft,TUint aRight) const;
	TInt OrderByCompletedDate(TUint aLeft,TUint aRight) const;
	TInt OrderByAppt(TUint aLeft,TUint aRight) const;
	TInt OrderByToDo(TUint aLeft,TUint aRight) const;
	TInt OrderByRepeating(TUint aLeft,TUint aRight) const;
	TInt OrderByDatePriority(TUint aLeft,TUint aRight) const;
	TInt OrderById(TUint aLeft,TUint aRight) const;
	TInt OrderByFileId(TUint aLeft,TUint aRight) const;
	TInt OrderByPriority(TUint aLeft,TUint aRight) const;
	TInt OrderByDate(TUint aLeft,TUint aRight) const;
	TInt OrderByTime(TUint aLeft,TUint aRight) const;
	TTime UndatedTodoTime() const { return iUndatedTodoTime; }
private:
	TTime iUndatedTodoTime;
	TTime iTargetDate;
	};

NONSHARABLE_CLASS(TAgnAlarmSortKey): public TKeyArrayFix
	{
public:
	TAgnAlarmSortKey();
	virtual TInt Compare(TInt aLeft, TInt aRight) const;
	};
	
#endif
