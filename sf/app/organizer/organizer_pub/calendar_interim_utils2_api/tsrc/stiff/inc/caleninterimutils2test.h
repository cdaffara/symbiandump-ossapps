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
* Description: STIF testclass CCalenInterimUtils2Test declaration.
*
*/

#ifndef CALENINTERIMUTILS2TEST_H
#define CALENINTERIMUTILS2TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <CalenInterimUtils2.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calentryview.h>


// MACROS
#define INTERIMUTILS2_TEST_CLASS_VERSION_MAJOR 1
#define INTERIMUTILS2_TEST_CLASS_VERSION_MINOR 0
#define INTERIMUTILS2_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalenInterimUtils2TestLogPath, 
        "\\logs\\testframework\\CalenInterimUtils2Test\\" ); 
// Log file
_LIT( KCalenInterimUtils2TestLogFile, 
        "CalenInterimUtils2Test.txt" ); 
_LIT( KCalenInterimUtils2TestLogFileWithTitle, 
        "CalenInterimUtils2Test_[%S].txt" );

/**
*  CCalenInterimUtils2Test test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib CalenInterimUtils2.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalenInterimUtils2Test ) : public CScriptBase, 
												public MCalProgressCallBack
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalenInterimUtils2Test* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalenInterimUtils2Test();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions
        void Progress(TInt /*aPercentageCompleted*/) {};
		void Completed(TInt aError);
		TBool NotifyProgress() { return EFalse; };
    	
    private:

        /**
        * C++ default constructor.
        */
        CCalenInterimUtils2Test( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Method used to log version of test class
        */
        void SendTestClassVersion();
        
        /**
        * Test funtion to test the API CCalenInterimUtils2::GlobalUidL
        * @param aItem Script line containing parameters.
        */
        TInt TestGlobalUUIDL( CStifItemParser& aItem );
        
        TInt TestPopulateChildEntryL( CStifItemParser& aItem );
        
        TInt TestStoreL( CStifItemParser& aItem );
        
    private:    // New functions
        
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        CCalenInterimUtils2* iCalenInterimUtils;
        CCalSession* iCalSession;
        CCalEntryView* iCalEntryView;
        CActiveSchedulerWait* iWait;
        TInt iError;
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif  //CALENINTERIMUTILS2TEST_H

// End of File
