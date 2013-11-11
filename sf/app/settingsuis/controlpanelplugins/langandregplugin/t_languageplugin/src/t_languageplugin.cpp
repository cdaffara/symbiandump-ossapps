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
* Description:  Main test class for Control Panel's Language plugin.
*
*/

#include "t_languageplugin.h"
#include <QtTest/QtTest>
#include <QModelIndex>
#include <hbmainwindow.h>
#include <hbinputsettingproxy.h>
#include <hblocaleutil.h>
#include <hbinpututils.h>
#include <cpsettingformentryitemdataimpl.h>
#include <cpitemdatahelper.h>
#include "cplanguageplugin.h"
#include "extendedlocaleutil.h"
#include "cplanguageview.h"
#include "cplanguagepluginutil.h"

/*!
    Initialize test case.
    Called before each testfunction is executed.
*/
void TestLanguagePlugin::init() 
{   
    mMainWindow = new HbMainWindow();
    QVERIFY(mMainWindow);
    
    mPlugin = new CpLanguagePlugin;
    QVERIFY(mPlugin);
    // Note: we get different values for condition coverage of CpLanguageView()
    // depending on which values have been selected in Control Panel settings. 
	mView = new CpLanguageView();
	QVERIFY(mView);
	
	CpItemDataHelper itemDataHelper;
	QList<CpSettingFormItemData*> itemData = mPlugin->createSettingFormItemData(itemDataHelper); 
	mEntryItemData = qobject_cast<CpSettingFormEntryItemData*>(itemData.front());
}

/*!
    Cleanup test case.
    Called after every testfunction.
*/
void TestLanguagePlugin::cleanup()
{
	delete mView;
	mView = 0;
	
	delete mPlugin;
    mPlugin = 0;  	
    
    delete mEntryItemData;
    mEntryItemData = 0;
    
    delete mMainWindow;
    mMainWindow = 0;
}

/* ---------------------------------------------------------------------------
 * Unit test cases 
 * ---------------------------------------------------------------------------*/

/*!
 *  test CpLanguagePlugin::createSettingFormItemData
 */
void TestLanguagePlugin::test_CpLanguagePlugin_createSettingFormItemData()
{    
	CpItemDataHelper itemDataHelper;
    QList<CpSettingFormItemData*> itemData = mPlugin->createSettingFormItemData(itemDataHelper); 
    QVERIFY(itemData.count() == 1);     
    
    CpSettingFormEntryItemData *entryItemData = qobject_cast<CpSettingFormEntryItemData*>(itemData.front());
    QVERIFY(entryItemData != 0);
    QVERIFY(entryItemData->type() == static_cast<HbDataFormModelItem::DataItemType>(CpSettingFormEntryItemData::ListEntryItem) );
}

/*!
 * test CpLanguagePlugin::updateEntryItem
 */
void TestLanguagePlugin::test_CpLanguagePlugin_updateEntryItem()
{
    CpLanguagePlugin::updateEntryItem();
    
    QString localisedLanguage = HbLocaleUtil::localisedLanguageName(HbLocaleUtil::currentLanguage());
    
    QVERIFY(mEntryItemData->description() == localisedLanguage);
}

/*!
 * test ExtendedLocaleUtil::localeMappings
 */
void TestLanguagePlugin::test_ExtendedLocaleUtil_localeMappings()
{
    QList<LanguageRegionMapping> allMaps = ExtendedLocaleUtil::localeMappings(false);
    QVERIFY(allMaps.count() > 0);
    
    QList<LanguageRegionMapping> supportedMaps = ExtendedLocaleUtil::localeMappings(true);
    QVERIFY(supportedMaps.count() > 0);
    
    QVERIFY(allMaps.count() >= supportedMaps.count());
}

/*!
 * test CpLanguagePluginUtil::localizedLanguageNames
 */
