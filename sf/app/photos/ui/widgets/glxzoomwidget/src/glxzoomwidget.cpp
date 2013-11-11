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
* Description:   glxzoomwidget.cpp
*               description of the class GlxGlxZoomWidget which controls the Zoom behavior of coverflow.
*
*/
#include <QPinchGesture>
#include <hbiconitem.h>
#include <QTimeLine>
#include <QGesture>
#include <hbinstance.h>
#include "glximagedecoderwrapper.h"
#include "glxmodelparm.h"
#include "glxzoomwidget.h"

const int MAXZVALUE = 100;
const int MINZVALUE = 0;
const int NOOFSTEPSZI = 24;
const int NOOFSTEPSZO = 18;
const float MAXDTZOOMIN = 3.5;
const int NO_OF_STEPS_FOR_SPRINGBACK_AT_MAX = 12;
const int NO_OF_STEPS_FOR_SPRINGBACK_AT_MIN = 5;

GlxZoomWidget::GlxZoomWidget(QGraphicsItem *parent):HbScrollArea(parent), 
            mModel(NULL), mMinZValue(MINZVALUE), 
            mMaxZValue(MAXZVALUE), mTimerId(0),
            mImageDecodeRequestSend(false), 
            mPinchGestureOngoing(false), mDecodedImageAvailable(false),
            mZoomOngoing(false)
{
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::TapGesture);
    setAcceptTouchEvents(true) ;
    setFrictionEnabled(true);
    setZValue(mMinZValue);
    //create the child items and background
    mZoomWidget = new QGraphicsWidget(this);
    mZoomItem = new QGraphicsPixmapItem(mZoomWidget);
    mZoomItem->setTransformationMode(Qt::SmoothTransformation);
    //the black background
    //replace when a proper substitute for setting backgrounds is known
    mBlackBackgroundItem = new HbIconItem(this);
    mBlackBackgroundItem->setBrush(QBrush(Qt::black));
    mBlackBackgroundItem->hide();
    //does not work so is commented
    //setBackgroundItem(mBlackBackgroundItem);

    //initializing the image decoder
    mImageDecoder = new GlxImageDecoderWrapper;

    setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
	//AA: signal and slot to perform double tap animation
    //after every step redraw, signal is emitted to perform the next step
	connect( this, SIGNAL( stepZoom() ), this, SLOT( animateDoubleTap() ), Qt::QueuedConnection );
	mDoubleTap = false;
}

GlxZoomWidget::~GlxZoomWidget()
{
   
    //AA
    disconnect( this, SIGNAL( stepZoom() ), this, SLOT( animateDoubleTap()));
    //no Null checks required
    delete mZoomItem;
//    delete mZoomWidget; //as this is a content widegt it will automatically be deleted
    delete mBlackBackgroundItem;
    //reset the decoder to cancel pending tasks
    if(mImageDecoder) {
        resetDecoder();
        delete mImageDecoder;
    }
}
void GlxZoomWidget::resetDecoder()
{
        mImageDecoder->resetDecoder();
        mImageDecodeRequestSend = false;
        mDecodedImageAvailable = false;
}

