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
* Description:   ?Description
*
*/


//Includes
#include <QDebug>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hblistview.h>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>
#include <hbgroupbox.h>
#include <hbparameterlengthlimiter.h>

//User Includes
#include "glxviewids.h"
#include "glxlistview.h"
#include "glxmodelparm.h"
#include "glxdocloaderdefs.h"
#include "glxcommandhandlers.hrh"
#include "glxlocalisationstrings.h"

GlxListView::GlxListView(HbMainWindow *window) 
    : GlxView ( GLX_LISTVIEW_ID ), 
      mListView(NULL), 
      mView(NULL), 
      mAlbumCount(NULL),
      mWindow(window), 
      mModel ( NULL),
      mIsLongPress( false )
{
    qDebug("GlxListView::GlxListView()");
    mDocLoader = new HbDocumentLoader();
}

void GlxListView::activate()
{
    qDebug("GlxListView::activate() %d", mWindow->orientation() );    

    if (mListView == NULL) {
        createListView();
    }
 }

void GlxListView::deActivate()
{
    qDebug("GlxListView::deActivate()");
}

void GlxListView::setModel(QAbstractItemModel *model) 
{
    qDebug("GlxListView::setModel()");
    if ( mModel ) {
        disconnect(mModel, SIGNAL(listPopulated()), this, SLOT( populated()));
        disconnect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( showAlbumCount() ) );
        disconnect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( showAlbumCount() ) );
    }
    mModel =  model ;
    mListView->setModel(mModel);
    connect(mModel, SIGNAL(listPopulated()), this, SLOT( populated()));
    connect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( showAlbumCount() ) );
    connect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( showAlbumCount() ) );
    showAlbumCount();
}

void GlxListView::addToolBar( HbToolBar *toolBar ) 
{
    setToolBar(toolBar) ;
}

void GlxListView::initializeView( QAbstractItemModel *model, GlxView *preView )
{
    Q_UNUSED( preView )
    qDebug("GlxListView::initializeView()");
    if ( mListView == NULL ) {
        createListView();
    }
    setModel( model );
}

QGraphicsItem * GlxListView::getAnimationItem( GlxEffect transtionEffect )
{
    if ( transtionEffect == GRID_TO_ALBUMLIST  || transtionEffect == ALBUMLIST_TO_GRID ) {
        return mListView;
    }
    
    return NULL;
}

void GlxListView::addViewConnection ()
{
    qDebug("GlxListView::addViewConnection()");
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    connect(mListView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
    connect( mListView, SIGNAL( scrollingEnded() ), this, SLOT( setVisvalWindowIndex() ) );
}

void GlxListView::removeViewConnection()
{
    qDebug("GlxListView::removeViewConnection()");
    disconnect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    disconnect(mListView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
    disconnect( mListView, SIGNAL( scrollingEnded() ), this, SLOT( setVisvalWindowIndex() ) );
}

void GlxListView::setVisvalWindowIndex()
{
    QList< HbAbstractViewItem * >  visibleItemList =  mListView->visibleItems();
    qDebug("GlxListView::setVisvalWindowIndex() %d", visibleItemList.count());    
    
    if ( visibleItemList.count() <= 0 )
        return ;

    HbAbstractViewItem *item = visibleItemList.at(0);    
    if ( item == NULL ) 
        return ;
        
    if (  item->modelIndex().row() < 0 || item->modelIndex().row() >= mModel->rowCount() )
        return ;
    
    mModel->setData( item->modelIndex (), item->modelIndex().row(), GlxVisualWindowIndex);
}

void GlxListView::loadListView()
{
    qDebug("GlxListView::loadListView()");
    bool loaded = true;
    //Load the widgets accroding to the current Orientation
    if (mListView == NULL ) {
        mDocLoader->load(GLX_LISTVIEW_DOCMLPATH,&loaded);
        if(loaded) {
            //retrieve the widgets
            mView = static_cast<HbView*>(mDocLoader->findWidget(QString(GLX_LISTVIEW_VIEW)));
            mListView = static_cast<HbListView*>(mDocLoader->findWidget(QString(GLX_LISTVIEW_LIST)));
            mAlbumCount = static_cast<HbGroupBox*>(mDocLoader->findWidget(QString(GLX_LISTVIEW_ALBUMCOUNT)));

            if( mListView ) { 
                //sets the widget
                setWidget((QGraphicsWidget*) mView);
                HbListViewItem *prototype = mListView->listItemPrototype();
                prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
            }
        }  
    }
}

void GlxListView::createListView()
{
    qDebug("GlxListView::createListView()");
    loadListView(); 
    addViewConnection();  //only one time add the connection and remove in side the destructor    
}

GlxListView::~GlxListView()
{
    qDebug("GlxListView::~GlxListView()");

    if(widget()) {
        qDebug("GlxListView::~GlxListView() takeWidget");
        takeWidget();    
    }       

    removeViewConnection();

    if(mAlbumCount) {
        delete mAlbumCount ;
        mAlbumCount = NULL;
    }

    if(mListView) {
        delete mListView;
        mListView = NULL;
    }

    if(mView) {
        delete mView ;
        mView = NULL;
    }
    
    if(mDocLoader) {
        delete mDocLoader;
        mDocLoader = NULL;
    }
}

void GlxListView::populated()
{
    QVariant variant = mModel->data(mModel->index(0,0), GlxVisualWindowIndex );
    int visualIndex = 0;
    if ( variant.isValid() &&  variant.canConvert<int> () ) 
    {
        visualIndex = variant.value<int>();
    }
	//Scroll To the Visible Index as mentioned in the AM.
    mListView->scrollTo(mModel->index(visualIndex, 0),  HbAbstractItemView::PositionAtTop );
}
void GlxListView::itemSelected(const QModelIndex &  index)
{
    qDebug("GlxListView::itemSelected() index = %d", index.row() );
    if ( mIsLongPress ) {
        mIsLongPress = false ;
        return ;
    }
    if ( mModel ) {
        mModel->setData( index, index.row(), GlxFocusIndexRole );
    }
    
    int curstate = NO_LIST_S; 
    qDebug() << "GlxListView::itemSelected READING STATE ";
    QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        curstate = variant.value<int>();
    }
    qDebug() << "GlxMenuManager::viewSubState = " << curstate ;
    if(curstate == FETCHER_ALBUM_S ){
        emit actionTriggered( EGlxCmdFetcherAlbumGridOpen );
    }else {        
        emit actionTriggered( EGlxCmdAlbumGridOpen );
    }
}


void GlxListView::indicateLongPress(HbAbstractViewItem *item, QPointF coords)
{
    qDebug() << "GlxListView:indicateLongPress Item " << item->modelIndex() << "long pressed at " << coords;
    if ( mModel ) {
        mModel->setData( item->modelIndex(), item->modelIndex().row(), GlxFocusIndexRole );
    }
    mIsLongPress = true;
    emit itemSpecificMenuTriggered(viewId(),coords);
}

void GlxListView::showAlbumCount()
{
    int albumCnt = mModel->rowCount();
    QString text = HbParameterLengthLimiter(GLX_ALBUM_LIST_COUNT_LABEL, albumCnt);
    mAlbumCount->setHeading(text);
}
