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
* Description:   MMSConformanceAPITest  declaration*
*/




#ifndef MMSCONFORMANCEAPITEST_H
#define MMSCONFORMANCEAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMMSConformanceAPITestLogPath, "\\logs\\testframework\\MMSConformanceAPITest\\" ); 
// Log file
_LIT( KMMSConformanceAPITestLogFile, "MMSConformanceAPITest.txt" ); 
_LIT( KMMSConformanceAPITestLogFileWithTitle, "MMSConformanceAPITest_[%S].txt" );
//Test data path
#ifdef __WINSCW__
_LIT( KTestFilePath, "C:\\testing\\Data\\MmsconformanceData\\%S" );
#endif

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMMSConformanceAPITest;
class CMmsConformance;
class CMsgMediaInfo;


// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMMSConformanceAPITest test class for STIF Test Framework TestScripter.
*  This Module Tests "MMSCoformanceAPI" that covers MMSConformance.h
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMMSConformanceAPITest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMMSConformanceAPITest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMMSConformanceAPITest();

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
        CMMSConformanceAPITest( CTestModuleIf& aTestModuleIf );

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

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        /**
        * Test case for NewL of CMmsConformance
        */
        TInt TestMmsConfNewL( CStifItemParser& aItem );
         
        /**
        * Test case for NewLC of CMmsConformance
        */
        TInt TestMmsConfNewLC( CStifItemParser& aItem );
        
        /**
        * Test case for IsConformantMime of CMmsConformance
        */
        TInt TestIsConformantMimeL( CStifItemParser& aItem );
        
        /**
        * Test case for MediaConformance of CMmsConformance
        */
        TInt TestMediaConformanceL( CStifItemParser& aItem );
        
        /**
        * Test case for ConformanceVersion of CMmsConformance
        */
        TInt TestConformanceVersion( CStifItemParser& aItem );        
        
        //HelperFunctions
        /**
        * Initializes iMediaInfo which will be used in other testcases
        */
        TInt InitDataL( CStifItemParser& aItem );
        
        /*
        * Cleans up data initialized in InitDataL
        * call it after calling InitDataL...
        */
        void CleanUp();        
         

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        //Pointer to 
        CMmsConformance* iMmsConformance;
        CMsgMediaInfo*   iMediaInfo;
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MMSCONFORMANCEAPITEST_H

// End of File
