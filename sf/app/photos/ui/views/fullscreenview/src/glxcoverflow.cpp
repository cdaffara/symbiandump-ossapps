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
#include <hbiconitem.h>
#include <QAbstractItemModel>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGesture>
#include <hbpangesture.h>
#include <hbiconanimator.h>
#include <hbinstance.h>

//User Includes
#include <glxmodelparm.h>
#include <glxcoverflow.h>
#include "glxviewids.h"

#define GLX_COVERFLOW_SPEED  32
#define GLX_BOUNCEBACK_SPEED 16
#define GLX_BOUNCEBACK_DELTA 8
#define MAX_GIF_DIMENSION    640
const int KBytesInMB = 1024 * 1024;

GlxCoverFlow::GlxCoverFlow( QGraphicsItem *parent ) 
     : HbWidget( parent ), 
       mSelItemIndex ( 0 ),
       mRows( 0 ),
       mSelIndex ( 0 ),
       mStripLen ( 0 ),
       mCurrentPos( 0 ),
       mItemSize ( QSize( 0, 0 ) ),
       mModel ( NULL ),
       mMoveDir( NO_MOVE ),
       mSpeed ( GLX_COVERFLOW_SPEED ),
	   mZoomOn( false ),
	   mMultitouchFilter( NULL ),
       mTimerId( 0 ),
       mIsInit( false ),
       mIsOrientChangeAnim( false )
{
    qDebug( "GlxCoverFlow::GlxCoverFlow" );
    mIsAutoMoving = FALSE;
    grabGesture( Qt::PanGesture );
    grabGesture( Qt::TapGesture );
    connect( this, SIGNAL( autoLeftMoveSignal() ), this, SLOT( autoLeftMove() ), Qt::QueuedConnection );
    connect( this, SIGNAL( autoRightMoveSignal() ), this, SLOT( autoRightMove() ), Qt::QueuedConnection );
    connect( this, SIGNAL( moveNextSignal() ), this, SLOT( moveNextImage() ), Qt::QueuedConnection );
    connect( this, SIGNAL( movePreviousSignal() ), this, SLOT( movePreviousImage() ), Qt::QueuedConnection ); 
}

void GlxCoverFlow::setMultitouchFilter(QGraphicsItem* mtFilter)
{
	mMultitouchFilter = mtFilter;
}

void GlxCoverFlow::setCoverFlow()
{
    qDebug("GlxCoverFlow::setCoverFlow");
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        mIconItem[i] = new HbIconItem( this );
        mIconItem[i]->grabGesture( Qt::PinchGesture, Qt::ReceivePartialGestures );
        mIconItem[i]->installSceneEventFilter( mMultitouchFilter );
        mIconItem[i]->setBrush( QBrush( Qt::black ) );
        mIconItem[i]->setSize( QSize( 0, 0 ) );
        mIconItem[i]->setAlignment( Qt::AlignCenter );
        mIconItem[i]->setIconScaling( false );
        mIconItem[i]->setObjectName( QString( "Cover%1" ).arg( i ) );
    }
    
    mIsInit = true;
    mUiOn = false;
    mBounceBackDeltaX = GLX_BOUNCEBACK_DELTA;
}

void GlxCoverFlow::setItemSize(QSize &size)
{
    qDebug("GlxCoverFlow::setSize width = %d",size.width() );
    if( mItemSize != size) {
        mItemSize = size;
        resetCoverFlow();
    }    
}

void GlxCoverFlow::setModel(QAbstractItemModel *model)
{
    qDebug("GlxCoverFlow::setModel model change = %d", model  );
    
    if ( model == mModel) {
        return ;
    }
    
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
    resetCoverFlow();
}

void GlxCoverFlow::indexChanged( int index )
{
    qDebug("GlxCoverFlow::indexChanged index = %d mSelIndex = %d ",index, mSelIndex );
    if ( index != mSelIndex && mModel) {
        loadIconItems();
    }
}

