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
#include <QTimer>
#include <e32base.h>
#include <hbiconitem.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <QAbstractItemModel>
#include <QGesture>
#include <hbpangesture.h>
#include <hblabel.h>

//User Includes
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxeffectengine.h"
#include "glxdocloaderdefs.h"
#include "glxslideshowwidget.h"
#include "glxsettinginterface.h"
#include "glxlocalisationstrings.h"
#include "glxlog.h"
#include "glxtracer.h"


GlxSlideShowWidget::GlxSlideShowWidget( QGraphicsItem *parent ) 
    : HbWidget( parent ), 
      mEffectEngine( NULL ), 
      mSettings( NULL ),
      mAnimItem( NULL ),
      mBackGroundItem( NULL ),
      mContinueButton( NULL ), 
      mErrorNote( NULL ),
      mItemIndex( 1 ),  
      mSlideTimer( NULL ), 
      mModel( NULL ), 
      mSlideShowItemCount( 0 ),
      mIsAutoMoving ( false )
{
    TRACER("GlxSlideShowWidget::GlxSlideShowWidget()");
    mSettings = GlxSettingInterface::instance() ; //no owner ship
    grabGesture( Qt::PanGesture );
    grabGesture( Qt::TapGesture );
}

void GlxSlideShowWidget::setSlideShowWidget(HbDocumentLoader *DocLoader)
{
    //To:Do error handling
    TRACER("GlxSlideShowWidget::setSlideShowWidget()");   
    
    //create the effect engine
    mEffectEngine = new GlxEffectEngine();
    mItemIndex = 1;
    
    mBackGroundItem = new HbIconItem( this );
    mBackGroundItem->setBrush( QBrush( Qt::black ) );
    mAnimItem = new HbIconItem( this );
    mAnimItem->setAlignment( Qt::AlignCenter );
    mAnimItem->setIconScaling( false );
    
    // Now load the view and the contents.
    // and then set the play icon to the button
    mContinueButton = static_cast<HbPushButton*>( DocLoader->findWidget( GLXSLIDESHOW_PB ) );
    mContinueButton->setIcon( HbIcon( GLXICON_PLAY ) );
    mContinueButton->hide();
    mIsPause = false;

    for ( int i = 0; i < NBR_ITEM ; i++) {
        mSelIndex[ i ] = -1;
        mIconItems[ i ] = new HbIconItem( this );
        mIconItems[ i ]->setAlignment( Qt::AlignCenter );
        mIconItems[ i ]->setIconScaling( false );
        mIconItems[ i ]->setObjectName( QString( "SlideShowIcon%1" ).arg( i ) );
    }    
    
    mSlideTimer = new QTimer();
    mItemList.clear();    

    //Add the signal-slot for this widget.
    addConnections();

    //Create the settings for the effects 
    mEffectEngine->registerEffect( QString("HbIconItem") );

    //provide the xml info for the effect to take place
    HbEffect::add( QString("HbIconItem"), QString(":/data/transition.fxml"), QString( "Move" ));
    HbEffect::add( QString("HbIconItem"), QString(":/data/transitionleft.fxml"), QString( "LeftMove" ));
    HbEffect::add( QString("HbIconItem"), QString(":/data/transitionright.fxml"), QString( "RightMove" ));
}

GlxSlideShowWidget::~GlxSlideShowWidget()
{
    TRACER("GlxSlideShowWidget::~GlxSlideShowWidget()");

    //Delete the resources allocated
    cleanUp();

    if(mContinueButton) {
        delete mContinueButton;
        mContinueButton = NULL;
    }
}

void GlxSlideShowWidget::cleanUp()
{
    TRACER("GlxSlideShowWidget::cleanUp()");
    removeConnections();

    if( mEffectEngine ) {
        mEffectEngine->deRegisterEffect( QString("HbIconItem") );    
        delete mEffectEngine;
        mEffectEngine = NULL;
    }     
    
    for ( int i = 0; i < NBR_ITEM ; i++) {
        delete mIconItems[i] ;
        mIconItems[i] = NULL;
    }
    
    delete mBackGroundItem ;
    mBackGroundItem = NULL;
    
    delete mAnimItem;
    mAnimItem = NULL;

    if( mSlideTimer ) {
        delete mSlideTimer;
        mSlideTimer = NULL;
    }
    
    if ( mErrorNote ) {
        delete mErrorNote ;
        mErrorNote = NULL;
    }

    clearCurrentModel();
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transition.fxml"), QString( "Move" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionleft.fxml"), QString( "LeftMove" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionright.fxml"), QString( "RightMove" ));
}

