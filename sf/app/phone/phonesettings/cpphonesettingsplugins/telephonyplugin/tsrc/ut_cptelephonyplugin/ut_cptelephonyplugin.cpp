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

#include "ut_cptelephonyplugin.h"
#include "qtestmains60ui.h"
#include <cpsettingformitemdata.h>
#define private public
#include "cptelephonyplugin.h"
#include "cpitemdatahelper.h"

Q_EXTERN_C const char * qt_plugin_query_verification_data();
Q_EXTERN_C ::QObject *  qt_plugin_instance();

/*!
  UT_CpTelephonyPlugin::UT_CpTelephonyPlugin
 */
UT_CpTelephonyPlugin::UT_CpTelephonyPlugin() 
    : m_callsplugin(NULL)
{
    const char * verificationData = qt_plugin_query_verification_data();
}

/*!
  UT_CpTelephonyPlugin::~UT_CpTelephonyPlugin
 */
UT_CpTelephonyPlugin::~UT_CpTelephonyPlugin()
{
    delete m_callsplugin;
}

/*!
  UT_CpTelephonyPlugin::init
 */
void UT_CpTelephonyPlugin::init()
{
    initialize();

    m_callsplugin = (CpTelephonyPlugin*)qt_plugin_instance();
}

/*!
  UT_CpTelephonyPlugin::cleanup
 */
void UT_CpTelephonyPlugin::cleanup()
{
    reset();
    
    delete m_callsplugin;
    m_callsplugin = NULL;
}

/*!
  UT_CpTelephonyPlugin::t_memleak
 */
void UT_CpTelephonyPlugin::t_memleak()
{
    
}

/*!
  UT_CpTelephonyPlugin::t_createSettingFormItemData
 */
void UT_CpTelephonyPlugin::t_createSettingFormItemData()
{
    CpItemDataHelper itemDataHelper;
    QList<CpSettingFormItemData*> items = 
            m_callsplugin->createSettingFormItemData(itemDataHelper);
    
    QVERIFY(items.count()==1);
    qDeleteAll(items);
}



QTEST_MAIN_S60UI(UT_CpTelephonyPlugin)