void GlxZoomWidget::setModel (QAbstractItemModel *model)
{
    if(model)
    {
        mModel = model;
        retreiveFocusedImage(); //Update mZoomItem with focused Image
        connect( mModel, SIGNAL( fullScreenDataChanged( QModelIndex, QModelIndex ) ), this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
        connect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
    }
}

void GlxZoomWidget::setWindowSize(QSize windowSize)
{
    mWindowSize = windowSize;
    mBlackBackgroundItem->setGeometry(QRectF(QPointF(0,0), mWindowSize));
    //try to reset the max and min zoomed size here
    //In case the zoom widget is in background reset it
    if(!mZoomOngoing && mModel) {
        retreiveFocusedImage();
    }
    setZoomParams();
}

void GlxZoomWidget::forceZoomToBackground()
{
    mBlackBackgroundItem->hide();
    //push the widget back to background
    setZValue(mMinZValue);
    mZoomOngoing = false;
    emit zoomWidgetMovedBackground(mFocusIndex);
    //this actually resets the ZoomWidget and decoder
    if(mImageDecoder) {
        resetDecoder();
    }
    retreiveFocusedImage();

}

void GlxZoomWidget::indexChanged(int index)
{
    if(mFocusIndex != index) {
        resetDecoder();          //reset the decoder first to cancel pending tasks
        retreiveFocusedImage();  //Update mZoomItem with focused Image
    }
}

void GlxZoomWidget::cleanUp()
{
    disconnect(this,SIGNAL( pinchGestureReceived(int) ), this, SLOT( sendDecodeRequest(int) ) );
    if(mModel) {
        disconnect( mModel, SIGNAL( fullScreenDataChanged( QModelIndex, QModelIndex ) ), this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
        disconnect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
        mModel = NULL;
    }
    if(mImageDecoder) {
        resetDecoder();
    }
    mZoomItem->setPixmap(QPixmap());
	mBlackBackgroundItem->setParentItem(this);
}

void GlxZoomWidget::activate()
{
}

void GlxZoomWidget::setMinMaxZValue(int minZvalue, int maxZvalue)
{
    mMinZValue = minZvalue;
    mMaxZValue = maxZvalue;
}

void GlxZoomWidget::connectDecodeRequestToPinchEvent()
{
    connect(this,SIGNAL( pinchGestureReceived(int) ), this, SLOT( sendDecodeRequest(int) ), Qt::QueuedConnection );
}

bool GlxZoomWidget::sceneEvent(QEvent *event)
{
    bool consume(false);
    if (event->type() == QEvent::Gesture) {
        consume = executeGestureEvent(this, static_cast<QGestureEvent*>(event));
    }
    if(!consume)
    {
        consume = HbScrollArea::sceneEvent(event);
    }
    return consume;
}

bool GlxZoomWidget::sceneEventFilter(QGraphicsItem *watched,QEvent *event)
{
    qDebug( "GlxZoomWidget::sceneEventFilter enter event type %d ", event->type() );
    bool consume = false;
    if (event->type() == QEvent::Gesture) {
        consume = executeGestureEvent(watched, static_cast<QGestureEvent*>(event));
    }

    if(!consume) {
        consume = HbScrollArea::sceneEventFilter(watched,event);
    }
    return consume;

}

bool GlxZoomWidget::executeGestureEvent(QGraphicsItem *source,QGestureEvent *event)
{
         if(QTapGesture *gesture = static_cast<QTapGesture *>(event->gesture(Qt::TapGesture))) {        
            if (gesture->state() == Qt::GestureFinished) {
                if(!mTimerId) {
                    mTimerId = startTimer( DOUBLETAP_TIMEINTERVAL );
                }
            else {
                killTimer(mTimerId);
                mTimerId = 0;
                animateZoomOut(hbInstance->allMainWindows().first()->mapToScene(gesture->position().toPoint()));
            }
        }
        event->accept(gesture);
        return true;
    }
     if (QGesture *pinch = event->gesture(Qt::PinchGesture))  {
         if (isFocussedItemCorrupt()){
         return true;
         }
       QPinchGesture* pinchG = static_cast<QPinchGesture *>(pinch);
       QPinchGesture::ChangeFlags changeFlags = pinchG->changeFlags();
       if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            mPinchGestureOngoing = true;
            mZoomOngoing = true;
            
            //retreive the gesture values
            qreal value = pinchG->scaleFactor() / pinchG->lastScaleFactor();
            QPointF center = pinchG->property("centerPoint").toPointF();
            //set the gesture center to the scene coordinates
            QPointF sceneGestureCenter = hbInstance->allMainWindows().first()->mapToScene(center.toPoint());
            zoomImage(value, sceneGestureCenter);

        }
       if (pinchG->state() == Qt::GestureStarted) {
           emit pinchGestureReceived(mFocusIndex);
           //bring the zoom widget to foreground
           if(zValue()!=mMaxZValue)
           {
            setZValue(mMaxZValue);
            //show the black background
            mBlackBackgroundItem->setParentItem(parentItem());
            mBlackBackgroundItem->setZValue(mMaxZValue - 1);
            mBlackBackgroundItem->show();
           }
       }

       if (pinchG->state() == Qt::GestureFinished) {
               //For giving a spring effect when user has zoomed more than normal.
		  mPinchGestureOngoing = false;               
        //scale the image to limited size
		if(mCurrentSize.width() > mMaxScaleDecSize.width() )
        {
        	  QPointF center(mWindowSize.width()/2, mWindowSize.height()/2);
		       m_AnimRefPoint = source->sceneTransform().map(center);
		       springEffectAtMax();
               return true;
		}
       
		if(mCurrentSize.width() < mMinDecScaleSize.width())
        {
              QPointF center(mWindowSize.width()/2, mWindowSize.height()/2);
              m_AnimRefPoint = source->sceneTransform().map(center);
              springEffectAtMin();
              return true;
               //do not reset the transform here as it will then zoom-in the widget to decoded image size
        }
        //finalize the transforms to the geometry else panning will not work
        finalizeWidgetTransform();
       }
       //gesture accepted
       return true;
     }
     //gesture rejected
     if(!mPinchGestureOngoing) {
         return false; 
     }
     return true;

}
void GlxZoomWidget::springEffectAtMax()
{
    //set the no. of steps for double tap animation 
    mdoubletapSteps = NO_OF_STEPS_FOR_SPRINGBACK_AT_MAX;
    //initiale variable for double tap animation
    mIncSF = 1;
    
    //For giving a spring effect when user has zoomed out more than normal.
    //Zoom out to mMaxScaleDecSize from the currentsize
    m_FinalAnimatedScaleFactor = mMaxScaleDecSize.width()/mCurrentSize.width();
    //calculate the step increment SF for each step
    msfInc = (1 - m_FinalAnimatedScaleFactor)/(mdoubletapSteps);
    //preserve the size when zoom out was initiated, requried for calculates applicable/req scale factor
    minitSize = mCurrentSize;
    animateDoubleTap();
}

void GlxZoomWidget::springEffectAtMin()
{
      
        m_FinalAnimatedScaleFactor = mMinDecScaleSize.width()/mCurrentSize.width();
        //set the no. of steps for double tap animation 
        mdoubletapSteps = NO_OF_STEPS_FOR_SPRINGBACK_AT_MIN;
        //initiale variable for double tap animation
        mIncSF = 1;
        //SF has to always greater than 1 for upscaling, hence range for zoomout is [1,m_FinalAnimatedScaleFactor]
        msfInc = (m_FinalAnimatedScaleFactor-1)/mdoubletapSteps;
        //preserve the size when zoom out was initiated, requried for calculates applicable/req scale factor
        minitSize = mCurrentSize;
        animateDoubleTap();
       
}

void GlxZoomWidget::zoomImage(qreal zoomFactor, QPointF center)
{
        // Pinch event filtering for very small zoom factors
    if (qAbs(1.0 - zoomFactor) < 0.007) {
        return;
    }
    adjustGestureCenter(center, zoomFactor);
    QSizeF requiredSize(mCurrentSize.width()*zoomFactor, mCurrentSize.height()*zoomFactor);
    limitRequiredSize(requiredSize);
    if(requiredSize != mCurrentSize) {
        QTransform zoomTransform = mZoomWidget->transform();
        QPointF transformedCenter = mZoomWidget->sceneTransform().inverted().map(center);
        zoomTransform.translate(transformedCenter.x(),transformedCenter.y());
        zoomTransform.scale(requiredSize.width()/mCurrentSize.width(), requiredSize.height()/mCurrentSize.height());
        zoomTransform.translate(-transformedCenter.x(),-transformedCenter.y());
        mZoomWidget->setTransform(zoomTransform);
        mCurrentSize = requiredSize;
    }

}


void GlxZoomWidget::limitRequiredSize(QSizeF &requiredSize)
{
    if(requiredSize.width() > mMaxScaleSize.width() ) {
        requiredSize = mMaxScaleSize ;
    }
    else if(requiredSize.width() < mMinScaleSize.width() ) {
        requiredSize = mMinScaleSize ;
    }


}

//makes sure that the gesture is on the screen center if the image is smaller than the screen
void GlxZoomWidget::adjustGestureCenter(QPointF & gestureCenter, qreal& zoomFactor)
{
    
    //clip zoom factor for pinch zoom. Double tap zoomfactor should never exceed
    //1.2 or 0.8 in any given step  
/*    if(zoomFactor > 1.2 )  {
        zoomFactor = 1.2;
    }
    if(zoomFactor < 0.7 )   {
        zoomFactor = 0.8;
    }*/

    qDebug("AA::adjustGestureCenter::ZoomFactor (%f)",zoomFactor);
    QPointF itemOriginPos = mZoomWidget->sceneTransform().map(QPointF(0,0)); 
    QPointF gesCenter = mZoomWidget->sceneTransform().map(gestureCenter); 

    
    //keep smaller image centered irrespective of zoomin or zoom out
    //note, only if the image is smaller than window size, else preserve the 
    //gesture center. Adjustments need to be done for both height/width
    //only one of them will be applicable, unless the image is smaller than
    //fullscreen size on both dimensions
    if( (mCurrentSize.width() <= mWindowSize.width()))  {
       //requires adjustment only in portrait orientation 
       if(mWindowSize.width() > mWindowSize.height())
           gestureCenter.setX(mWindowSize.width()/2);
    }
    //handle the case when CurrentSize is grater than window size
    //this applies to zoomout flow
    else  {
        //when the image is positioned beyond the left edge of the window
        //clamp the image to left edge
        if(itemOriginPos.x() >= 0) {
            gestureCenter.setX(itemOriginPos.x());    
        }
        //same applies when the image needs clamping on right edge
        else if(itemOriginPos.x()+ mCurrentSize.width() <= mWindowSize.width()){
            gestureCenter.setX(itemOriginPos.x() + mCurrentSize.width());
        }
        //else no clamping is required, theg esture center can be preserved
        //as is for the zoom step
   }
        
    //same logic applied for Y axis
    if( (mCurrentSize.height() <= mWindowSize.height())  )  {
        //requires adjustment only in landscape orientation
        if(mWindowSize.width() < mWindowSize.height())
                   gestureCenter.setY(mWindowSize.height()/2);
    }     
    else  {
            if(itemOriginPos.y() >= 0) {
               gestureCenter.setY(itemOriginPos.y());    
            }
            else if(itemOriginPos.y()+ mCurrentSize.height() <= mWindowSize.height()){
                gestureCenter.setY(itemOriginPos.y() + mCurrentSize.height());
            }
        }
    
    //special case for images that are smaller on both sides
    //centering of the gesture is important to prevent movement of the image
    //while zoom in or out
    if( (mCurrentSize.height() <= mWindowSize.height()) && (mCurrentSize.width() <= mWindowSize.width()) ) {
        gestureCenter.setX(mWindowSize.width()/2);
        gestureCenter.setY(mWindowSize.height()/2);
    }
        
    //reduce the ZF so as to show a decelerated effect at max/min levels
/*
    if(mCurrentSize.width() > mMaxScaleDecSize.width() && zoomFactor > 1 ) {
        zoomFactor = 1.0 + ((zoomFactor-1.0)/6) ;
    }
        if(mCurrentSize.width() < mMinDecScaleSize.width() && zoomFactor < 1 ) {
        zoomFactor = 1.0 - ((1.0-zoomFactor)/6) ;
    }
*/

}

//get the latest focused image and set it to mZoomItem
void GlxZoomWidget::retreiveFocusedImage()
{

    QPixmap targetPixmap(getFocusedImage());
    //initialize all the variables wrt the focussed pixmap
    mZoomWidget->resetTransform();
    mItemSize = targetPixmap.size();
    mMaxScaleSize = mItemSize;
    mMaxScaleSize.scale(mWindowSize*13, Qt::KeepAspectRatio);
    mMaxScaleDecSize = mItemSize;
    mMaxScaleDecSize.scale(mWindowSize*7, Qt::KeepAspectRatio);
    mMinScaleSize = mItemSize* 0.7;
    mMinDecScaleSize = mItemSize;
    QPointF originPos = sceneTransform().map(QPointF(0,0));
    mZoomWidget->setGeometry(QRectF(QPointF(mWindowSize.width()/2 - mItemSize.width()/2,mWindowSize.height()/2 - mItemSize.height()/2),mItemSize )); //chk this
    mZoomWidget->setPreferredSize(mItemSize);
    mZoomItem->setPixmap(targetPixmap);
    mCurrentSize = mItemSize;
    mStepCurrentSize = mItemSize;
    setContentWidget(mZoomWidget);
    show();
}


void GlxZoomWidget::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    if(mFocusIndex >= startIndex.row() && mFocusIndex <= endIndex.row()) {
        //get the latest image from the model
        //will replace a decoded image if callback is received after decoded image is received so a fix is required
        //retreiveFocusedImage();
        if(!mDecodedImageAvailable)  {
        QPixmap targetPixmap(getFocusedImage());
        mItemSize = targetPixmap.size();
        mMaxScaleSize = mItemSize;
        mMaxScaleSize.scale(mWindowSize*13, Qt::KeepAspectRatio);
        mMaxScaleDecSize = mItemSize;
        mMaxScaleDecSize.scale(mWindowSize*7, Qt::KeepAspectRatio);
        mMinScaleSize = mItemSize* 0.7;
        mMinDecScaleSize = mItemSize;
        mCurrentSize = mItemSize;
        mZoomItem->setPixmap(targetPixmap);
        finalizeWidgetTransform();
        }
    }
}

