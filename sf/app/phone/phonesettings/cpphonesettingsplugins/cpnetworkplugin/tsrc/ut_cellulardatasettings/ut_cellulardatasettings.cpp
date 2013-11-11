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

#include "ut_cellulardatasettings.h"
#include "qtestmains60.h"
#include "cmmanager.h"
#define private public
#include "cellulardatasettings.h"

TCmGenConnSettings g_settings;

void fillSettings(TCmGenConnSettings& aGenConnSettings)
{
    aGenConnSettings = g_settings;
}

/*!
  UT_CellularDataSettings::UT_CellularDataSettings
 */
UT_CellularDataSettings::UT_CellularDataSettings() 
    : m_cellularDataSettings(NULL)
{

}


/*!
  UT_CellularDataSettings::~UT_CellularDataSettings
 */
UT_CellularDataSettings::~UT_CellularDataSettings()
{
    delete m_cellularDataSettings;
}


/*!
  UT_CellularDataSettings::init
 */
void UT_CellularDataSettings::init()
{
    initialize();
    m_cellularDataSettings = new CellularDataSettings;
}


/*!
  UT_CellularDataSettings::cleanup
 */
void UT_CellularDataSettings::cleanup()
{
    reset();
    
    delete m_cellularDataSettings;
    m_cellularDataSettings = NULL;
}

/*!
  UT_CellularDataSettings::t_memleak
 */
void UT_CellularDataSettings::t_memleak()
{
    
}

/*!
  UT_CellularDataSettings::t_dataUseHome
 */
void UT_CellularDataSettings::t_dataUseHome()
{
    g_settings.iCellularDataUsageVisitor = TCmCellularDataUsage(-1);
    g_settings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageHomeAutomatic == m_cellularDataSettings->dataUseHome());
    QVERIFY(verify());
    
    g_settings.iCellularDataUsageHome = ECmCellularDataUsageConfirm;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageHomeConfirm == m_cellularDataSettings->dataUseHome());
    QVERIFY(verify());
    
    g_settings.iCellularDataUsageHome = ECmCellularDataUsageDisabled;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageHomeWlanOnly == m_cellularDataSettings->dataUseHome());
    QVERIFY(verify());
}


/*!
  UT_CellularDataSettings::t_setDataUseHome
 */
void UT_CellularDataSettings::t_setDataUseHome()
{
    TCmGenConnSettings res;
    res.iCellularDataUsageHome = ECmCellularDataUsageAutomatic;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseHome(CellularDataSettings::DataUsageHomeAutomatic);
    QVERIFY(verify());
    
    res.iCellularDataUsageHome = ECmCellularDataUsageConfirm;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseHome(CellularDataSettings::DataUsageHomeConfirm);
    QVERIFY(verify());
    
    res.iCellularDataUsageHome = ECmCellularDataUsageDisabled;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseHome(CellularDataSettings::DataUsageHomeWlanOnly);
    QVERIFY(verify());
}

/*!
  UT_CellularDataSettings::t_dataUseRoam
 */
void UT_CellularDataSettings::t_dataUseRoam()
{
    g_settings.iCellularDataUsageHome = TCmCellularDataUsage(-1);
    g_settings.iCellularDataUsageVisitor = ECmCellularDataUsageAutomatic;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageAbroadAutomatic == m_cellularDataSettings->dataUseRoam());
    QVERIFY(verify());
    
    g_settings.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageAbroadConfirm == m_cellularDataSettings->dataUseRoam());
    QVERIFY(verify());
    
    g_settings.iCellularDataUsageVisitor = ECmCellularDataUsageDisabled;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::ReadGenConnSettingsL").willOnce(invoke(fillSettings));
    expect("RCmManager::Close");
    QVERIFY(CellularDataSettings::DataUsageAbroadWlanOnly == m_cellularDataSettings->dataUseRoam());
    QVERIFY(verify());
}

/*!
  UT_CellularDataSettings::t_setDataUseRoam
 */
void UT_CellularDataSettings::t_setDataUseRoam()
{
    TCmGenConnSettings res;
    res.iCellularDataUsageVisitor = ECmCellularDataUsageAutomatic;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseRoam(CellularDataSettings::DataUsageAbroadAutomatic);
    QVERIFY(verify());
    
    res.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseRoam(CellularDataSettings::DataUsageAbroadConfirm);
    QVERIFY(verify());
    
    res.iCellularDataUsageVisitor = ECmCellularDataUsageDisabled;
    expect("RCmManager::CreateTablesAndOpenL");
    expect("RCmManager::WriteGenConnSettingsL").with(res);
    expect("RCmManager::Close");
    m_cellularDataSettings->setDataUseRoam(CellularDataSettings::DataUsageAbroadWlanOnly);
    QVERIFY(verify());
}

QTEST_MAIN_S60(UT_CellularDataSettings)
