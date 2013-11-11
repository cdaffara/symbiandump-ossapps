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

#include "ut_callsplugin.h"
#include "qtestmains60.h"
#include <cpsettingformitemdata.h>
#define private public
#include "cpcallsplugin.h"
#include "cpitemdatahelper.h"

Q_EXTERN_C const char * qt_plugin_query_verification_data();
Q_EXTERN_C ::QObject *  qt_plugin_instance();
/*!
  UT_CpCallsPlugin::UT_CpCallsPlugin
 */
UT_CpCallsPlugin::UT_CpCallsPlugin() 
    : m_callsplugin(NULL)
{
    const char * verificationData = qt_plugin_query_verification_data();
}

/*!
  UT_CpCallsPlugin::~UT_CpCallsPlugin
 */
UT_CpCallsPlugin::~UT_CpCallsPlugin()
{
    delete m_callsplugin;
}

/*!
  UT_CpCallsPlugin::init
 */
void UT_CpCallsPlugin::init()
{
    initialize();

    m_callsplugin = (CpCallsPlugin*)qt_plugin_instance();
}



void UT_CpCallsPlugin::t_createSettingFormItemData()
{
    CpItemDataHelper itemDataHelper;
    CpCallsPlugin* p = (CpCallsPlugin*)qt_plugin_instance();
    QList<CpSettingFormItemData*> list;
    list.append(p->createSettingFormItemData(itemDataHelper));
    qDeleteAll(list);
}


/*!
  UT_CpCallsPlugin::cleanup
 */
void UT_CpCallsPlugin::cleanup()
{
    reset();
    
    delete m_callsplugin;
    m_callsplugin = NULL;
}

/*!
  UT_CpCallsPlugin::t_memleak
 */
void UT_CpCallsPlugin::t_memleak()
{
    
}

QTEST_MAIN_S60(UT_CpCallsPlugin)
