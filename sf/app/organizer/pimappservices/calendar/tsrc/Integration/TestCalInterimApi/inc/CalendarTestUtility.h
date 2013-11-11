/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


 
#ifndef CALENDAR_TEST_UTITLITY__INCLUDED_
#define CALENDAR_TEST_UTITLITY__INCLUDED_


#include <test/testexecutestepbase.h>
#include <tz.h>
#include <calentry.h>
#include <calentryview.h>

class CCalendarTestUtility : public CBase
{

public:
    enum TServerCloseType
        {
        EWaitForServerToClose,
        EForceCloseServer,
        };
        
    enum TTimePeriod
    	{
    	EDay,
    	EWeek,
    	EMonth,
    	EYear,
    	};

public:
	virtual ~CCalendarTestUtility();
	static  CCalendarTestUtility* NewLC(CTestStep& aTestStep);
	void    FindAndCloseCalendarServer(
				const TServerCloseType aCloseType,
				const TBool aVerbose
				);
	TBool   GetTimeFromConfig( const TDesC& aSection, const TDesC& aKey, TTime& aTime );
	CTzId*  GetTZFromConfigL( const TDesC& aSection, const TDesC& aKey );
	TBool   PrintBooleanTest(
	            const TBool aResult, 
	            const TText* aCompare, 
	            const TInt aIteration, 
            	const TInt aSubIteration, 
	            const TText* aFile, 
	            const TInt aLine
	            );
	TBool   PrintComparison(
	            const TInt aCompareL, 
	            const TText* aCompareOperator, 
	            const TInt aCompareR, 
	            const TBool aResult, 
	            const TInt aIteration, 
	            const TInt aSubIteration, 
	            const TText* aFile, 
	            const TInt aLine
	            );
	TBool   PrintComparison(
	            const TDesC& aCompareL, 
	            const TText* aCompareOperator, 
	            const TDesC& aCompareR, 
	            const TBool aResult, 
	            const TInt aIteration, 
	            const TInt aSubIteration, 
	            const TText* aFile, 
	            const TInt aLine
	            );
	TBool PrintComparison(
	            const TDesC8& aCompareL, 
	            const TText* aCompareOperator, 
	            const TDesC8& aCompareR, 
	            const TBool aResult, 
	            const TInt aIteration, 
	            const TInt aSubIteration, 
	            const TText* aFile, 
	            const TInt aLine
	            );            
	TBool 	EntryWithinTimeRange(
		        const CCalEntry& aEntry,
		        const CalCommon::TCalTimeRange& aTimeRange
		        );
	void 	DeleteEntryL( 
				CCalEntryView& aEntryView, 
				const CCalEntry& aEntry 
				);
	static 	TBool CompareEntriesL(
				const CCalEntry& aEntry1, 
				const CCalEntry& aEntry2 
				);

	TBool 	CompareEntryListsL(
				const RPointerArray< CCalEntry >& aList1,
				const RPointerArray< CCalEntry >& aList2
				);

	void 	SetPeriodStartAndEndDates(
				TTime& aStartTime, 
				TTime& aEndTime, 
				TTimePeriod aPeriod
				);
	template< class T > static void CopyArrayL( 
				const RArray< T >& aOriginal, 
				RArray< T >& aCopy 
				);
	template< class T > static void CopyArrayL( 
				const RPointerArray< T >& aOriginal, 
				RPointerArray< T >& aCopy 
				);

#ifndef __WINSCW__
	void 		FillDiskL();
	void 		ClearDiskL();
	void 		HighDiskModeL();
	void 		RefillL();				
#endif

protected:
	CCalendarTestUtility(CTestStep& aStep);

private:
	void WaitForProcessToClose(
			const TDesC& aProcessName, 
			const TInt aIteration,
			const TBool aVerbose
			);
	void CloseProcess(
			const TDesC& aProcessName, 
			const TInt aIteration,
			const TBool aVerbose
			);
	
private:
	CTestStep& 				iTestStep;
	RFs 					iFsSession;
	RFile					iFile;
	TInt					iManyFiles;

};

template < class T >
class CleanupResetDestroy
	{
public:
	static void PushL(RPointerArray< T >& aRef);
private:
	static void ResetDestroy(TAny *aPtr);
	};

#include "calendartestutility.inl"
					
#endif // ifndef(CALENDAR_TEST_UTITLITY__INCLUDED_)
