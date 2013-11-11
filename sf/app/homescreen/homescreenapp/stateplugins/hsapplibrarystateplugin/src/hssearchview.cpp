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
 * Description: hssearchview.cpp
 *
 */

#include <QApplication>
#include <HbGroupBox>
#include <HbLineEdit>
#include <HbListView>
#include <HbAbstractViewItem>
#include <HbView>
#include <HbSearchPanel>
#include <HbScrollBar>
#include <HbIndexFeedback>

#include <caitemmodel.h>

#include "hslistviewitem.h"
#include "hsmainwindow.h"
#include "hssearchview.h"

/*!
 * \fn void activated(const QModelIndex &index)
 * This signal is emitted when item is tapped.
 * \param index of an item that was tapped.
 */

 /*!
 * \fn void longPressed(HbAbstractViewItem *item, const QPointF &coords)
 * This singal is emitted when view is long pressed.
 * \param item View item that was long pressed.
 * \param coords View coordinates of the long press.
 */
 
 /*!
  * \fn void searchComplete(const QModelIndex &firstMatching)
  * This signal is emitted when search action has been finished 
  * and search view is about to quit.
  * \param firstMatching of first item in search filtered view.
  */
    
    
/*!
  Set up proxy model for search.
  \param builder Retrieves UI widgets.
  \param stateContext Identifies the state where search starts from.
  \param mainWindow Enables switching between originating and search view.
 */
HsSearchView::HsSearchView(HsMenuViewBuilder &builder,
        HsStateContext stateContext, HsMainWindow &mainWindow) :
    mProxyModel(new HsSearchFilterProxyModel(this)), mSearchView(NULL),
    mSearchListView(NULL), mSearchPanel(NULL), mStateContext(stateContext),
    mBuilder(builder), mMainWindow(mainWindow), mListView(NULL),
    mVkbHost(NULL), mSearchViewBuilder(), mEmptyResultText(true)
{
    mProxyModel->setFilterRole(CaItemModel::TextRole);
    mProxyModel->setFilterKeyColumn(0);
    mProxyModel->setSortRole(CaItemModel::TextRole);
    mProxyModel->setDynamicSortFilter(true);
}

/*!
  Empty.
 */
HsSearchView::~HsSearchView()
{
}

/*!
    Makes the UI to show or hide view search panel.
    When search panel is shown the view toolbar and status pane
    are hidden until search panel is hidden.
    \param visible When true search panel will be shown,
     otherwise it will be hidden.
 */
void HsSearchView::setSearchPanelVisible(bool visible)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::setSearchPanelVisible");

    if (visible && !isActive()) {
        searchBegins();
        connectSearchItemViewsSignals();
        connectSearchPanelSignals();
    } else if (isActive()) {
        disconnectSearchPanelSignals();
        disconnectSearchItemViewsSignals();
        searchFinished();
    }
    HSMENUTEST_FUNC_EXIT("HsMenuView::setSearchPanelVisible");
}

/*!
    \param view List view to operate on.
    \return Index of F=first visible item of \a view if any
    or default QModelIndex otherwise.
 */
QModelIndex HsSearchView::firstVisibleItemIndex(const HbListView *view) const
{
    const QList<HbAbstractViewItem *> array = view->visibleItems();
    if (array.count()) {
        return array[0]->modelIndex();
    }
    return QModelIndex();
}

/*!
 Displays search panel with view representing search results.
 */
void HsSearchView::searchBegins()
{
    HSMENUTEST_FUNC_ENTRY("HsSearchView::searchBegins");

    setOriginatingContext();

    mListView = mBuilder.currentListView();
    mSearchViewInitialIndex = firstVisibleItemIndex(mListView);

    mProxyModel->invalidate();
    mProxyModel->setSourceModel(mListView->model());
    mCriteria = QString("");
    mProxyModel->setFilterString(mCriteria);

    mSearchView = mSearchViewBuilder.searchView();
    mSearchListView = mSearchViewBuilder.searchListView();
    mSearchPanel = mSearchViewBuilder.searchPanel();
    initSearchPanel(*mSearchPanel);
    mVkbHost.reset(new HbShrinkingVkbHost(mSearchView));

    if (mBuilder.currentViewLabel()) {
        mSearchViewBuilder.searchViewLabel()->setHeading(
                mBuilder.currentViewLabel()->heading());
    }
    
    if (mStateContext == HsCollectionContext){
    	mSearchViewBuilder.setSearchLabledContext();
    }
    
    mSearchView->hideItems(Hb::AllItems);
    HsListViewItem *item = new HsListViewItem();
    item->setTextFormat(Qt::RichText);

    mSearchListView->setModel(mProxyModel, item);
    mSearchListView->verticalScrollBar()->setInteractive(true);
    if (mStateContext == HsAllAppsContext) {
        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mSearchListView);
        indexFeedback->setIndexFeedbackPolicy(
                HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mSearchListView);
    }
    
    mMainWindow.setCurrentView(mSearchView);

    openVkb();

    mSearchListView->scrollTo(
        mProxyModel->mapFromSource(mSearchViewInitialIndex),
        HbAbstractItemView::PositionAtTop);

    setNoResultsVisibility();
    HSMENUTEST_FUNC_EXIT("HsSearchView::searchBegins");
}