void GlxCoverFlow::gestureEvent(QGestureEvent *event)
{    
    if(QTapGesture *gesture = static_cast<QTapGesture *>(event->gesture(Qt::TapGesture))) {        
        if (gesture->state() == Qt::GestureFinished) {
            if(!mTimerId) {
                mTimerId = startTimer( DOUBLETAP_TIMEINTERVAL );
            }
            else {
                killTimer(mTimerId);
                mTimerId = 0;
                emit doubleTapEventReceived(hbInstance->allMainWindows().first()->mapToScene(gesture->position().toPoint()));
            }
            event->accept(gesture);
        }
    }
  
    if (QPanGesture *panningGesture = qobject_cast<QPanGesture*>(event->gesture(Qt::PanGesture))) {
        HbPanGesture *hbPanGesture = qobject_cast<HbPanGesture *>(panningGesture);
        if (hbPanGesture) {
            if(hbPanGesture->state() == Qt::GestureUpdated) {
                QPointF delta(hbPanGesture->sceneDelta());
                panGesture(delta);
                event->accept(panningGesture);            
            }
            
            if( hbPanGesture->state() == Qt::GestureFinished  ) {
                if( mIsAutoMoving == FALSE ) {
                    switch( mMoveDir ) {
                        case LEFT_MOVE : 
                            mMoveDir = NO_MOVE;
                            mBounceBackDeltaX = ( mItemSize.width() >> 2 )  + ( mItemSize.width() >> 3 );
                            emit autoLeftMoveSignal();
                            mIsAutoMoving = TRUE;
                            break ;
                    
                        case RIGHT_MOVE :
                            mMoveDir = NO_MOVE;
                            mBounceBackDeltaX = ( mItemSize.width() >> 2 )  + ( mItemSize.width() >> 3 );
                            emit autoRightMoveSignal();
                            mIsAutoMoving = TRUE;
                            break;
                    
                        default:
                            break;
                    } 
                }
                event->accept(panningGesture);
            }
        }
    }  
}

void GlxCoverFlow::panGesture ( const QPointF & delta )  
{
    qDebug("GlxCoverFlow::panGesture deltaX= %d", (int)delta.x());  
   
    if( !mIsInit 
            || !mModel 
            || getSubState() == IMAGEVIEWER_S 
            || getSubState() == FETCHER_S 
            || mIsAutoMoving ) {
        return;
    }
    
    move( ( int ) delta.x() );    
    if( delta.x() > 0 ) {     
        mMoveDir = RIGHT_MOVE;
    }
    else if ( delta.x() < 0) {
        mMoveDir = LEFT_MOVE;
    }
    
    if ( mUiOn ) {
        emit coverFlowEvent( PANNING_START_EVENT );
        mUiOn = FALSE;
    }
}

void GlxCoverFlow::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    Q_UNUSED(endIndex);
    qDebug("GlxCoverFlow::dataChanged startIndex = %d mSelIndex = %d ", startIndex.row(), mSelIndex  );
    
    if ( mIsOrientChangeAnim ) {
        return;
    }
    
    int index = 0;
    for (int i = 0; i < NBR_ICON_ITEM ; i++) {
        index = calculateIndex( mSelIndex + i - 2 );
        if ( index == startIndex.row() ) {
            index = ( mSelItemIndex + i - 2 + NBR_ICON_ITEM ) % NBR_ICON_ITEM;
            mIconItem[ index ]->setIcon( getIcon( startIndex.row() ) );
            if ( index == mSelItemIndex ) {
                playAnimation( );
            }
        }
    }
}

void GlxCoverFlow::rowsInserted(const QModelIndex &parent, int start, int end)
{
    qDebug("GlxCoverFlow::rowsInserted");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    resetCoverFlow();  
    emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
}

void GlxCoverFlow::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    qDebug("GlxCoverFlow::rowsRemoved model %d", mModel);
    
    if ( mModel->rowCount() <= 0 ) {
        emit coverFlowEvent( EMPTY_ROW_EVENT );
    }
    else {
        resetCoverFlow();   
        emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
    }
}

void GlxCoverFlow::modelDestroyed()
{
    mModel = NULL ;    
}

