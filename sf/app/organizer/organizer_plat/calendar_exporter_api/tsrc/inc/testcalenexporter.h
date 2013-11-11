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
* Description:   test calenexporter api
*
*/



#ifndef __TESTCALENEXPORTER_H__
#define __TESTCALENEXPORTER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <s32file.h>                // RFileReadStream
#include <e32def.h>

//  FORWARD DECLARATIONS
class CCalSession;
class CCalenExporter;
class CCalEntry;
        

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 * EUnitWizard generated test class. 
 */
NONSHARABLE_CLASS( CCalenExporterTest )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCalenExporterTest* NewL();
        static CCalenExporterTest* NewLC();
        /**
         * Destructor
         */
        ~CCalenExporterTest();

    private:    // Constructors and destructors
        CCalenExporterTest();
        void ConstructL();

    private:    
        
        //Specific export function test cases
        void SetupExporterL();      //Setup
        void TeardownExporter();    //Teardown
        void GetExportFlagsTestL(); //Test case
        void GetExportFlagsTestL( CCalEntry& aEntry ); //Test function
        void ExportICalTestL();     //Test case
        void ExportVCalTestL();     //Test case
    
    private:    // Data
        CCalSession *iCalSession;       
        CCalenExporter *iCalenExporter;
                 
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __TESTCALENEXPORTER_H__

// End of file
