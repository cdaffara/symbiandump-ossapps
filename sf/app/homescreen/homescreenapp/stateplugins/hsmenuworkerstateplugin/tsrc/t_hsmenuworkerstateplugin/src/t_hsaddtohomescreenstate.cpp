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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/


#include "t_hsmenuworkerstateplugin.h"

#include <hbmainwindow.h>
#include <HbMessageBox>
#include <HbAction>

#include <QGraphicsItem>
#include <qstatemachine.h>

#include "hsmenuitemmodel.h"
#include "hsmenuservice.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenuworkerstate.h"
#include "hsaddtohomescreenstate.h"
#include "hsdialogcontroller.h"
#include "hsapp_defs.h"
#include "hspage.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState *parent = new QState();

        HsAddToHomeScreenState *as1 = new HsAddToHomeScreenState(parent);

        HsAddToHomeScreenState *as2 = new HsAddToHomeScreenState(parent);

        QString on1 = as1->objectName();
        QString on2 = as2->objectName();
        QVERIFY(on1 == on2);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        const QString SHORTCUT_WIDGET_URI("hsshortcutwidgetplugin");

        HsAddToHomeScreenState addToHomeScreenState;

        QScopedPointer<HsMenuItemModel>
            model(HsMenuService::getAllApplicationsModel(
                  Hs::DescendingNameHsSortAttribute));
        
        const QString clockWidgetName("Clock");

        int clockId = 0;
        int appId = 0;
        for (int i = 0; i< model->rowCount(); ++i) {
            const QString textRole = model->data((model->index(i)),
                                           CaItemModel::TextRole).toString();
            const QString entryTypeName = model->data((model->index(i)),
                                           CaItemModel::TypeRole).toString();
            
            if (entryTypeName == Hs::applicationTypeName) {
                appId = model->data((model->index(i)), CaItemModel::IdRole).toInt();
            }

            if((entryTypeName == Hs::widgetTypeName) && (textRole == clockWidgetName)) {
                clockId = model->data((model->index(i)),
                        CaItemModel::IdRole).toInt();
            }
        }
        QVERIFY(clockId != 0);
        const QString clockWidgetUri("hsclockwidgetplugin");

        QVariantMap eventAttributes;

        eventAttributes.insert(Hs::widgetUriAttributeName,
                               clockWidgetUri);

        eventAttributes.insert(Hs::entryTypeNameKey, Hs::widgetTypeName);

        eventAttributes.insert(Hs::itemIdKey, clockId);

        HsMenuEvent *addWidgetEvent =
            new HsMenuEvent(HsMenuEvent::AddToHomeScreen, eventAttributes);

        addToHomeScreenState.onEntry(addWidgetEvent);

        {
            QCOMPARE(HsContentService::instance()->mParams["uri"].toString(), 
                clockWidgetUri);
        }

        eventAttributes.remove(Hs::itemIdKey);
        eventAttributes.insert(Hs::itemIdKey, appId);

        HsMenuEvent *addShortcutEvent =
            new HsMenuEvent(HsMenuEvent::AddToHomeScreen, eventAttributes);

        addToHomeScreenState.onEntry(addShortcutEvent);

        {
            QCOMPARE(HsContentService::instance()->mParams["uri"].toString(), 
                SHORTCUT_WIDGET_URI);
        }

        delete addWidgetEvent;
        delete addShortcutEvent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_onEntry_2()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    const QString SHORTCUT_WIDGET_URI("hsshortcutwidgetplugin");

     HsAddToHomeScreenState addToHomeScreenState;

     QScopedPointer<HsMenuItemModel>
         model(HsMenuService::getAllApplicationsModel(
               Hs::DescendingNameHsSortAttribute));

     const QString clockWidgetUri("homescreen.nokia.com/widget/clock");
     
     CaEntry item( ItemEntryRole );
     item.setText( "TestTemplateAppWithUri" );
     item.setEntryTypeName(Hs::templatedApplicationTypeName);
     item.setAttribute(Hs::widgetUriAttributeName, clockWidgetUri);
     QSharedPointer<CaEntry> newItem = CaService::instance()->createEntry( item );
     QVERIFY(!newItem.isNull());
     QVERIFY(newItem->id());
     
     QVariantMap eventAttributes;
     eventAttributes.insert(Hs::entryTypeNameKey, Hs::templatedApplicationTypeName);
     eventAttributes.insert(Hs::itemIdKey, newItem->id());
     eventAttributes.insert(Hs::widgetUriAttributeName, clockWidgetUri);

     HsMenuEvent *addWidgetEvent =
         new HsMenuEvent(HsMenuEvent::AddToHomeScreen, eventAttributes);

     addToHomeScreenState.onEntry(addWidgetEvent);
     
     {
         QCOMPARE(HsContentService::instance()->mParams["uri"].toString(), clockWidgetUri);
     }
     
     QVERIFY(CaService::instance()->removeEntry(newItem->id()));

     delete addWidgetEvent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_onEntry_3()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    const QString SHORTCUT_WIDGET_URI("hsshortcutwidgetplugin");

     HsAddToHomeScreenState addToHomeScreenState;

     QScopedPointer<HsMenuItemModel>
         model(HsMenuService::getAllApplicationsModel(
               Hs::DescendingNameHsSortAttribute));

     const QString clockWidgetName("Clock");

     int clockId = 0;

     for (int i = 0; i< model->rowCount(); ++i) {
         const QString textRole = model->data((model->index(i)),
                                        CaItemModel::TextRole).toString();

         if(textRole == clockWidgetName) {
             clockId = model->data((model->index(i)),
                     CaItemModel::IdRole).toInt();
         }
     }
  
     CaEntry item( ItemEntryRole );
     item.setText( "TestTemplateAppWithoutUri" );
     item.setEntryTypeName(Hs::templatedApplicationTypeName);
     QSharedPointer<CaEntry> newItem = CaService::instance()->createEntry( item );
     QVERIFY(newItem->id());
     

     QVariantMap eventAttributes;
     eventAttributes.insert(Hs::entryTypeNameKey, Hs::templatedApplicationTypeName);
     eventAttributes.insert(Hs::itemIdKey, newItem->id());

     HsMenuEvent *addWidgetEvent =
         new HsMenuEvent(HsMenuEvent::AddToHomeScreen, eventAttributes);

     addToHomeScreenState.onEntry(addWidgetEvent);
     
     QVERIFY(CaService::instance()->removeEntry(newItem->id()));

     delete addWidgetEvent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_onEntry_4()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    const QString SHORTCUT_WIDGET_URI("hsshortcutwidgetplugin");

     HsAddToHomeScreenState addToHomeScreenState;

     QScopedPointer<HsMenuItemModel>
         model(HsMenuService::getAllApplicationsModel(
               Hs::DescendingNameHsSortAttribute));

     const QString clockWidgetUri("homescreen.nokia.com/widget/clock");
     
     CaEntry item( ItemEntryRole );
     item.setText( "TestParametrizedTemplateAppWithUri" );
     item.setEntryTypeName(Hs::templatedApplicationTypeName);
     item.setAttribute(Hs::widgetUriAttributeName, clockWidgetUri);
     // add widget param atrib to entry
     item.setAttribute(QString(Hs::widgetParam+QString("caEntryId")), QString("12"));
     QSharedPointer<CaEntry> newItem = CaService::instance()->createEntry( item );
     QVERIFY(!newItem.isNull());
     QVERIFY(newItem->id());
     
     QVariantMap eventAttributes;
     eventAttributes.insert(Hs::entryTypeNameKey, Hs::templatedApplicationTypeName);
     eventAttributes.insert(Hs::itemIdKey, newItem->id());
     eventAttributes.insert(Hs::widgetUriAttributeName, clockWidgetUri);

     QVariantMap widgetParams;
     widgetParams.insert(QString(Hs::widgetParam+QString("mcsId")), QString("12"));
     eventAttributes.insert(Hs::widgetParam,widgetParams);

     HsMenuEvent *addWidgetEvent =
         new HsMenuEvent(HsMenuEvent::AddToHomeScreen, eventAttributes);

     addToHomeScreenState.onEntry(addWidgetEvent);
     
     {
         QCOMPARE(HsContentService::instance()->mParams["uri"].toString(), 
             clockWidgetUri);
     }
     
     QVERIFY(CaService::instance()->removeEntry(newItem->id()));

     delete addWidgetEvent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddToHomeScreenState_showMessageWidgetCorrupted()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<HsAddToHomeScreenState> as(new HsAddToHomeScreenState());

        as->showMessageWidgetCorrupted();
        checkDialogController();
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

void MenuStatesTest::AddToHomeScreenState_openHomeScreen()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<HsAddToHomeScreenState> as(new HsAddToHomeScreenState());
        QSignalSpy spy(as.data(), SIGNAL(exit()));
        as->openHomeScreen();
        QCOMPARE(spy.count(), 1);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