QGraphicsItem * GlxSlideShowWidget::animationItem() 
{ 
    mAnimItem->setIcon( getIcon( mSelIndex[ mItemIndex ] ) ); 
    return mAnimItem;
}

void GlxSlideShowWidget::updateAnimationItem() 
{
    mIconItems[ mItemIndex ]->setIcon( HbIcon() );
    mAnimItem->setZValue( mAnimItem->zValue() + 10 );
}

void GlxSlideShowWidget::setModel ( QAbstractItemModel *model )
{
    TRACER("GlxSlideShowWidget::setModel()");
    if ( model == mModel ) {
        return ;
    }
    clearCurrentModel();   
    mModel = model;
    initializeNewModel();
    resetSlideShow();
}

void GlxSlideShowWidget::setItemGeometry( QRect screenRect )
{
    TRACER("GlxSlideShowWidget::setItemGeometry()");
    int index = mItemIndex;
    mScreenRect = screenRect;   
    mIconItems[ index ]->setGeometry( mScreenRect );
    index = ( mItemIndex + 1 ) % NBR_ITEM;
    mIconItems[ index ]->setGeometry( QRect( mScreenRect.width(), mScreenRect.top(), mScreenRect.width(), mScreenRect.height() ) );
    index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;    
    mIconItems[ index ]->setGeometry( QRect( -mScreenRect.width(), mScreenRect.top(), mScreenRect.width(), mScreenRect.height() ) );
    mBackGroundItem->setGeometry( mScreenRect );
    int xPos = ( mScreenRect.width() - mScreenRect.height() ) >> 1 ;
    mAnimItem->setGeometry( QRect( xPos, -xPos, mScreenRect.height(), mScreenRect.width() ) );
}

void GlxSlideShowWidget::triggeredEffect()
{ 
    TRACER("GlxSlideShowWidget::triggeredEffect()"); 
    int index = mItemIndex;
    mSlideTimer->stop();
    mItemList.clear();
    User::ResetInactivityTime();

    mItemList.append( mIconItems[index] );
    index = ( mItemIndex + 1 ) % NBR_ITEM;
    mItemList.append( mIconItems[index] );

    GLX_LOG_INFO3("GlxSlideShowWidget::triggeredEffect() image selected index %d array index %d index %d", mSelIndex, mItemIndex, index);  
    mEffectEngine->runEffect( mItemList, QString("HbIconItem") );
    emit slideShowEvent( EFFECT_STARTED );
}

void GlxSlideShowWidget::effectFinshed()
{
    GLX_LOG_INFO2("GlxSlideShowWidget::effectFinshed() before image selected index %d array index %d", mSelIndex, mItemIndex); 

    mItemIndex = ( ++mItemIndex ) % NBR_ITEM;
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxFocusIndexRole );
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxVisualWindowIndex );
    setNextItemIcon();
  
    GLX_LOG_INFO2("GlxSlideShowWidget::effectFinshed() after image selected index %d array index %d ", mSelIndex, mItemIndex);
    
    startSlideShow();
    mItemList.clear();
    emit indexchanged(); // on each item change
}

void GlxSlideShowWidget::cancelEffect()
{
    TRACER("GlxSlideShowWidget::cancelEffect()");
    mEffectEngine->cancelEffect( mItemList );
}

void GlxSlideShowWidget::pauseSlideShow()
{
    TRACER("GlxSlideShowWidget::pauseSlideShow()");
    GLX_LOG_INFO1("GlxSlideShowWidget::pauseSlideShow() %d", this->zValue());
    mIsPause = true;
    mSlideTimer->stop();
    cancelEffect();
    mContinueButton->setZValue( this->zValue() + 2);
    mContinueButton->show() ;
    emit slideShowEvent( UI_ON_EVENT );
}

void GlxSlideShowWidget::continueSlideShow(bool checked)
{
    Q_UNUSED( checked )
    TRACER("GlxSlideShowWidget::continueSlideShow()");
    mIsPause = false;
    if ( mModel &&  mSlideShowItemCount > 1 ) {
        mSlideTimer->start( mSettings->slideShowDelayTime() ); 
    }
    mContinueButton->hide(); 
    emit slideShowEvent( UI_OFF_EVENT );
}

