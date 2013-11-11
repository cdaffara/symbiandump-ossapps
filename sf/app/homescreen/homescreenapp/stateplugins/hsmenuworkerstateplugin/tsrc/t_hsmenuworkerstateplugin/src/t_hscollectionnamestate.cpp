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

#include <hbaction>
#include "hscollectionnamedialog.h"
#include "hscollectionnamestate.h"
#include "hsmenuevent.h"
#include "t_hsmenuworkerstateplugin.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::CollectionNameState_construction1()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsCollectionNameState rename1(&parentState1);
        QString s1 = rename1.objectName();
        QVERIFY(s1 == tr("testName1/collectionnamestate"));

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
void MenuStatesTest::CollectionNameState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        HsCollectionNameState rename1(&parentState1);

        // prepare input event
        QVariantMap params;
        params.insert(Hs::itemIdKey, 1000);
        QScopedPointer<QEvent> event (new HsMenuEvent(
                    HsMenuEvent::RenameCollection, params));

        rename1.onEntry(event.data());

        QVERIFY(rename1.mCollectionNameDialog);
        QVERIFY(rename1.mCollectionNameDialog->testAttribute(Qt::WA_DeleteOnClose));

        // disconnect to check if connection was done
        bool result = disconnect(rename1.mCollectionNameDialog, SIGNAL(finished(HbAction*)),
                   &rename1, SLOT(dialogFinished(HbAction*)));
        QVERIFY (result);

        rename1.mCollectionNameDialog->close();

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
void MenuStatesTest::CollectionNameState_dialogFinished()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsCollectionNameState rename1(&parentState1);
        QString s1 = rename1.objectName();
        QVERIFY(s1 == tr("testName1/collectionnamestate"));


        rename1.mCollectionNameDialog = new HsCollectionNameDialog();
        rename1.dialogFinished(qobject_cast<HbAction*>(rename1.mCollectionNameDialog->actions().value(1)));

        QVERIFY(rename1.mCollectionNameDialog == NULL);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

