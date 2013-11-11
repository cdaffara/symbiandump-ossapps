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
* Description:   MmsCodecClientApiTest implementation*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MmsCodecClientApiTest.h"
#include <SettingServerClient.h>
#include <MSVAPI.H>
#include <mmscodecclient.h>
#include <mmsclient.h>
#include <mmssettings.h>
#include <e32base.h>    





// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::CMmsCodecClientApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsCodecClientApiTest::CMmsCodecClientApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsCodecClientApiTest::ConstructL()
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
        logFileName.Format(KMmsCodecClientApiTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMmsCodecClientApiTestLogFile);
        }

    iLog = CStifLogger::NewL( KMmsCodecClientApiTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    if(!iSession)
        {
    iSession = CMsvSession::OpenSyncL(*this);
        }
    iCodecClient = CMmsCodecClient::NewL(*iSession);
	iWait = CMsvOperationActiveSchedulerWait::NewLC();
    CleanupStack::Pop(iWait);
    iEncodeBuffer = CBufFlat::NewL(1);
    User::LeaveIfError(iFs.Connect());
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CMmsCodecClientApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsCodecClientApiTest* CMmsCodecClientApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMmsCodecClientApiTest* self = new (ELeave) CMmsCodecClientApiTest( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CMmsCodecClientApiTest::~CMmsCodecClientApiTest()
    { 

    // Delete resources allocated from test methods
    Delete();
    if(iCodecClient)
        {
        delete iCodecClient;
        iCodecClient = NULL;
        }
    if(iSession)
        {
        delete iSession;
        iSession = NULL;
        }
    if(iWait)
       {
       delete iWait;
       iWait = NULL;
       }
   if(iEncodeBuffer)
    	{
    	delete iEncodeBuffer;
    	iEncodeBuffer = NULL;
    	}
   iFs.Close();
      
    delete iLog; 
    }

//-----------------------------------------------------------------------------
// CMmsCodecClientApiTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMmsCodecClientApiTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MmsCodecClientApiTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

void CMmsCodecClientApiTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
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

    return ( CScriptBase* ) CMmsCodecClientApiTest::NewL( aTestModuleIf );

    }


//  End of File
