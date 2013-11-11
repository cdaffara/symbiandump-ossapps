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
* Description: LocationPickerView implementation
*
*/

#include "locationpickerview.h"

#include <HbMainWindow>
#include <qstandarditemmodel.h>
#include <HbListView>
#include <HbListViewItem>
#include <HbAction>
#include <HbMenu>
#include <HbToolBar>
#include <HbStyleLoader>
#include <QGraphicsLinearLayout>
#include <HbDialog>

#include "hgwidgetdatamodel.h"
#include "locationpickerproxymodel.h"
#include "locationpickerdatamanager.h"
#include "locationpickercontent.h" 
#include "locationpickercollectionlistcontent.h"
#include "locationpickercollectioncontent.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------
// LocationPickerView::LocationPickerView()
// ----------------------------------------------------
LocationPickerView::LocationPickerView( HbDocumentLoader* aLoader )
    :mDocumentLoader(aLoader),
    mLocationPickerCollectionListContent(NULL),
    mProxyModel(NULL),
    mAllAction(NULL),
    mCollectionAction(NULL),
    mSearchAction(NULL),
    mAscendingAction(NULL),
    mDescendingAction(NULL),
    mListView(NULL),
    mCollectionContent(NULL),
    mViewType(ELocationPickerContent),
    mLinerLayout(NULL),
    mColllabel(NULL),
    mEmptyLabel(NULL),
    mDialog(NULL),
    mMapIconLabel(NULL),
    mTitleLabel(NULL),
    mAddressMiddle(NULL),
    mAddressBottom(NULL),
    mLongPressMenu(NULL),
    mSelectAction(NULL),
    mDetailsAction(NULL),
    mPopulated(false)
{   
    //Create Action Items
    mAllAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("allAction")));
    mCollectionAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("collectionAction")));
    mSearchAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("searchAction")));
    mAscendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("ascendingAction")));
    mDescendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("descendingAction")));
    if( !mAllAction || !mCollectionAction || !mSearchAction ||
    	 !mAscendingAction || !mDescendingAction )
    {
        qFatal("Error Reading Docml");
    }
    // create back action
    mBackAction = new HbAction(Hb::BackNaviAction);
    // add back key action
    setNavigationAction(mBackAction);
    //connect to slots
     connect(mBackAction, SIGNAL(triggered()), this,
            SLOT(backTriggered()));     
    //create list item  
    mListItem = new HbListViewItem();
    //set the graphics size
    mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
}
// ----------------------------------------------------
// LocationPickerView::~LocationPickerView()
// ----------------------------------------------------
LocationPickerView::~LocationPickerView()
{
    delete mListItem;
    delete mBackAction;
    if(mPopulated)
    {
    	mLinerLayout->removeItem(mListView);
    	mLinerLayout->removeItem(mWidget);
    	delete mCollectionContent;
    	delete mLocationPickerCollectionListContent;
    	delete mListView;
    	delete mWidget;
    	delete mHgModel;
    	delete mProxyModel;
    	delete mColllabel;
    }
}

// ----------------------------------------------------------------------------
// LocationPickerView::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerView::backTriggered()
{
    //if current model is collection content, go back to collectionlist content  
    if(mViewType == ELocationPickerCollectionContent)
    {
        mListItem->setGraphicsSize(HbListViewItem::MediumIcon);
		removeDetailsLabel();
        mAscendingAction->setDisabled(true);
        mDescendingAction->setDisabled(true);
        mAllAction->setChecked(false);
        mViewType = ELocationPickerCollectionListContent;
        manageListView();

        clearContentModel();
    }
    else
    {
        //complete the service
        emit completeService();
    } 
    
}