void GlxSlideShowWidget::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    Q_UNUSED( endIndex )
    TRACER("GlxSlideShowWidget::dataChanged()");
    GLX_LOG_INFO2("GlxSlideShowWidget::dataChanged startIndex = %d mSelIndex = %d ", startIndex.row(), mSelIndex  );

    if ( HbEffect::effectRunning( mAnimItem ) ) {
        return ;
    }
    
    for( int i = 0; i < NBR_ITEM; ++i ) {
        if ( mSelIndex[ i ] == startIndex.row() ) {
            mIconItems[ i ]->setIcon( getIcon( startIndex.row() ) );
        }
    }
}

void GlxSlideShowWidget::rowsInserted(const QModelIndex &parent, int start, int end)
{
    TRACER("GlxSlideShowWidget::rowsInserted()");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    if ( HbEffect::effectRunning( mAnimItem ) ) {
        return ;
    }
    resetSlideShow();  
}

void GlxSlideShowWidget::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    TRACER("GlxSlideShowWidget::rowsRemoved()");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    GLX_LOG_INFO1( "GlxSlideShowWidget::rowsRemoved row count = %d ", mModel->rowCount()  );

    if ( mModel->rowCount() <= 0 ) {
        clearCurrentModel();
        emit slideShowEvent( EMPTY_DATA_EVENT );
    }
    else {
        resetSlideShow();
    }
}

void GlxSlideShowWidget::modelDestroyed()
{
    TRACER("GlxSlideShowWidget::modelDestroyed()");
    clearCurrentModel();
}


void GlxSlideShowWidget::orientationChanged(QRect screenRect)
{
    TRACER("GlxSlideShowWidget::orientationChanged()");
    mAnimItem->setIcon( HbIcon() );
    mAnimItem->setZValue( mAnimItem->zValue() - 10 );
    cancelEffect();
    setItemGeometry( screenRect);
    resetSlideShow();
}

void GlxSlideShowWidget::leftGesture(int value)
{
    Q_UNUSED(value)
    TRACER("GlxSlideShowWidget::leftGesture()");

    int index = ( mItemIndex + 1 ) % NBR_ITEM;
    moveImage( index, mScreenRect.width(), QString("LeftMove"), "leftMoveEffectFinished");
}

void GlxSlideShowWidget::rightGesture(int value)
{
    Q_UNUSED(value)
    TRACER ("GlxSlideShowWidget::rightGesture()");

    int index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    moveImage( index, -mScreenRect.width(), QString("RightMove"), "rightMoveEffectFinished"); 
}

void GlxSlideShowWidget::leftMoveEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status)
    TRACER("GlxSlideShowWidget::leftMoveEffectFinished()");
    GLX_LOG_INFO1("GlxSlideShowWidget::leftMoveEffectFinished() %d status", status.reason);
   
    mItemIndex = ( ++mItemIndex ) % NBR_ITEM;
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxFocusIndexRole );
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxVisualWindowIndex );
    setNextItemIcon();
    startSlideShow();
    mIsAutoMoving = false;
    emit indexchanged(); // on left swipe
}

void GlxSlideShowWidget::rightMoveEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status)
    TRACER ( "GlxSlideShowWidget::rightMoveEffectFinished( ) ");
    GLX_LOG_INFO1("GlxSlideShowWidget::rightMoveEffectFinished() %d status", status.reason);

    mItemIndex = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxFocusIndexRole );
    mModel->setData( mModel->index( 0, 0 ), mSelIndex[ mItemIndex ], GlxVisualWindowIndex );
    setPreItemIcon();
    startSlideShow();
    mIsAutoMoving = false;
    emit indexchanged(); // on right swipe
} 
 
void GlxSlideShowWidget::gestureEvent(QGestureEvent *event)
{  
    if( QTapGesture *gesture = static_cast<QTapGesture *>( event->gesture( Qt::TapGesture ) ) ) {        
        if ( gesture->state() == Qt::GestureFinished ) {
            if ( mIsPause == false ) {
                pauseSlideShow();
            }
            event->accept(gesture);
        }
    }
      
    if ( QPanGesture *panningGesture = qobject_cast<QPanGesture*>( event->gesture( Qt::PanGesture ) ) ) {
        HbPanGesture *hbPanGesture = qobject_cast<HbPanGesture *>( panningGesture );
        if ( hbPanGesture ) {
            if( hbPanGesture->state() == Qt::GestureFinished && mIsAutoMoving == FALSE ) {
                QPointF delta( hbPanGesture->sceneDelta() );
                if ( delta.x() > 0 ) {
                    rightGesture( delta.x() );
                }
                else {
                    leftGesture( delta.x() );
                }
            }
            event->accept(panningGesture);
        }
    }
}    

