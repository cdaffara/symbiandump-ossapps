/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test calenimporter api
*
*/



#ifndef __TESTCALENIMPORTER_H__
#define __TESTCALENIMPORTER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <s32file.h>                // RFileReadStream
//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CCalSession;
class CCalenImporter;
class CCalEntry;
        
#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 * EUnitWizard generated test class. 
 */
NONSHARABLE_CLASS( CCalenImporterTest )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCalenImporterTest* NewL();
        static CCalenImporterTest* NewLC();
        
        /**
         * Destructor
         */
        ~CCalenImporterTest();

    private:    // Constructors and destructors
        
        CCalenImporterTest();
        void ConstructL();

    private:    // New methods
        
        //import & export function test cases
        void SetupL();              //Setup
        void Teardown();            //Teardown
        void ImportTestL();   //Test case
        void RunTestL();            //Test function
        
        //Specific import function test cases
        void SetupImporterL();      //Setup
        void TeardownImporter();    //Teardown
        void SetImportModeTestL();  //Test case
        
    private: 
    	void ImportL();
      void ParseFileL( const TDesC16& aFile );	
    	void ParseLineL( const TDesC8& aLine );
    	
    private:    // Data
        CCalSession *iCalSession;
        CCalenImporter *iCalenImporter;
     
        RFs iFs;
        RPointerArray<CCalEntry> iEntries;
        
     		HBufC* iImportFile;	// File to import from
    		HBufC* iExportFile;	// File to export to
    		HBufC* iCheckFile;	// File to verify export against
    		HBufC* iType;       // Defines if the test is for vcal or ical
       
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __TESTCALENIMPORTER_H__

// End of file
