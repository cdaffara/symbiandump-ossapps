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
* Description: This file contains testclass implementation.
*
*/

// INCLUDE FILES
#include <Stiftestinterface.h>
#include <SettingServerClient.h>

#include "calenExporterTest.h"



// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::CCalendarDateUtilsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCalenExporterTest::CCalenExporterTest( CTestModuleIf& aTestModuleIf ): 
CScriptBase( aTestModuleIf )
    {
	
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::ConstructL()
    {
    
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KCalenExporterTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KCalenExporterTestLogFile);
        }

    iLog = CStifLogger::NewL( KCalenExporterTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenExporterTest* CCalenExporterTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CCalenExporterTest* self = new (ELeave) CCalenExporterTest( 
            aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CCalenExporterTest::~CCalenExporterTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CCalenExporterTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CCalenExporterTest::SendTestClassVersion()
	{
	
	TVersion moduleVersion;
	moduleVersion.iMajor = EXPORTER_TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = EXPORTER_TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = EXPORTER_TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("testcalenexporter.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, 
	        moduleName, 
	        newVersionOfMethod);
	
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CCalenExporterTest::NewL( aTestModuleIf );

    }

// End of File
