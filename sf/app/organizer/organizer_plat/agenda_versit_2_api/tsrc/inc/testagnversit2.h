/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test agnversit2 api
*
*/



#ifndef __TESTAGNVERSIT2_H__
#define __TESTAGNVERSIT2_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include <e32base.h>
#include <e32def.h>
#include <s32file.h>                // RFileReadStream
#include "AgnImportObserver.h"		// MAgnImportObserver
#include "AgnExportObserver.h"		// MAgnExportObserver
#include <calprogresscallback.h>
#include <CalSession.h>				// CCalSession


//  FORWARD DECLARATIONS
class CCalSession;
class CAgnExternalInterface;
class CCalEntry;
class CCalEntryView;
class RAgendaServ;
class RFileReadStream;
class CCalEntryView;                // Calendar entry view
class CCalenGlobalData; 
class CEikonEnv;

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 * EUnitWizard generated test class. 
 */
NONSHARABLE_CLASS( CCAgnVersit2Test )
     : public CEUnitTestSuiteClass, MAgnImportObserver
     , MAgnExportObserver, MCalProgressCallBack
       
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCAgnVersit2Test* NewL();
        static CCAgnVersit2Test* NewLC();
        /**
         * Destructor
         */
        ~CCAgnVersit2Test();

    private:    // Constructors and destructors

        CCAgnVersit2Test();
        void ConstructL();

    public:     // From MCalProgressCallBack

        void Completed(TInt aError);
            
        TBool NotifyProgress();
        
        void Progress(TInt /*aPercentageCompleted*/);
        
    private:    // New methods

        void SetupL();
        
        void Teardown();
         
        void doTestStepL();
         
        void SVEventL();
         
        void SVEventAttendeeL();
         
        void SVEventCategoriesL();
         
        void SVEventDescriptionL();
         
        void SVEventDurationL();
         
        void SVEventExDateL();
         
        void SVEventLastModL();
         
        void SVEventLocationL();
         
        void SVEventOrganizerL();
         
        void SVEventRDateL();
         
        void SVEventRecurIdL();
         
        void SVEventRRuleL();
         
        void SVEventSequenceL();
         
        void SVEventStatusL();
         
        void SVEventSummaryL();
         
        void SVEventRRuleTzL();
         
        void OutlookAllDayRepeatWeeklyL();
         
        void NotSupportedContinueL();
         
        void NotSupportedSkipL();

        void NotSupportedStopL();

        void NotSupportedLeaveL();
        
   //     void NotSupportedDieL();
        
        void RRuleNumberingWeeklyL();
        
        void RRuleNumberingMonthlyL();

        void RRuleNumberingYearlyL();

        // iTip Method Tests
        void VEventPublishL();
        
        void VEventRequestL();
        
        void VEventReplyL();
        
        void VEventAddL();
        
        void VEventCancelL();
        
        void VEventRefreshL();
        
        void VEventCounterL();
        
        void VEventDeclineCounterL();
         
        void VTodoPublishL();
        
        void VTodoRequestL();
        
        void VTodoReplyL();
        
        void VTodoAddL();
        
        void VTodoCancelL();
        
        void VTodoRefreshL();
        
        void VTodoCounterL();
        
        void VTodoDeclineCounterL();
        
        void NotAnIcalL();
        
        void RRuleTzAllDaysFromIntL();
        
        void AlternativeExportL();
        
        void FloatingTimeL();
        
        void AddAnniversaryL();
        
        void AllCategoriesL();
        
        void AttendeeRoleStatusL();
        
        void VTodoStatusL();
        
        void VEventStatusL();
        
        void MyOwnTestL();
         
     	// From MAgnImportObserver.
    	MAgnImportObserver::TImpResponse AgnImportErrorL(TImpError aType, const TDesC8& aUid, const TDesC& aContext);

    	// From MAgnExportObserver.
    	MAgnExportObserver::TExpResponse AgnExportErrorL(TExpError aType, const TDesC8& aUid, const TDesC& aContext);
	        
        void DoCompletedL(TInt aFirstPassError);
        
        /**
	     * Sets the Calendar view creation status.
	     */

		void SetCalendarViewStatus(TInt);

	    /**
	     * Gets the Calendar view creation status.
	     */

	    TInt CalendarViewStatus();
        
        //TInt Install( const TDesC& aPackagePath );
                
    protected:
    
        void ClearGuidAndLuidL(RPointerArray<CCalEntry>& aEntries);    
    
        TBool CompareL(RPointerArray<CCalEntry>& aEntries1, RPointerArray<CCalEntry>& aEntries2);
    
        void WriteToDbL(const RPointerArray<CCalEntry> &aEntryArray );

        void FetchFromDbL(RPointerArray<CCalEntry>& aEntriesToFetch, RPointerArray<CCalEntry>& aFethedEntries);
        
        void RemoveImportedEntriesFromDBL(RPointerArray<CCalEntry>& aEntriesToDelete);
                
    	void ImportL();
    
    	void ExportL(const RPointerArray<CCalEntry> &aEntryArray, HBufC* aExportFile );
    
        TInt GetExportFlagsL(const CCalEntry& aEntry);
    	
    	// Helper methods.
    	TBool CompareFilesL( HBufC* aFile1, HBufC* aFile2 );
    	
    	void ErrorCheck(const TDesC& aDes);
    
    	void GetLineL(RFileReadStream& aStream, TPtr8& aPtr);

    	void GetStringFromFileL(const TDesC8& aSectName, const TDesC8& aKeyName, TPtrC& aResult);
    	
    	HBufC* Convert8To16BitL(const TDesC8& aDes8);
    	
    	void GetFilesL(const TDesC8& aDes8);

        TBool CleanDatabaseL();
    	
    	void SetInitialTimeZoneL();
    	
    	void SetOriginalTimeZoneL();
    private:    // Data
    
    	RFs iFs;
    	RPointerArray<CCalEntry> iImportedEntries;
        RPointerArray<CCalEntry> iEntriesFromDb1;
        RPointerArray<CCalEntry> iEntriesFromDb2;
        TTime iEndTime;    	// to delete entries starting at midnight
        CEikonEnv* iEikEnv;
                
    	CAgnExternalInterface* iInterface;
        CCalEntryView* iEntryView;
        
    	HBufC* iInputDir;	// Directory to import from
    	HBufC* iOutputDir;	// Directory to export to
    	HBufC* iImportFile;	// File to import from
    	HBufC* iExportFile1;	// File to export to
    	HBufC* iExportFile2;	// File to export to    	
    	HBufC* iCheckFile;	// File to verify export against

        CActiveScheduler* iAs;
        CActiveSchedulerWait* iWait;        
    	CCalSession* iCalSession;
    	TBool iOOMTesting;	// Perform out of memory testing
    	TInt iAllocCells;	// Number of allocated heap cells
 
     	TBool iDieOnError;	// Leave when receiving an error
    	MAgnImportObserver::TImpResponse iOnImpError;	// Response to give to import errors
    	MAgnExportObserver::TExpResponse iOnExpError;	// Response to give to export errors
    	
    	HBufC* iString;
   
		TUint32 iCurrentZone;
	    /**
	    * Pointer to CActiveSchedulerWait object which is  entry view.
	    * Own. Remember to release after use!
	    */
	    CActiveSchedulerWait* iEntryAsyncWait;
		
	    /**
	    * Pointer to CActiveSchedulerWait object to wait for completion of current view creation request.
	    * Own. Remember to release after use!
	    */

	    CActiveSchedulerWait* iQueueManagerAsw;
	  
	    /**
	    * Calendar entry view creation status.
	    */
		enum TCalViewCreateStatus
			{
			ECalViewCreateError = -1,
			ECalViewCreateNone = 0,
			ECalViewCreateReqPending,
			ECalViewCreateCompleted
		  	};
	  
	    TInt iCalEntryViewStatus;     
    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __TESTAGNVERSIT2_H__

// End of file
