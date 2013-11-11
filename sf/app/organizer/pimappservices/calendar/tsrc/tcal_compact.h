// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALCOMPACTTESTMANAGER_H__
#define __CALCOMPACTTESTMANAGER_H__

#include "caltestlib.h"
#include <calalarm.h>
#include <calrrule.h>
#include <calcategory.h>
#include <caluser.h>

#include <calentryview.h>

RTest test(_L("tcal_compact"));

_LIT(KCalendarFile, "tcal_compact");
_LIT(KCalendarFilePath, "c:\\private\\10003a5b\\tcal_compact");

_LIT(KEntryGuid, "Compact_Guid");
const int KMinEntriesBeforeCompact = 31;

class CCompactTestManager : public CBase
	{
public:
	~CCompactTestManager();
	static CCompactTestManager* NewLC();
	static CCompactTestManager* NewL();
	void TestSynchronousCompactingL();
	void AddEntriesToCalendarFileL(TInt aNumEntries);
	void AddCategoriesAndAttendeesToEntryL(CCalEntry& aEntry);

private:
	CCalTestLibrary*	iTestLibrary;	

	// No copy constructor and assignment operator
	CCompactTestManager(CCompactTestManager&);
	CCompactTestManager& operator = (const CCompactTestManager&);
	CCompactTestManager();
	
	void ConstructL();
	};






























#include <caldataexchange.h>
#include "caltestlib.h"
#include <tz.h>
#include <coreappstest/testserver.h>

enum TImportedDaylightTest
     {
     EImportedDLRuleBadStart,
     EImportedDLRuleBadEnd,
     EImportedDLRuleEndBeforeStart,
     EImportedDLRuleMissing,
     EImportedDLOneGoodRule,
     EImportedDLTwoBadRules,
     EImportedDLOneBadOneGoodRule,
     EImportedDLNoTZOneGoodRule,
     EImportedDLNoTZTwoBadRules,
     EImportedDLNoTZOneBadOneGoodRule,
     EImportedDLNoTZMissing
     };
      

class CCalInstance;

// A callback for the multi async import test.
class CAsyncImportDataExCallback : public MCalDataExchangeCallBack
    {
    public:
        void Progress( TInt aPercentageCompleted );
	    void Completed();
    };

