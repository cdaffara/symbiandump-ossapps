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

#include <QStateMachine>
#include <hblistwidget.h>
#include <qabstractitemmodel.h>
#include <hblabel.h>
#include <hbdialog.h>
#include <HbAction>

#include "t_hsmenuworkerstateplugin.h"
#include "hsmenuworkerstate.h"
#include "hsarrangestate.h"
#include "hsmenuitemmodel.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::ArrangeState_FulfillEntriesList()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState;

        HsArrangeState arrangeState(&parentState);
        QString s1 = arrangeState.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/arrangestate"));

        arrangeState.mEntriesList = new HbListWidget();
        arrangeState.mItemModel = HsMenuService::getAllCollectionsModel();

        arrangeState.mTopItemId = arrangeState.mItemModel->index(
                                      arrangeState.mItemModel->rowCount() / 2).data(
                                      CaItemModel::IdRole).toInt();
        arrangeState.fulfillEntriesList(*arrangeState.mEntriesList);

        QVERIFY(arrangeState.mEntriesList->count() > 0);
        QVERIFY(arrangeState.mItemModel->rowCount() > 0);

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
void MenuStatesTest::ArrangeState_Save()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState;

        HsArrangeState arrangeState(&parentState);
        QString s1 = arrangeState.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/arrangestate"));

        arrangeState.mEntriesList = new HbListWidget();
        arrangeState.mItemModel = HsMenuService::getAllCollectionsModel();

        arrangeState.mTopItemId = arrangeState.mItemModel->index(
                                      arrangeState.mItemModel->rowCount() / 2).data(
                                      CaItemModel::IdRole).toInt();
        arrangeState.fulfillEntriesList(*arrangeState.mEntriesList);

        QVERIFY(arrangeState.mEntriesList->count() > 0);
        QVERIFY(arrangeState.mTopModelIndex != QModelIndex());
        QVERIFY(arrangeState.mItemModel->rowCount() > 0);

        // arrange items
        HbListWidgetItem *itemFirst = arrangeState.mEntriesList->takeItem(0);
        HbListWidgetItem *itemLast = arrangeState.mEntriesList->takeItem(
                                         arrangeState.mEntriesList->count()-1);

        arrangeState.mEntriesList->insertItem(0, itemLast);
        arrangeState.mEntriesList->addItem(itemFirst);
        HbLabel *label = new HbLabel(QString("arrange"));
        arrangeState.mDialog = new HbDialog();
        arrangeState.mDialog->setHeadingWidget(label);
        arrangeState.mDialog->setContentWidget(arrangeState.mEntriesList);
        arrangeState.save(*arrangeState.mEntriesList);

        QVERIFY(arrangeState.mCollItemIdList != arrangeState.mArrangedCollItemIdList);
        QCOMPARE(arrangeState.mCollItemIdList.at(0),
                 arrangeState.mArrangedCollItemIdList.at(arrangeState.mArrangedCollItemIdList.count()-1));
        QCOMPARE(arrangeState.mCollItemIdList.at(arrangeState.mCollItemIdList.count()-1),
                 arrangeState.mArrangedCollItemIdList.at(0));

        arrangeState.stateExited();

        QVERIFY(!arrangeState.mCollItemIdList.count());
        QVERIFY(!arrangeState.mArrangedCollItemIdList.count());

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
void MenuStatesTest::ArrangeState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState;
        HsArrangeState arrangeState (&parentState);

        QVariantMap params;
        params.insert(Hs::itemIdKey, 1);
        QScopedPointer<HsMenuEvent> event (new HsMenuEvent(HsMenuEvent::ArrangeCollection, params));

        arrangeState.onEntry(event.data());

        QVERIFY(arrangeState.mEntriesList);
        QVERIFY(arrangeState.mDialog);
        QVERIFY(arrangeState.mEntriesList->arrangeMode());

        QVERIFY(arrangeState.mDialog);
        QVERIFY(arrangeState.mDialog->testAttribute(Qt::WA_DeleteOnClose));

        // disconnect to check if connection was done
        bool result = disconnect(arrangeState.mDialog, SIGNAL(finished(HbAction*)),
                   &arrangeState, SLOT(arrangeDialogFinished(HbAction*)));
        QVERIFY(result);

        arrangeState.mDialog->close();
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
void MenuStatesTest::ArrangeState_arrangeDialogFinished()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState;

        HsArrangeState arrangeState(&parentState);
        QString s1 = arrangeState.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/arrangestate"));


        arrangeState.mEntriesList = new HbListWidget();
        arrangeState.mItemModel = HsMenuService::getAllCollectionsModel();
        arrangeState.mTopItemId = arrangeState.mItemModel->index(
                                      arrangeState.mItemModel->rowCount() / 2).data(
                                      CaItemModel::IdRole).toInt();
        arrangeState.fulfillEntriesList(*arrangeState.mEntriesList);

        arrangeState.mDialog = new HbDialog();
        arrangeState.mDialog->clearActions();
        HbAction *action  = new HbAction("Primary", arrangeState.mDialog);
        arrangeState.mDialog->addAction(action);

        arrangeState.arrangeDialogFinished(action);

        QVERIFY(arrangeState.mEntriesList->arrangeMode() == false);

        arrangeState.stateExited();
        QVERIFY(arrangeState.mDialog == NULL);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
