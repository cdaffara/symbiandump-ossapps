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
* Description:   MmsCodecClientApiTest  declaration*
*/




#ifndef MMSCODECCLIENTAPITEST_H
#define MMSCODECCLIENTAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <msvapi.h>
#include <e32cmn.h>
// CONSTANTS


// MACROS

#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMmsCodecClientApiTestLogPath, "\\logs\\testframework\\MmsCodecClientApiTest\\" ); 
// Log file
_LIT( KMmsCodecClientApiTestLogFile, "MmsCodecClientApiTest.txt" ); 
_LIT( KMmsCodecClientApiTestLogFileWithTitle, "MmsCodecClientApiTest_[%S].txt" );

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMmsCodecClientApiTest;
class CMsvSession;
class CMmsCodecClient;
class CMsvOperationActiveSchedulerWait;


// DATA TYPES


// CLASS DECLARATION

/**
*  CMmsCodecClientApiTest test class for STIF Test Framework TestScripter.

*/
NONSHARABLE_CLASS(CMmsCodecClientApiTest) : public CScriptBase ,public MMsvSessionObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmsCodecClientApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMmsCodecClientApiTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        //From MMsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */


    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
         */

    private:

        /**
        * C++ default constructor.
        */
        CMmsCodecClientApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();



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
        * 
        * Test method for CreateNewMessageEntryL in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt CreateNewMessageEntryL();
        /**
        * 
        * Test method for AddMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt AddMMTestL();
        /**
        * 
        * Test method for ReplaceMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt RelaceMMTestL();
        /**
        * 
        * Test method for RetreiveMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt RetreiveMMTestL();
        /**
        * 
        * Test method for SendMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt SendMMTestL();
        /**
        * 
        * Test method for MoveMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt MoveMMTestL();
        /**
        * 
        * Test method for SetFlags in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt SetFlagsTestL();
        /**
        * 
        * Test method for InitializeChunkedRetrievingL in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt InitializeChunkedRetrievingTestL();
        /**
        * 
        * Test method for DiskSpaceBelowCriticalLevelL in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt DiskSpaceBelowCriticalLevelTestL();
        /**
        * 
        * Test method for InitializeChunkedAddingL in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt InitializeChunkedAddingTestL();
        /**
        * 
        * Test method for DeleteMML in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt DeleteMMTestL();
        /**
        * 
        * Test method for InitializeChunkedReplacingL in CMmsCodecClient
        * @ return Symbian OS Error Code
        */
        virtual TInt InitializeChunkedReplacingTestL();
        
       /**
        * Method used to log version of test class
        */
        void SendTestClassVersion();
        
        /*
        * method for cleaning all the messages
        */
        void CleanAllL();
        /*
        * method for cleaning a folder
        */
        void CleanBoxL(TMsvId aBoxId, CMsvSession& aSession);
        
        TInt FeedDataInChunks( CMmsCodecClient*  );
        
        void FillBufferFromFileL( const TDesC& aFilePath,RFs& aFs,CBufFlat* aEncodeBuffer);
               

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    private:
        CMsvSession* iSession;
        CMmsCodecClient* iCodecClient;
        CMsvOperationActiveSchedulerWait* iWait;
        CBufFlat* iEncodeBuffer;
        RFs iFs;
        TFileName iFilePath;
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MMSCODECCLIENTAPITEST_H

// End of File
