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
* Description: STIF testclass CCalenImporterTest declaration.
*
*/

#ifndef CALENIMPORTERTEST_H
#define CALENIMPORTERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <Calenimporter.h>

// FORWARD DECLARATIONS
class CCalSession;
class CCalenimporter;
// MACROS
#define IMPORTER_TEST_CLASS_VERSION_MAJOR 0
#define IMPORTER_TEST_CLASS_VERSION_MINOR 0
#define IMPORTER_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalenimporterTestLogPath, 
        "\\logs\\testframework\\CalenimporterTest\\" ); 
// Log file
_LIT( KCalenimporterTestLogFile, 
        "CalenimporterTest.txt" ); 
_LIT( KCalenimporterTestLogFileWithTitle, 
        "CalenimporterTest_[%S].txt" );

/**
*  CCalenImporterTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib Calenimporter.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalenImporterTest ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalenImporterTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalenImporterTest();

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

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CCalenImporterTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void SetupimporterL();

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
        * Test funtion to test the API CCalenimporter::TestimporterVCalL
        * @param aItem Script line containing parameters.
        */
        TInt TestImporterVCalL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CCalenimporter::TestimporterICalL
        * @param aItem Script line containing parameters.
        */
        TInt TestImporterICalL( CStifItemParser& aItem );
        
        TInt TestSetImportMode( CStifItemParser& aItem );
        
    private:    // New functions
        CCalSession *iCalSession;       
		CCalenImporter *iCalenImporter;
		RFs iFs;
		RPointerArray<CCalEntry> iEntries;
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

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

#endif  //CALENimporterTEST_H

// End of File
