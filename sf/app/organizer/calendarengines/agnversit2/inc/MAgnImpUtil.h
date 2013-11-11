/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This file contains pure virtual declarations of utility
*                functions used in the iCal parser implementation.
*
*/

#ifndef MAgnImpUtil_H
#define MAgnImpUtil_H

//System includes
#include <caltime.h>

//User includes
#include <AgnImportObserver.h>	// for MAgnImportObserver::TImpResponse

#include "ICalRuleSegment.h"	// For CICalRuleSegment

//Forward declarations
class CICalProperty;
class CTzRules;
class CTzNamedRules;

// Constants.
const TInt KSecondsInOneMinute = 60;
const TInt KMinutesInOneHour = 60;
const TInt KMaxWeekDayNum = 4;
const TInt KDaysPerWeek = 7;

//Class declarations
class MAgnImpUtil
    {
    public:	// Enumerations
	    enum TValueType
		    {
		    EDefault,
		    EDate,
		    EDateTime,
		    ETime,
		    EDuration,
		    EPeriod
		    };
			    
	public: //Helper functions
	    virtual void GetCalTimeL(const CICalProperty& aProperty, TCalTime& aTime, TValueType aValueType, TInt aValue = 0) = 0;
	    virtual TInt FindRuleSegment(const RPointerArray<CICalRuleSegment>& aRules, CICalRuleSegment::TSegmentType aType) const = 0;
	    virtual const CTzRules* FindTzRule(const TDesC& aName) const = 0;

    public: //Report errors
	    virtual void ReportErrorL(MAgnImportObserver::TImpError aType, const TDesC8& aUid, const TDesC& aContext, TBool aCanContinue = ETrue) = 0;
    };
   
#endif	// MAgnImpUtil_H