void GlxSlideShowWidget::startSlideShow ( )
{
    TRACER ( "GlxSlideShowWidget::startSlideShow( ) ");
    GLX_LOG_INFO1 ( "GlxSlideShowWidget::startSlideShow( ) is pause %d", mIsPause);    
    
    if ( mSlideShowItemCount == 0 ) { 
        showErrorNote();
    }
    if ( mIsPause == false && mModel &&  mSlideShowItemCount > 1 ) {
        mSlideTimer->start( mSettings->slideShowDelayTime() );  
    }    
}

void GlxSlideShowWidget::stopSlideShow (  )
{
    TRACER ( "GlxSlideShowWidget::stopSlideShow( ) ");
    cancelEffect();
    mSlideTimer->stop();                
}

void GlxSlideShowWidget::clearCurrentModel()
{
    TRACER ( "GlxSlideShowWidget::clearCurrentModel( ) ");
    if ( mModel ) {
        disconnect( mModel, SIGNAL( fullScreenDataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
        disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        disconnect(mModel, SIGNAL(destroyed()), this, SLOT( modelDestroyed()));
        mModel = NULL ;
    }
}

void GlxSlideShowWidget::initializeNewModel()
{
    TRACER("GlxSlideShowWidget::initializeNewModel" );
    if ( mModel ) {
        connect( mModel, SIGNAL( fullScreenDataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        connect(mModel, SIGNAL(destroyed()), this, SLOT( modelDestroyed()));
    }	
}

void GlxSlideShowWidget::resetSlideShow()
{
    TRACER("GlxSlideShowWidget::resetSlideShow()" );
	if(  mModel == NULL || mModel->rowCount() == 0 ) {
		return;
	}
	
	mSlideShowItemCount = mModel->rowCount();
	setFocusItemIcon() ;
	setNextItemIcon() ;
	setPreItemIcon() ;
	startSlideShow();
	
	qDebug( "GlxSlideShowWidget::resetSlideShow slide show item count %d" , mSlideShowItemCount );
	if ( mErrorNote && mErrorNote->isVisible() && mSlideShowItemCount > 1 ){
	    hideErrorNote();
	}
}

void GlxSlideShowWidget::moveImage(int nextIndex, int posX, const QString & move, char * callBack)
{
    TRACER("GlxSlideShowWidget::MoveImage()");

    if ( mSlideShowItemCount <= 1 || mEffectEngine->isEffectRuning( mItemList ) ) {
        return ;
    }
    
    mSlideTimer->stop();
    HbEffect::start( mIconItems[mItemIndex], QString("HbIconItem"), move );
    mIconItems[nextIndex]->setPos( posX, mScreenRect.top());
    mIconItems[nextIndex]->setOpacity(1);
    mIsAutoMoving = true;
    HbEffect::start(mIconItems[nextIndex], QString("HbIconItem"), QString("Move"), this, callBack );    
}

void GlxSlideShowWidget::addConnections()
{
    TRACER("GlxSlideShowWidget::addConnections()");
    if ( mEffectEngine )  {
        connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinshed() ) );
    }
    if ( mSlideTimer ) {
        connect( mSlideTimer, SIGNAL(timeout()), this, SLOT( triggeredEffect() ) );
    }
    if ( mContinueButton ) {
        connect( mContinueButton, SIGNAL( clicked(bool) ), this, SLOT( continueSlideShow(bool) ) );
    }
}

void GlxSlideShowWidget::removeConnections()
{
    TRACER("GlxSlideShowWidget::removeConnections()");
    if ( mEffectEngine )  {
        disconnect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinshed() ) );
    }
    if ( mSlideTimer ) {
        disconnect( mSlideTimer, SIGNAL(timeout()), this, SLOT( triggeredEffect() ) );
    }
    if ( mContinueButton ) {
        disconnect( mContinueButton, SIGNAL( clicked(bool) ), this, SLOT( continueSlideShow(bool) ) );
    }
}