// ----------------------------------------------------
// LocationPickerView::init()
// ----------------------------------------------------
void LocationPickerView::init( Qt::Orientation aOrientation, QStandardItemModel *aModel )
{   
    mPopulated = true;
    // Create Collection List Content
    mLocationPickerCollectionListContent = new LocationPickerCollectionListContent();
    mModel = aModel;        
    // Create Collection List Content
    mLocationPickerCollectionListContent = new LocationPickerCollectionListContent();
    //create proxy model
    mProxyModel = new LocationPickerProxyModel( this  );
    mProxyModel->setSourceModel(aModel);
    // set sort properties
    mProxyModel->setDynamicSortFilter(TRUE);
    mProxyModel->setSortRole(Qt::DisplayRole);
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    // sort in ascending order
    mProxyModel->sort(0, Qt::AscendingOrder); 
    mLinerLayout = static_cast<QGraphicsLinearLayout*>(widget()->layout());
    //Get HbAction items
    mListView = new HbListView();
    mColllabel =  new HbLabel();
    createHurriganesWidget();  
    mWidget->setModel(mHgModel);
    //connect to slots
    connect(mAscendingAction, SIGNAL(triggered()), this, SLOT(sortAscending()));
    connect(mDescendingAction, SIGNAL(triggered()), this,
            SLOT(sortDescending()));
    //connect all action Items to respective slots
    connect(mAllAction, SIGNAL(triggered()), this, SLOT(allTabTriggered()));
    connect(mCollectionAction, SIGNAL(triggered()), this,SLOT(colectionTabTriggered()));
    connect(mSearchAction, SIGNAL(triggered()), this,
            SLOT(searchTabTriggered()));
    // connect the signal of the list activated to a slot.
    connect(mListView, SIGNAL(activated(const QModelIndex &)), 
    this, SLOT(handleActivated(const QModelIndex &)));
    connect(mListView,SIGNAL(longPressed(HbAbstractViewItem*, const QPointF &)),this,
            SLOT(launchPopUpMenu(HbAbstractViewItem*, const QPointF &)));
    //set widget according to orientation
    if(aOrientation == Qt::Horizontal)
    {
        manageHgWidget();
    }
    else
    {
        manageListView();
    }
}


void LocationPickerView::createHurriganesWidget()
{
    //hurrriganes widget
    mHgModel = new HgWidgetDataModel( mProxyModel, this );
    mHgModel->setImageDataType(HgWidgetDataModel::ETypeQImage);
    //create MediaWall Object
    mWidget = new HgMediawall();
    HbIcon defaultIcon(KDummyImage);
    QPainter painter;
    QPixmap defaultImage(defaultIcon.width(),defaultIcon.height());
    painter.begin(&defaultImage);
    defaultIcon.paint(&painter,QRectF(0,0,defaultIcon.width(),defaultIcon.height()));
    painter.end();
    mWidget->setDefaultImage( defaultImage.toImage() );
    HgMediawall* mediawall = qobject_cast<HgMediawall*>( mWidget );
    mediawall->setObjectName("location");
    HbStyleLoader::registerFilePath(":/location.hgmediawall.widgetml");
    HbStyleLoader::registerFilePath(":/location.hgmediawall.css");
    mediawall->enableReflections(false);
    mediawall->setFontSpec(HbFontSpec(HbFontSpec::Primary));
    mediawall->setItemSize(QSize(4,3));
    connect(this->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),mWidget,
            SLOT(orientationChanged(Qt::Orientation)));

    mWidget->setAcceptTouchEvents(true);
    connect(mWidget, SIGNAL(activated(const QModelIndex &)),
    this, SLOT(handleActivated(const QModelIndex &)));
    connect(mWidget, SIGNAL(longPressed(const QModelIndex &, const QPointF &)),this, 
            SLOT(launchPopUpMenu(const QModelIndex &, const QPointF &)));
    mWidget->setLongPressEnabled(true);
    mWidget->scrollTo(mWidget->currentIndex());
}

// -----------------------------------------------------------------------------
// LocationPickerView::manageHgWidget()
// -----------------------------------------------------------------------------
void LocationPickerView::manageHgWidget()
{   
    removeDetailsLabel();
    mLinerLayout->removeItem(mListView);
    mListView->hide();
    mWidget->show();
    mLinerLayout->updateGeometry();
    //set the appropriate model
    switch(mViewType)
    {
        case ELocationPickerContent:
        {  
            mHgModel->resetModel(mProxyModel);
            mAllAction->setChecked(true);
            mCollectionAction->setChecked(false);
            mLinerLayout->insertItem(1,mWidget);
        }
        break;
        case ELocationPickerCollectionContent:
        {
            setCollectionData(mCategoryId);
            mCollectionAction->setChecked(true);
            mAllAction->setChecked(false);
        }
        break;
    }
}

