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
* Description: STIF testclass CCalenExporterTest declaration.
*
*/

#ifndef CALENEXPORTERTEST_H
#define CALENEXPORTERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <CalenExporter.h>

// FORWARD DECLARATIONS
class CCalSession;
class CCalenExporter;
// MACROS
#define EXPORTER_TEST_CLASS_VERSION_MAJOR 1
#define EXPORTER_TEST_CLASS_VERSION_MINOR 0
#define EXPORTER_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalenExporterTestLogPath, 
        "\\logs\\testframework\\CalenExporterTest\\" ); 
// Log file
_LIT( KCalenExporterTestLogFile, 
        "CalenExporterTest.txt" ); 
_LIT( KCalenExporterTestLogFileWithTitle, 
        "CalenExporterTest_[%S].txt" );

/**
*  CCalenExporterTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib CalenExporter.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalenExporterTest ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalenExporterTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalenExporterTest();

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
        CCalenExporterTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void SetupExporterL();

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
        * Test funtion to test the API CCalenExporter::TestExporterVCalL
        * @param aItem Script line containing parameters.
        */
        TInt TestExporterVCalL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CCalenExporter::TestExporterICalL
        * @param aItem Script line containing parameters.
        */
        TInt TestExporterICalL( CStifItemParser& aItem );
        
    private:    // New functions
        CCalSession *iCalSession;       
		CCalenExporter *iCalenExporter;
        
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

#endif  //CALENEXPORTERTEST_H

// End of File