void GlxZoomWidget::modelDestroyed()
{
    mModel = NULL ;    
}

void GlxZoomWidget::indexChanged()
    {
    retreiveFocusedImage();
    }

void GlxZoomWidget::decodedImageAvailable()
{
    //new bitmap with better resolution is available
    //so set it to the item
    QPixmap decodedPixmap = mImageDecoder->getPixmap();
    disconnect(mImageDecoder, SIGNAL(pixmapDecoded()), this, SLOT(decodedImageAvailable()));
    if(decodedPixmap.isNull()){
        return;
    }
    mDecodedImageAvailable = true;
    mZoomItem->setPixmap(decodedPixmap);
    mItemSize = decodedPixmap.size();
    //this is important if not done then old transforms will be applied on the new image
    finalizeWidgetTransform();
}

void GlxZoomWidget::sendDecodeRequest(int index)
{
    if(!mImageDecodeRequestSend) {
        QString imagePath = (mModel->data(mModel->index(index,0),GlxUriRole)).value<QString>();
        mImageDecoder->decodeImage(imagePath);
        connect(mImageDecoder, SIGNAL(pixmapDecoded()), this, SLOT(decodedImageAvailable()));
        mImageDecodeRequestSend = true;
    }
}


void GlxZoomWidget::finalizeWidgetTransform()
{
    QPointF widgetPos = mZoomWidget->sceneTransform().map(QPointF(0,0)); //Map the origin wrt scene
    mZoomWidget->resetTransform();
    mZoomWidget->scale(mCurrentSize.width()/mItemSize.width(), mCurrentSize.height()/mItemSize.height());
    mZoomWidget->setGeometry(QRectF(widgetPos , mCurrentSize));
    // this updates HbScrollArea on the sizeHint of ZoomWidget
    mZoomWidget->setPreferredSize(mCurrentSize);
    mStepCurrentSize = mCurrentSize;
}