void TestLanguagePlugin::test_CpLanguagePluginUtil_localizedLanguageNames()
{
    QStringList supportLanguages = HbLocaleUtil::supportedLanguages();
    QStringList localisedLanguages = CpLanguagePluginUtil::localizedLanguageNames(supportLanguages);
    
    QVERIFY (supportLanguages.count() == localisedLanguages.count());
}

/*!
 * test CpLanguagePluginUtil::localizedRegionNames
 */
void TestLanguagePlugin::test_CpLanguagePluginUtil_localizedRegionNames()
{
    QStringList supportedRegions = HbLocaleUtil::supportedRegions();
    QStringList localisedRegions = CpLanguagePluginUtil::localizedRegionNames(supportedRegions);
    
    QVERIFY (supportedRegions.count() == localisedRegions.count());
}

/*
 * test CpLanguagePluginUtil::localizedInputLanguageNames
 */
void TestLanguagePlugin::test_CpLanguagePluginUtil_localizedInputLanguageNames()
{
    QList<HbInputLanguage> supportedInputLanguages;
    HbInputUtils::listSupportedInputLanguages(supportedInputLanguages);
    
    QStringList localisedInputLanguages = CpLanguagePluginUtil::localizedInputLanguageNames(supportedInputLanguages);
    QVERIFY(supportedInputLanguages.count() == localisedInputLanguages.count());
}

/*!
 * test CpLanguagePluginUtil::isChineseVariant
 */
void TestLanguagePlugin::test_CpLanguagePluginUtil_isChineseVariant()
{
    bool isChineseVariant = CpLanguagePluginUtil::isChineseVariant();
    
    QStringList supportedLanguages = HbLocaleUtil::supportedLanguages();
    foreach(const QString &language, supportedLanguages) {
        if (QLocale(language).language() == QLocale::Chinese) {
            QVERIFY (isChineseVariant);
            return;
        }
    }
    
    QVERIFY (!isChineseVariant);
}

/*!
 * test new CpLanguageView
 */
void TestLanguagePlugin::test_CpLanguageView_new()
{
    CpLanguageView* view = new CpLanguageView();
    QVERIFY(view != 0);
    delete view;
}

/*!
 * test CpLanguageView::onRegionChanged(int index)
 */
void TestLanguagePlugin::test_CpLanguageView_onRegionChanged()
{
    QString oldRegion = HbLocaleUtil::currentRegion();
    
    QStringList allRegions = mView->mSupportedRegions;
    QVERIFY(allRegions.count() > 0);
    
    for(int i = 0; i < allRegions.count();i++) {
        if (!allRegions.at(i).trimmed().isEmpty()) {
            mView->onRegionChanged(i);
            QVERIFY(HbLocaleUtil::currentRegion() == allRegions.at(i));
        }
    }
    
    HbLocaleUtil::changeRegion(oldRegion);
    QVERIFY(HbLocaleUtil::currentRegion() == oldRegion);
    
    mView->onRegionChanged(-1);
    QVERIFY(HbLocaleUtil::currentRegion() == oldRegion);
    
    mView->onRegionChanged(allRegions.count());
    QVERIFY(HbLocaleUtil::currentRegion() == oldRegion);
}

/*!
 * test CpLanguageView::onPrimaryInputLanguageChanged(int index)
 */
