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

#include <HbAction>
#include <HbInstance>
#include <HbDialog>
#include <HbIndexFeedback>
#include <HbListView>
#include <HbScrollBar>
#include <HbDocumentLoader>
#include <hsmenuservice.h>
#include <hsmenuitemmodel.h>

#include "hsappschecklist.h"

/*!
 \class HsAppsCheckList
 \ingroup group_hsworkerstateplugin
 \brief Application Library State.
 State responsible for selecting a group of applications to be added to
 a collection.
 */

/*!
 \fn void HsAppsCheckList::commit(const QList<int> &appList);
 Signal emitted when applications are selected.
 \param appList application list.
 */

/*!
 \var HsAppsCheckList::mView
 View. Owned.
 */

/*!
 \var HsAppsCheckList::mPreviousView
 Previous view. Not owned.
 */

/*!
 \var HsAppsCheckList::mActionConfirm
 Confirm action. Owned.
 */

/*!
 \var HsAppsCheckList::mListView
 List view. Owned.
 */

/*!
 \var HsAppsCheckList::mModel
 Standard Item Model for the List.
 Own.
 */

/*!
 \var HsAppsCheckList::mSortAttribute
 Sort order for applications.
 */

/*!
 \var HsAppsCheckList::mLoadedObjects
 List of objects loaded by document loader.
 Content owned.
 */

/*!
 Constructor
 \retval void
 */
HsAppsCheckList::HsAppsCheckList() :
    mAppsSelectDialog(0), mActionConfirm(0), mListView(0), mModel(0),
    mSortAttribute(Hs::AscendingNameHsSortAttribute)
{
}

/*!
 Destructor
 */
HsAppsCheckList::~HsAppsCheckList()
{
    cleanUp();
}

/*!
 Cleans data
 \retval void
 */
void HsAppsCheckList::cleanUp()
{
    //clean up
    if (mAppsSelectDialog) {
        mAppsSelectDialog->close();
    }

    mSortAttribute = Hs::AscendingNameHsSortAttribute;
}

/*!
 Sets sort order for applications.
 \param sortAttribute sort order.
 */
void HsAppsCheckList::setSortOrder(Hs::HsSortAttribute sortOrder)
{
    mSortAttribute = sortOrder;
}

/*!
 Shows check box list with all application.
 \param sortAttribute order to sort applications.
 */
void HsAppsCheckList::showAppsCheckboxList(Hs::HsSortAttribute sortOrder)
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::showAppsCheckboxList");
    if (!mModel) {
        mModel = HsMenuService::getAllApplicationsModel(sortOrder);
    }
    // fills model with data
    // constucts controls such as checklist
    constructControls();
    
    if (mAppsSelectDialog) {
        mAppsSelectDialog->setTimeout(HbPopup::NoTimeout);
        mAppsSelectDialog->setAttribute(Qt::WA_DeleteOnClose);
        mAppsSelectDialog->open(this,
            SLOT(appsSelectionDialogFinished(HbAction*)));
    }

    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::showAppsCheckboxList");
}

/*!
 Construct contrlos.
 */
void HsAppsCheckList::constructControls()
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::constructControls");

    bool loadStatusOk = false;
    HbDocumentLoader loader;
    loader.load(HS_APP_CHECK_LIST_LAYOUT, &loadStatusOk);

    Q_ASSERT_X(loadStatusOk,
                HS_APP_CHECK_LIST_LAYOUT,
               "Error while loading docml file.");

    static const QString APPS_SELECTION_DIALOG("applications_selection_dialog");
    mAppsSelectDialog =
        qobject_cast<HbDialog *>(loader.findWidget(APPS_SELECTION_DIALOG));

    if (mAppsSelectDialog) {
        static const QString LIST_VIEW_WIDGET("listView");
        mListView = qobject_cast<HbListView *> (loader.findWidget(
            LIST_VIEW_WIDGET));
        mModel->setParent(mAppsSelectDialog);
        mListView->setModel(mModel);
        mListView->verticalScrollBar()->setInteractive(true);
        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mListView);
        indexFeedback->setIndexFeedbackPolicy(
                HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mListView);
        
        connect(mListView,SIGNAL(activated(const QModelIndex&) ),this
                ,SLOT(selectedItemsChanged()));
        connect(mModel,SIGNAL(rowsRemoved(const QModelIndex&, int,int)),
        		this, SLOT(selectedItemsChanged()));

        // add confirm action
        static const QString HS_APPS_SELECTION_DIALOG_CONFIRMATION_ACTION
                ("aps_dialog_softkey_2_left");
        mActionConfirm = qobject_cast<HbAction*>(loader.findObject(
                        HS_APPS_SELECTION_DIALOG_CONFIRMATION_ACTION));
        mActionConfirm->setEnabled(false);
        mAppsSelectDialog->actions()[0]->setParent(mAppsSelectDialog);
        mAppsSelectDialog->actions()[1]->setParent(mAppsSelectDialog);
    }
    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::constructControls");
}

/*!
 Action after closed application selection dialog.
 \param finishedAction chosen action.
 \retval void
 */
void HsAppsCheckList::appsSelectionDialogFinished(HbAction* finishedAction)
{
    HSMENUTEST_FUNC_ENTRY("HsAppsCheckList::appsSelectionDialogFinished");
    QList<int> itemsList;
    if (finishedAction == mActionConfirm) {
        QItemSelectionModel *itemSelectionModel = mListView->selectionModel();
        if (itemSelectionModel) {
            QModelIndexList modelIndexList =
                itemSelectionModel->selectedIndexes();
            itemsList = getSortedItemsList(modelIndexList);
        }
    }
    mAppsSelectDialog = NULL;
    emit commit(itemsList);
    HSMENUTEST_FUNC_EXIT("HsAppsCheckList::appsSelectionDialogFinished");
}

/*!
 Returns list with selected items ids.
 \param modelIndexList list with selected model indexes.
 \return list with items ids.
 */
QList<int> HsAppsCheckList::getSortedItemsList(
    const QModelIndexList &modelIndexList)
{
    QMap<QString, int> itemsMap;

    foreach(QModelIndex index, modelIndexList) {
        int itemId = index.data(CaItemModel::IdRole).toInt();
        QString itemName(index.data(Qt::DisplayRole).toString());
        if (itemName.isNull()) {
            itemName
            = index.data(Qt::DisplayRole).toList()[0].toString();
        }
        itemsMap.insertMulti(itemName.toLower(), itemId);
    }

    QList<int> list = itemsMap.values();
    if (mSortAttribute == Hs::DescendingNameHsSortAttribute) {
        QList<int> reversedList;
        int count = list.count();
        for (int i = 0; i < count; i++) {
            reversedList.append(list.takeLast());
        }
        list = reversedList;
    }
    return list;
}

/*!
 Slot activated then user select or unselect item
 */
void HsAppsCheckList::selectedItemsChanged()
{
    if (mListView->selectionModel()->hasSelection()) {
        mActionConfirm->setEnabled(true);
    } else {
        mActionConfirm->setEnabled(false);
    }
}
