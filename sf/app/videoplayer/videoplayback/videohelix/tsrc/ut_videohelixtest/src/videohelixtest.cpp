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
* Description:  Test Harness for VideoHelix Playback Plugin
*
*/

// Version : %version: 7 %


// INCLUDE FILES

#include "stifutilities.h"
#include "mpxvideoprivatecrkeys.h"
#include "videohelixtest.h"
#include "mpxvideoplayerutility_stub.h"
#include "mpxvideohelixplayback.h"
#include "mpxvideo_debug.h"

#include <stiftestinterface.h>
#include <settingserverclient.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVHPPTestClass::CVHPPTestClass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVHPPTestClass::CVHPPTestClass( CTestModuleIf& aTestModuleIf )
    : CScriptBase( aTestModuleIf )
    , iTestModuleIf( aTestModuleIf )
    , iFirstPlayCmd( ETrue )
{
}

// -----------------------------------------------------------------------------
// CVHPPTestClass::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVHPPTestClass::ConstructL()
{
    MPX_DEBUG(_L("CVHPPTestClass::ConstructL"));

    //
    //  Read logger settings to check whether test case name is to be
    //  appended to log file name.
    //
    RSettingServer settingServer;

    User::LeaveIfError( settingServer.Connect() );

    //
    //  Struct to StifLogger settigs.
    //
    TLoggerSettings loggerSettings;

    //
    //  Parse StifLogger defaults from STIF initialization file.
    //
    User::LeaveIfError( settingServer.GetLoggerSettings(loggerSettings) );

    //
    //  Close Setting server session
    //
    settingServer.Close();

    TFileName logFileName;

    if ( loggerSettings.iAddTestCaseTitle )
    {
        _LIT( KVHPPTestClassLogFileWithTitle, "VHPPTestClass_[%S].txt" );

        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format( KVHPPTestClassLogFileWithTitle, &title );
    }
    else
    {
        _LIT( KVHPPTestClassLogFile, "VHPPTestClass.txt" );

        logFileName.Copy( KVHPPTestClassLogFile );
    }

    _LIT( KVHPPTestClassLogPath, "\\logs\\testframework\\VHPPTestClass\\" );


    iLog = CStifLogger::NewL( KVHPPTestClassLogPath,
                              logFileName,
                              CStifLogger::ETxt,
                              CStifLogger::EFile,
                              EFalse );

    TVersion moduleVersion;

    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;

    moduleName = _L("CVHPPTestClass.dll");

    TBool newVersionOfMethod = ETrue;

    TestModuleIf().SendTestModuleVersion( moduleVersion,
                                          moduleName,
                                          newVersionOfMethod );

    StifUtilities::CreateStifUtility();

    iAccObserver = CMPXVideoAccessoryObserver::NewL( NULL );

    iTimeoutController = CSimpleTimeout::NewL (this, iLog);

    iExpectedCallbackArray = new (ELeave) CArrayPtrFlat<TCallbackEvent>( 1 );

    //
    //  CenRep volume range is 0 - number of steps (either 10 or 20).
    //  Set default CenRep values as volume(40), mute(0)
    //
    CRepository* volCenRep = CRepository::NewL( KCRUidMPXVideoSettings );
    volCenRep->Set( KMPXVideoPlaybackVolume, 40 );
    volCenRep->Set( KMPXVideoPlaybackMute, 0 );
    delete volCenRep;

    //
    //  Create VideoHelix Playback Plugin for STIF testing
    //
    iPlaybackPlugin = CMPXVideoHelixPlayback::NewL( iLog );

    iPlayerUtility = CMpxVideoPlayerUtility::NewL( NULL );
    iPlayerUtility->AddStifObserver( this );
}

// -----------------------------------------------------------------------------
// CVHPPTestClass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVHPPTestClass*
CVHPPTestClass::NewL( CTestModuleIf& aTestModuleIf )
{
    CVHPPTestClass* self =
        new (ELeave) CVHPPTestClass( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -----------------------------------------------------------------------------
//  Destructor
// -----------------------------------------------------------------------------
//
CVHPPTestClass::~CVHPPTestClass()
{
    MPX_DEBUG(_L("CVHPPTestClass::~CVHPPTestClass()"));

    CleanupTempFiles();

    if ( iTimeoutController->IsActive() )
    {
        _LIT( KPanic, "Event Pending" );
        User::Panic( KPanic, -33 );
    }

    delete iTimeoutController;

    delete iPlaybackPlugin;

    REComSession::FinalClose();

    delete iLog;

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );
    delete utility;
    Dll::SetTls( NULL );
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
//  LibEntryL is a polymorphic Dll entry point.
//  Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
{
    return ( CScriptBase* ) CVHPPTestClass::NewL( aTestModuleIf );
}

//  End of File
