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
#include <QTest>
#include <qstandarditemmodel.h>
#include <hbaction.h>
#include <hblistview.h>

#include "t_hsmenuworkerstateplugin.h"
#include "hsaddappstocollectionstate.h"
#include "hsmenuworkerstate.h"
#include "hsaddtohomescreenstate.h"
#include "hsappschecklist.h"
#include "hscollectionnamedialog.h"
#include "hsapp_defs.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AddAppsToCollectionState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);
        QString s1 = aatcs1.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/addappstocollectionstate"));

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
void MenuStatesTest::AddAppsToCollectionState_save()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);

        aatcs1.mCollectionId = 10;
        aatcs1.save();
        QCOMPARE(aatcs1.mCollectionId, 10);

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
void MenuStatesTest::AddAppsToCollectionState_cleanData()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);

        aatcs1.mCollectionId = 10;
        aatcs1.cleanData();
        QVERIFY(aatcs1.mCollectionId == 0);

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
void MenuStatesTest::AddAppsToCollectionState_setCollectionName()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);

        aatcs1.setCollectionName("test");
        QVERIFY(aatcs1.mCollectionName == "test");

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
void MenuStatesTest::AddAppsToCollectionState_setCollectionId()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);

        aatcs1.setCollectionId(1);
        QVERIFY(aatcs1.mCollectionId == 1);

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
void MenuStatesTest::AddAppsToCollectionState_setAppList()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsAddAppsToCollectionState aatcs1(&parentState1);

        QList<int> appList;
        appList.append(1);
        aatcs1.setAppList(appList);
        QVERIFY(aatcs1.mAppList.count() == 1);

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
void MenuStatesTest::AddAppsToCollectionState_showMessageAppsAdded()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState;
        HsAddAppsToCollectionState aatcs(&parentState);

        //aatcs.mMenuService = new MenuService();

        aatcs.onEntry(new HsMenuEvent(HsMenuEvent::OpenCollectionFromAppLibrary, QVariantMap()));
        aatcs.showMessageAppsAdded(10);
        aatcs.stateExited();
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
void MenuStatesTest::AddAppsToCollectionState_selectCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuWorkerState parentState;
    HsAddAppsToCollectionState aatcs(&parentState);
    QVERIFY(aatcs.mListDialog == NULL);

    aatcs.selectCollection();

    QVERIFY(aatcs.mListDialog);
    QVERIFY(aatcs.mListDialog->testAttribute(Qt::WA_DeleteOnClose));
    QCOMPARE(aatcs.mListDialog->dismissPolicy(), HbPopup::NoDismiss);
    // disconnect to check if connection was done
    bool result = disconnect(aatcs.mListDialog, SIGNAL(finished(HbAction*)),
               &aatcs, SLOT(listDialogFinished(HbAction*)));
    QVERIFY (result);

    aatcs.mListDialog->close();

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
void MenuStatesTest::AddAppsToCollectionState_listDialogFinished()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuWorkerState parentState;
    HsAddAppsToCollectionState aatcs(&parentState);
    QVERIFY(aatcs.mListDialog == NULL);

    aatcs.selectCollection();

    QVERIFY(aatcs.mListDialog);
    
    aatcs.listDialogFinished(qobject_cast<HbAction *>(aatcs.mListDialog->actions().value(1))); //Cancel
    
    QVERIFY(aatcs.mListDialog == NULL);

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
void MenuStatesTest::AddAppsToCollectionState_newCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuWorkerState parentState;
    HsAddAppsToCollectionState aatcs(&parentState);
    QVERIFY(aatcs.mEditorDialog == NULL);

    aatcs.newCollection();

    QVERIFY(aatcs.mEditorDialog);
    QVERIFY(aatcs.mEditorDialog->testAttribute(Qt::WA_DeleteOnClose));

    // disconnect to check if connection was done 
    bool result = disconnect(aatcs.mEditorDialog, SIGNAL(finished(HbAction*)),
               &aatcs, SLOT(editorDialogFinished(HbAction*)));
    QVERIFY (result);

    aatcs.mEditorDialog->close();


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
void MenuStatesTest::AddAppsToCollectionState_editorDialogFinished()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuWorkerState parentState;
    HsAddAppsToCollectionState aatcs(&parentState);
    aatcs.mEditorDialog = new HsCollectionNameDialog();
    
    aatcs.editorDialogFinished(qobject_cast<HbAction *>(aatcs.mEditorDialog->actions().value(0)));
    
    QVERIFY(aatcs.mEditorDialog == NULL);

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
void MenuStatesTest::AddAppsToCollectionState_collectionSelected()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsMenuWorkerState parentState;
    HsAddAppsToCollectionState aatcs(&parentState);
    QVERIFY(aatcs.mListDialog == NULL);

    aatcs.selectCollection();

    QVERIFY(aatcs.mListDialog);

    QStandardItemModel *standardItemModel = aatcs.standardItemModel(0);
    QModelIndex index(standardItemModel->indexFromItem(standardItemModel->item(3,0)));
    QVERIFY(index.isValid());
    aatcs.collectionSelected(index);
    
    QVERIFY(aatcs.mListDialog == NULL);
    delete standardItemModel;
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
void MenuStatesTest::AddAppsToCollectionState_appsCheckListState()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsAddAppsToCollectionState addAppsState;
    connect(&addAppsState,SIGNAL(transitToFinalState()),
        this,SLOT(testTransitToStateSlot()));
    QCOMPARE((int)addAppsState.mAppsCheckList,NULL);
    addAppsState.appsCheckListState();
    QVERIFY(addAppsState.mAppsCheckList != NULL);
    QCOMPARE(addAppsState.mAppsCheckList->mSortAttribute,
        addAppsState.mApplicationsSortAttribute);
    mTransitToStateEmited = false;
    addAppsState.mAppsCheckList->mAppsSelectDialog->close();
    QTest::qWait(150);
    QCOMPARE(mTransitToStateEmited,true);
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
void MenuStatesTest::AddAppsToCollectionState_selectApplicationsDone()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
    HsAddAppsToCollectionState addAppsState;
    connect(&addAppsState,SIGNAL(transitToSaveState(const QList<int>&)),
        this,SLOT(testTransitToStateSlot()));
    QList<int> appList;
    appList.append(1222);
    QCOMPARE(addAppsState.mActionType,
        HsAddAppsToCollectionState::NoActionType);
    mTransitToStateEmited = false;
    addAppsState.selectApplicationsDone(appList);
    QTest::qWait(100);
    QCOMPARE(mTransitToStateEmited,true);
    
    addAppsState.mActionType =
        HsAddAppsToCollectionState::ViaAllViewOptionMenuType;
    disconnect(&addAppsState,SIGNAL(transitToFinalState()),
            this,SLOT(testTransitToStateSlot()));
    connect(&addAppsState,SIGNAL(transitToSelectCollectionState()),
            this,SLOT(testTransitToStateSlot()));
    mTransitToStateEmited = false;
    addAppsState.selectApplicationsDone(appList);
    QTest::qWait(100);
    QCOMPARE(mTransitToStateEmited,true);
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
void MenuStatesTest::AppsCheckList_showAppsCheckboxList()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsAppsCheckList checkList;
        Hs::HsSortAttribute sortOrder = Hs::NoHsSortAttribute;
        mCommitEmited = false;
        connect(&checkList,SIGNAL(commit(const QList<int>&)),
                this,SLOT(testCommitSlot(const QList<int>&)));
        checkList.showAppsCheckboxList(sortOrder);
        QVERIFY(checkList.mModel != NULL);
        QVERIFY(checkList.mActionConfirm != NULL);
        QVERIFY(checkList.mListView != NULL);
        QVERIFY(checkList.mAppsSelectDialog != NULL);
        QCOMPARE(mCommitEmited,false);
        checkList.mAppsSelectDialog->close();
        QTest::qWait(100);
        QCOMPARE((int)checkList.mAppsSelectDialog,NULL);
        QCOMPARE(mCommitEmited,true);
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
void MenuStatesTest::testCommitSlot(const QList<int> &appList)
{
    if(appList.isEmpty()){
        mCommitEmited = true;
    } else {
        mCommitEmited = false;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::AppsCheckList_getItemsList()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QStandardItemModel *standardModel = new QStandardItemModel(2,1);
        QStandardItem *standardItem = new QStandardItem();
        standardItem->setData(2, CaItemModel::IdRole);
        standardItem->setData(QString("bbb"), Qt::DisplayRole);
        QList<QStandardItem *> items;
        items << standardItem;
        standardModel->insertRow(0, items);
        {
            QStandardItem *standardItem = new QStandardItem();
            standardItem->setData(3, CaItemModel::IdRole);
            //standardItem->setData(QString("aaa"), Qt::DisplayRole);
            QList<QVariant> text;
            text << QString("aaa");
            text << QString("ccc");
            standardItem->setData(text, Qt::DisplayRole);

            QList<QStandardItem *> items;
            items << standardItem;
            standardModel->insertRow(0, items);
        }

        QModelIndexList modelIndexList;
        modelIndexList.append(standardModel->index(0,0));
        modelIndexList.append(standardModel->index(1,0));

        HsMenuWorkerState parentState;

        HsAppsCheckList checkList;
        QList<int> list = checkList.getSortedItemsList(modelIndexList);
        QCOMPARE(list.count(), 2);
        QCOMPARE(list[0], 3);
        QCOMPARE(list[1], 2);
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
void MenuStatesTest::AppsCheckList_setSortOrder()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsAppsCheckList checkList;
        QCOMPARE(checkList.mSortAttribute, Hs::AscendingNameHsSortAttribute);
        checkList.setSortOrder(Hs::DescendingNameHsSortAttribute);
        QCOMPARE(checkList.mSortAttribute, Hs::DescendingNameHsSortAttribute);
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
void MenuStatesTest::AppsCheckList_selectedItemsChanged()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsAppsCheckList checkList;
        checkList.mModel =
                HsMenuService::getAllApplicationsModel(Hs::NoHsSortAttribute);
        checkList.constructControls();
        QVERIFY(checkList.mActionConfirm != NULL);
        QVERIFY(checkList.mListView != NULL);
        QVERIFY(checkList.mAppsSelectDialog != NULL);
        checkList.selectedItemsChanged();
        QCOMPARE(checkList.mActionConfirm->isEnabled(),false);
        QItemSelectionModel *model = checkList.mListView->selectionModel();
        model->select(model->model()->index(0,0),QItemSelectionModel::Select);
        QTest::qWait(100);
        checkList.selectedItemsChanged();
        QCOMPARE(checkList.mActionConfirm->isEnabled(),true);
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
void MenuStatesTest::testTransitToStateSlot()
{
    mTransitToStateEmited = true;
}

