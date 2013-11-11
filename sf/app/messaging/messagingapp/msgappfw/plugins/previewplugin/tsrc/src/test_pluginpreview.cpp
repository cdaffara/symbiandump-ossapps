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
# Description: Test class for CS Preview Plugin
#
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "test_pluginpreview.h"
#include <SettingServerClient.h>

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::Ctest_pluginpreview
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

Ctest_pluginpreview::Ctest_pluginpreview( CTestModuleIf& aTestModuleIf ) : CScriptBase( aTestModuleIf )
            {
            }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void Ctest_pluginpreview::ConstructL()
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
	    
	     //create msv session
   	       iSession = CMsvSession::OpenSyncL(*this);
	    
	    //create mtm registry
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);

           //create mms client mtm
            iMmsMtm = static_cast<CMmsClientMtm*> (iMtmRegistry-> NewMtmL(KSenduiMtmMmsUid));
 
          //create thumbnail manager
          // iThumbnailManager = CThumbnailManager::NewL(*this);

      
	    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );  
    }

// -----------------------------------------------------------------------------
// Ctest_pluginpreview::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Ctest_pluginpreview* Ctest_pluginpreview::NewL( CTestModuleIf& aTestModuleIf )
    {
	Ctest_pluginpreview* self = new ( ELeave ) Ctest_pluginpreview( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// Ctest_pluginpreview::~Ctest_pluginpreview
// Destructor
// -----------------------------------------------------------------------------
//
Ctest_pluginpreview::~Ctest_pluginpreview()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

// ----------------------------------------------------------------------------
       // Ctest_pluginpreview::HandleSessionEventL
       // Implemented for MMsvSessionObserver
       // ----------------------------------------------------------------------------
       //
void Ctest_pluginpreview::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
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
    return ( CScriptBase* ) Ctest_pluginpreview::NewL( aTestModuleIf );
    }


//  End of File
