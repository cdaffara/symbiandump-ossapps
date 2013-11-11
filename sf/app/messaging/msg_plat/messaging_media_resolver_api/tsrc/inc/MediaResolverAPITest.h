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
* Description:   MediaResolverAPITest  declaration*
*/




#ifndef MEDIARESOLVERAPITEST_H
#define MEDIARESOLVERAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h> //for RFile

// CONSTANTS

// MACROS

#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMediaResolverAPITestLogPath, "\\logs\\testframework\\MediaResolverAPITest\\" ); 
// Log file
_LIT( KMediaResolverAPITestLogFile, "MediaResolverAPITest.txt" ); 
_LIT( KMediaResolverAPITestLogFileWithTitle, "MediaResolverAPITest_[%S].txt" );
//Testing Data path
#ifdef __WINSCW__
_LIT( KTestFilePath, "C:\\testing\\data\\mresolverData\\%S" );
#endif
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMediaResolverAPITest;
class CMsgMediaResolver;
class CMsgMediaInfo;
class RFile;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMediaResolverAPITest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMediaResolverAPITest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMediaResolverAPITest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMediaResolverAPITest();

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
        CMediaResolverAPITest( CTestModuleIf& aTestModuleIf );

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
         * Tests NewL of CMsgMediaResolver
         * */
        TInt TestMediaResolverNewL( CStifItemParser& aItem );
        
        /**
         * Tests NewLC of CMsgMediaResolver
         * */
        TInt TestMediaResolverNewLC( CStifItemParser& aItem );
        
        /**
         * Tests FileHnaldeL of CMsgMediaResolver
         * */
        TInt TestFileHandleL( CStifItemParser& aItem );
        
        /**
         * Tests RecognizeL of CMsgMediaResolver
         * */
        TInt TestRecognizeL( CStifItemParser& aItem );
        
        /**
         * Tests MediaType of CMsgMediaResolver
         * */
        TInt TestMediaType( CStifItemParser& aItem );
        
        /**
         * Tests CreateMediaInfoL of CMsgMediaResolver
         * */
        TInt TestCreateMediaInfoL( CStifItemParser& aItem );
        
        /**
         * Tests CreateMediaInfoL of CMsgMediaResolver which has 2 parameters
         */
        TInt Test2CreateMediaInfoL( CStifItemParser& aItem );
        
        /**
         * Tests ParseInfoDetailsL of CMsgMediaResolver
         * */
        TInt TestParseInfoDetailsL( CStifItemParser& aItem ); 
        
        /**
         * Tests NewL of CMsgMediaInfo
         * */
        TInt TestMediaInfoNewL( CStifItemParser& aItem );

        /**
         * Tests ParseInfoDetails of CMsgMediaResolver
         * */
        TInt TestMediaInfoParseInfoDetailsL( CStifItemParser& aItem );
        
        /**
         * Tests SetFileL of CMsgMediaResolver
         * */
        TInt TestSetFileL( CStifItemParser& aItem );
        
        /**
         * Tests Corrupt of CMsgMediaResolver
         * */
        TInt TestCorruptL( CStifItemParser& aItem );        
        
        /**
         * used to call NewL function of audio, video, text, Imageinfo classes
         */
        TInt TestAnyMediaInfoNewL( CStifItemParser& aItem );
        
        //ADD NEW METHOD DEC HERE ^
        //[TestMethods] - Do not remove        
        
        //HELPER FUNCTIONS
        /**
         * This is helper function used to open a file (given media file as parameter)
         * thorough MsgMediaResolver
         * aItem IN : passed from cfg file
         * aFile OUT: created file's handle
         * returns KErrNone on successful open
         */        
        TInt OpenFile( CStifItemParser& aItem, RFile& aFile);
        
        /**
         * This Function creates an object of MsgMediaInfo and internally uses
         * OpenFile
         */
        TInt InitMediaInfoL( CStifItemParser& aItem );
        
        /**
         * This function cleans up mediainfo object
         */
        void MediaInfoCleanUp();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
        CMsgMediaResolver*  iMediaResolver;
        CMsgMediaInfo*      iMediaInfo;
        RFile               iFileHandle;
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // MEDIARESOLVERAPITEST_H

// End of File
