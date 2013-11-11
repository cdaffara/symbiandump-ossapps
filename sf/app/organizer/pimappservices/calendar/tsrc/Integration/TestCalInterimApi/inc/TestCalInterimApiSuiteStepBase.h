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



#ifndef __TEST_CAL_INTERIM_API_SUITE_STEP_BASE__
#define __TEST_CAL_INTERIM_API_SUITE_STEP_BASE__

#include <test/testexecutestepbase.h>
#include <calprogresscallback.h>
#include <coreappstest/testserver.h>
#include <calcontent.h>
#include <calalarm.h>
#include <caltime.h>
#include <calentry.h>
#include <caluser.h>
#include <calentryview.h>

// Value of integer when data not given in ini file (default value)
const TInt KValueIntNotIni = -858993460;

const TInt KMinGEOLat = -90;
const TInt KMaxGEOLat = 90;
const TInt KMinGEOLong = -180;
const TInt KMaxGEOLong = 180;
const TInt KMaxGEODecimalPoint = 6;
const TInt KDefaultGEOLatLong = -200;

const TInt KDefaultTranspStatus = 0;

const TInt KMaxLengthString = 100;
const TInt KMaxLengthUid = 13;
const TInt KTimeDelayForDSTChange = 1000000;

#define	MAX_TEXT_MESSAGE	700

// Struct for fetching/updating the properties of the attachment
struct	TAttachment
	{
	TInt	iAttachType;
	TPtrC	iMimeType;
	TPtrC	iValue;
	TPtrC	iName;	
	TInt	iSize;
	TPtrC	iLastModifiedDate;
	TInt	iAttribute;
	TPtrC	iDrive;
	TPtrC	iNewLabel;
	TBool	iHasAttributes;
	TInt	iClearAttribute;
	TBool	iHasClearAttribute;
	TPtrC	iContentId;
	TPtrC	iCreateContentId;
	}; 
 
// Struct for fetching/updating the properties of the entry
struct	TEntryProperties
	{
	TPtrC	iSummary;
	TPtrC	iDescription;
	TPtrC	iRecurrenceId;
	TPtrC	iCategories;
	TBool	iHasAlarm;
	TInt 	iAlarmOffset;
	TPtrC 	iGeoLat;
	TPtrC   iGeoLong;
	TInt 	iTrans;
	TPtrC	iLocation;
	TPtrC	iAttendees;
	TPtrC	iCommonName;
	TInt	iCalRole;
	TBool	iHasRole;
	TInt	iCalStatus;
	TBool	iHasStatus;
	TBool	iRsvp;
	TBool	iHasRsvp;
	TInt	iLocalId;
	TBool	iHasLocalId;
	TInt	iPriority;
	TBool	iHasPriority;
	};

class CCalSession;
class CTestCalInterimApiSuiteStepBase : public CTestStep, protected MCalProgressCallBack
	{
public:
	CTestCalInterimApiSuiteStepBase();
	virtual ~CTestCalInterimApiSuiteStepBase();

	virtual TVerdict		doTestStepPreambleL();
	virtual TVerdict		doTestStepPostambleL();

	static CCalContent*		CreateCalContentL(HBufC8* aContent, HBufC8* aMimeType, CCalContent::TDisposition aDisposition);
	static void 			GetDataFromFileL(const TFileName& aFileName, HBufC8*& aDataBuf);
	static TInt 			ConvertToDisposition(CCalContent::TDisposition& aDisposition, const TDesC& aData);
	static void				SetCalTimeL(const TTime& aTimeData, TCalTime& aCalTimeData, TCalTime::TTimeMode aTimeMode);
	static void 			ResetAndDestroyEntryArray(TAny* aObject);
	static void 			ResetAndDestroyInstanceArray(TAny* aObject);
	static void 			ResetAndDestroyCategoryArray(TAny* aObject);
	static void				ResetAndDestroyAttendeeArray(TAny* aObject);
	static TInt				ConvertStringToBool(const TDesC& aString);
	void 					TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');

protected:
	inline CCalSession&		GetSession();
	inline RFs&				GetFsSession();
	inline const TFileName&	GetCalenderFileName() const;
	inline RPIMTestServer&	GetPIMTestServerL();

	void 					PrintHomeTimeL();
	void 					PrintTimeL(const TTime& aTime);
	TCalTime::TTimeMode		SetTimeMode(const TPtrC& aTimeMode);
	void 					CompareAlarmsL(CCalAlarm* aAlarm1, CCalAlarm* aAlarm2);
	TTime 					FormatDateTime(const TDesC& aParamString);
	void					StoreCalendarEntryL(CCalEntryView* aCalEntryView, CCalEntry* aCalEntry);
	void					CopyImportFileToWritableDriveL();
	void 					OpenSessionL();
	void 					CloseSessionL();
	TReal32 				GetTReal32(const TPtrC& aString);
	TReal32					CalculateTimeDifference(const TTime& aStartTime, const TTime& aEndTime, const TReal32& aMaxTimeLimit);
	TBool					ComputeTimeDifference(const TTime& aStartTime, const TTime& aEndTime, const TReal32& aMaxTimeLimit);
	CCalEntry*				GetMatchingEntryL(RPointerArray<CCalEntry>& aEntries, TCalTime aRecurrenceId);
	void					CompareEntriesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CreateAndOpenCalenderFileL(const TDesC& aFileName, TBool aCreateFile);
	void 				WaitForAgendaServerShutdown();
	
private:
	//	MCalProgressCallBack implementation
	void					Progress(TInt aPercentageCompleted);
	void					Completed(TInt aError);
	TBool					NotifyProgress();

	void					DeleteCalenderFileL();
	void 					DefaultFileNameL();
	
	void					CompareNonRecurrencePropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CompareRepeatRulePropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CompareOrganizerPropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CompareRDatesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CompareExDatesL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void					CompareGeoL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	void 					CompareTransL(CCalEntry* aEntry1, CCalEntry* aEntry2);
	TInt 					OOMGeoValueL(CCalEntry* aEntry, CCalGeoValue*& aGeoValue);
	void					CompareAttachmentPropertiesL(CCalEntry* aEntry1, CCalEntry* aEntry2);

private:
	CActiveScheduler*		iActiveScheduler;
	TBool					iFsSessionConnected;
	TBool					iHasCalenderFileName;


public:
	TBool					iNotifyProgress;
	CCalSession*			iSession;
	TFileName				iCalenderFileName;
	RFs 					iFsSession;
	TCalTime::TTimeMode		iTimeMode;
    TBool                   iHeapCheck;
	RPIMTestServer 			iPIMTestServer;
	TBool 					iTestServerConnected;
	};

#include "TestCalInterimApiSuiteStepBase.inl"

#endif