void GlxCoverFlow::autoLeftMove()
{
    int width = mItemSize.width() ;
    
    qDebug("GlxCoverFlow::autoLeftMove current pos = %d mBounceBackDeltaX x = %d", mCurrentPos, mBounceBackDeltaX);
    if ( !mIsInit ) {
        return;
    }
    
    if ( mSelIndex == ( mRows -1 ) ) {
        mSpeed = GLX_BOUNCEBACK_SPEED;
    }
    //for bounce back effect for last image ( it will do the back)
    if ( ( mCurrentPos + width ) > ( mStripLen + mBounceBackDeltaX ) && mMoveDir == NO_MOVE ) {
        mMoveDir = LEFT_MOVE;
        mBounceBackDeltaX = GLX_BOUNCEBACK_DELTA;
        autoRightMoveSignal();
        return ;
    }
    
    int deltaX = width - mCurrentPos %  width ; 
    int moveX = deltaX > mSpeed ? mSpeed : deltaX;
    
    qDebug("GlxCoverFlow::autoLeftMove delta x = %d current pos = %d move x = %d", deltaX, mCurrentPos, moveX);
    
    move (-moveX);//move the image
    deltaX -= moveX; //To know the auto move is required or not
    
    if ( deltaX ) {
        emit autoLeftMoveSignal();
    }
    else {
        //for bounce back of first image
        if ( mMoveDir == RIGHT_MOVE ) {
            emit autoRightMoveSignal();
        }
        else {
            emit moveNextSignal();
        }        
    }   
}

void GlxCoverFlow::autoRightMove()
{
    qDebug("GlxCoverFlow::autoRightMove ");
    if ( !mIsInit ) {
        return;
    }
    int width = mItemSize.width()  ;
    int diffX = mStripLen - mCurrentPos ;
 
    //slow the speed for bounce back effect
    if ( mSelIndex == 0 ) {
        mSpeed = GLX_BOUNCEBACK_SPEED;
    }
    //for bounce back effect for back image ( it will do the back)
    qDebug("GlxCoverFlow::autoRightMove diffX x = %d current pos = %d mBounceBackDeltaX x = %d", diffX, mCurrentPos, mBounceBackDeltaX);
    if ( diffX > mBounceBackDeltaX && diffX < width && mMoveDir == NO_MOVE ){
        mMoveDir = RIGHT_MOVE;
        mBounceBackDeltaX = GLX_BOUNCEBACK_DELTA;
        autoLeftMoveSignal();  
        return ;
    }
    
    int deltaX = mCurrentPos %  width ; 
    //in the case of deltaX == 0 ( right flick case ) complete image should move
    deltaX = deltaX ? deltaX : width ; 
    int moveX = deltaX > mSpeed ? mSpeed : deltaX;
    
    qDebug("GlxCoverFlow::autoRightMove delta x = %d current pos = %d move x = %d", deltaX, mCurrentPos, moveX);
    
    move (moveX);
    deltaX -= moveX;

    if ( deltaX ) {
        emit autoRightMoveSignal();
    }
    else {
        //for bounce back of last image
        if ( mMoveDir == LEFT_MOVE ) {
            emit autoLeftMoveSignal();
        }
        else {
            emit movePreviousSignal();
        }
    }
}

void GlxCoverFlow::moveNextImage()
{
    int width = mItemSize.width() ;
    int selIndex = mCurrentPos / width ;
    if ( mRows == 1 || selIndex != mSelIndex ) {
        stopAnimation() ;
        mSelIndex = selIndex ;
        mSelItemIndex = ( ++mSelItemIndex ) % NBR_ICON_ITEM ;
        selIndex = ( mSelItemIndex + 2 ) % NBR_ICON_ITEM ;
        updateIconItem( mSelIndex + 2, selIndex, width * 2 ) ;
        playAnimation() ;
        if( !mZoomOn ) {
            emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
        }
    }
    mMoveDir = NO_MOVE;
    mIsAutoMoving = FALSE;
    mBounceBackDeltaX = GLX_BOUNCEBACK_DELTA ;
    mSpeed = GLX_COVERFLOW_SPEED ;
}

void GlxCoverFlow::movePreviousImage()
{
    int width = mItemSize.width();
    int selIndex = mCurrentPos / width;
    if ( mRows == 1 || selIndex != mSelIndex ) {
        stopAnimation();
        mSelIndex = selIndex;
        mSelItemIndex = ( mSelItemIndex == 0 ) ?  NBR_ICON_ITEM -1 : --mSelItemIndex;
        selIndex = ( mSelItemIndex + 3 ) % NBR_ICON_ITEM;
        updateIconItem( mSelIndex - 2, selIndex, - width * 2 ) ;
        playAnimation();
        if(!mZoomOn) {
            emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
        }
    }
    mMoveDir = NO_MOVE;
    mIsAutoMoving = FALSE;
    mBounceBackDeltaX = GLX_BOUNCEBACK_DELTA;
    mSpeed = GLX_COVERFLOW_SPEED;
}

