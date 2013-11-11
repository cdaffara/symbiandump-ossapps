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



#ifndef GLXSLIDESHOWWIDGET_H
#define GLXSLIDESHOWWIDGET_H

//Includes
#include <hbwidget.h>
#include <hbeffect.h>

//Qt/Orbit forward declarations
class QTimer;
class QGestureEvent;
class HbIconItem;
class HbMainWindow;
class HbPushButton;
class HbMainWindow;
class QGraphicsItem;
class HbDocumentLoader;
class HbAbstractDataModel;
class HbLabel;

//User Forward Declarations
class GlxEffectEngine;
class GlxSettingInterface;

#define NBR_ITEM 3
typedef enum
{
    UI_ON_EVENT, //send the signal when user tap on screen ( on the UI )
    UI_OFF_EVENT, //send the signal when user press continous button ( off the UI)
    EMPTY_DATA_EVENT, //send the signal when model have no data
    EFFECT_STARTED  // sends the signal when effect is started.
} GlxSlideShowEvent;

/**
 * Class Description
 * This is Slideshow widget class used to play the slide show animation and ahndle the user action like pause and play.
 */
class GlxSlideShowWidget : public HbWidget
{
Q_OBJECT

public :
    /**
     * Constructor
     * @param - QGraphicsItem
     */    
    GlxSlideShowWidget ( QGraphicsItem *parent = NULL );
    
    /**
     * Destructor
     */    
    ~GlxSlideShowWidget ();
    
    /**
     * setmodel() - To set the model of the widget.
     * @param - QAbstractItemModel.
     */    
    void setModel ( QAbstractItemModel *model );
    
    /**
     * setItemGeometry() - To Set the layout data of widgets item.
     * @param scrren gemoetry
     */
    void setItemGeometry( QRect screenRect );
    
    /**
     * startSlideShow() - To start the slide show timer to run the slide show.
     */
    void startSlideShow();
    
    /**
     * stopSlideShow() - To stop slide show timer.
     */
    void stopSlideShow();
    
    /*
     * setSlideShowWidget() - Initialise the slideshow widget and creation of the icons are done here.
     * @param - docloader
     */
    void setSlideShowWidget( HbDocumentLoader *DocLoader );
    
    /*
     * cleanUp() -Cleans up the slide show widget
     */
    void cleanUp();
    
    /*
     * animationItem() - Return the icon to use for some transition effect
     */
    QGraphicsItem * animationItem();
    
    /**
     * updateAnimationItem() - upadte the z value of anition item for smooth effect.
     */    
    void updateAnimationItem();
    
    
signals:
    /**
     * slideShowEvent() - This signal is emitted when slide show event happend lide ui on/off
     * @param - GlxSlideShowEvent.
     */
    void slideShowEvent( GlxSlideShowEvent e );
    
    /**
     * indexchanged() - This signal is emmitted when image selected index is changed. 
     */
    void indexchanged();
    
public slots :
    /**
     * triggeredEffect() - To start the slide show animation.
     */
    void triggeredEffect();

    /**
     * continueSlideShow() - Play the slide show.
     * @param - From HbAbstractButton.
     */
    void continueSlideShow( bool check );
    
    /**
     * effectFinshed() -  Call back to get notification of slide show animation has been finshed.
     */
    void effectFinshed();
    
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
     * orientationChanged() - relayout the data when orientation has been changed
     * @param - Screen Geometry
     */
    void orientationChanged( QRect screenRect );

    /**
     * leftMoveEffectFinished() - call back, when animation of browse the image in forward direction
     * has been finished.
     * @param - staus of effect
     */
    void leftMoveEffectFinished( const HbEffect::EffectStatus &status );

    /**
     * leftMoveEffectFinished() - call back, when animation of browse the image in backward direction
     * has been finished.
     * @param - staus of effect
     */
    void rightMoveEffectFinished( const HbEffect::EffectStatus &status );
    
protected slots :
    /**
     * leftGesture() - To handle the left move event
     * @parma number of pixel  move.
     */
    void leftGesture ( int value );
    
    /**
     * rightGesture() - To handle the right move event
     * @parma number of pixel  move.
     */
    void rightGesture ( int value );
    
protected :
    /**
     * gestureEvent() - gesture event handler.
     * @param - QGestureEvent
     */
    void gestureEvent( QGestureEvent *event );
    
private :
    /**
     * clearCurrentModel() - clear all the model call backs
     */
    void clearCurrentModel();
    
    /**
     * initializeNewModel() - register the model data change call backs
     */
    void initializeNewModel();
    
    /**
     * resetSlideShow() - Reinitialise the widget property.
     */
    void resetSlideShow();
    
    /**
     * moveImage() - Sopport function to handle the user browsing.
     * @param -  next selected image index.
     * @param - position of next image index.
     * @param - effect event
     * @param - animation finished callback function
     */
    void moveImage( int nextIndex, int posX, const QString & move, char * callBack );
    
    /**
     * addConnections() - register the internal and effect engine callback.
     */
    void addConnections();
	
    /**
     * removeConnections() - deregister the internal and effect engine callback.
     */
    void removeConnections();
    
    /**
     * cancelEffect() - To cancel the currnet effect running.
     */
    void cancelEffect();
    
    /**
     * pauseSlideShow() - Pause the slide show.
     */
    void pauseSlideShow();
    
    /**
     * getFocusIndex() -To get the focus index
     */
    int getFocusIndex( );

    /*
     * getIcon() - To get the full screen icon of the image
     * @param - index of the icon
     */
    HbIcon getIcon( int index );
    
    /**
     * isCorrupt() - To check the itemis corrupted or not
     * @param - index of the icon
     */
    bool isCorrupt( int index );
    
    /**
     * setFocusItemIcon() - To set the current ( focus ) item icon
     * @return - return the success or failure status
     */
    bool setFocusItemIcon();
    
    /**
     * setNextItemIcon() - To set the next itme icon in the list
     * @return - return the success or failure status
     */
    bool setNextItemIcon();
    
    /**
     * setPreItemIcon() - To set the previous icon in the list
     * @return - return the success or failure status
     */
    bool setPreItemIcon();
    
    /**
     * showErrorNote() - In the case of all the image are corrupted then show the error notes
     */
    void showErrorNote();
    
    /**
     * hideErrorNote() - It will hide the corrupted images note
     */
    void hideErrorNote();    

private:
    GlxEffectEngine          *mEffectEngine;
    GlxSettingInterface      *mSettings;               //no ownership
    HbIconItem               *mIconItems[ NBR_ITEM ]; 
    HbIconItem               *mAnimItem;
    HbIconItem               *mBackGroundItem;
    HbPushButton             *mContinueButton;
    HbLabel                  *mErrorNote ;               //when all the image are corrupted then show the no image label
    int                      mItemIndex;
    int                      mSelIndex[ NBR_ITEM ];
    QTimer                   *mSlideTimer;
    QAbstractItemModel       *mModel;
    QRect                    mScreenRect;
    QList <QGraphicsItem *>  mItemList;
    bool                     mIsPause;
    int                      mSlideShowItemCount;
    bool                     mIsAutoMoving;
};

#endif /* GLXSLIDESHOWWIDGET_H */
