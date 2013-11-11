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
* Description:    
*
*/


#ifndef APPREGEXEC_TEST_H
#define APPREGEXEC_TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <e32std.h> 
#include <f32file.h> 
#include <bautils.h>


// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KAppRegister_testLogPath, "\\logs\\testframework\\AppRegExec_test\\" ); 
// Log file
_LIT( KAppRegister_testLogFile, "AppRegExec_test.txt" ); 
_LIT( KAppRegister_testLogFileWithTitle, "AppRegExec_test_[%S].txt" );

_LIT(KMatchName,"AppRegExec.exe"); 
_LIT( KAppRegisterEXE,"C:\\Sys\\Bin\\AppRegExec.exe");

_LIT( KAppUnRegisterEXE,"C:\\Sys\\Bin\\AppUnRegister.exe");
_LIT( KTestDataEXE,"C:\\Sys\\Bin\\Hello3.exe");
_LIT( KTestDataRSC_SRC,"C:\\data\\Hello3_reg.RSC");
_LIT( KTestDataRSC_DST,"c:\\private\\10003a3f\\import\\apps\\Hello3_reg.RSC");

_LIT(KTestDataSessionPath,"C:\\logs\\");
_LIT(KTestDataFile,"stopmodetestdata.txt");
_LIT(KResourceSessionPath,"c:\\private\\10003a3f\\import\\apps\\");
_LIT(KResRegFile,"Hello3_reg.RSC");  


// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CAppRegExec_test;


/**
*  CAppRegExec_test test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CAppRegExec_test) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAppRegExec_test* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CAppRegExec_test();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CAppRegExec_test( CTestModuleIf& aTestModuleIf );

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

        /*
         * Positive Test Method: This Test Case we assume that 
         * the console App is running in backround and will try to 
         * copy GUI app resource registration file . 
         * So the GUI app corresponding to this resource should launch successfully.
         */
        virtual TInt Positive(CStifItemParser& aItem);
        /*
         * Negative Test Method: This Test Case checks whether the console App is not running in backround if yes, will shutdown it.
         * Then it Will try to copy resource registration file . 
         * So the GUI app corresponding to this resource should not launch.
         */
        virtual TInt Negative(CStifItemParser& aItem);
        /*
         * StartBackRoundApp Test mothod: This Test cases should be used to launch the Backround process
         * which actually registers the GUI app.& which was shut down by Negative Test Case.
         */         
        virtual TInt StartBackRoundApp(CStifItemParser& aItem);
        /*
         * StartProcess - wil be used to launch any application.
         */
        TInt StartProcess(const TDesC16& aFileName,TBool wait);
        /*
         * FindProcess - This function searches the given process . 
         * it will shutdown the process if running in backround & return true else False.
         */
        TBool FindProcess(const TDesC16& aPath);
        /*
         * CheckAndDeleteL- will takes dirPath & Filename.
         * will check whether the provided file is Valid or not. If valid then delete it.
         */
        void CheckAndDeleteL(const TDesC16& aPath,const TDesC16& aFile);
        /*
         * IsExist - will takes dirPath & Filename.
         * return True if valid file else False.
         */
        TBool IsExist(const TDesC16& aPath,const TDesC16& aFile);
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
    private:
        RFs iFsSession;
    };

#endif      // APPREGEXEC_TEST_H

// End of File
