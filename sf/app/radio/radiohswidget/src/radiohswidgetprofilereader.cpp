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
* Description: FM Radio widget process handler
*
*/

// System includes
#include <QDateTime>
#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "xqpublishandsubscribeutils.h"

// User includes
#include "radiohswidgetprofilereader.h"
#include "radiohswidget.h"
#include "radioservicedef.h"
#include "radio_global.h"
#include "radiologger.h"

// Constants
/** Constant for radio running undefined status. */
const int RADIO_RUNNING_STATUS_UNDEFINED(-1);

/*!
    \class RadioHsWidgetProfileReader
    \brief Implementation of P&S key reader and monitor.

    RadioHsWidgetProfileReader implements reader and monitor for P&S keys.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a profile reader which is a child of \a parent.
    
    Creates XQSettingsManager for monitoring and accessing the P&S keys.
    Connects to the signals of XQSettingsManager.
*/
RadioHsWidgetProfileReader::RadioHsWidgetProfileReader(
    RadioHsWidget *parent) :
    QObject(parent),
    mParent(*parent),
    mSettingsManager(new XQSettingsManager(this)),
    mRadioStatus(RADIO_RUNNING_STATUS_UNDEFINED)
{
    LOG_METHOD;
    Radio::connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)), this,
        SLOT(handleDeletedItem(XQSettingsKey)));
    Radio::connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleChanges(XQSettingsKey, QVariant)));
}

/*!
    Destructor 
 */
RadioHsWidgetProfileReader::~RadioHsWidgetProfileReader()
{
    LOG_METHOD;
    XQSettingsKey radioRunningKey(XQSettingsKey::TargetPublishAndSubscribe,
        KRadioPSUid, KRadioStartupKey);
    mSettingsManager->stopMonitoring(radioRunningKey);
}

/*!
    Start monitoring of radio P&S key. Read also the initial value.
 */
void RadioHsWidgetProfileReader::startMonitoringRadioRunningStatus()
{
    LOG_METHOD;
    XQSettingsKey radioRunningKey(XQSettingsKey::TargetPublishAndSubscribe,
        KRadioPSUid, KRadioStartupKey);
    // Start monitoring.
    mSettingsManager->startMonitoring(radioRunningKey);
    // Read current value.
    radioRunningStatus(
        mSettingsManager->readItemValue(radioRunningKey));
}

/*!
    Checks if device is in offline mode and \returns \c true if it is
    and \c false if not.
 */
bool RadioHsWidgetProfileReader::isInOfflineMode()
{
    LOG_METHOD_RET("%d");
    XQSettingsKey connectionKey( XQSettingsKey::TargetCentralRepository,
            CENREP_CORE_APPLICATION_UIS, ID_NETWORK_CONNECTION_ALLOWED );

    // Read current value.
    const QVariant connectionAllowed = mSettingsManager->readItemValue( connectionKey );
    if ( connectionAllowed.canConvert( QVariant::Int ) && connectionAllowed.toInt() == NetworkNotAllowed ) {
        return true;
    }

    return false;
}

/*!
    Handling of deletion of listened keys.
 
    \param key Deleted key.
 */
void RadioHsWidgetProfileReader::handleDeletedItem(const XQSettingsKey &key)
{
    LOG_METHOD_ENTER;
    if (key.uid() == KRadioPSUid && key.key() == KRadioStartupKey) {
        LOG("KRadioStartupKey deleted");
        startMonitoringRadioRunningStatus();
    }
}

/*!
    Notifications from settings manager are handled and routed to appropriate
    private slots.

    \param key Changed key.
    \param value Value of changed key.
 */
void RadioHsWidgetProfileReader::handleChanges(const XQSettingsKey &key,
    const QVariant& value)
{
    LOG_SLOT_CALLER;

    if (key.uid() == KRadioPSUid && key.key()
        == KRadioStartupKey) {
        LOG("KRadioStartupKey changed");
        radioRunningStatus(value);
    }
}

/*!
    Handling changes in radio running P&S key.
 
    \param value is int representation of time in seconds when radio was
    started.
 */
void RadioHsWidgetProfileReader::radioRunningStatus(
    const QVariant &value)
{
    LOG_METHOD_ENTER;
    if (value.canConvert(QVariant::Int)) {
        mRadioStatus = value.toInt();
        // Notify the observer that radio is running.
        mParent.handleRadioStateChange(FmRadio::StateRunning);
    } else {
        mRadioStatus = RADIO_RUNNING_STATUS_UNDEFINED;
        // Notify the observer that radio is not running.
        mParent.handleRadioStateChange(FmRadio::StateNotRunning);
    }
}