QPixmap GlxZoomWidget::getFocusedImage()
{
    mFocusIndex = mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>();
    QVariant iconVariant = mModel->data(mModel->index(mFocusIndex,0),GlxFsImageRole);
    QVariant sizeVariant = mModel->data(mModel->index(mFocusIndex,0),GlxDimensionsRole);
    QPixmap targetPixmap;
    //retreive pixmap from the HbIcon received from model
    //should change the model to return and save pixmaps and convert to HbIcons Instead
    if ( iconVariant.isValid() &&  iconVariant.canConvert<HbIcon> () ) {
         QIcon itemIcon = iconVariant.value<HbIcon>().qicon();
         QSize itemSize = itemIcon.actualSize(mWindowSize);
         QSize scaleSize;
         if(sizeVariant.isValid() &&  sizeVariant.canConvert<QSize> ()) {
             scaleSize = sizeVariant.toSize();
             if(!(scaleSize.width() < mWindowSize.width() && scaleSize.height() < mWindowSize.height()))  {
                 scaleSize = mWindowSize;
             }
         }
         targetPixmap = itemIcon.pixmap(itemSize).scaled(scaleSize, Qt::KeepAspectRatio);
         mItemSize = targetPixmap.size();
    }
    return targetPixmap;

}

void GlxZoomWidget::setZoomParams()
{
    if (mModel)  {
        QVariant sizeVariant = mModel->data(mModel->index(mFocusIndex,0),GlxDimensionsRole);
        QSize fsSize;
        if(sizeVariant.isValid() &&  sizeVariant.canConvert<QSize> ()) {
            fsSize = sizeVariant.toSize();
            if(!(fsSize.width() < mWindowSize.width() && fsSize.height() < mWindowSize.height()))  {
                fsSize.scale( mWindowSize, Qt::KeepAspectRatio);
            }
            mMaxScaleSize = fsSize;
            mMaxScaleSize.scale(mWindowSize*13, Qt::KeepAspectRatio);
            mMaxScaleDecSize = fsSize;
            mMaxScaleDecSize.scale(mWindowSize*7, Qt::KeepAspectRatio);
            mMinScaleSize = fsSize* 0.7;
            mMinDecScaleSize = fsSize;
        }
    }


}