void GlxCoverFlow::move(int value)
{
    qDebug("GlxCoverFlow::move ");
    QPointF pos(0,0);

    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        pos.setX( mIconItem[i]->pos().x() + value );
        pos.setY( mIconItem[i]->pos().y() );
        mIconItem[i]->setPos( pos );
    }
    
    mCurrentPos -= value;
    if ( mCurrentPos < 0 ) {
        mCurrentPos += mStripLen;
    }
    else if ( mCurrentPos >= mStripLen ) {
        mCurrentPos -= mStripLen;
    }
    qDebug("GlxCoverFlow::Move value = %d current pos = %d", value, mCurrentPos); 
}

void GlxCoverFlow::setRows()
{
    qDebug("GlxCoverFlow::setRows ");
    if (mModel) {
        mRows = mModel->rowCount();
    }
    else {
        mRows = 0;
    }
    qDebug("GlxCoverFlow::setRows number of rows = %d", mRows);
}

void GlxCoverFlow::setStripLen()
{  
    qDebug("GlxCoverFlow::setStripLen ");
    mStripLen = mRows * mItemSize.width();  
    qDebug("GlxCoverFlow::setStripLen rows = %d, striplen %d", mRows, mStripLen);
}

int GlxCoverFlow::calculateIndex(int index)
{
    qDebug("GlxCoverFlow::calculateIndex index = %d, mRows = %d", index, mRows);
    
    if ( mRows == 1 )
        return 0;
    
    if ( index < 0 ) 
        return ( index + mRows );
   
    if ( index >= mRows ) 
        return ( index - mRows ) ;
    
    return index;        
}

void GlxCoverFlow::loadIconItems()
{  
    qDebug("GlxCoverFlow::loadIconItems ");
    
    if ( !mIsInit ) {
        return ;
    }

    int index = 0;
    stopAnimation();
    mSelIndex = getFocusIndex();
    
    qDebug("GlxCoverFlow::loadIconItems index = %d, width = %d", mSelIndex, size().width() );    
    
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        index = calculateIndex ( mSelIndex - 2 + i) ; 
        mIconItem[i]->setIcon( getIcon( index ) );
        mIconItem[i]->setSize ( mItemSize );
        mIconItem[i]->setPos ( QPointF ( (i - 2) * mItemSize.width(), 0) );   
    }
    
    mSelItemIndex = 2;
    mCurrentPos = mItemSize.width() * mSelIndex;
    playAnimation();
}

void GlxCoverFlow::playAnimation()
{
    if ( isAnimatedImage( mSelIndex ) ) {
        mIconItem[ mSelItemIndex ]->setIcon( HbIcon( getUri( mSelIndex ) ) );
        mIconItem[ mSelItemIndex ]->animator().startAnimation();
    }
}

void GlxCoverFlow::stopAnimation()
{
    mIconItem[ mSelItemIndex ]->animator().stopAnimation();
    mIconItem[ mSelItemIndex ]->setIcon( getIcon( mSelIndex ) );
}

void GlxCoverFlow::updateIconItem (qint16 selIndex, qint16 selItemIndex, qint16 posX)
{
    qDebug("GlxCoverFlow::updateIconItem selIndex = %d, selIconIndex = %d posX = %d", selIndex, selItemIndex, posX );
    
    selIndex = calculateIndex( selIndex );
    mIconItem[ selItemIndex ]->setPos( QPointF( posX, 0 ) );    
    mIconItem[ selItemIndex ]->setIcon( getIcon( selIndex ) );
    mIconItem[ selItemIndex ]->setSize ( mItemSize );    
}

void GlxCoverFlow::clearCurrentModel()
{
    qDebug("GlxCoverFlow::clearCurrentModel ");
    if ( mModel ) {
        disconnect( mModel, SIGNAL( fullScreenDataChanged( QModelIndex, QModelIndex ) ), this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
        disconnect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( rowsInserted( QModelIndex, int, int ) ) );
        disconnect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int) ), this, SLOT( rowsRemoved( QModelIndex, int, int ) ) );
        disconnect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
        mModel = NULL ;
    } 
}

void GlxCoverFlow::initializeNewModel()
{
    qDebug("GlxCoverFlow::initializeNewModel" );
    if ( mModel ) {
        connect( mModel, SIGNAL( fullScreenDataChanged( QModelIndex, QModelIndex ) ), this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
        connect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( rowsInserted( QModelIndex, int, int ) ) );
        connect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int) ), this, SLOT( rowsRemoved( QModelIndex, int, int ) ) );
        connect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
    } 
}

