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

#if (!defined __T_CR_ALARM_H__)
#define __T_CR_ALARM_H__



#include "T_CrSuiteStepBase.h"



_LIT(KTestCalInterimApiAlarmBasic,"TestCalInterimApiAlarmBasic");

class CTestCalInterimApiAlarmBasic: public CCrTestStepBase
	{
public:
	CTestCalInterimApiAlarmBasic();
	~CTestCalInterimApiAlarmBasic();
	void CreateTwoAlarmedEntriesL(RPointerArray<CCalEntry>& aEntries);
	TVerdict doTestStepL();
	};


_LIT(KTestCalInterimApiAlarmMany,"TestCalInterimApiAlarmMany");

class CTestCalInterimApiAlarmMany: public CCrTestStepBase
	{
public:
	CTestCalInterimApiAlarmMany();
	~CTestCalInterimApiAlarmMany();
	TVerdict doTestStepL();

private:
	struct TTestAlarm
		{
	public:
		TBool iFixed;
		TInt iAlarmTime;
		TBuf<16> iName;
		};

private:
	void GetAlarmsFromConfigL(const TDesC& aList, RArray<TTestAlarm>& aAlarms);
	void GetAlarmFromConfigL(const TDesC& aAlarmName, TTestAlarm& aAlarm);
	void CreateAlarmedEntriesL(RPointerArray<CCalEntry>& aEntries, RArray<TTestAlarm> aAlarms, RPointerArray<CCalAlarm>& aCalAlarms);
	};


#endif //__T_CR_ALARM_H__