void GlxZoomWidget::animateZoomIn(QPointF animRefPoint)
{
    if (isFocussedItemCorrupt()){
    return;
    }
    emit pinchGestureReceived(mFocusIndex);
    //bring the zoom widget to foreground
    mZoomOngoing = true;
    setZValue(mMaxZValue);
    //show the black background
    mBlackBackgroundItem->setParentItem(parentItem());
    mBlackBackgroundItem->setZValue(mMaxZValue - 1);
    mBlackBackgroundItem->show();
    m_AnimRefPoint = animRefPoint;
    QSizeF requiredSize = mMinDecScaleSize;
    //MAXDTZOOMIN size is set to 3.5 times window size
    requiredSize.scale(mWindowSize*MAXDTZOOMIN, Qt::KeepAspectRatio);
	m_FinalAnimatedScaleFactor = requiredSize.width()/mMinDecScaleSize.width();
	//initiale variable for double tap animation
    mIncSF = 1;
    //preserve the size when zoom out was initiated, requried for calculates applicable/req scale factor
    //SF has to always greater than 1 for upscaling, hence range for zoomout is [1,m_FinalAnimatedScaleFactor]
    msfInc = (m_FinalAnimatedScaleFactor-1)/(NOOFSTEPSZI);
    //preserve the size when zoom out was initiated, requried for calculates applicable/req scale factor
    minitSize = mCurrentSize;
    //set the no. of steps for double tap animation 
    mdoubletapSteps = NOOFSTEPSZI;
    animateDoubleTap();

}
void GlxZoomWidget::animateZoomOut(QPointF animRefPoint)
{
    m_AnimRefPoint = animRefPoint;
    //Zoom out to FS (mMinDecScaleSize) from the currentsize
    m_FinalAnimatedScaleFactor = mMinDecScaleSize.width()/mCurrentSize.width();
    //initiale variable for double tap animation
    mIncSF = 1;
    //calculate the step increment SF for each step
    msfInc = (1 - m_FinalAnimatedScaleFactor)/(NOOFSTEPSZO);
    //preserve the size when zoom out was initiated, requried for calculates applicable/req scale factor
    minitSize = mCurrentSize;
    //set the no. of steps for double tap animation 
    //AA:: the no.of steps are kept the same for zoomin/zoomout, however tweaking them can be considered
    mdoubletapSteps = NOOFSTEPSZO;
    animateDoubleTap();
    //AA
   
}