void GlxCoverFlow::resetCoverFlow()
{
    qDebug("GlxCoverFlow::resetCoverFlow model %u", mModel );
    setRows();
    setStripLen();
    if ( mModel ) {
        loadIconItems( ) ;  
    }
}

void GlxCoverFlow::partiallyClean()
{
    qDebug("GlxCoverFlow::partiallyClean Enter");
    clearCurrentModel(); //during the animation data update will not cause the crash
    mIsInit = false;
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        if ( mSelItemIndex != i){
            delete mIconItem[i] ;
            mIconItem[i] = NULL;
        }
    }      
}

void GlxCoverFlow::partiallyCreate( QAbstractItemModel *model, QSize itemSize, int posY )
{
    qDebug("GlxCoverFlow::resetpartiallyCreated poxY %d", posY );
    mIconItem[2]->setSize ( itemSize );
    mIconItem[2]->setPos ( QPointF ( 0, posY ) ); 
    mModel = model ; 
    mSelIndex = getFocusIndex();
    mIconItem[2]->setIcon( getIcon( mSelIndex ) ) ;
    mModel = NULL;
}

GlxCoverFlow::~GlxCoverFlow()
{
    qDebug("GlxCoverFlow::~GlxCoverFlow model " );
    ClearCoverFlow();
    disconnect( this, SIGNAL( autoLeftMoveSignal() ), this, SLOT( autoLeftMove() ) );
    disconnect( this, SIGNAL( autoRightMoveSignal() ), this, SLOT( autoRightMove() ) );
}

void GlxCoverFlow::ClearCoverFlow()
{
    qDebug("GlxCoverFlow::ClearCoverFlow  " );
    mIsInit = false;
    clearCurrentModel();    
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        if(mIconItem[i] != NULL ) {
            delete mIconItem[i] ;
            mIconItem[i] = NULL;
        }
    }   
}

int GlxCoverFlow::getSubState()
{
    int substate = NO_FULLSCREEN_S;
    QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        substate = variant.value<int>();
    }
    return substate;
}

void GlxCoverFlow::zoomStarted(int index)
{
    Q_UNUSED(index)
    emit coverFlowEvent( ZOOM_START_EVENT );
    stopAnimation();
	mZoomOn = true;	
}

void GlxCoverFlow::zoomFinished( int index )
{ 
	mZoomOn = false;
	playAnimation();
	indexChanged(index);
}

void GlxCoverFlow::timerEvent( QTimerEvent *event )
{
    if(mTimerId == event->timerId()) {
        killTimer( mTimerId );
        mTimerId = 0;
        emit coverFlowEvent( TAP_EVENT );
    }
}

int GlxCoverFlow::getFocusIndex()
{
    QVariant variant = mModel->data( mModel->index( 0, 0 ), GlxFocusIndexRole ) ;
    if ( variant.isValid() && variant.canConvert< int > () ) {
        return variant.value< int > ();
    }
    return -1;
}

HbIcon GlxCoverFlow::getIcon( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert< HbIcon > () ) {
        return variant.value<HbIcon> () ;       
    }
    return HbIcon() ;
}

QString GlxCoverFlow::getUri( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxUriRole );
    if ( variant.isValid() && variant.canConvert< QString > () ){
        return variant.value< QString > () ;
    }
    return QString();
}

QSize GlxCoverFlow::getImageDimension( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxDimensionsRole );
    if ( variant.isValid() && variant.canConvert< QSize > () ){
        return variant.value< QSize > () ;
    }
    return QSize();    
}

int GlxCoverFlow::getImageSize( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxSizeRole );
    if ( variant.isValid() && variant.canConvert< int > () ){
        return variant.value< int > () ;
    }
    return 0;     
}


bool GlxCoverFlow::isAnimatedImage( int index )
{
    int frameCount = 0;
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxFrameCount );
    if ( variant.isValid() && variant.canConvert< int > () ) {
         frameCount = variant.value< int > () ;
    }
    
    if ( frameCount > 1 ) {
        int size = getImageSize( index );
        QSize dimn = getImageDimension( index );
        
        if ( size <=  KBytesInMB  && dimn.width() <= MAX_GIF_DIMENSION && dimn.height() <= MAX_GIF_DIMENSION  ) {
            return true;
        }
    }
    return false ;
}
