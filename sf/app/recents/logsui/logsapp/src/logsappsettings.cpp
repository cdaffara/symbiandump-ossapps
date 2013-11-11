/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include "logsappsettings.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logsforegroundwatcher.h"
#include <xqsettingsmanager.h>
#include <LogsDomainCRKeys.h>
#include <QVariant>

const int logsFakeExitDisabled = 0;

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsAppSettings::LogsAppSettings(int &argc, char *argv[]) :
      mFeaturePreloadedEnabled(isBootupStart(argc, argv)), 
      mFeatureFakeExitEnabledVal(-1)
{
    if ( mFeaturePreloadedEnabled ){
        LogsForegroundWatcher::ensureBackgroundStartup();
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsAppSettings::~LogsAppSettings()
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
bool LogsAppSettings::logsFeaturePreloadingEnabled()
{
    return mFeaturePreloadedEnabled;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
bool LogsAppSettings::logsFeatureFakeExitEnabled()
{
    if ( mFeatureFakeExitEnabledVal < 0 ){
        XQSettingsManager manager;
        XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
                          KCRUidLogs.iUid, 
                          KLogsFakeExitEnabled);
        QVariant value = manager.readItemValue(
                                key, XQSettingsManager::TypeInt);
        if (!value.isNull()) {
            mFeatureFakeExitEnabledVal = value.toInt();
        }
        if ( mFeatureFakeExitEnabledVal < 0 ){
            mFeatureFakeExitEnabledVal = logsFakeExitDisabled;
        }
    }
    return ( mFeatureFakeExitEnabledVal != logsFakeExitDisabled );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
bool LogsAppSettings::isBootupStart(int &argc, char *argv[])
{
    for (int i=0; i<argc; i++) {
        if ( QString(argv[i]) == logsBootupStartCmdLineArg ){
            return true;
        }
    }
    return false;
}

// end of file