/*!
 Handles button visibility.
 \param visibility indicates whether show or not to show 'empty' label.
 */
void HsSearchView::setNoResultsVisibility()
{
    if (mProxyModel->rowCount() && mEmptyResultText) {
        mSearchViewBuilder.loadViewListSection();
        mEmptyResultText = false;
    } else if (!mProxyModel->rowCount() && !mEmptyResultText) {
        mSearchViewBuilder.loadViewEmptySection();
        mEmptyResultText = true;
    }
}

/*
 Connects \a activated and \a longPressed signals coming from search list
 view to emit corresponding signal of this object with translated model index.
*/
void HsSearchView::connectSearchItemViewsSignals()
{
    connect(mSearchListView, SIGNAL(pressed(QModelIndex)),
                this, SLOT(hideVkb()), Qt::UniqueConnection);
    connect(mSearchListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(activatedProxySlot(QModelIndex)), Qt::UniqueConnection);
    connect(mSearchListView, SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)), 
			    Qt::UniqueConnection);

}

/*!
 Disconnects \a activated and \a longPressed signals coming from list
 view from to emit corresponding signal
 of this object with translated model index.
*/
void HsSearchView::disconnectSearchItemViewsSignals()
{
    disconnect(mSearchListView, SIGNAL(pressed(QModelIndex)),
            this, SLOT(hideVkb()));
    disconnect(mSearchListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(activatedProxySlot(QModelIndex)));
    disconnect(mSearchListView,
            SIGNAL(longPressed(HbAbstractViewItem *, QPointF)),
            this, SLOT(longPressedProxySlot(HbAbstractViewItem *, QPointF)));
}


/*!
 Makes search panel invisible.
 Equivalent to \a setSearchPanelVisible(false).
 */
void HsSearchView::hideSearchPanel()
{
    HSMENUTEST_FUNC_ENTRY("HsMenuView::hideSearchPanel");
    setSearchPanelVisible(false);
    HSMENUTEST_FUNC_EXIT("HsMenuView::hideSearchPanel");
}

/*!
 Connects signals \a exitClicked and \a criteriaChanged emitted
 by search panel with handling slots of the object or its members.
*/
void HsSearchView::connectSearchPanelSignals()
{
    connect(mProxyModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(setNoResultsVisibility()), Qt::UniqueConnection);
    connect(mProxyModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this, SLOT(setNoResultsVisibility()));
    connect(mProxyModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            this, SLOT(setNoResultsVisibility()));
    connect(mSearchPanel, SIGNAL(exitClicked()),
            this, SLOT(hideSearchPanel()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)),
            this, SLOT(findItem(QString)), Qt::UniqueConnection);
}

/*!
 Disconnects signals \a exitClicked and \a criteriaChanged emitted
 by search panel from handling slots of the object or its members
 Scrolls view to state before connections.
*/
void HsSearchView::disconnectSearchPanelSignals()
{
    mProxyModel->disconnect(this);
    mSearchPanel->disconnect(this);
}

/*!
  Scrolls originating view to first item matching search criteria.
 */
void HsSearchView::searchFinished()
{
    HSMENUTEST_FUNC_ENTRY("HsSearchView::searchFinished");

    mIndexToScrollAfterSearchDone = firstVisibleItemIndex(mSearchListView);

    // emiting searchComplete must be done
    // after this->isActive() returns false
    mSearchListView = NULL;
    mSearchPanel = NULL;
    
    emit searchComplete(mProxyModel->mapToSource(
            mIndexToScrollAfterSearchDone));

    HbVkbHost::detachHost(mSearchView);
    mVkbHost.reset(NULL);

    mProxyModel->setSourceModel(NULL);
    HSMENUTEST_FUNC_EXIT("HsSearchView::searchFinished");
}

