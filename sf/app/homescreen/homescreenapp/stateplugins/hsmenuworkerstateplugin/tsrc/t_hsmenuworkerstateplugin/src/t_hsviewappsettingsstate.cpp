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

#include <QStateMachine>

#include "hsmenuworkerstate.h"
#include "hsviewappsettingsstate.h"

#include "t_hsmenuworkerstateplugin.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::ViewAppSettings_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsViewAppSettingsState vas1(&parentState1);
        QString s1 = vas1.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/MenuWorkerState/ViewAppSettingsState"));

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
void MenuStatesTest::ViewAppSettings_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;
        HsViewAppSettingsState viewAppSettingsState(&parentState1);
        
        const QString javaSettingsPlugin("dumy/resource/qt/plugins/appsettings/javaapplicationsettingsview.qtplugin");
        const QString javaAppUid("269636027");
        CaEntry item( ItemEntryRole );
        item.setText( "TestJavaApp" );
        item.setEntryTypeName(Hs::applicationTypeName);
        //item.setAttribute(Hs::applicationUidEntryKey, javaAppUid);
        item.setAttribute(Hs::appSettingsPlugin, javaSettingsPlugin);
        QSharedPointer<CaEntry>  newItem = CaService::instance()->createEntry( item );
        QVERIFY(!newItem.isNull());
        QVERIFY(newItem->id());
        
        QVariantMap eventAttributes;
        eventAttributes.insert(Hs::itemIdKey, newItem->id());

        HsMenuEvent *viewAppSettingsEvent =
            new HsMenuEvent(HsMenuEvent::ShowAppSettings, eventAttributes);
        
        viewAppSettingsState.onEntry(viewAppSettingsEvent);
        
        QVERIFY(CaService::instance()->removeEntry(newItem->id()));

        delete viewAppSettingsEvent;        

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
void MenuStatesTest::ViewAppSettings_onExit()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    QState *parent = new QState();
    HsViewAppSettingsState *viewAppSettingsState = new HsViewAppSettingsState(parent);

    QEvent *event = new QEvent(QEvent::StateMachineSignal);
    viewAppSettingsState->onExit(event);
    delete event;
    delete viewAppSettingsState;
    delete parent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

