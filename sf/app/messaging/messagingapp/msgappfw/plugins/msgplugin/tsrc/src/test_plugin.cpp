/*
#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Test class for CS Message Plugin
#
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "test_plugin.h"
#include <SettingServerClient.h>

// -----------------------------------------------------------------------------
// Ctest_plugin::Ctest_plugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

Ctest_plugin::Ctest_plugin( CTestModuleIf& aTestModuleIf ) : CScriptBase( aTestModuleIf )
            {
            }

// -----------------------------------------------------------------------------
// Ctest_plugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void Ctest_plugin::ConstructL()
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
	        logFileName.Format(KTestPluginTestLogFileWithTitle, &title);
	        }
	    else
	        {
	        logFileName.Copy(KTestPluginTestLogFile);
	        }

	    iLog = CStifLogger::NewL( KTestPluginLogPath, 
	                          logFileName,
	                          CStifLogger::ETxt,
	                          CStifLogger::EFile,
	                          EFalse );
	    
	    iSession = CMsvSession::OpenSyncL( *this);
	    iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
	    iSmsClientMtm = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ));
	    
	    iMmsClientMtm = STATIC_CAST( CMmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeMultimedia ));
	    iPlugin = NULL;
	    
	    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Ctest_plugin* Ctest_plugin::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
	Ctest_plugin* self = new (ELeave) Ctest_plugin( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Ctest_plugin::~Cimcache_test
// Destructor
// -----------------------------------------------------------------------------
//
Ctest_plugin::~Ctest_plugin()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

// ----------------------------------------------------------------------------
       // Ctest_plugin::HandleSessionEventL
       // Implemented for MMsvSessionObserver
       // ----------------------------------------------------------------------------
       //
void Ctest_plugin::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
               TAny */*aArg1*/, 
               TAny */*aArg2*/, 
               TAny */*aArg3*/)
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
    return ( CScriptBase* ) Ctest_plugin::NewL( aTestModuleIf );
    }


//  End of File