void GlxZoomWidget::animateDoubleTap()
    {
    mDoubleTap = true;
    //calculate increamental scale factor based on the step and then calculate the applicable scale factor this step
    //increamental SF works on the ImageSize when double tap started, applicable(required) SF calculates the delate SF
    if(m_FinalAnimatedScaleFactor > 1) {
        //AA::zoomin case
        mIncSF += msfInc;
        qreal reqSF = (minitSize.width()*(mIncSF))/mCurrentSize.width();
        zoomImage(reqSF, m_AnimRefPoint);
        }
    if(m_FinalAnimatedScaleFactor < 1) {
        //AA::zoomout case
        mIncSF -= msfInc;
        qreal reqSF = (minitSize.width()* mIncSF)/mCurrentSize.width();
        zoomImage(reqSF, m_AnimRefPoint);
        }   
    //check if all steps are done,if not emit signal to continue the animation
    if(mdoubletapSteps > 1 ){
        mdoubletapSteps -= 1;
        emit stepZoom();    
        }
    else {
        //animation is complete, finalize the widget transform using setgeometry
        //reset the counter
        mdoubletapSteps = 0;
        animationTimeLineFinished();
        }
        
    }
void GlxZoomWidget::animationTimeLineFinished()
{
	finalizeWidgetTransform();
    //push the Zoom widget to background when zoomed image size nears FS image
	if(mDoubleTap && (mStepCurrentSize.width() <= mMinDecScaleSize.width()*1.3))  {
               mBlackBackgroundItem->hide();
               //push the widget back to background
               setZValue(mMinZValue);
               mZoomOngoing = false;
               emit zoomWidgetMovedBackground(mFocusIndex);
               //do not reset the transform here as it will then zoom-in the widget to decoded image size
			   mDoubleTap = false;
    }
	if(mCurrentSize.width() == mMinDecScaleSize.width()) {
	    	mBlackBackgroundItem->hide();
    	 	//push the widget back to background
	         setZValue(mMinZValue);
	         mZoomOngoing = false;
	         emit zoomWidgetMovedBackground(mFocusIndex);
	       
	}
}


void GlxZoomWidget::timerEvent(QTimerEvent *event)
{
    if(mTimerId == event->timerId())
    {
        killTimer(mTimerId);
        mTimerId = 0;
    }
    else {
        HbScrollArea::timerEvent( event );
    }
}

bool GlxZoomWidget::isFocussedItemCorrupt()
{
    QVariant variant = mModel->data( mModel->index( mFocusIndex, 0 ), GlxImageCorruptRole );
    if ( variant.isValid() && variant.canConvert< bool> () ) {
        return variant.value< bool > () ;
    }
    return false ;    
}

