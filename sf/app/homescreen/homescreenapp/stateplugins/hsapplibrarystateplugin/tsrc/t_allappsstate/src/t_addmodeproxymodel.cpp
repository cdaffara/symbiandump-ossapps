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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include <QScopedPointer>
#include <QModelIndex>

#include "caentry.h"
#include "caservice.h"
#include "hsmenuitemmodel.h"
#include "t_allappsstate.h"
#include "hsapp_defs.h"
#include "hsaddmodeproxymodel.h"
#include "t_hsproxymodeltestmockmodel.h"

const char MenuStatesTest::TEST_PARAM_NAME[] = "test_param_name";
const char MenuStatesTest::TEST_PARAM_VALUE[] = "test_param_value";

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addModeProxyModel_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {		
        ////clear test entries
        CaQuery q;
        q.setAttribute(QString(Hs::widgetParam)+TEST_PARAM_NAME, TEST_PARAM_VALUE);
        QSharedPointer<CaService> caServiceInstance = CaService::instance(); 
        QList< QSharedPointer<CaEntry> > testEntries = caServiceInstance->getEntries(q);
        caServiceInstance->removeEntries(testEntries);
        ////init
        CaEntry cwrtWidgetWithMiniViewEntry(ItemEntryRole);
        cwrtWidgetWithMiniViewEntry.setEntryTypeName(Hs::applicationTypeName);
        cwrtWidgetWithMiniViewEntry.setText("AccuWeather-W3C_TEST");
        cwrtWidgetWithMiniViewEntry.setAttribute(Hs::swTypeKey,Hs::HS_CWRT_APP_TYPE);
        cwrtWidgetWithMiniViewEntry.setAttribute(Hs::widgetUriAttributeName,Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE);
        cwrtWidgetWithMiniViewEntry.setAttribute(QString(Hs::widgetParam)+TEST_PARAM_NAME,TEST_PARAM_VALUE);
        QSharedPointer<CaEntry> entry = caServiceInstance->createEntry(cwrtWidgetWithMiniViewEntry);
        QVERIFY(entry.data() != NULL);
        
        ////test
        QVariantHash preferences;
        preferences.insert(TEST_PARAM_NAME,TEST_PARAM_VALUE);
        int widgetEntryId = HsAddModeProxyModel::findCwrtWidgetEntryId(preferences);
        QCOMPARE(widgetEntryId,entry->id());
        
        QScopedPointer<HsAddModeProxyModel> addModeProxyModel;
        addModeProxyModel.reset( new HsAddModeProxyModel());
        
        QVERIFY(addModeProxyModel->mNotifier.data() != NULL );
        QCOMPARE(addModeProxyModel->mQuery.entryTypeNames().contains(Hs::applicationTypeName), QBool(true));
        QMap<QString, QString> queryAttributes = addModeProxyModel->mQuery.attributes();
        QCOMPARE(queryAttributes.value(Hs::swTypeKey), QString(Hs::HS_CWRT_APP_TYPE));
        QCOMPARE(queryAttributes.value(Hs::widgetUriAttributeName), QString(Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE));
        
        ////cwrt widget entry present in cache
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.keys().contains(entry->id()), QBool(true));
        
        ////notification: remove entry
        addModeProxyModel->updateEntryStatus(*entry.data(), RemoveChangeType); 
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.keys().contains(entry->id()), QBool(false));
        ////notification: add entry
        addModeProxyModel->updateEntryStatus(*entry.data(), AddChangeType); 
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.keys().contains(entry->id()), QBool(true));
        
        ////clear
        bool removeResult = caServiceInstance->removeEntry(*entry.data());
        QCOMPARE( removeResult , true );
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void MenuStatesTest::addModeProxyModel_filterAcceptsRow()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {       
        ////clear test entries
        CaQuery q;
        q.setAttribute(QString(Hs::widgetParam)+TEST_PARAM_NAME, TEST_PARAM_VALUE);
        QSharedPointer<CaService> caServiceInstance = CaService::instance();
        QList< QSharedPointer<CaEntry> > testEntries = caServiceInstance->getEntries(q);
        caServiceInstance->removeEntries(testEntries);
        ////init
        CaEntry cwrtWidgetWithMiniViewEntry(ItemEntryRole);
        cwrtWidgetWithMiniViewEntry.setEntryTypeName(Hs::applicationTypeName);
        cwrtWidgetWithMiniViewEntry.setText("AccuWeather-W3C_TEST");
        cwrtWidgetWithMiniViewEntry.setAttribute(Hs::swTypeKey,Hs::HS_CWRT_APP_TYPE);
        cwrtWidgetWithMiniViewEntry.setAttribute(Hs::widgetUriAttributeName,Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE);
        cwrtWidgetWithMiniViewEntry.setAttribute(QString(Hs::widgetParam)+TEST_PARAM_NAME,TEST_PARAM_VALUE);
        QSharedPointer<CaEntry> entry = caServiceInstance->createEntry(cwrtWidgetWithMiniViewEntry);
        QVERIFY(entry.data() != NULL);
        
        ////test
        QScopedPointer<HsAddModeProxyModel> addModeProxyModel;
        addModeProxyModel.reset( new HsAddModeProxyModel());
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.value(entry->id()), false);
        
        QScopedPointer<ProxyModelTestMockModel> proxyModelTestMockModel;
        proxyModelTestMockModel.reset( new ProxyModelTestMockModel(entry->id()));
        addModeProxyModel->setSourceModel(proxyModelTestMockModel.data());
        bool result = addModeProxyModel->filterAcceptsRow(0,QModelIndex());
        ///widget is not on HS so we show it
        QCOMPARE(result, true);

        QVariantHash preferences;
        preferences.insert(TEST_PARAM_NAME,TEST_PARAM_VALUE);
        addModeProxyModel->updateCacheOnAddWidget(Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE, preferences);
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.value(entry->id()), true);
        result = addModeProxyModel->filterAcceptsRow(0,QModelIndex());
        ///widget is on HS so we do not show it
        QCOMPARE(result, false);
        
        addModeProxyModel->updateCacheOnRemoveWidget(Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE, preferences);
        QCOMPARE(addModeProxyModel->mCwrtWidgetCache.value(entry->id()), false);
        result = addModeProxyModel->filterAcceptsRow(0,QModelIndex());
        ///widget is not on HS so we show it
        QCOMPARE(result, true);

        ////clear
        bool removeResult = caServiceInstance->removeEntry(*entry.data());
        QCOMPARE( removeResult , true );
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
