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
* Description: LocationPickerSearchView implementation
*
*/

#include <HbListViewItem>
#include <QStandardItemModel>
#include <HbSearchPanel>
#include <HbListView>
#include <HbTextItem>
#include <HbDocumentLoader>
#include <QGraphicsLinearLayout>
#include <HbMenu>
#include <HbAction>

#include "locationpickerproxymodel.h"
#include "locationpickersearchview.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------
// LocationPickerSearchView::LocationPickerSearchView()
// ----------------------------------------------------
LocationPickerSearchView::LocationPickerSearchView( HbDocumentLoader &aLoader )
    :mProxyModel(NULL),
    mModel(NULL),
    mListView(NULL),
    mSearchPanel(NULL),
    mEmptyLabel(NULL),
    mVerticalLayout(NULL),
    mDocumentLoader(aLoader),
	mLongPressMenu(NULL),
	mSelectAction(NULL),
	mBackAction(NULL)
{
    
    // create back action
    mBackAction = new HbAction(Hb::BackNaviAction);
    // add back key action
    setNavigationAction(mBackAction);
    //connect to slots
    connect(mBackAction, SIGNAL(triggered()), this,SLOT(backTriggered())); 

}
// ----------------------------------------------------
// LocationPickerSearchView::~LocationPickerSearchView()
// ----------------------------------------------------
LocationPickerSearchView::~LocationPickerSearchView()
{
    delete mProxyModel;
    delete mEmptyLabel;
    delete mBackAction;
}

// ----------------------------------------------------
// LocationPickerSearchView::init()
// ----------------------------------------------------
void LocationPickerSearchView::init( QStandardItemModel *aModel )
{   
    mModel = aModel;
    //get listview from docml
    mListView = qobject_cast<HbListView*>(
            mDocumentLoader.findObject(QString("SearchListView")));
    if(!mListView)
    {
        qFatal("Error Reading Docml");   
    }
    //get search panel from docml
    mSearchPanel = qobject_cast<HbSearchPanel*>(
            mDocumentLoader.findObject(QString("searchPanel")));
    //conect to respective slots
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated
    (const QModelIndex &)));
    connect(mListView,SIGNAL(longPressed(HbAbstractViewItem*, const QPointF &)),this,
                SLOT(launchPopUpMenu(HbAbstractViewItem*, const QPointF &)));
    connect(mSearchPanel, SIGNAL(exitClicked()),this, SLOT(handleExit()));
    connect(mSearchPanel,SIGNAL(criteriaChanged(QString)),this,SLOT(doSearch(QString)));
    
    //Set graphics size for the list items.
    HbListViewItem *hbListItem = new HbListViewItem();
    hbListItem->setGraphicsSize(HbListViewItem::Thumbnail);
    mListView->setItemPrototype( hbListItem );

    // Create the proxy model.
    mProxyModel = new LocationPickerProxyModel();
    mProxyModel->setSourceModel(mModel);
    mListView->setModel(mProxyModel);

    // set filter properties
    mProxyModel->setDynamicSortFilter(TRUE);
    mProxyModel->setSortRole(Qt::DisplayRole);
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    // sort 
    mProxyModel->sort(0, Qt::AscendingOrder);
}

// ----------------------------------------------------
// LocationPickerSearchView::handleExit()
// ----------------------------------------------------
void LocationPickerSearchView::handleExit()
{   
    //emit signal to switch the current view
    emit switchView();
}

// ----------------------------------------------------
// LocationPickerSearchView::doSearch()
// ----------------------------------------------------
void LocationPickerSearchView::doSearch( QString aCriteria )
{
    // use the string to search
    mProxyModel->filterParameterChanged(aCriteria);
    mProxyModel->setFilterFixedString(aCriteria);
    //if no entries presentdisplay empty text item
    if (!mProxyModel->rowCount() )
    {
        if(!mEmptyLabel)
        {    
            QGraphicsWidget *widget = NULL;
            widget = mDocumentLoader.findWidget(QString("container"));
            mVerticalLayout = static_cast<QGraphicsLinearLayout*>(widget->layout());
            if(!widget || !mVerticalLayout)
            {
                qFatal("Error Reading Docml"); 
            }
            mVerticalLayout->removeItem(mListView);
            mListView->setVisible(false);
            mEmptyLabel = new HbTextItem(hbTrId("txt_lint_list_no_results"));
            mEmptyLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            mEmptyLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
            mEmptyLabel->setAlignment(Qt::AlignCenter);
            mVerticalLayout->insertItem(0, mEmptyLabel);
            mEmptyLabel->setVisible(true);
        }
    }
    //else display the result
    else if (mEmptyLabel)
    {   
        mVerticalLayout->removeItem(mEmptyLabel);
        mEmptyLabel->setVisible(false);
        delete mEmptyLabel;
        mEmptyLabel=NULL;
        mVerticalLayout->insertItem(0, mListView);
        mListView->setVisible(true);
    }
}

// ----------------------------------------------------
// LocationPickerSearchView::handleActivated()
// ----------------------------------------------------
void LocationPickerSearchView::handleActivated( const QModelIndex &aIndex )
{
    QModelIndex index = mProxyModel->mapToSource(aIndex);
    quint32 lm = 0;
    getData( index, lm );
    //emit item is selectedsignal
    emit selectItem( lm );
}

// ----------------------------------------------------------------------------
// LocationPickerSearchView::getData()
// ----------------------------------------------------------------------------

void LocationPickerSearchView::getData( QModelIndex aIndex, quint32& aValue )
{
    QStandardItem* item = mModel->item( aIndex.row(), aIndex.column() );
    QVariant var = item->data( Qt::UserRole );
    aValue = var.toUInt();
}

// -----------------------------------------------------------------------------
// LocationPickerSearchView::launchPopUpMenu()
// -----------------------------------------------------------------------------
void LocationPickerSearchView::launchPopUpMenu(HbAbstractViewItem *aItem, const QPointF &aPoint)
{
    mLongPressMenu = new HbMenu();
    mLongPressMenu->setTimeout(HbMenu::NoTimeout);
    mSelectAction  = mLongPressMenu->addAction(hbTrId("txt_lint_menu_select"));
    mIndex = aItem->modelIndex();
    connect(mSelectAction, SIGNAL(triggered()),this, SLOT(handleLongPress()));
    mLongPressMenu->setPreferredPos(aPoint);
    connect(mLongPressMenu,SIGNAL(aboutToClose ()),this,SLOT(deleteMenu()));
    mLongPressMenu->open();
}

// -----------------------------------------------------------------------------
// LocationPickerSearchView::handleLongPress()
// -----------------------------------------------------------------------------
void LocationPickerSearchView::handleLongPress()
{
    handleActivated(mIndex);
}

// -----------------------------------------------------------------------------
// LocationPickerSearchView::deleteMenu()
// -----------------------------------------------------------------------------
void LocationPickerSearchView::deleteMenu()
{
    mLongPressMenu->deleteLater();
    mLongPressMenu = NULL;
    mSelectAction->deleteLater();
    mSelectAction = NULL;
}

// ----------------------------------------------------------------------------
// LocationPickerSearchView::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerSearchView::backTriggered()
{
    
   //complete the service
    emit completeService();
    
    
}
