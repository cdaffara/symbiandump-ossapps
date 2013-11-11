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

#ifndef GLXDETAILSVIEW_H
#define GLXDETAILSVIEW_H

//Includes
#include <glxview.h>
#include <QAbstractItemModel>

//Orbit/Qt forward declartion
class HbIcon;
class HbLabel;
class HbIconItem;
class HbListView;
class HbPushButton;

//User Defined forward declartion
class GlxDetailsIcon;
class GlxFavMediaModel;
class GlxDetailsTextEdit;
class GlxDetailsViewDocLoader;
class GlxDetailsViewModelWrapper;

/**
 * Class Description
 * This is Details view class .The functionality of details view is to show the detail of the image like
 * Name of the image ,Date the image was created , time the image was modified , size of the image
 * and show the description of the image.here in this view , the name of the image and description can be changed. 
 */
class GlxDetailsView : public GlxView
    {

Q_OBJECT

public:

    /*
     * class constructor.
     * @param - HbMainWindow object
     */
    GlxDetailsView(HbMainWindow *window);
    /*
     * class Destructor.
     */
    ~GlxDetailsView();

    /*
     * From GlxView
     * This is called from the view manager before the view is going to Activated.
     */
    void initializeView(QAbstractItemModel *model, GlxView *preView);

    /**
     * From GlxView
     * This is called from the view manager when the view is going to be activated.
     */
    void activate();

    /*
     * From GlxView
     * This is called from the view manager before the view is going to de-activated.
     */
    void resetView();

    /*
     * From GlxView 
     * This is called from the view manager when the view is going to be de-activated.
     */
    void deActivate();

    /*
     * From GlxView
     * Sets the model corresponding to this view.
     * This is called from the view manager before the view is going to Activated.
     * @Param - model is of type GlxMediaModel
     */
    void setModel(QAbstractItemModel *model);

    /*
     * //From GlxView
     * This is called from the view manager before the view is going to Activated.
     * return the view item for animation
     */
    QGraphicsItem * getAnimationItem(GlxEffect transtionEffect);

private:

    /*
     * cleanUp()
     * cleans up the resources used.
     */
    void cleanUp();

    /*
     * showImage()
     * Retrieves the image from the model and displays the image
     */
    void showImage();

    /*
     * setFavModel
     * This Function creates the favourite model, to determine if the image is added to favourite.
     */
    void setFavModel();

    /*
     * initializeNewModel()
     * Add all the model related connection and data
     */
    void initializeNewModel();

    /*
     * clearCurrentModel()
     * clear all the model releted connection and data
     */
    void clearCurrentModel();

    /*
     * setConnections()
     * connects the items to slot to respective signals
     */
    void setConnections();

    /*
     * clearConnections()
     * clear the connections
     */
    void clearConnections();

    /**
     * getSubState()
     * This Function retrieves the state of the details view, whether it launched from the Photos app
     * or whether it was launched from other app like File Manager, MMS , Camera ...
     */
    int getSubState();

private slots:

    /**
     * updateLayout()
     * This Loads the View/Section corresponding to the  Orientation.
     * @Param - Orientation , specifies the current Orientation.
     */
    void updateLayout(Qt::Orientation);

    /**
     * rowsRemoved()
     * This Slot is called when the item is removed from the model.
     * @Param - parent is of type QModelIndex.
     * @Param - start, this specifies the starting index of the item being removed. 
     * @Param - end ,this specifies the ending  index of the item being removed.
     */
    void rowsRemoved(const QModelIndex &parent, int start, int end);

    /**
     * modelDestroyed()
     * This Slot is called whenever the model is deleted.
     */
    void modelDestroyed();

    /**
     * dataChanged()
     * This Slot is called whenever the data in an existing item changes.
     * @Param - startIndex, this specifies the starting index of the  affected items.  
     * @Param - endIndex ,this specifies the ending  index of the  affected items .
     */
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);

    /**
     * refreshDetailsview()
     * This Slot is called  when the data shown in details view has to be refreshed.
     */
    void refreshDetailsview();

    /**
     * triggerTheCommand()
     * This Slot is called when the file details has to be changed like file name and description
     * @param - row, The Row specifies the row index of the widget in the list widget of details view.
     */
    void triggerTheCommand(int row);

    /**
     * updateFavourites()
     * This Slot is called whenever the image is added to Favourites
     */
    void updateFavourites();

    /**
     * launchShareDialog()
     * This Slot is called  when the image has to be sent via services like MMS, Bluetooth,Email...
     */
    void launchShareDialog();

private:

    //Contains the thumbnail shown in teh details view.
    HbLabel *mImageBackGround;

    //Contains the thumbnail shown in teh details view.
    HbLabel *mDetailsIcon;

    //contains the view instance 
    HbView *mView;
    
    //Contains the favourite icon which adds or removes the image to favourite folder
    GlxDetailsIcon *mFavIcon;

    //The media Model to acess the attributes; not owned,dont delete.
    QAbstractItemModel *mModel;

    //The media Model to acess the attributes whether image is in favourite folder or not;owned by this view
    GlxFavMediaModel *mFavModel;

    //The reference to the Main Window
    HbMainWindow *mWindow;

    int mSelIndex;

    //custom Document Loader to load the details view  
    GlxDetailsViewDocLoader *mDocLoader;

    //Contains the disabled favourite icon 
    HbIcon mFavIconEnabled;

    //Contains the enabled favourite icon 
    HbIcon mFavIconDisabled;

    //Contains the List view
    HbListView *mListView;

    //Contains the List model
    GlxDetailsViewModelWrapper *mListModel;

    //holds the share button
    HbPushButton *mShareButton;
    };
#endif //GLXDETAILSVIEW_H
