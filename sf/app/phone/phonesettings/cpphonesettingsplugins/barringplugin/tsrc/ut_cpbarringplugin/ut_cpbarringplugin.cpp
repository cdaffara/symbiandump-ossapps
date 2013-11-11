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

#include "ut_cpbarringplugin.h"
#include "qtestmains60.h"
#include <cpsettingformitemdata.h>
#define private public
#include "cpbarringplugin.h"
#include "cpitemdatahelper.h"

Q_EXTERN_C const char * qt_plugin_query_verification_data();
Q_EXTERN_C ::QObject *  qt_plugin_instance();

/*!
  UT_CpBarringPlugin::UT_CpBarringPlugin
 */
UT_CpBarringPlugin::UT_CpBarringPlugin() 
    : m_barringplugin(NULL)
{
    const char * verificationData = qt_plugin_query_verification_data();
}

/*!
  UT_CpBarringPlugin::~UT_CpBarringPlugin
 */
UT_CpBarringPlugin::~UT_CpBarringPlugin()
{
    delete m_barringplugin;
}

/*!
  UT_CpBarringPlugin::init
 */
void UT_CpBarringPlugin::init()
{
    initialize();

    m_barringplugin = (CpBarringPlugin*)qt_plugin_instance();
    QVERIFY(m_barringplugin);
}


/*!
  UT_CpBarringPlugin::t_createSettingFormItemData
 */
void UT_CpBarringPlugin::t_createSettingFormItemData()
{
    CpItemDataHelper itemDataHelper;
    CpBarringPlugin* p = (CpBarringPlugin*)qt_plugin_instance();
    QList<CpSettingFormItemData*> list;
    list.append(p->createSettingFormItemData(itemDataHelper));
    qDeleteAll(list);
}

/*!
  UT_CpBarringPlugin::cleanup
 */
void UT_CpBarringPlugin::cleanup()
{
    reset();
    
    delete m_barringplugin;
    m_barringplugin = NULL;
}

/*!
  UT_CpBarringPlugin::t_memleak
 */
void UT_CpBarringPlugin::t_memleak()
{
    
}

QTEST_MAIN_S60(UT_CpBarringPlugin)