// ----------------------------------------------------
// LocationPickerView::manageListView()
// ----------------------------------------------------
void LocationPickerView::manageListView()
{   
    if(mEmptyLabel)
    {
        removeDetailsLabel();
    }
    else
    {
        mLinerLayout->removeItem(mWidget);
        mWidget->hide();   
    }
    mLinerLayout->insertItem(1,mListView);
    mListView->show();
    //set the appropriate model
    switch(mViewType)
    {
        case ELocationPickerContent:
        {   
            mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
            mListView->setModel(mProxyModel,mListItem);
            mAllAction->setChecked(true);
            mCollectionAction->setChecked(false);
            mViewType = ELocationPickerContent;
        }
        break;
        case ELocationPickerCollectionListContent:
        {
            mListView->setModel(mLocationPickerCollectionListContent->getStandardModel()
            ,mListItem);
            mCollectionAction->setChecked(true);
            mAllAction->setChecked(false);
            if(mAscendingAction->isEnabled())
            {
                mAscendingAction->setDisabled(true);
                mDescendingAction->setDisabled(true);
            }
            mViewType = ELocationPickerCollectionListContent;
        }
        break;
        case ELocationPickerCollectionContent:
        {
            mCollectionAction->setChecked(true);
            mAllAction->setChecked(false);
            mViewType = ELocationPickerCollectionContent;
            setCollectionData(mCategoryId);
        }
        break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::disableTabs()
// -----------------------------------------------------------------------------
void LocationPickerView::disableTabs()
{
    //if no location entries present
    mLinerLayout = static_cast<QGraphicsLinearLayout*>(widget()->layout());
    mEmptyLabel =  new HbTextItem( hbTrId( "txt_lint_list_no_location_entries_present" ) );
    mEmptyLabel->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    mEmptyLabel->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mEmptyLabel->setAlignment( Qt::AlignCenter );
    mLinerLayout->insertItem( 1, mEmptyLabel );
    mAllAction->setDisabled( true );
    mCollectionAction->setDisabled( true );
    mSearchAction->setDisabled( true );
    mAscendingAction->setDisabled( true );
    mDescendingAction->setDisabled( true );
}

// -----------------------------------------------------------------------------
// LocationPickerView::handleActivated()
// -----------------------------------------------------------------------------
void LocationPickerView::handleActivated( const QModelIndex &aIndex )
{
    QModelIndex  index;
    QStandardItem* item;
    int row= aIndex.row();
    int col = aIndex.column();
    //handle the activated signal according to model set
    switch(mViewType)
    {    
        case ELocationPickerContent:
        {   
            if(mainWindow()->orientation() == Qt::Vertical)
            {
                index = mProxyModel->mapToSource(
                    aIndex);
                item = mModel->item( index.row(), index.column() );
            }
            else
            {
                QModelIndex proxyModelIndex = mProxyModel->index(row,col);
                index = mProxyModel->mapToSource(
                        proxyModelIndex);
                item = mModel->item( index.row(), index.column() );
            }
            QVariant var = item->data( Qt::UserRole );
            quint32 lm = var.toUInt();
            //item selected, complete request
            emit selectItem( lm );
        }
            break;
        case ELocationPickerCollectionListContent:
        {
            mLocationPickerCollectionListContent->getData(
                    aIndex, mCategoryId );
            setCollectionData(mCategoryId);
            mViewType = ELocationPickerCollectionContent;
        }
            break;
        case ELocationPickerCollectionContent:
        {
            if(mainWindow()->orientation() == Qt::Vertical)
            {
                index = mCollectionContent->getProxyModel()->mapToSource(
                    aIndex);
            }
            else
            {
                QModelIndex proxyModelIndex = mCollectionContent->getProxyModel()->index(row,col);
                index = mCollectionContent->getProxyModel()->mapToSource(
                        proxyModelIndex);
            }
            quint32 lm = 0;
            mCollectionContent->getData(index, lm);
            //item selected, complete request
            emit selectItem(lm);
        }
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::sortAscending()
// -----------------------------------------------------------------------------
void LocationPickerView::sortAscending()
{   
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mProxyModel->sort( 0, Qt::AscendingOrder );
        if(mainWindow()->orientation()==Qt::Horizontal)
            mHgModel->resetModel(mProxyModel);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort( 0, Qt::AscendingOrder );
        if(mainWindow()->orientation()==Qt::Horizontal)
                    mHgModel->resetModel(mCollectionContent->getProxyModel());
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::sortDescending()
// -----------------------------------------------------------------------------
void LocationPickerView::sortDescending()
{
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mProxyModel->sort(0, Qt::DescendingOrder);
        if(mainWindow()->orientation()==Qt::Horizontal)
                    mHgModel->resetModel(mProxyModel);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort( 0, Qt::DescendingOrder );
        if(mainWindow()->orientation()==Qt::Horizontal)
                    mHgModel->resetModel(mCollectionContent->getProxyModel());
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::allTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerView::allTabTriggered()
{
    removeDetailsLabel();
    //execute only if tab is not pressed
    if (mAllAction->isChecked())
    {    
        mViewType = ELocationPickerContent;
        mCollectionAction->setChecked(false);
        mAscendingAction->setEnabled(true);
        mDescendingAction->setEnabled(true);
        //delete mCollectionContent if coming back from collectioncontent
        if (mCollectionContent)
        {
            delete mCollectionContent;
            mCollectionContent = NULL;
        }
        if(mainWindow()->orientation() == Qt::Vertical)
        {
            manageListView();
        }
        else
        {
            manageHgWidget();
        }
    }
    else
    {
        //Keep the tab pressed
        mAllAction->setChecked(true);
    }
    
}

// -----------------------------------------------------------------------------
// LocationPickerView::colectionTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerView::colectionTabTriggered()
{
	  if( mViewType == ELocationPickerCollectionListContent ||
	      mViewType == ELocationPickerCollectionContent )
	  {
	  	  mCollectionAction->setChecked(true);
	      return;
	  }
    mListItem->setGraphicsSize(HbListViewItem::MediumIcon);
    removeDetailsLabel();
    //execute only if tab is not pressed
    if (mCollectionAction->isChecked())
    {           
        mAscendingAction->setDisabled(true);
        mDescendingAction->setDisabled(true);
        mAllAction->setChecked(false);
        mViewType = ELocationPickerCollectionListContent;
        manageListView();
    }
    else
    {
        //Keep the tab pressed
        mCollectionAction->setChecked(true);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::searchTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerView::searchTabTriggered()
{
    if (mCollectionContent)
    {
        delete mCollectionContent;
        mCollectionContent = NULL;
    }
    mWidget->hide();
    emit switchToSearchView();
}

// -----------------------------------------------------------------------------
// LocationPickerView::setCollectionData()
// -----------------------------------------------------------------------------
void LocationPickerView::setCollectionData( quint32 acategoryId )
{
    if(!mCollectionContent)
    {
        mCollectionContent
        = new LocationPickerCollectionContent( acategoryId );
    }
    
    if(mainWindow()->orientation() == Qt::Vertical)
    {
        QString categoryname;
        removeDetailsLabel();    
        switch(acategoryId)
        {

            case 1: categoryname = QString(hbTrId("txt_lint_subhead_places"));
            break;
            case 8: categoryname = QString(hbTrId("txt_lint_subhead_contact_addresses"));     
            break;
            case 9: categoryname = QString(hbTrId("txt_lint_subhead_calendar_event_locations"));
            break;
        }
        if(mCollectionContent->locationFound())
        {
            mListView->setModel( mCollectionContent->getProxyModel(),mListItem );
            //Enable the options
            mAscendingAction->setEnabled(true);
            mDescendingAction->setEnabled(true);
            mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
        }
        else
        {
            displayNoEntries();
        }
        mCollectionAction->setChecked(true);

        if(mColllabel)
        {
            mLinerLayout->insertItem(0,mColllabel);
            mColllabel->setPlainText(categoryname);   
            mColllabel->show();
        }
    }
    else
    {
           if(mCollectionContent->locationFound())
           {
               mHgModel->resetModel( mCollectionContent->getProxyModel() );
               //Enable the options
               mAscendingAction->setEnabled(true);
               mDescendingAction->setEnabled(true);
               mLinerLayout->removeItem(mListView);
               mListView->hide();
               mLinerLayout->insertItem(1,mWidget);
               mWidget->show();
               mLinerLayout->updateGeometry();
           }
           else
           {
               displayNoEntries();
           }
    }
    mViewType = ELocationPickerCollectionContent;
}

// -----------------------------------------------------------------------------
// LocationPickerView::getViewType()
// -----------------------------------------------------------------------------
TViewType LocationPickerView::getViewType()
{
    return mViewType;
}

// -----------------------------------------------------------------------------
// LocationPickerView::setViewType()
// -----------------------------------------------------------------------------
void LocationPickerView::setViewType( TViewType aViewType )
{
    mViewType = aViewType;
}


// -----------------------------------------------------------------------------
// LocationPickerView::clearContentModel()
// -----------------------------------------------------------------------------
void LocationPickerView::clearContentModel()
{
    if(mCollectionContent)
    {
    	delete mCollectionContent;
    	mCollectionContent = NULL;
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::launchPopUpMenu()
// -----------------------------------------------------------------------------
void LocationPickerView::launchPopUpMenu( HbAbstractViewItem *aItem, const QPointF &aPoint )
{
    mLongPressMenu = new HbMenu();
    mLongPressMenu->setTimeout(HbMenu::NoTimeout);
    connect(mLongPressMenu,SIGNAL(aboutToClose ()),this,SLOT(deleteMenu()));
    mSelectAction  = mLongPressMenu->addAction(hbTrId("txt_lint_menu_select"));
    if( mViewType == ELocationPickerCollectionContent || mViewType == ELocationPickerContent )
    {
        mDetailsAction  = mLongPressMenu->addAction(hbTrId("txt_lint_menu_details"));
        connect(mDetailsAction, SIGNAL(triggered()), this, SLOT(handleDetails()));
    }
    mIndex = aItem->modelIndex();
    connect(mSelectAction, SIGNAL(triggered()), this, SLOT(handleSelect()));
    mLongPressMenu->setPreferredPos(aPoint);
    mLongPressMenu->open();
}

// -----------------------------------------------------------------------------
// LocationPickerView::launchPopUpMenu()
// -----------------------------------------------------------------------------
void LocationPickerView::launchPopUpMenu( const QModelIndex &aIndex, const QPointF &aPoint )
{
    mLongPressMenu = new HbMenu();
    mLongPressMenu->setTimeout(HbMenu::NoTimeout);
    connect(mLongPressMenu,SIGNAL(aboutToClose ()),this,SLOT(deleteMenu()));
    mSelectAction  = mLongPressMenu->addAction(hbTrId("txt_lint_menu_select"));
    mIndex = aIndex;
    connect(mSelectAction, SIGNAL(triggered()), this, SLOT(handleSelect()));
    mLongPressMenu->setPreferredPos(aPoint);
    mLongPressMenu->open();
}
// -----------------------------------------------------------------------------
// LocationPickerView::handleSelect()
// -----------------------------------------------------------------------------
void LocationPickerView::handleSelect()
{
    handleActivated(mIndex);
}

// -----------------------------------------------------------------------------
// LocationPickerView::deleteMenu()
// -----------------------------------------------------------------------------
void LocationPickerView::deleteMenu()
{
    mLongPressMenu->deleteLater();
    mLongPressMenu = NULL;
    mSelectAction->deleteLater();
    mSelectAction = NULL;
    mDetailsAction->deleteLater();
    mDetailsAction = NULL;
    
}

// -----------------------------------------------------------------------------
// LocationPickerView::handleSelect()
// -----------------------------------------------------------------------------
void LocationPickerView::handleDetails()
{
    HbDocumentLoader* loader = new HbDocumentLoader();

    bool ok = false;
    //load the popup dialog
    loader->load(":/popupdialog.docml", &ok);
    Q_ASSERT_X(ok,"locationpickerservice","invalid DocML file");
    //find graphics popup dialog
    QGraphicsWidget *popUpDialog = loader->findWidget("dialog");
    Q_ASSERT_X((popUpDialog != 0), "locationpickerservice", "invalid DocML file");
    mDialog = qobject_cast<HbDialog*>(popUpDialog);
    mMapIconLabel = qobject_cast<HbLabel*>(loader->findWidget("mapLabel"));
    mTitleLabel = qobject_cast<HbLabel*>(loader->findWidget("titleLabel"));
    mAddressMiddle = qobject_cast<HbLabel*>(loader->findWidget("addressMiddle"));
    mAddressBottom = qobject_cast<HbLabel*>(loader->findWidget("addressBottom"));
    mDone = qobject_cast<HbAction*>(loader->findObject(QString("action")));
    mTitleLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
    mDialog->setDismissPolicy(HbDialog::NoDismiss);
    mDialog->setTimeout(HbDialog::NoTimeout);
    connect(mDone, SIGNAL(triggered()), this, SLOT(closeDetailsDialog()));
    QStringList adressDetail;
    QString iconName;
    mMapIconLabel->setAlignment(Qt::AlignCenter);
    if(mViewType == ELocationPickerContent)
    {
        adressDetail = mProxyModel->data(mIndex,Qt::UserRole+3).toStringList();
        iconName =  mProxyModel->data(mIndex,Qt::UserRole+1).toString();
    }
    else
    {
        adressDetail = mCollectionContent->getProxyModel()->data(mIndex,Qt::UserRole+3).
        	toStringList();
        iconName =  mCollectionContent->getProxyModel()->data(mIndex,Qt::UserRole+1).
        	toString();
    }
    if(iconName.isEmpty())
    {
        mMapIconLabel->setIcon(KDummyImage); 
    }
    else
    {
        mMapIconLabel->setIcon(HbIcon(iconName)); 
    }

    if(adressDetail[0].isEmpty() && adressDetail[1].isEmpty())
    {
        mTitleLabel->setPlainText(adressDetail[2]);
        mAddressMiddle->setPlainText(QString(""));
        mAddressBottom->setPlainText(QString(""));
    }
    else if(adressDetail[0].isEmpty())
    {
        mTitleLabel->setPlainText(adressDetail[1]);
        mAddressMiddle->setPlainText(adressDetail[2]);
        mAddressBottom->setPlainText(QString(""));
    }
    else if (adressDetail[1].isEmpty())
    {
        mTitleLabel->setPlainText(adressDetail[0]);
        mAddressMiddle->setPlainText(adressDetail[2]);
        mAddressBottom->setPlainText(QString(""));
    }
    else
    {
        mTitleLabel->setPlainText(adressDetail[0]);
        mAddressMiddle->setPlainText(adressDetail[1]);
        mAddressBottom->setPlainText(adressDetail[2]);
    }
 
    mDialog->open();
    delete loader;
}

// -----------------------------------------------------------------------------
// LocationPickerView::closeDetailsDialog()
// -----------------------------------------------------------------------------
void LocationPickerView::closeDetailsDialog()
{
    if(mDialog)
    {
        mDialog->close();
        delete mDialog;
        mDialog = NULL;
    }
}
// -----------------------------------------------------------------------------
// LocationPickerView::displayNoEntries()
// -----------------------------------------------------------------------------
void LocationPickerView::displayNoEntries()
{
    if(!mEmptyLabel)
    {
        mEmptyLabel =  new HbTextItem(hbTrId("txt_lint_list_no_location_entries_present"));
        mEmptyLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        mEmptyLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
        mEmptyLabel->setAlignment(Qt::AlignCenter);
        mListView->reset();
        mLinerLayout->removeItem(mListView);        
        mListView->hide();
        mWidget->setVisible(false);
        mLinerLayout->insertItem(0, mEmptyLabel);
    }
    
}

// -----------------------------------------------------------------------------
// LocationPickerView::removeDetailsLabel()
// -----------------------------------------------------------------------------
void LocationPickerView::removeDetailsLabel()
{
    if (mLinerLayout && mColllabel)
    {
        mColllabel->setPlainText("");
        mLinerLayout->removeItem(mColllabel);
        mColllabel->hide();
    }
    if(mEmptyLabel)
    {   
        mLinerLayout->removeItem(mEmptyLabel);
        mEmptyLabel->hide();
        delete mEmptyLabel;
        mEmptyLabel = NULL;
    }
}

