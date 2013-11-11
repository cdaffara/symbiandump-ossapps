/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MmsApplicationTest  declaration*
*/




#ifndef MMSAPPLICATIONTEST_H
#define MMSAPPLICATIONTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <msvids.h>
#include <MSVAPI.H>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMmsApplicationTestLogPath, "\\logs\\testframework\\MmsApplicationTest\\" ); 
// Log file
_LIT( KMmsApplicationTestLogFile, "MmsApplicationTest.txt" ); 
_LIT( KMmsApplicationTestLogFileWithTitle, "MmsApplicationTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMmsApplicationTest;
class CMmsApplicationAdapter;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMmsApplicationTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMmsApplicationTest) : public CScriptBase ,public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmsApplicationTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMmsApplicationTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        //From MmsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

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
        CMmsApplicationTest( CTestModuleIf& aTestModuleIf );

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
        virtual TInt ExampleL( CStifItemParser& aItem );
        /**
        * Test Method for RegisterL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisterL( CStifItemParser&  );
        /**
        * Test Method for UnRegisterL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UnRegisterL( CStifItemParser&  );
        /**
        * Test Method for RegisteredL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisteredL( CStifItemParser&  );
        /**
        * Test Method for FolderIdL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FolderIdL();
        /**
        * Test Method for ApplicationIdL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ApplicationIdL();
        /**
        * Test Method for SetApplicationIdL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetApplicationIdL();
        /**
        * Test Method for ReplyToApplicationIdL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ReplyToApplicationIdL();  
        /**
        * Test Method for SetReplyToApplicationIdL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt SetReplyToApplicationIdL();
        /**
        * Test Method for DeleteL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt DeleteL();
        /**
        * Test Method for MmscUrlL of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt MmscUrlL();
        /**
        * Test Method for RegisteredApplicationList of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisteredApplicationListL();
        /**
        * Test Method for RegisteredApplicationList of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RegisterWithNameL();
        /**
        * Test Method for UserFriendlyName of CMmsApplicationAdapter class
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt UserFriendlyNameL();
        
        
        /**
        * Method for creating the message int he message store
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TMsvId CreateMessageL();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        CMmsApplicationAdapter* iAppAdapter;
        CMsvSession* iSession;
        

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
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

#endif      // MMSAPPLICATIONTEST_H

// End of File
