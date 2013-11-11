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

#include "ut_cpnetworkplugin.h"
#include "qtestmains60.h"
#include <cpsettingformitemdata.h>
#include <hbicon.h>
#define private public
#include "cpnetworkplugin.h"
#include "cpitemdatahelper.h"

Q_EXTERN_C const char * qt_plugin_query_verification_data();
Q_EXTERN_C ::QObject *  qt_plugin_instance();

/*!
  UT_CpNetworkPlugin::UT_CpNetworkPlugin
 */
UT_CpNetworkPlugin::UT_CpNetworkPlugin() 
    : m_networkPlugin(NULL)
{
    const char * verificationData = qt_plugin_query_verification_data();
}


/*!
  UT_CpNetworkPlugin::~UT_CpNetworkPlugin
 */
UT_CpNetworkPlugin::~UT_CpNetworkPlugin()
{
    delete m_networkPlugin;
}


/*!
  UT_CpNetworkPlugin::init
 */
void UT_CpNetworkPlugin::init()
{
    initialize();

    QT_TRAP_THROWING(SmcDefaultValue<QString>::SetL(QString("")));
    QT_TRAP_THROWING(SmcDefaultValue<HbIcon>::SetL(HbIcon("")));
    
    m_networkPlugin = (CpNetworkPlugin*)qt_plugin_instance();
}


/*!
  UT_CpNetworkPlugin::cleanup
 */
void UT_CpNetworkPlugin::cleanup()
{
    reset();
    
    delete m_networkPlugin;
    m_networkPlugin = NULL;
}


/*!
  UT_CpNetworkPlugin::t_createSettingFormItemData
 */
void UT_CpNetworkPlugin::t_createSettingFormItemData()
{
    CpItemDataHelper itemDataHelper;
    CpNetworkPlugin* p = (CpNetworkPlugin*)qt_plugin_instance();
    QList<CpSettingFormItemData*> list;
    
    EXPECT(CpNetworkStatus, statusText);
    EXPECT(CpNetworkStatus, statusIcon);
    list.append(p->createSettingFormItemData(itemDataHelper));
    qDeleteAll(list);
    
    QVERIFY(verify());
}


/*!
  UT_CpNetworkPlugin::t_createSettingView
 */
void UT_CpNetworkPlugin::t_createSettingView()
{
    CpNetworkPlugin* p = (CpNetworkPlugin*)qt_plugin_instance();
    QVariant hint;
    
    EXPECT(CpNetworkStatus, statusText);
    CpBaseSettingView *view = p->createSettingView(hint);
    delete view;
    
    QVERIFY(verify());
}

/*!
  UT_CpNetworkPlugin::t_memleak
 */
void UT_CpNetworkPlugin::t_memleak()
{
    
}

QTEST_MAIN_S60(UT_CpNetworkPlugin)
