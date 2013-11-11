// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ARITHMETICALCAL_H__
#define __ARITHMETICALCAL_H__

#include <e32std.h>
#include "calconvcalendar.h"

class TArithmeticalCalendar : public TCalendar  
	{
public:
	TArithmeticalCalendar();

protected:
	TInt DayMonthInYear(const TInt) const;
	TInt DaysMonthsElapsed(const TInt) const;
	virtual TBool IsLeapYear(const TInt) const;
	};


#endif
