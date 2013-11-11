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

#include<qscopedpointer.h>
#include<hbmessagebox.h>
#include<hbaction.h>
#include <HbParameterLengthLimiter>

#include "hsdeletecollectionstate.h"
#include "t_hsmenuworkerstateplugin.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenuevent.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::DeleteCollectionState_construction1()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsDeleteCollectionState del1(&parentState1);
        QString s1 = del1.objectName();
        QVERIFY(s1 == tr("testName1/DeleteCollectionState"));

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
void MenuStatesTest::DeleteCollectionState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState;
        HsDeleteCollectionState del(&parentState);

        //test event
        QVariantMap params;
        const int testId = 1000;
        params.insert(Hs::itemIdKey, testId);
        QScopedPointer<HsMenuEvent> event(new HsMenuEvent(HsMenuEvent::DeleteCollection, params));

        del.onEntry(event.data());
        checkDialogController();

        QCOMPARE(del.mItemId, testId);

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
void MenuStatesTest::DeleteCollectionState_deleteCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QScopedPointer<QStateMachine> machine(new QStateMachine(0));

        QState parentState;
        parentState.setObjectName(tr("testName"));
        HsDeleteCollectionState del(&parentState);

        machine->addState(&del);
        machine->setInitialState(&del);

        machine->start();

        const int newCollId = HsMenuService::createCollection("DeleteColl_test");

        del.mItemId = newCollId;

        del.deleteCollection();

        const QString nonExistingCollName = HsMenuService::getName(newCollId);

        QCOMPARE(nonExistingCollName, QString());
        machine->stop();

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