/*!
 Looks up for item and if found scrolls to it.
 \param criteriaStr The item name to find.
 */
void HsSearchView::findItem(QString criteriaStr)
{
    qDebug() << "HsSearchView::findItem: " + criteriaStr;
    HSMENUTEST_FUNC_ENTRY("HsSearchView::findItem");

    if (criteriaStr == mCriteria) 
        return;

    mCriteria = criteriaStr;
    mProxyModel->setFilterString(criteriaStr);
    if(criteriaStr.isEmpty())
        mSearchListView->scrollTo(
                mProxyModel->index(0, 0), HbAbstractItemView::PositionAtTop);

    HSMENUTEST_FUNC_EXIT("HsSearchView::findItem");
}

/*!
 Slot used to translate activated signal from proxy model to normal model.
 \param index Represents an item activated in search list view.
 */
void HsSearchView::activatedProxySlot(const QModelIndex &index)
{
    emit activated(mProxyModel->mapToSource(index));
}

/*!
 Slot used to forward 'long pressed' signal with item description transladed
 from search view context to context of the view search was requested from.
  \param item Long pressed item.
  \param coords Coordinates of the long press action.
 */
void HsSearchView::longPressedProxySlot(
        HbAbstractViewItem *item, const QPointF &coords)
{
    /*
    this is a kind of hack, introduced for reasons:
    item object should be reusable later, but orbit (or qt) prevents setting
    its index model to previous state
    */
    mSearchViewLongPressedIndex = mProxyModel->mapToSource(
            item->modelIndex());
    QScopedPointer<HbAbstractViewItem> itemNew(item->createItem());
    itemNew->setModelIndex(mSearchViewLongPressedIndex);
    emit longPressed(itemNew.data(), coords);
}

/*!
  Sets up search panel.

 \param searchPanel Search panel to initialize.
 */
void HsSearchView::initSearchPanel(HbSearchPanel &searchPanel)
{
    HbLineEdit *const lineEdit(searchPanelLineEdit(searchPanel));

    lineEdit->setText("");
    lineEdit->setFocus();
    lineEdit->setInputMethodHints(
            Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
}

/*!
 \param searchPanel Search panel to operate on.
 \return Line edit of the searchPanel on success, NULL otherwise.
 */
HbLineEdit *HsSearchView::searchPanelLineEdit(
        HbSearchPanel &searchPanel) const
{
    HSMENUTEST_FUNC_ENTRY("HsSearchView::searchPanelLineEdit");

    HbLineEdit *result(0);
    foreach(QGraphicsItem *obj, searchPanel.childItems()) {
        QGraphicsWidget *const widget = static_cast<QGraphicsWidget *>(obj);
        if (widget != NULL) {
            HbLineEdit *const lineEdit = qobject_cast<HbLineEdit *>(widget);
            if (lineEdit != NULL) {
                result = lineEdit;
                break;
            }
        }
    }
    HSMENUTEST_FUNC_EXIT("HsSearchView::searchPanelLineEdit");

    return result;
}

/*!
  Sets the builder context to the one determined by \a mStateContext and
  \a HsItemViewContext.
 */
void HsSearchView::setOriginatingContext()
{
    mBuilder.setStateContext(mStateContext);
    mBuilder.setOperationalContext(HsItemViewContext);
}

/*!
  \retval true when search view is actually responsible for view management, 
  \a false otherwise.
 */
bool HsSearchView::isActive() const
{
    return mSearchListView != NULL;
}

/*!
  Slot to close virtual keyboard.
 */
void HsSearchView::hideVkb()
{
    sendEvent(QEvent::CloseSoftwareInputPanel);
}

/*!
 Makes Vkb open provided there is an editor visible in the graphics scene.
 */
void HsSearchView::openVkb()
{
    sendEvent(QEvent::RequestSoftwareInputPanel);
}

/*!
  Sends QEvent.
  \param eventType Identifies event to be sent.
 */
void HsSearchView::sendEvent(const QEvent::Type eventType)
{
    QInputContext *const ic = qApp->inputContext();
    if (ic != NULL) {
        QScopedPointer<QEvent> event(
            new QEvent(eventType));

        ic->filterEvent(event.data());
    }

}