int GlxSlideShowWidget::getFocusIndex( )
{
    QVariant variant = mModel->data( mModel->index( 0, 0 ), GlxFocusIndexRole ) ;
    if ( variant.isValid() && variant.canConvert< int > () ) {
        return variant.value< int > ();
    }
    return -1;
}

HbIcon GlxSlideShowWidget::getIcon( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert< HbIcon > () ) {
        return variant.value< HbIcon > () ;       
    }
    return HbIcon() ;
}

bool GlxSlideShowWidget::isCorrupt( int index )
{
    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxImageCorruptRole );
    if ( variant.isValid() && variant.canConvert< bool> () ) {
        return variant.value< bool > () ;
    }
    return false ;
}

bool GlxSlideShowWidget::setFocusItemIcon( )
{
    int nbrItem = mModel->rowCount();
    int focusIndex = getFocusIndex();
    
    for ( int i = 0; i < nbrItem ; ++i ) {
        if ( isCorrupt( focusIndex  ) == false ) {  
            qDebug( "GlxSlideShowWidget::setFocusItemIcon1 focus index %d" , focusIndex );
            mIconItems[ mItemIndex ]->setIcon( getIcon( focusIndex ) ) ;
            mSelIndex[ mItemIndex ] = focusIndex ;
            mModel->setData( mModel->index( 0, 0 ), focusIndex, GlxFocusIndexRole );
            mModel->setData( mModel->index( 0, 0 ), focusIndex, GlxVisualWindowIndex );
            return true;
        }
        focusIndex = ( focusIndex + 1 ) % nbrItem;
    }
    mSlideShowItemCount = 0;
    return false;    
}

bool GlxSlideShowWidget::setNextItemIcon( )
{
    int nbrItem = mModel->rowCount();
    int imageIndex = ( mSelIndex[ mItemIndex ] + 1 ) % nbrItem ;
    int itemIndex = ( mItemIndex + 1 ) % NBR_ITEM ;
    
    for( int i = 1; i < nbrItem; ++i ) {
       if ( isCorrupt( imageIndex ) == false ) {
           mIconItems[ itemIndex ]->setIcon( getIcon( imageIndex ) );
           mSelIndex[ itemIndex ] = imageIndex ;
           return true;
       }
       imageIndex = ( imageIndex + 1 ) % nbrItem ;
    }
    
    if ( isCorrupt( imageIndex ) ) {
        mSlideShowItemCount = 0;
    }
    else {
        mSlideShowItemCount = 1;
    }
    return false ;
}

bool GlxSlideShowWidget::setPreItemIcon()
{
    int nbrItem = mModel->rowCount() ;
    int imageIndex = mSelIndex[ mItemIndex ] > 0 ? mSelIndex[ mItemIndex ] - 1 : nbrItem - 1 ;
    int itemIndex = mItemIndex > 0 ? mItemIndex - 1 : NBR_ITEM - 1  ;
    
    for( int i = 1; i < nbrItem; ++i ) {
        if ( isCorrupt( imageIndex ) == false ) {
            mIconItems[ itemIndex ]->setIcon( getIcon( imageIndex ) ) ;
            mSelIndex[ itemIndex ] = imageIndex ;
            return true;        
        }
        imageIndex = imageIndex > 0 ? imageIndex - 1 : nbrItem - 1 ; 
    }
    if ( isCorrupt( imageIndex ) ) {
        mSlideShowItemCount = 0;
    }
    else {
        mSlideShowItemCount = 1;
    }
    return false;
}

void GlxSlideShowWidget::showErrorNote()
{
    if ( mErrorNote == NULL ){
        mErrorNote = new HbLabel( QString( GLX_NOIMAGE_PLAY_SLIDESHOW ), this);
        mErrorNote->setObjectName( "No Image" );
        mErrorNote->setGeometry( mIconItems[ 0 ]->geometry() );
        mErrorNote->setAlignment( Qt::AlignCenter );
    }
    
    for( int i = 0; i < NBR_ITEM; ++i ){
        mIconItems[ i ]->setVisible( false );
    }
    mErrorNote->setVisible( true );
    emit slideShowEvent( UI_ON_EVENT );
}

void GlxSlideShowWidget::hideErrorNote()
{
    for( int i = 0; i < NBR_ITEM; ++i ){
        mIconItems[ i ]->setVisible( true );
    }
    mErrorNote->setVisible( false );
    emit slideShowEvent( UI_OFF_EVENT );
}

