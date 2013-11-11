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
#include "ut_infowidgetpreferences.h"
#define private public
#include "infowidgetpreferences.h"
#include "qtestmains60.h"

/*!
  UT_InfoWidgetPreferences::UT_InfoWidgetPreferences
 */
UT_InfoWidgetPreferences::UT_InfoWidgetPreferences() 
    : 
    m_infoWidgetPreferences(0)
{
    
}


/*!
  UT_InfoWidgetPreferences::~UT_InfoWidgetPreferences
 */
UT_InfoWidgetPreferences::~UT_InfoWidgetPreferences()
{
    delete m_infoWidgetPreferences;
}


/*!
  UT_InfoWidgetPreferences::init
 */
void UT_InfoWidgetPreferences::init()
{
    initialize();
    
    m_infoWidgetPreferences = new InfoWidgetPreferences();
    
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetPreferences::cleanup
 */
void UT_InfoWidgetPreferences::cleanup()
{
    reset();
    
    delete m_infoWidgetPreferences;
    m_infoWidgetPreferences = 0;
}


/*!
  UT_InfoWidgetPreferences::t_storePreferences
 */
void UT_InfoWidgetPreferences::t_storePreferences()
{
    m_infoWidgetPreferences->storePreferences();
    
    m_infoWidgetPreferences->m_options = (InfoWidgetPreferences::Option)1;
    m_infoWidgetPreferences->m_validatedOptions = (InfoWidgetPreferences::Option)2;
    m_infoWidgetPreferences->storePreferences();
            
    QVERIFY(verify());
}


/*!
  UT_InfoWidgetPreferences::t_preference
 */
void UT_InfoWidgetPreferences::t_preference()
{
    QString preferenceValue = m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayHomeZone);
    QVERIFY(DISPLAY_SETTING_OFF == preferenceValue);
    
    preferenceValue = m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayMcn);
    QVERIFY(DISPLAY_SETTING_OFF == preferenceValue);
    
    preferenceValue = m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayActiveLine);
    QVERIFY(DISPLAY_SETTING_OFF == preferenceValue);
    
    preferenceValue = m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplaySatText);
    QVERIFY(DISPLAY_SETTING_OFF == preferenceValue);
}


/*!
  UT_InfoWidgetPreferences::t_setPreference
 */
void UT_InfoWidgetPreferences::t_setPreference()
{
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayHomeZone, DISPLAY_SETTING_ON);
    QVERIFY(DISPLAY_SETTING_ON == m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayHomeZone));
    
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayMcn, DISPLAY_SETTING_ON);
    QVERIFY(DISPLAY_SETTING_ON == m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayMcn));
    
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayActiveLine, DISPLAY_SETTING_ON);
    QVERIFY(DISPLAY_SETTING_ON == m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplayActiveLine));
    
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplaySatText, DISPLAY_SETTING_ON);
    QVERIFY(DISPLAY_SETTING_ON == m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplaySatText));
    
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplaySatText, DISPLAY_SETTING_OFF);
    QVERIFY(DISPLAY_SETTING_OFF == m_infoWidgetPreferences->preference(
        InfoWidgetPreferences::DisplaySatText));
}

/*!
  UT_InfoWidgetPreferences::t_visibleItemCount
 */
void UT_InfoWidgetPreferences::t_visibleItemCount()
{
    int visibleItems = m_infoWidgetPreferences->visibleItemCount();
    QVERIFY(visibleItems == 0);
    
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayHomeZone, DISPLAY_SETTING_ON);
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayMcn, DISPLAY_SETTING_ON);
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplayActiveLine, DISPLAY_SETTING_ON);
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplaySatText, DISPLAY_SETTING_ON);
    m_infoWidgetPreferences->setPreference(
        InfoWidgetPreferences::DisplaySpn, DISPLAY_SETTING_ON);
    visibleItems = m_infoWidgetPreferences->visibleItemCount();
    QVERIFY(visibleItems == 5);
}

/*!
  UT_InfoWidgetPreferences::t_isPreferenceSet
 */
void UT_InfoWidgetPreferences::t_isPreferenceSet()
{
    m_infoWidgetPreferences->isPreferenceSet
        (InfoWidgetPreferences::DisplayHomeZone);
            
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetPreferences::t_preferences
 */
void UT_InfoWidgetPreferences::t_preferences()
{
    m_infoWidgetPreferences->preferences();
                
    QVERIFY(verify());
}

/*!
  UT_InfoWidgetPreferences::t_preferenceNames
 */
void UT_InfoWidgetPreferences::t_preferenceNames()
{
    m_infoWidgetPreferences->preferenceNames();
                
    QVERIFY(verify());
}

QTEST_MAIN_S60(UT_InfoWidgetPreferences)
