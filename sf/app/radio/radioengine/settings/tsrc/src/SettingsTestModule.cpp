/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// System includes
#include <Stiftestinterface.h>
#include <SettingServerClient.h>

// User includes
#include "SettingsTestModule.h"
#include "trace.h"

// -----------------------------------------------------------------------------
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSettingTestModule::CPresetUtilityTestModule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSettingsTestModule::CSettingsTestModule(CTestModuleIf& aTestModuleIf) :
    CScriptBase(aTestModuleIf)
    {
    FUNC_LOG;
    }

// CSettingsTestModule::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSettingsTestModule::ConstructL()
    {
    FUNC_LOG;

    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if (ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings;
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if (ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;

    if (loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KSettingsTestModuleLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KSettingsTestModuleLogFile);
        }

    iLog = CStifLogger::NewL(KSettingsTestModuleLogPath, logFileName,
            CStifLogger::ETxt, CStifLogger::EFile, EFalse);

    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CSettingsTestModule::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSettingsTestModule* CSettingsTestModule::NewL(CTestModuleIf& aTestModuleIf)
    {
    FUNC_LOG;
    CSettingsTestModule* self = new (ELeave) CSettingsTestModule(
            aTestModuleIf);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CSettingsTestModule::~CSettingsTestModule()
    {
    FUNC_LOG;

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog;

    }

//-----------------------------------------------------------------------------
// CSettingsTestModule::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CSettingsTestModule::SendTestClassVersion()
    {
    FUNC_LOG;
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;
    moduleName = _L("SettingsTestModule.dll");

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName,
            newVersionOfMethod);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(CTestModuleIf& aTestModuleIf) // Backpointer to STIF Test Framework
    {
    FUNC_LOG;

    return (CScriptBase*) CSettingsTestModule::NewL(aTestModuleIf);

    }

//  End of File
