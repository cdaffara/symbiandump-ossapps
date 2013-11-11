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

#include "ut_cpnetworkstatus.h"
#include "qtestmains60.h"
#include "cpnetworkstatus.h"
#include <cpsettingformitemdata.h>
#include <hbicon.h>

/*!
  ut_CpNetworkStatus::ut_CpNetworkStatus
 */
ut_CpNetworkStatus::ut_CpNetworkStatus() 
    : m_networkStatus(NULL)
{

}


/*!
  ut_CpNetworkStatus::~ut_CpNetworkStatus
 */
ut_CpNetworkStatus::~ut_CpNetworkStatus()
{
    delete m_networkStatus;
}


/*!
  ut_CpNetworkStatus::init
 */
void ut_CpNetworkStatus::init()
{
    initialize();

    QT_TRAP_THROWING(SmcDefaultValue<QString>::SetL(QString("testi")));
    QT_TRAP_THROWING(SmcDefaultValue<HbIcon>::SetL(HbIcon("")));
    QT_TRAP_THROWING(SmcDefaultValue< QSystemNetworkInfo::NetworkStatus >::SetL(QSystemNetworkInfo::Connected));
    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::SimStatus >::SetL(QSystemDeviceInfo::SingleSimAvailable ));
//    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::Profile >::SetL(QSystemDeviceInfo::NormalProfile ));
    
    m_networkStatus = new CpNetworkStatus;
}


/*!
  ut_CpNetworkStatus::cleanup
 */
void ut_CpNetworkStatus::cleanup()
{
    reset();
    
    delete m_networkStatus;
    m_networkStatus = NULL;
}

/*!
  ut_CpNetworkStatus::t_statusText
 */
void ut_CpNetworkStatus::t_statusText()
{
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::WcdmaMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::GsmMode );
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    EXPECT( QtMobility::QSystemNetworkInfo, networkName );
    m_networkStatus->statusText();
    QVERIFY(verify());
    
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::WcdmaMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::GsmMode );
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    EXPECT( QtMobility::QSystemNetworkInfo, networkName );
    QT_TRAP_THROWING(SmcDefaultValue<QString>::SetL(QString("")));
    m_networkStatus->statusText();
    QVERIFY(verify());
    
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    EXPECT( QtMobility::QSystemNetworkInfo, networkName ).times(0);
    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::SimStatus >::SetL(QSystemDeviceInfo::SimNotAvailable ));
    m_networkStatus->statusText();
    QVERIFY(verify());
    
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::WcdmaMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::GsmMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkName ).times(0);
    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::SimStatus >::SetL(QSystemDeviceInfo::SingleSimAvailable ));
    QT_TRAP_THROWING(SmcDefaultValue< QSystemNetworkInfo::NetworkStatus >::SetL(QSystemNetworkInfo::NoNetworkAvailable));
    m_networkStatus->statusText();
    QVERIFY(verify());
}

/*!
  ut_CpNetworkStatus::t_statusIcon
 */
void ut_CpNetworkStatus::t_statusIcon()
{
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::WcdmaMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::GsmMode );
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    m_networkStatus->statusIcon();
    QVERIFY(verify());
    
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::SimStatus >::SetL(QSystemDeviceInfo::SimNotAvailable ));
    m_networkStatus->statusIcon();
    QVERIFY(verify());
    
    EXPECT( QtMobility::QSystemDeviceInfo, simStatus );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::WcdmaMode );
    EXPECT( QtMobility::QSystemNetworkInfo, networkStatus ).with ( QSystemNetworkInfo::GsmMode );
    QT_TRAP_THROWING(SmcDefaultValue< QSystemDeviceInfo::SimStatus >::SetL(QSystemDeviceInfo::SingleSimAvailable ));
    QT_TRAP_THROWING(SmcDefaultValue< QSystemNetworkInfo::NetworkStatus >::SetL(QSystemNetworkInfo::NoNetworkAvailable));
    m_networkStatus->statusIcon();
    QVERIFY(verify());
}

/*!
  ut_CpNetworkStatus::t_networkNameChanged
 */
void ut_CpNetworkStatus::t_networkNameChanged()
{
    CpSettingFormItemData data;
    m_networkStatus->setSettingFormItemData(&data);
    m_networkStatus->networkNameChanged(QSystemNetworkInfo::WcdmaMode, QString(""));
    m_networkStatus->networkNameChanged(QSystemNetworkInfo::BluetoothMode, QString(""));
}

/*!
  ut_CpNetworkStatus::t_networkStatusChanged
 */
void ut_CpNetworkStatus::t_networkStatusChanged()
{
    CpSettingFormItemData data;
    m_networkStatus->setSettingFormItemData(&data);
    m_networkStatus->networkStatusChanged(QSystemNetworkInfo::WcdmaMode, QSystemNetworkInfo::EmergencyOnly);
    m_networkStatus->networkStatusChanged(QSystemNetworkInfo::BluetoothMode, QSystemNetworkInfo::EmergencyOnly);
}

/*!
  ut_CpNetworkStatus::t_setSettingFormItemData
 */
void ut_CpNetworkStatus::t_setSettingFormItemData()
{
    CpSettingFormItemData *data(NULL);
    m_networkStatus->setSettingFormItemData(data);
}

/*!
  ut_CpNetworkStatus::t_memleak
 */
void ut_CpNetworkStatus::t_memleak()
{
    
}

QTEST_MAIN_S60(ut_CpNetworkStatus)
