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

#include "ut_cptelephonypluginview.h"
#include "qtestmains60ui.h"
#include <cpplugininterface.h>
#include <cpitemdatahelper.h>
#include <cpsettingformitemdata.h>
#include <smcmockclassincludes.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include "cptelephonypluginview.h"


class DummyPlugin : public CpPluginInterface
{
public:
    DummyPlugin(){}
    ~DummyPlugin(){}
    
    QList<CpSettingFormItemData*> createSettingFormItemData(
            CpItemDataHelper &itemDataHelper) const
    {
        SMC_MOCK_METHOD1( QList<CpSettingFormItemData*>, CpItemDataHelper &, itemDataHelper )
    }
};

/*!
  UT_CpTelephonyPluginView::UT_CpTelephonyPluginView
 */
UT_CpTelephonyPluginView::UT_CpTelephonyPluginView() 
    : m_callspluginview(NULL)
{

}

/*!
  UT_CpTelephonyPluginView::~UT_CpTelephonyPluginView
 */
UT_CpTelephonyPluginView::~UT_CpTelephonyPluginView()
{
    delete m_callspluginview;
}



/*!
  UT_CpTelephonyPluginView::init
 */
void UT_CpTelephonyPluginView::init()
{
    initialize();

    CpPluginInterface *nullPlugin=0;
    DummyPlugin *ret = new DummyPlugin;

    QList<CpSettingFormItemData*> list;
    list.append(new CpSettingFormItemData);
    expect("CpPluginLoader::loadCpPluginInterface").
            with(QString("cpcallsplugin")).returns(ret);
    expect("DummyPlugin::createSettingFormItemData").returns(list);
    expect("CpPluginLoader::loadCpPluginInterface").
            with(QString("cpdivertplugin")).returns(nullPlugin);
    
    m_callspluginview = new CpTelephonyPluginView;
    QVERIFY( verify() );

}

/*!
  UT_CpTelephonyPluginView::cleanup
 */
void UT_CpTelephonyPluginView::cleanup()
{
    reset();
    
    delete m_callspluginview;
    m_callspluginview = NULL;
}

/*!
  UT_CpTelephonyPluginView::t_memleak
 */
void UT_CpTelephonyPluginView::t_memleak()
{
    QList<CpSettingFormItemData*> list;
    list.append(new CpSettingFormItemData);
    DummyPlugin *ret = new DummyPlugin;
    expect("CpPluginLoader::loadCpPluginInterface").
            with(QString("cpcallsplugin")).returns(ret);
    expect("DummyPlugin::createSettingFormItemData").returns(list);
    
    ret = new DummyPlugin;
    expect("CpPluginLoader::loadCpPluginInterface").
            with(QString("cpdivertplugin")).returns(ret);
    expect("DummyPlugin::createSettingFormItemData").returns(list);
    
    
    CpTelephonyPluginView *tmp = new CpTelephonyPluginView;
    QVERIFY( verify() );
    delete tmp;
}



QTEST_MAIN_S60UI(UT_CpTelephonyPluginView)
