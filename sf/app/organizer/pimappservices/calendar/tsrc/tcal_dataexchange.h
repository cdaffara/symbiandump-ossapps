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

#ifndef __DATAEXCHANGETESTMANAGER_H__
#define __DATAEXCHANGETESTMANAGER_H__


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
class CDataExchangeTestManager;

// A callback for the multi async import test.
class CAsyncImportDataExCallback : public MCalDataExchangeCallBack
    {
    public:
        void Progress( TInt aPercentageCompleted );
	    void Completed();
    };

//Used by CDataExchangeTestManager::ChangeTimeZoneAndRunTest
typedef void (CDataExchangeTestManager::*Test)(TBool aIsSynch);

class CDataExchangeTestManager : public CBase, public MCalDataExchangeCallBack
	{
public:
    static CDataExchangeTestManager* NewLC();
    ~CDataExchangeTestManager();
	void ChangeTimeZoneAndRunTestL(const TDesC8& aTimeZone, Test aTestToRun, TBool aIsSynch);	    
    
    // from MCalDataExchangeCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed();
	TInt NumberOfEntriesToHandleAtOnce();

    void ExtractAlarmL(TBool aIsSynch);
    void ExportFloatingAlarmL(TBool aIsSynch);
	void TestImportEntriesL(TBool aIsSynch);
	void TestLUIDFromImportEntriesL(TBool aIsSynch);
	void TestImportNEntriesL();
	void TestImportEntriesWithTrailingSpaceL(TBool aIsSynch);
	void TestExportEntriesL(TBool aIsSynch);
	void TestImportingInvalidChildEntriesL(TBool aIsSynch);
	void TestPhoneOwnerPersistenceL();
	void TestOrganizerIsStoredL(TBool aIsSynch);
	void TestOrganizerIsStored2();
	void TestGSEntriesL(TBool aIsSynch);
	void TestEntriesL(CCalEntry::TType aType, TBool aIsSynch);
	void TestAlarmEntriesL(CCalEntry::TType aType, TBool aIsSynch);

	void TestEntryUpdateL(TBool aIsSynch);
	void TestReminderEntryExportL(TBool aIsSynch);
	void TestReminderEntryImportL(TBool aIsSynch);
	void TestAsyncImportL(const TDesC& aFileName);
	void TestAsyncExportL(const TDesC& aFileName);
	void SetNumEntriesToImportAndExport(TInt aNumEntries);
	void BadExceptionDateTestL(TBool aIsSynch);
	void TestTzPropertiesL(TBool aIsSynch);
	void ResetAgendaFileL();
	void TestFixedAndFloatingTodoL(TBool aIsSynch);
	void TestTodoWithoutDueDateL(TBool aIsSynch);
	void TestAlarmedTodoWithoutDueDateL(TBool aIsSynch);
	void TestEntriesWithoutDatesL(TBool aIsSynch);
	void TestFloatingExceptionL(TBool aIsSynch);
	void TestTodoCompletedDateL(TBool aIsSynch);
	void TestOutOfRangeRDateL(TBool aIsSynch);
	void TestInvalidUntilDateL(TBool aIsSynch);
	void TestInvalidExceptionsL(TBool aIsSynch);
	
	void CreateVcalFileL(const TDesC8& aVCalData, const TDesC& aFileName);
	void DeleteVcalFileL(const TDesC& aFileName);
	void DoImportL(const TDesC& aVCalendarFile);
	void DoImportLastDayL(const TDesC& aVCalendarFile);
	void TestEntriesL();
	void TestChildEntryL();
 	void TestVCalTypeSequenceL(TBool aIsSynch);
 	void TestMidnightRecIdL(TBool aIsSynch);
 	void TestMidnightRecId2L(TBool aIsSynch);
 	void TestMidnightRecId3L(TBool aIsSynch);

	void TestImportTodoWithTzL(TBool aIsSynch);

	void TestImportTodoWithWrongTzL(TBool aIsSynch);
	void TestUIDImportL(const TDesC& aFileToImport, const TDesC& aFileToExport, TBool aIsSynch);
	void TestRRuleEndDateL(TBool aIsSynch);
 	
    void TestOrphanedEntryL(TBool aIsSynch);
	void ImportAndExportL(const TDesC8& aVCalData, const TDesC& aImportVCalendarFile, TInt aNumEntry, TBool aIsSynch);
	void TestExceptionDatesConversionL(TBool aIsSynch);
    void TestMultipleAsyncImportL();

    void TestUntilDateImportL(TBool aIsSynch);
    void TestUntilDateImportForVCalL(const TDesC8& aVcal, TInt aDay, TInt aHour, TCalTime::TTimeMode aTimeMode, TBool aIsSynch);
    
    void TestRDateAndExceptionL(TBool aIsSynch);

    void StoreEntryL(CCalEntry* aEntry);
    void FetchAllL( RPointerArray<CCalInstance>& aInstances );
    void OutputAllL( RPointerArray<CCalInstance>& aInstances );
    
    void TestToDoStartDateL(TBool aIsSynch);
    void TestFloatingToDoStartDateL(TBool aIsSynch);
    void TestFloatingRecIdL(TBool aIsSynch);
    void TestMonthlyRepeatDateL(TBool aIsSynch);
	void TestImportingAlarmedFloatingAniversaryL(TBool aIsSynch);
    void TestImportExportParentChildEntriesL(const TDesC8& aVcal,
	    									 TBool aIsSynch,	
                                             const TBool aTestParentRptStartTrimToRecId = EFalse,
                                             const TBool aTestParentRptUntilTrimToRecId = EFalse,
                                             const TTime& aNewRecId = Time::NullTTime());
    void TestImportStoreParentChildEntriesL(TBool aIsSynch);
    void BadDaylightRuleL(TBool aIsSynch);
    void TestImportParentChildEntriesL(const TDesC8& aVcal, TBool aIsSynch);
    void TestImportStoreNotMatchingParentChildEntriesL(TBool aIsSynch);
	void TestAttachmentImportExportL(TBool aIsSynch, TBool aNegtiveTest = EFalse);
	void TestAttachmentExportImportL(TBool aIsSynch);
	void PrepareFileForContentIdAttachmentL();
	void TestTzInstanceCountL(TBool aIsSynch);
	void TestFloatingEntryFixedExDateL(TBool aIsSynch);
	void TestExceptedEntryWithRDateL(TBool aIsSynch);
	void TestRDateExceptionL(TBool aIsSynch);
    void PanicOnUpdateL(TBool aIsSynch);
	void TestChildDTSTARTExportL(TBool aIsSynch);
	void TestReccurenceIDRangeL(TBool aIsSynch);
	void TestParentDTENDExportL(TBool aIsSynch) ;
	void TestExDateTimeL(TBool aIsSynch);
    void TestImportingInvalidDayLightL(TImportedDaylightTest aTestToRun, TBool aExpectDaylight, TBool aIsSynch);
    void TestLUIDL(TBool aIsSynch);
    void TestRRuleUntilTimeL(TBool aIsSynch);
	void TestBadTimeZoneL(TBool aIsSynch);
	void TestRepeatEntryHasCountAndEndDateL(TBool aIsSynch);
	void TestExDateUntilTimeL(TBool aIsSynch);
	void TestOOMExportL(TBool aIsSynch);
	void TestImportTodoWithXEpocAlarmL(TBool aIsSynch);
	void TestImportvCalsGeneratesUniqueIdL(TBool aIsSynch);
	void TestImportWithInvalidRRuleDateL(TBool aIsSynch);
	void TestMP1RRuleL(TBool aIsSynch);
	void TestLostRangeL(TBool aIsSynch);
	void EmptyAttachmentL(TBool aIsSynch);

	void TestMailtoL();
	void TestExDateTzProblemL(TBool aIsSynch);
	void AlarmOffsetForUndatedTodoL(TBool aIsSynch);
	void TestTzExportL(TBool aIsSynch);
	void TestTzImportL(TBool aIsSynch);
	void TestCategoryL(TBool aIsSynch);
    void ImportInvalidChildEntryL(TBool aIsSynch);
	void TestImportParentWithoutRRuleL(TBool aIsSynch);
	void TestExportToDoStartDateEqualDueDateL(TBool aIsSynch);
	void TestGEOImportAndExportL(TBool aIsSynch);
	void TestGEOImportL(RPointerArray<HBufC8>& aBufArray, RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch);
	void TestGEOExportL(RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch);
	TBool TestGEOVerifyImportL(const TReal& aExpLat, const TReal& aExpLong, CCalEntry& aEntry);
	TBool TestGEOVerifyExportL(const TDesC &aFileName, CCalEntry& aEntry);
	void TestChildAddedToFirstInstanceL(TBool aIsSynch);
	void TestEntryAlarmTimeL(TBool aIsSynch);
	void TestRDATEOOMExportL(TBool aIsSynch);
	void TestOOMImportL(TBool aIsSynch);
	void TestLocalToUtcConversionAgainstEventDurationL(TBool aIsSynch);
	void TestMaxDateTimeHandlingForRepeatRuleLocalTimeL(TBool aIsSynch);
	//PDEF111292 - Begin
	void TestTzExportKSTL(TBool aIsSynch);
	void TestTzExportPSTL(TBool aIsSynch);
	//PDEF111292 - End
	void TestImportEntryWithoutGuidL(TBool aIsSynch);
	void TestMultipleTZPropertiesL();
	
    void TestImportParentAndChildEntryL(const TDesC8& aParentEntry,
                                        const TDesC8& aChildEntry,
                                        const TInt aExpectedNumOfEntriesStored,
                                        TBool aIsSynch);
    
    void TestDaylightExportOverlapL(TBool aIsSynch);
    void SyncAndAsynchronousTests(TBool aIsSynch);
  	
private:
	void ConstructL();
	void CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName);	
	void ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported, TBool aIsSynch);
	void ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesExported, TBool aIsSynch);	
	void ExportImportAndCheckL(RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch);
	void TestToDoEntryL(CCalEntry& aEntry);
	TBool TestEntry1L(CCalEntry& aEntry);
	TBool TestEntry2L(CCalEntry& aEntry);
	TBool TestEntry3L(CCalEntry& aEntry);
    TBool TestTodoDateImportL( CCalEntry& aEntry, TPtrC8 uidValue );
    void TestAttendeeImportL( CCalEntry& aEntry );
	void TestRecurrenceIdImportL( CCalEntry& aParentEntry, CCalEntry& aChildEntry );
	void PrintAttendeeL(CCalAttendee* aAttendee);
	void ModifyRRuleL(CCalEntry& aEntry, TInt aStartYear, TInt aEndYear, TBool aRepeatForever, TMonth aMonth);
	TBool TestEntriesForTzPropertiesL(RPointerArray<CCalEntry>& aEntryArray, TInt* aExpectedYears, TBool aIsSynch);
	TInt  GetDaylightYear(const TDesC8& aLine);
	TInt  GetDaylightYearsL(const TDesC& aFile, RArray<TInt>& aYears);
	void  SetTimeZoneL(const TDesC8& aTimeZone);
	void  SetCurrentYear(RTz& aTz, TInt aYear);
	TBool CheckTwoArrayEntryL(RPointerArray<CCalEntry>& aSouceArray, RPointerArray<CCalEntry>& aCompareArray);
	void CreateEntriesL(CCalEntry::TType aType, RPointerArray<CCalEntry>& aEntryArray);
	void CreateAlarmTestEntriesL(CCalEntry::TType aType, RPointerArray<CCalEntry>& aEntryArray);

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
								TInt aExpectToStore,TEntryCheckData& aCheckData, TBool aIsSynch);
	void ListEntriesL(RPointerArray<CCalEntry>& aEntryArray);
	TInt NumberAllEntryL();
	
private:
	CCalTestLibrary*  iTestLib;
	CCalDataExchange* iDataExchange;
	TInt              iNumEntriesToImportAndExport;
	CAsyncImportDataExCallback* iAsyncImportCallback;
	};

	
#endif