class CDataExchangeTestManager : public CBase, public MCalDataExchangeCallBack
	{
public:
    static CDataExchangeTestManager* NewLC();
    ~CDataExchangeTestManager();
    void ExtractAlarm();
    void ExportFloatingAlarm();
	void TestImportEntries();
	void TestLUIDFromImportEntries();
	void TestImportNEntriesL();
	void TestImportEntriesWithTrailingSpaceL();
	void TestExportEntries();
	void TestImportingInvalidChildEntries();
	void TestPhoneOwnerPersistence();
	void TestOrganizerIsStored();
	void TestOrganizerIsStored2();
	void TestGSEntries();
	void TestEntries(CCalEntry::TType aType);
	void TestAlarmEntries(CCalEntry::TType aType);

	void TestEntryUpdateL();
	void TestReminderEntryExportL();
	void TestReminderEntryImportL();
	void TestAsyncImportL(const TDesC& aFileName);
	void TestAsyncExportL(const TDesC& aFileName);
	void SetNumEntriesToImportAndExport(TInt aNumEntries);
	void BadExceptionDateTestL();
	void TestTzPropertiesL();
	void ResetAgendaFileL();
	void TestFixedAndFloatingTodoL();
	void TestTodoWithoutDueDateL();
	void TestEntriesWithoutDatesL();
	void TestFloatingExceptionL();
	void TestTodoCompletedDateL();
	void TestOutOfRangeRDateL();
	void TestInvalidUntilDateL();
	void TestInvalidExceptionsL();
	
	// from former tcal_entry
	void CreateVcalFileL(const TDesC8& aVCalData, const TDesC& aFileName);
	void DeleteVcalFileL(const TDesC& aFileName);
	void DoImportL(const TDesC& aVCalendarFile);
	void DoImportLastDayL(const TDesC& aVCalendarFile);
	void TestEntriesL();
	void TestChildEntry();
 	void TestVCalTypeSequenceL();
 	void TestMidnightRecIdL();
 	void TestMidnightRecId2L();
 	void TestMidnightRecId3L();

	void TestImportTodoWithTzL();
	void TestImportTodoWithWrongTzL();
	void TestUIDImportL(const TDesC& aFileToImport, const TDesC& aFileToExport);
	void TestRRuleEndDateL();
 	
    void TestOrphanedEntryL();
	void ImportAndExportL(const TDesC8& aVCalData, const TDesC& aImportVCalendarFile, TInt aNumEntry, TBool aIsAsynch);
	void TestExceptionDatesConversionL();
    void TestMultipleAsyncImportL();

    void TestUntilDateImportL();
    void TestUntilDateImportForVCalL(const TDesC8& aVcal, TInt aDay, TInt aHour, TCalTime::TTimeMode aTimeMode);
    
    void TestRDateAndExceptionL();

    void StoreEntryL(CCalEntry* aEntry);
    void FetchAllL( RPointerArray<CCalInstance>& aInstances );
    void OutputAllL( RPointerArray<CCalInstance>& aInstances );
    
    void TestToDoStartDate();
    void TestFloatingToDoStartDateL();
    void TestFloatingRecIdL();
    void TestMonthlyRepeatDateL();
	void TestImportingAlarmedFloatingAniversaryL();
    void TestImportExportParentChildEntriesL(const TDesC8& aVcal);
    void TestImportStoreParentChildEntriesL();
    void BadDaylightRuleL();
    void TestImportParentChildEntriesL(const TDesC8& aVcal);
    void TestImportStoreNotMatchingParentChildEntriesL();
	void TestAttachmentImportExportL();
	void TestAttachmentExportImportL();
	void PrepareFileForContentIdAttachmentL();
	void TestTzInstanceCountL();
	void TestFloatingEntryFixedExDateL();
	void TestExceptedEntryWithRDateL();
	void TestRDateExceptionL();
    void PanicOnUpdateL();
	void TestChildDTSTARTExportL();
	void TestReccurenceIDRangeL();
	void TestParentDTENDExportL() ;
	void TestExDateTimeL();
	void TestBadTimeZoneL();
	void TestRRuleUntilTimeL();


	// from MCalDataExchangeCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed();
	TInt NumberOfEntriesToHandleAtOnce();
	void TestMailtoL();
	void TestExDateTzProblemL();
	void AlarmOffsetForUndatedTodoL();
	void TestTzExport();
	void TestTzImport();
	
private:
	void ConstructL();
	void CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName);	
	void ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported, TBool aIsAsynch = ETrue);
	void ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesExported, TBool aIsAsynch = ETrue);
	void ExportImportAndCheckL(RPointerArray<CCalEntry>& aEntryArray);
	void TestToDoEntryL(CCalEntry& aEntry);
	TBool TestEntry1L(CCalEntry& aEntry);
	TBool TestEntry2L(CCalEntry& aEntry);
	TBool TestEntry3L(CCalEntry& aEntry);
    TBool TestTodoDateImportL( CCalEntry& aEntry, TPtrC8 uidValue );
    void TestAttendeeImportL( CCalEntry& aEntry );
	void TestRecurrenceIdImportL( CCalEntry& aParentEntry, CCalEntry& aChildEntry );
	void PrintAttendeeL(CCalAttendee* aAttendee);
	void ModifyRRuleL(CCalEntry& aEntry, TInt aStartYear, TInt aEndYear, TBool aRepeatForever);
	TBool TestEntriesForTzPropertiesL(RPointerArray<CCalEntry>& aEntryArray, TInt* aExpectedYears);
	TInt  GetDaylightYear(const TDesC8& aLine);
	TInt  GetDaylightYearsL(const TDesC& aFile, RArray<TInt>& aYears);
	void  SetTimeZoneL(const TDesC8& aTimeZone);
	void  SetCurrentYear(RTz& aTz, TInt aYear);
	TBool CheckTwoArrayEntryL(RPointerArray<CCalEntry>& aSouceArray, RPointerArray<CCalEntry>& aCompareArray);

 	TBool CheckStringInFileL(const TDesC8 &aString, const TDesC &aFileName, TBool aToBeFound) ;

private:	// TestVCalTypeSequence
	struct TEntryCheckData
		{
		TInt iPos;
		CCalEntry::TType iType;
		TTime iStartTime;
		TTime iEndTime;
		};
	void TestVCalTypeSequenceL(const TDesC8& aVCalData, TInt aKEntryNum, 
								TInt aExpectToStore,TEntryCheckData& aCheckData);
	void ListEntriesL(RPointerArray<CCalEntry>& aEntryArray);
	
private:
	CTestRegister*    iFileRegister; 
	CCalTestLibrary*  iTestLib;
	CCalDataExchange* iDataExchange;
	TInt              iNumEntriesToImportAndExport;
	CAsyncImportDataExCallback* iAsyncImportCallback;
	};

	
#endif
