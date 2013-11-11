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



#ifndef GLXCOVERFLOW_H
#define GLXCOVERFLOW_H

#define NBR_ICON_ITEM 5

#include <hbeffect.h>
#include <hbwidget.h>

//forward declaration
class HbIconItem;
class HbMainWindow;
class QAbstractItemModel;
class QGestureEvent;

typedef enum
{
    NO_MOVE,
    TAP_MOVE,
    LEFT_MOVE,
    RIGHT_MOVE
} GlxUserMove;

typedef enum
{
    TAP_EVENT, //send the signal when user tap on full screen
    PANNING_START_EVENT, //send the signal when panning of full screen start
    EMPTY_ROW_EVENT, //send the signal when model have no data
    ZOOM_START_EVENT
} GlxCoverFlowEvent;

/**
 * Class Description
 * This is cover flow class to show and browse the image in fullscreen mode.
 */
class GlxCoverFlow : public HbWidget
{
Q_OBJECT

public :
    /**
     * Constructor
     * @param - QGraphicsItem parent of this widget
     */    
	GlxCoverFlow (QGraphicsItem *parent = NULL);
	
    /**
     * Destructor
     */	
	~GlxCoverFlow ();
	
	/**
	 * setItemSize() - To set the fullscreen image size.
	 * @param - size of fullscreen.
	 */
    void setItemSize( QSize &size );
    
    /**
     * setModel() - To set the data provider of the widget.
     * @param - data model pointor 
     */
    void setModel( QAbstractItemModel *model );
    
    /**
     * indexChanged() - Call back to get the focus index change.
     * @param - new focus index.
     */
    void indexChanged( int index );
    
    /**
     * setUiOn() - Call back to get the ui on/off status.
     * @param - ui on/off status
     */
    void setUiOn( bool uiOn ) { mUiOn = uiOn; }
    
    /**
     * partiallyClean() - To clean all the item except focus one for improved the
     * view transition animation.
     */
    void partiallyClean();
    
    /*
     * partiallyCreate() - To update the focus item only for improved the view transition
     * animtaion.
     * @param - data model pointer
     * @param - size of fullscreen.
     * @param - image pos in the screen. 
     */
    void partiallyCreate( QAbstractItemModel *model, QSize itemSize, int posY = 0 );
	
    /**
	 * setCoverFlow() - To initialise the coverflow.
	 */
    void setCoverFlow();
    
    /**
     * ClearCoverFlow() - To cleanup the memory and connection.
     */
    void ClearCoverFlow();
    
    /**
     * setMultitouchFilter(() - To Set the widget who is interested in multi touch
     * gesture event.
     * @param - widget pointer
     */
	void setMultitouchFilter( QGraphicsItem* multitouchFilter );
	
	/**
     * getFocusIndex() - To return the focus index
     * @return - focus index.
     */
    int getFocusIndex( );

    /**
     * getIcon() - to retuen the full screen icon of the image
     * @param  - index of imges which icon is required.
     * @return - icon of the image.
     */
    HbIcon getIcon( int index );
		    
public slots:
    /**
     * zoomStarted() - Call back about user has start the zooming.
     * @param - focus index.
     */
	void zoomStarted( int index );
	
	/**
	 * zoomFinished() - Call back about user has finished the zoom and come back 
	 * to coverflow.
	 *  @param - focus index.
	 */
	void zoomFinished( int index );

    /**
     * setOrientChangeAnim() - To set the playing status of orientation change animation
     * to coverflow.
     *  @param - focus index.
     */	
	void setOrientChangeAnim( bool isPlaying ) { mIsOrientChangeAnim = isPlaying ; }

signals :
    /**
     * coverFlowEvent() - To emit signal about coverflow event happen like panning, zomming ( For ui off ),
     * Tap ( for ui on/off ) etc.
     * @param - type of event
     */
    void coverFlowEvent( GlxCoverFlowEvent e );
    
    /**
     * changeSelectedIndex() - To emit the signal about focus inde change
     * @param - new focus index
     */
    void changeSelectedIndex( const QModelIndex &index );
    
    /**
     * autoLeftMoveSignal() - Internal signal to move the next image with transition animation.
     */
    void autoLeftMoveSignal();
    
    /**
     * autoRightMoveSignal() - Internal signal to move the previous image with transition animation.
     */
    void autoRightMoveSignal();
    
    /**
     * doubleTapEventReceived() - To emit the signal about double tap happen.
     * @param - coordinate of double tap postion with respect of view.
     */
    void doubleTapEventReceived( QPointF position );
    
    /**
     * moveNextSignal() - Internal signal to handle the buffering logic when user move
     * to prevoius image.
     */
    void moveNextSignal();
    
    /**
     * movePreviousSignal() - Internal signal to handle the buffernig loigc when user move 
     * to previous image.
     */
    void movePreviousSignal();

protected slots:
    /**
     * panGesture() - To handle the pan gesture event.
     * @param -  user move delta postion.
     */
    void panGesture ( const QPointF & delta ) ;
    
