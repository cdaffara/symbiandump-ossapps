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
* Description:  MmsServerSettingsTest implementation
*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MmsServerSettingsTest.h"
#include <SettingServerClient.h>
#include <mmssettings.h>
#include <mmsaccount.h>
#include <mmsregisteredapplications.h>

// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS


// MACROS


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::CMmsServerSettingsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsServerSettingsTest::CMmsServerSettingsTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsServerSettingsTest::ConstructL()
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
        logFileName.Format(KMmsServerSettingsTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMmsServerSettingsTestLogFile);
        }

    iLog = CStifLogger::NewL( KMmsServerSettingsTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    iMmsAccount = CMmsAccount::NewL();
    iMmsSettings = CMmsSettings::NewL();
    iMmsRegisteredApps = CMmsRegisteredApplications::NewL(); 
    iSession = CMsvSession::OpenSyncL(*this);
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CMmsServerSettingsTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsServerSettingsTest* CMmsServerSettingsTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMmsServerSettingsTest* self = new (ELeave) CMmsServerSettingsTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;

    }

// Destructor
CMmsServerSettingsTest::~CMmsServerSettingsTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
	if(iMmsAccount)
        {
        delete iMmsAccount;
        iMmsAccount = NULL;
        }
    if(iMmsSettings)
        {
        delete iMmsSettings;
        iMmsSettings = NULL;
        }
    if(iMmsRegisteredApps)
            {
            delete iMmsRegisteredApps;
            iMmsRegisteredApps = NULL;
            }
    if(iSession)
            {
            delete iSession;
			iSession=NULL;
            }
    delete iLog; 
  

    }

//-----------------------------------------------------------------------------
// CMmsServerSettingsTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMmsServerSettingsTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MmsServerSettingsTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}
	
void CMmsServerSettingsTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
	
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

    return ( CScriptBase* ) CMmsServerSettingsTest::NewL( aTestModuleIf );

    }


//  End of File