void TestLanguagePlugin::test_CpLanguageView_onPrimaryInputLanguageChanged()
{
    HbInputLanguage oldPrimaryInputLanguage = HbInputSettingProxy::instance()->globalInputLanguage();
    
    QList<HbInputLanguage> supportedInputLanguages = mView->mPrimaryInputLanguages;
    
    QVERIFY(supportedInputLanguages.count() > 0);
    for (int i = 0; i < supportedInputLanguages.count();i++) {
        mView->onPrimaryInputLanguageChanged(i);
        QVERIFY(HbInputSettingProxy::instance()->globalInputLanguage() == supportedInputLanguages.at(i));
    }
    
    HbInputSettingProxy::instance()->setGlobalInputLanguage(oldPrimaryInputLanguage);
    QVERIFY(HbInputSettingProxy::instance()->globalInputLanguage() == oldPrimaryInputLanguage);
    
    mView->onPrimaryInputLanguageChanged(-1);
    QVERIFY(HbInputSettingProxy::instance()->globalInputLanguage() == oldPrimaryInputLanguage);
    
    mView->onPrimaryInputLanguageChanged(supportedInputLanguages.count());
    QVERIFY(HbInputSettingProxy::instance()->globalInputLanguage() == oldPrimaryInputLanguage);
}

/*!
 * test CpLanguageView::onSecondaryInputLanguageChanged(int index)
 */
void TestLanguagePlugin::test_CpLanguageView_onSecondaryInputLanguageChanged()
{
    HbInputLanguage primaryInputLanguage = HbInputSettingProxy::instance()->globalInputLanguage();
    HbInputLanguage oldSecondaryInputLanguage = HbInputSettingProxy::instance()->globalSecondaryInputLanguage();
    
    QList<HbInputLanguage> supportedInputLanguages = mView->mSecondaryInputLanguages;
    
    QVERIFY(supportedInputLanguages.count() > 0);
    for (int i = 0; i < supportedInputLanguages.count();i++) {
        if ( supportedInputLanguages.at(i) != primaryInputLanguage) {
            mView->onSecondaryInputLanguageChanged(i);
            QVERIFY(HbInputSettingProxy::instance()->globalSecondaryInputLanguage() == supportedInputLanguages.at(i));
        }
    }
    
    HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(oldSecondaryInputLanguage);
    QVERIFY(HbInputSettingProxy::instance()->globalSecondaryInputLanguage() == oldSecondaryInputLanguage);
    
    mView->onSecondaryInputLanguageChanged(-1);
    QVERIFY(HbInputSettingProxy::instance()->globalSecondaryInputLanguage() == oldSecondaryInputLanguage);
    
    mView->onSecondaryInputLanguageChanged(supportedInputLanguages.count());
    QVERIFY(HbInputSettingProxy::instance()->globalSecondaryInputLanguage() == oldSecondaryInputLanguage);
}

/*!
 * test CpLanguageView::onPrimaryInputLanguageChanged(const HbInputLanguage &inputLanguage)
 */
void TestLanguagePlugin::test_CpLanguageView_onPrimaryInputLanguageChanged2()
{
    int oldIndex = mView->mCurrentPrimaryInputLanguageItem->contentWidgetData("currentIndex").toInt();
    
    QList<HbInputLanguage> supportedInputLanguages = mView->mPrimaryInputLanguages;
    
    QVERIFY(supportedInputLanguages.count() > 0);
    for (int i = 0; i < supportedInputLanguages.count();i++) {
        mView->onPrimaryInputLanguageChanged(supportedInputLanguages.at(i));
        QVERIFY( mView->mCurrentPrimaryInputLanguageItem->contentWidgetData("currentIndex").toInt() == i );
    }
    
    mView->mCurrentPrimaryInputLanguageItem->setContentWidgetData("currentIndex",oldIndex);
    
    QVERIFY(oldIndex == mView->mCurrentPrimaryInputLanguageItem->contentWidgetData("currentIndex").toInt());
}

/*!
 * test CpLanguageView::onSecondaryInputLanguageChanged(const HbInputLanguage &inputLanguage)
 */
