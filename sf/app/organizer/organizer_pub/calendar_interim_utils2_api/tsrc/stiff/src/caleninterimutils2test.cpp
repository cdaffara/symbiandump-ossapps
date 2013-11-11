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

#include "caleninterimutils2Test.h"



// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarDateUtilsTest::CCalendarDateUtilsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test::CCalenInterimUtils2Test( CTestModuleIf& aTestModuleIf ): 
CScriptBase( aTestModuleIf )
    {
	
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::ConstructL()
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
        logFileName.Format(KCalenInterimUtils2TestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KCalenInterimUtils2TestLogFile);
        }

    iLog = CStifLogger::NewL( KCalenInterimUtils2TestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    iError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test* CCalenInterimUtils2Test::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CCalenInterimUtils2Test* self = new (ELeave) CCalenInterimUtils2Test( 
            aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CCalenInterimUtils2Test::~CCalenInterimUtils2Test()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CCalenInterimUtils2Test::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::SendTestClassVersion()
	{
	
	TVersion moduleVersion;
	moduleVersion.iMajor = INTERIMUTILS2_TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = INTERIMUTILS2_TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = INTERIMUTILS2_TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("caleninterimutils2test.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, 
	        moduleName, 
	        newVersionOfMethod);
	
	}

// End of File
