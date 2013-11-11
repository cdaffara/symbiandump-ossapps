/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __TEST_CALINTERIMAPI_PARSER_ENTRY_H__
#define __TEST_CALINTERIMAPI_PARSER_ENTRY_H__

#include "TestCalInterimApiParserBase.h"
#include <caltime.h>
#include <calentry.h>
#include <calattachment.h>

/**
This test step sets the recurrence and non-recurrence properties of the entry.
*/
class CCalEntry;
class CCalCategory;
class CTestCalInterimApiParserEntry : public CTestCalInterimApiParserBase
	{
public:
	CTestCalInterimApiParserEntry(CCalEntry& aEntry, CTestStep& aTestStep, MParserCallback& aParserCallback, TCalTime::TTimeMode aTimeMode,RFs* aFs);
	~CTestCalInterimApiParserEntry();
	virtual TInt		ParseL(const TDesC& aCommand, const TDesC& aData);

private:
	void				ProcessDtStartL(const TDesC& aData);
	void				ProcessDtEndL(const TDesC& aData);
	void				ProcessDueL(const TDesC& aData);
	void				ProcessCompletedL(const TDesC& aData);
	void				ProcessUntilL(const TDesC& aData);
	void				ProcessSummaryL(const TDesC& aData);
	void				ProcessLocationL(const TDesC& aData);
	void				ProcessCategoriesL(const TDesC& aData);
	void				ProcessPriorityL(const TDesC& aData);
	void				ProcessStatusL(const TDesC& aData);
	void				ProcessDescriptionL(const TDesC& aData);
	void				ProcessAlarmL(const TDesC& aData);
	void				ProcessOrganizerL(const TDesC& aData);
	void				ProcessAttendeeL(const TDesC& aData);
	void				ProcessRRuleL(const TDesC& aData);
	void				ProcessRDateL(const TDesC& aData);
	void				ProcessExDateL(const TDesC& aData);
	void 				ProcessClearRepeatPropertyL(const TDesC& aData);
	void				ProcessDTStampL(const TDesC& aData);
	void				ProcessReplicationStatusL(const TDesC& aData);
	void				ProcessLocalIdL(const TDesC& aData);
	void				SetAttachType(const TDesC& aData);
	void				ProcessEntryAttachmentL(const TDesC& aData);
	TBool				ValidateFileL(const TDesC& aData);
	void 				ProcessTranspL(const TDesC& aData);
	void 				ProcessGEOLatL(const TDesC& aData);
	void 				ProcessGEOLongL(const TDesC& aData);
	TInt 				OOMSetLatLongL(CCalGeoValue& aGeoValue, const TReal& aResult);
	TInt 				OOMSetGeoValueL(CCalGeoValue& aGeoValue);
	TInt 				OOMSetTimeTransparencyL(const TReal& aResult);

	CCalAttachment::TType	ConvertToAttachType(const TDesC& aData);
	
private:
	CCalEntry&			iEntry;
	TCalTime::TTimeMode	iTimeMode;
	CCalAttachment::TType	iAttachType;
	RFs*				iFs;
	RArray<TCalTime>	iRDateArray;
	RArray<TCalTime>	iExDateArray;
	TBool				iHasDtStart;
	TBool				iHasDtEnd;
	TBool				iHasDue;
	TBool				iHasCompleted;
	TBool				iHasUntil;
	TBool				iHasSummary;
	TBool				iHasLocation;
	TBool				iHasCategories;
	TBool				iHasPriority;
	TBool				iHasStatus;
	TBool				iHasDescription;
	TBool				iHasAlarm;
	TBool				iHasOrganizer;
	TBool				iHasRRule;
	TBool				iHasTransp;
	TReal 				iGEOLat;
	TBool				iHasGEOLat;
	TBool				iHasGEOLong;
	TBool				iHasDTStamp;
	TBool				iHasReplicationStatus;
	TBool				iHasLocalId;
	TBool				iHasAttachType;
	TBool				iBinaryData;
	};

#endif