void TestLanguagePlugin::test_CpLanguageView_onSecondaryInputLanguageChanged2()
{
    int oldIndex = mView->mCurrentSecondaryInputLanguageItem->contentWidgetData("currentIndex").toInt();
    
    QList<HbInputLanguage> supportedInputLanguages = mView->mSecondaryInputLanguages;
    
    QVERIFY(supportedInputLanguages.count() > 0);
    for (int i = 0; i < supportedInputLanguages.count();i++) {
        mView->onSecondaryInputLanguageChanged(supportedInputLanguages.at(i));
        QVERIFY( mView->mCurrentSecondaryInputLanguageItem->contentWidgetData("currentIndex").toInt() == i );
    }
    
    mView->mCurrentSecondaryInputLanguageItem->setContentWidgetData("currentIndex",oldIndex);
    
    QVERIFY(oldIndex == mView->mCurrentSecondaryInputLanguageItem->contentWidgetData("currentIndex").toInt());
}

/*!
 * test CpLanguageView::launchRegionalSettingsView()
 */
void TestLanguagePlugin::test_CpLanguageView_launchRegionalSettingsView()
{
    mView->launchRegionalSettingsView();
    QVERIFY(mView->mClockPluginLoader != 0);
}

/*!
 * test CpLanguageView::onDataFormItemActivated(const QModelIndex &modelIndex)
 */
void TestLanguagePlugin::test_CpLanguageView_onDataFormItemActivated()
{
    QVERIFY(true);
}

/*!
 * test CpLanguageView::close()
 */
void TestLanguagePlugin::test_CpLanguageView_close()
{
    QVERIFY(true);
}

/*!
 * test CpLanguageView::fillDataFormComboBoxItem
 */
void TestLanguagePlugin::test_CpLanguageView_fillDataFormComboBoxItem()
{
    QStringList supportedRegions = HbLocaleUtil::supportedRegions();
    QStringList localizedRegions = CpLanguagePluginUtil::localizedRegionNames(supportedRegions);
    
    int regionIndex = supportedRegions.indexOf(HbLocaleUtil::currentRegion());
    
    mView->fillDataFormComboBoxItem(mView->mCurrentRegionItem,localizedRegions,regionIndex);
    
    QVERIFY(mView->mCurrentRegionItem->contentWidgetData("currentIndex").toInt() == regionIndex);
    QVERIFY(mView->mCurrentRegionItem->contentWidgetData("items").toStringList() == localizedRegions);
}

/*!
 * test CpLanguageView::updateSecondaryInputLanguageItem
 */
void TestLanguagePlugin::test_CpLanguageView_updateSecondaryInputLanguageItem()
{
    QList<HbInputLanguage> supportedInputLanguages;
    HbInputUtils::listSupportedInputLanguages(supportedInputLanguages);
    
    HbInputLanguage primaryInputLanguage = HbInputSettingProxy::instance()->globalInputLanguage();
    HbInputLanguage secondaryInputLanguage = HbInputSettingProxy::instance()->globalSecondaryInputLanguage();
    
    QVERIFY(supportedInputLanguages.count() > 0);
    
    HbInputSettingProxy::instance()->setGlobalInputLanguage(supportedInputLanguages.front());
    HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(supportedInputLanguages.front());
    
    mView->updateSecondaryInputLanguageItem();
    
    QVERIFY(mView->mCurrentSecondaryInputLanguageItem->contentWidgetData("currentIndex").toInt() == 0);
    
}

/*!
 * test CpLanguageView::promptRestart
 */
void TestLanguagePlugin::test_CpLanguageView_promptRestart()
{
    QVERIFY(true);
}

/*!
 * test CpLanguageView::restartDevice
 */
void TestLanguagePlugin::test_CpLanguageView_restartDevice()
{
    QVERIFY(true);
}

/*!
 * test CpLanguageView::languageRegionMapping
 */
void TestLanguagePlugin::test_CpLanguageView_languageRegionMapping()
{
    QStringList supportedLanguages = HbLocaleUtil::supportedLanguages();
    foreach(const QString &language,supportedLanguages) {
        LanguageRegionMapping *entry = mView->languageRegionMapping(language);
        QVERIFY(entry != 0);
    }
}

QTEST_MAIN(TestLanguagePlugin)
