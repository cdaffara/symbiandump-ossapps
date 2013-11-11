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
* Description:   MmsClientMtmApiTest implementation*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MmsClientMtmApiTest.h"
#include <SettingServerClient.h>
#include <mmsclient.h>
#include <MTCLREG.H>
#include  <MSVAPI.H>

// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS


// MACROS

// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsClientMtmApiTest::CMmsClientMtmApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsClientMtmApiTest::CMmsClientMtmApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CMmsClientMtmApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsClientMtmApiTest::ConstructL()
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
        logFileName.Format(KMmsClientMtmApiTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMmsClientMtmApiTestLogFile);
        }

    iLog = CStifLogger::NewL( KMmsClientMtmApiTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
    iSession = CMsvSession::OpenSyncL(*this);
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);
    iClientMtm = (CMmsClientMtm *) iMtmRegistry-> NewMtmL(KUidMsgTypeMultimedia); 
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CMmsClientMtmApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsClientMtmApiTest* CMmsClientMtmApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMmsClientMtmApiTest* self = new (ELeave) CMmsClientMtmApiTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;

    }

// Destructor
CMmsClientMtmApiTest::~CMmsClientMtmApiTest()
    { 

    // Delete resources allocated from test methods
    Delete();
	if(iClientMtm)
		{
		delete iClientMtm;
		iClientMtm = NULL;
		}
	if(iMtmRegistry)
		{
		delete iMtmRegistry;
		iMtmRegistry = NULL;
		}
	if(iSession)
        {
        delete iSession;
        iSession = NULL;
        }

    // Delete logger
    delete iLog;
    
     

    }

//-----------------------------------------------------------------------------
// CMmsClientMtmApiTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMmsClientMtmApiTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MmsClientMtmApiTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

void CMmsClientMtmApiTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	   //No Implementation 
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

    return ( CScriptBase* ) CMmsClientMtmApiTest::NewL( aTestModuleIf );

    }


//  End of File
