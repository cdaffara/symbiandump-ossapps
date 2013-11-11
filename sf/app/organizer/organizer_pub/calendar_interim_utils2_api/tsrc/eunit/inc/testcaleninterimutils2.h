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
* Description:   test caleninterimutils2 api
*
*/



#ifndef __TESTCALENINTERIMUTILS_H__
#define __TESTCALENINTERIMUTILS_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <e32def.h>

//  INTERNAL INCLUDES
#include <CalProgressCallback.h>	//MCalProgressCallback

//  FORWARD DECLARATIONS
class CCalenInterimUtils2;
class CCalEntry;
class CCalEntryView;
class CCalSession;

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 * EUnitWizard generated test class. 
 */
NONSHARABLE_CLASS( CCalenInterimUtils2Test )
     : public CEUnitTestSuiteClass, public MCalProgressCallBack
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCalenInterimUtils2Test* NewL();
        
        /**
         * Two phase construction
         */
        static CCalenInterimUtils2Test* NewLC();
        
        /**
         * Destructor
         */
        ~CCalenInterimUtils2Test();

    private:    // Constructors and destructors
    
		/**
         * Default constructor
         */
        CCalenInterimUtils2Test();
        
        /**
         *  Two phase construction
         */ 
        void ConstructL();

    public:     // From MCalProgressCallback
	    
	    /**
         * Current operation is finished
         */
	    void Completed(TInt aStatus);
        
        /**
         * Percentage complete of the current operation. 
         */
        void Progress( TInt aPercentageCompleted );

        /**
         * Notify the progress
         */
	    TBool NotifyProgress();

    private:    // New methods
        
        void SetupL();

        void Teardown();
      
        void CCalenInterimUtils2TestNewL();

        void GlobalUidL();
        
        void PopulateChildFromParentL();
       
        void MRViewersEnabledL();
       
        void StoreL();
         
        void Store2L();
        
        void IsMeetingRequestL();

		CCalEntry* CreateNonRepeatingApptL();

		CCalEntry* CreateRepeatingApptL();
		
		CCalEntry* CreateExceptionL(const CCalEntry& aParent);

        CCalEntry* ImportSingleEntryL( CCalSession& aSession, 
                                   const TDesC8& aVCal );

        CCalEntry* ImportSingleEntryL( CCalSession& aSession, 
                               RReadStream& aStream );
		
    private:    // Data

        CCalenInterimUtils2* iUtils;
        CCalSession* iCalSession;
        CCalEntryView* iEntryView;
        CActiveSchedulerWait* iAsWait;

		//Array of created entries
		RPointerArray<CCalEntry> iEntries;

        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __TESTCALENINTERIMUTILS_H__

// End of file
