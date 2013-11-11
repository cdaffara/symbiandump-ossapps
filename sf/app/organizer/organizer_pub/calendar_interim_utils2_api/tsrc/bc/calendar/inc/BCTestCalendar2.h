/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









#ifndef BCTESTCALENDAR2_H
#define BCTESTCALENDAR2_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <CalenInterimUtils2.h>

#include <CalRRule.h>				//TCalRRule
#include <CalEntry.h>				//CCalEntry
#include <CalUser.h>				//CCalUser
#include <CalEntryView.h>			//CCalEntryView
#include <CalSession.h>				//CCalSession

#include <CalProgressCallback.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 31
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KBCTestCalendar2LogPath, "//logs//testframework//BCTestCalendar2//" ); 
// Log file
_LIT( KBCTestCalendar2LogFile, "BCTestCalendar2.txt" ); 
_LIT( KBCTestCalendar2LogFileWithTitle, "BCTestCalendar2_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBCTestCalendar2;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBCTestCalendar2 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/											
NONSHARABLE_CLASS(CBCTestCalendar2) : public CScriptBase,public MCalProgressCallBack
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTestCalendar2* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBCTestCalendar2();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes
		
		//callbacks
		
				void Completed(TInt aStatus);
	    	void Progress( TInt aPercentageCompleted );
	    	TBool NotifyProgress(); 
        
        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
    
        protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CBCTestCalendar2( CTestModuleIf& aTestModuleIf );

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
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        CCalEntry* CreateNonRepeatingApptL();
        CCalEntry* CreateRepeatingApptL();
        void	CreateSessionAndViewsL();
        CCalEntry* CreateExceptionL(const CCalEntry& aParent);
        
        virtual TInt ExampleL( CStifItemParser& aItem );
        virtual TInt TestNewL( CStifItemParser& aItem );
        virtual TInt TestMRViewEnabledL( CStifItemParser& aItem );
        virtual TInt TestGlobalUidGenL( CStifItemParser& aItem );
        virtual TInt TestIsMeetingRequestL( CStifItemParser& aItem );
        virtual TInt TestPopulateChildL( CStifItemParser& aItem );
        virtual TInt TestStaticPopulateChildL( CStifItemParser& aItem );
        virtual TInt TestStoreL( CStifItemParser& aItem );
        
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    
    		//Array of created entries
				RPointerArray<CCalEntry> iEntries;
    
		    CCalenInterimUtils2*		iUtils;
		    CCalSession* 						iCalSession;
		    CCalEntryView* 					iEntryView;
		    CActiveSchedulerWait* iAsWait;
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

#endif      // BCTESTCALENDAR2_H

// End of File
