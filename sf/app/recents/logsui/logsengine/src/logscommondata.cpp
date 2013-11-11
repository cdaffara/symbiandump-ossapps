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
* Description:
*
*/

//USER
#include "logscommondata.h"
#include "logslogger.h"
#include "logsconfigurationparams.h"
#include "logsengdefs.h"

//SYSTEM
#include <xqsettingsmanager.h>
#include <qcontactmanager.h>
#include <LogsDomainCRKeys.h>

static LogsCommonData* mLogsCommonInstance = 0;

// CONSTANTS

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid logsTelConfigurationCRUid = {0x102828B8};

// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 logsTelMatchDigits = 0x00000001;

const int logsNotInitialized = -1;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData::LogsCommonData() : 
    mContactManager(0), mMaxReadSize(-1), 
    mMaxReadSizeDir(LogsEvent::DirUndefined), 
    mSettingsManager(new XQSettingsManager()),
    mMatchLen(logsDefaultMatchLength),
    mPredictiveSearchStatus(logsNotInitialized)
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsCommonData::LogsCommonData()" )
}
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData::~LogsCommonData()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsCommonData::~LogsCommonData()" )
    delete mContactManager;
    delete mSettingsManager;
    LOGS_QDEBUG( "logs [ENG] <- LogsCommonData::~LogsCommonData()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCommonData& LogsCommonData::getInstance() 
{ 
    if ( !mLogsCommonInstance ){
        mLogsCommonInstance = new LogsCommonData;
    }
    return *mLogsCommonInstance; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsCommonData::freeCommonData()
{
    delete mLogsCommonInstance;
    mLogsCommonInstance = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QContactManager& LogsCommonData::contactManager()
{
    if (!mContactManager){
        LOGS_QDEBUG( "logs [ENG] -> LogsCommonData::contactManager(), create mgr" )
        mContactManager = new QContactManager("symbian");
        LOGS_QDEBUG( "logs [ENG] <- LogsCommonData::contactManager()" )
    }
    return *mContactManager;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsCommonData::configureReadSize(int maxSize, LogsEvent::LogsDirection dir)
{
    mMaxReadSize = maxSize;
    mMaxReadSizeDir = dir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::maxReadSize() const
{
    return mMaxReadSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEvent::LogsDirection LogsCommonData::maxReadSizeDirection() const
{
    return mMaxReadSizeDir;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::updateConfiguration(const LogsConfigurationParams& params)
{
    mConfiguration = params;
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsConfigurationParams& LogsCommonData::currentConfiguration()
{
    return mConfiguration;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::telNumMatchLen() const
{
    return mMatchLen;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::predictiveSearchStatus()
{
    if (mPredictiveSearchStatus == logsNotInitialized) {
        mPredictiveSearchStatus = getPredictiveSearchStatus();
    }
    return mPredictiveSearchStatus;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::getPredictiveSearchStatus()
{
    int status(logsNotInitialized);
    LOGS_QDEBUG( "logs [ENG] -> LogsCommonData::getPredictiveSearchStatus()" )
    XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
                      KCRUidLogs.iUid, 
                      KLogsPredictiveSearch);
    QVariant value = mSettingsManager->readItemValue(key, 
                                                     XQSettingsManager::TypeInt);
    if (!value.isNull()) {
        status = value.toInt();
    }
    LOGS_QDEBUG_2( "logs [ENG] <- LogsCommonData::getPredictiveSearchStatus(), status: ", status )
    return status;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::setPredictiveSearch(bool enabled)
{
    int err(-1);
    LOGS_QDEBUG_2( "logs [ENG] -> LogsCommonData::setPredictiveSearch(), enabled: ", enabled )
    if (predictiveSearchStatus() != 0) {
        XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
                          KCRUidLogs.iUid, 
                          KLogsPredictiveSearch);
        int value = enabled ? 1 : 2;
        if (mSettingsManager->writeItemValue(key, QVariant(value))) {
            err = 0;
            mPredictiveSearchStatus = value;
        }
    }
    LOGS_QDEBUG_2( "logs [ENG] <- LogsCommonData::setPredictiveSearch(), err: ", err )
    return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsCommonData::clearMissedCallsCounter()
{
    int err(-1);
    XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
                      KCRUidLogs.iUid, 
                      KLogsNewMissedCalls);
    QVariant value = mSettingsManager->readItemValue(
                            key, XQSettingsManager::TypeInt);
    if (!value.isNull()) {
        err = 0;
        if (value.toInt() > 0) {
            err = mSettingsManager->writeItemValue(key, 0) ? 0 : -1;
        }
    }
    return err;    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsCommonData::getTelNumMatchLen(int& matchLen)
{
    bool ok(false);
    //use local manager, since normally this function is called only once, 
    //after that we can clean related cenrep handler resources
    XQSettingsManager manager;
    XQSettingsKey key(XQSettingsKey::TargetCentralRepository, 
                      logsTelConfigurationCRUid.iUid, 
                      logsTelMatchDigits);
    QVariant value = manager.readItemValue(
                             key, XQSettingsManager::TypeInt);
    if (!value.isNull()) {
        matchLen = value.toInt();
        mMatchLen = matchLen;
        ok = true;
    }
    return ok;
}

// End of file

