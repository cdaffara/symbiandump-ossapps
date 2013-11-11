/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main test class for Control Panel's Language plugin 
*
*/

#ifndef TESTLANGUAGEPLUGIN_H
#define TESTLANGUAGEPLUGIN_H

#include <QtTest/QtTest>
#include <HbInputLanguage>

class CpLanguagePlugin;
class CpLanguageView;
class CpSettingFormEntryItemData;
class HbMainWindow;

/**
* TestLanguagePlugin
*/
class TestLanguagePlugin : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    // Test cases start from here
    // CpLanguagePlugin
    void test_CpLanguagePlugin_createSettingFormItemData();
    void test_CpLanguagePlugin_updateEntryItem();
       
    void test_ExtendedLocaleUtil_localeMappings();
    
    void test_CpLanguagePluginUtil_localizedLanguageNames();
    void test_CpLanguagePluginUtil_localizedRegionNames();
    void test_CpLanguagePluginUtil_localizedInputLanguageNames();
    void test_CpLanguagePluginUtil_isChineseVariant();
    
    void test_CpLanguageView_new();
    void test_CpLanguageView_onRegionChanged();
    void test_CpLanguageView_onPrimaryInputLanguageChanged();
    void test_CpLanguageView_onSecondaryInputLanguageChanged();
    void test_CpLanguageView_onPrimaryInputLanguageChanged2();
    void test_CpLanguageView_onSecondaryInputLanguageChanged2();
    void test_CpLanguageView_launchRegionalSettingsView();
    void test_CpLanguageView_onDataFormItemActivated();    
    void test_CpLanguageView_close();
    void test_CpLanguageView_fillDataFormComboBoxItem();
    void test_CpLanguageView_updateSecondaryInputLanguageItem();
    void test_CpLanguageView_promptRestart();
    void test_CpLanguageView_restartDevice();
    void test_CpLanguageView_languageRegionMapping();
    
private:
    HbMainWindow *mMainWindow;  
    CpLanguagePlugin* mPlugin;  	
    CpLanguageView* mView;    
    CpSettingFormEntryItemData *mEntryItemData;

};

#endif // T_CPLANGUAGEPLUGIN_H