    /**
     * dataChanged() - call back to monitor the widget data changed.
     * @param - start index of data changed.
     * @param - end index of data changed.
     */
    void dataChanged( QModelIndex startIndex, QModelIndex endIndex );
    
    /**
     * rowsInserted() - call back of new row inserted in the model.
     * @param - Items are inserted under parent.
     * @param - start index of items inserted.
     * @param - end index of items removed.
     */    
    void rowsInserted( const QModelIndex &parent, int start, int end );
    
    /**
     * rowsRemoved() - call back of new row removed in the model.
     * @param - Items are removed from parent item.
     * @param - start index of items inserted.
     * @param - end index of items removed.
     */    
    void rowsRemoved( const QModelIndex &parent, int start, int end );
    
    /**
     * modelDestroyed() - call back to monitor the model destroy.
     */
    void modelDestroyed();

    /**
     * autoLeftMove() - To play the next image move and bounce back transition animation.
     */
    void autoLeftMove();
    
    /**
     * autoRightMove() - To play the previous image move and bounce back transition animation.
     */
    void autoRightMove();
    
    /**
     * moveNextImage() - To Handle user next image move for buffering logic, emit the signal about
     * new focus index and set some widget properties.
     */
    void moveNextImage();
    
    /**
     * movePreviousImage() - To Handle user pervious image move for buffering logic, emit the signal about
     * new focus index and set some widget properties.
     */    
    void movePreviousImage();

protected:
    /**
     * gestureEvent() - override function to handle the gesture event.
     * @param - pointer of QGestureEvent
     */
	void gestureEvent( QGestureEvent *event );
	
	/**
	 * move() - Change the postion of items for transition animation.
	 * @parma - delta move.
	 */
    void move( int value );
    
    /**
     * setRows() - set the number of rows.
     */
    void setRows() ;
    
    /**
     * setStripLen() - set the virtual strip length of coverflow.
     */
    void setStripLen();
    
    /**
     * calculateIndex() - For cyclic cover flow map the index to actual item index.
     * @param - index of the index.
     */
    int calculateIndex(int index);
    
    /**
     * loadIconItems() - Set the icon, size and postion of the icon items.
     */
    void loadIconItems (); 
    
    /**
     * updateIconItem() - Set the icon, size and postion of the a particular item.
     * It is used to support buffering logic and when user move to next or previous image.
     * @param - image index.
     * @param - item index in array.
     * @param - position of the item.
     */
    void updateIconItem ( qint16 selIndex, qint16 selIconIndex, qint16 posX );
    
    /**
     * playAnimation() - In the case of animated image, it will play the animation for focus image
     */
    void playAnimation();
    
    /**
     * stopAnimation() - To stop the animation
     */
    void stopAnimation();
    
    /*
     * clearCurrentModel() - clear all the model connection
     */
    void clearCurrentModel();
    
    /*
     * initializeNewModel() - add the connection to the model
     */
    void initializeNewModel();
    
    /*
     * resetCoverFlow() - reset all the data of cover flow
     */    
    void resetCoverFlow();
    
    /**
     * getSubState() - To get the substate of fullscreen state.
     * @return - return the substate of fullscreen state.
     */
    int getSubState();
    
    /**
     * timerEvent() - To handle the double tap timer
     * @param - pointer of QTimerEvent.
     */
    void timerEvent( QTimerEvent *event );
    
    /**
     * getUri() - To get the URI of the image.
     * @param - image index.
     * @return - return the uri of the image.
     */
    QString getUri( int index );

    /**
     * getImageDimension - To get the dimension of the image.
     * @param - image index.
     * @return - return the dimension of the image.
     */    
    QSize getImageDimension( int index );
    
    /**
     * getImageSize() - To get the image size.
     * @param - image index.
     * @return - return the size of the image.
     */
    int getImageSize( int index ); 
    
    /**
     * isAnimatedImage() - To get the GIF file info of the image
     * @param - image index.
     * @return - true if it GIF image else false.
     */
    bool isAnimatedImage( int index );
    
private:
	HbIconItem *mIconItem[NBR_ICON_ITEM];      //at most contain only five item
    qint16 mSelItemIndex;                    // current full screen index
    qint16 mRows;                        // total number of item  
    qint16 mSelIndex;                    // remove once model concept is integreted
    qint32 mStripLen;                    //virtual strip lenght
    qint32 mCurrentPos;                   //current postion in the virtual strip
    QSize mItemSize;                     //hb icon item size
    bool mUiOn;
    int mBounceBackDeltaX;
    QAbstractItemModel *mModel;
    GlxUserMove mMoveDir;
    int mSpeed;        
	bool mZoomOn;
	QGraphicsItem* mMultitouchFilter;
    int mTimerId;
    bool mIsInit;
    bool mIsAutoMoving;
    bool mIsOrientChangeAnim;                 //To check status of orientation change animation.
};

#endif /* GLXCOVERFLOW_H_ */
