/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:   ?Description
 *
 */

//Includes Qt

#include <QModelIndex>
#include <QDebug>

//Includes Orbit
#include <hblabel.h>
#include <hbinstance.h>
#include <hblistview.h>
#include <hbpushbutton.h>
#include <hbparameterlengthlimiter.h>

//User Includes
#include "glxlog.h"      //contains the declaration of GLX LOGS
#include "glxviewids.h"  //contains view ids used here 
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h" //contains the declaration of GlxModelParm used to check for favourites 
#include "shareuidialog.h" //contains the declaration for share ui/send dialogs
#include "glxdetailsicon.h" //contains the declaration of glxdetailsicon to show the image
#include "glxdetailsview.h" //contains the declaration of this file/class
#include "glxviewdocloader.h" //contains the declaration of GlxDetailsViewDocLoader
#include "glxfavmediamodel.h" //contains the declaration of GlxFavMediaModel,used to check for favourites
#include "glxdocloaderdefs.h" //contains the #define/constants of docml constants 
#include "glxcommandhandlers.hrh" //contains the declaration of rename and description command
#include "glxlocalisationstrings.h" //contains the localisation strings
#include "glxdetailsviewmodelwrapper.h" //contains the model wrapper for the media model of list widget
#include "glxdetailsviewitemprototype.h" //contains the custom widgets prototype decalaration
#include "glxcollectionpluginalbums.hrh" //contains declaration of KGlxCollectionPluginFavoritesAlbumId
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxdetailsviewTraces.h"
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) :
    GlxView(GLX_DETAILSVIEW_ID), mImageBackGround(NULL), mDetailsIcon(NULL), mView(NULL), mFavIcon(
        NULL), mModel(NULL), mFavModel(NULL), mWindow(window), mSelIndex(0), mDocLoader(NULL),
        mListView(NULL), mListModel(NULL), mShareButton(NULL)
{
    GLX_LOG_INFO("GlxDetailsView::GlxDetailsView");
    OstTraceFunctionEntry0( GLXDETAILSVIEW_GLXDETAILSVIEW_ENTRY );
    setContentFullScreen(true);//for smooth transtion between grid to full screen and vice versa
    OstTraceFunctionExit0( GLXDETAILSVIEW_GLXDETAILSVIEW_EXIT );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::~GlxDetailsView()
{
    OstTrace0( TRACE_IMPORTANT, GLXDETAILSVIEW_GLXDETAILSVIEW, "GlxDetailsView::~GlxDetailsView" );

    delete mDetailsIcon;
    mDetailsIcon = NULL;

    delete mImageBackGround;
    mImageBackGround = NULL;

    delete mView;
    mView = NULL;

    mDocLoader->reset();
    delete mDocLoader;
    mDocLoader = NULL;   

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeView(QAbstractItemModel *model, GlxView *preView)
{
    Q_UNUSED( preView )
    OstTraceFunctionEntry0( GLXDETAILSVIEW_INITIALIZEVIEW_ENTRY );
    bool loaded = false;

    if (!mDocLoader) {
        mDocLoader = new GlxDetailsViewDocLoader();
    }
    
    if ( mDocLoader ) {
        mDocLoader->load(GLX_DETAILSVIEW_DOCMLPATH, &loaded);

        if (!mView) {

            mView = static_cast<HbView*> (mDocLoader->findWidget(GLX_DETAILSVIEW_VIEW));
            mImageBackGround = static_cast<HbLabel*> (mDocLoader->findWidget(
                GLX_DETAILSVIEW_IMGBCKGROUND));
            mDetailsIcon = static_cast<HbLabel*> (mDocLoader->findWidget(GLX_DETAILSVIEW_IMAGE));
        }

        mFavIcon = static_cast<GlxDetailsIcon*> (mDocLoader->findWidget(GLX_DETAILSVIEW_FAVICON));
        mShareButton = static_cast<HbPushButton*> (mDocLoader->findWidget(GLX_DETAILSVIEW_SHAREBUTTON));
        mListView = static_cast<HbListView*> (mDocLoader->findWidget(GLX_DETAILSVIEW_LISTWIDGET));
    }

    //Set the Model
    mModel = model;

    //Set the Layout Correspondingly.
    updateLayout(mWindow->orientation());
        
    GlxDetailsViewItemProtoType *prototype = new GlxDetailsViewItemProtoType(mListView);
    mListView->setItemPrototype(prototype);
    mListModel = new GlxDetailsViewModelWrapper(mModel);
    mListView->setModel(mListModel);

    mImageBackGround->show();
    mDetailsIcon->show();
    
    setWidget(mView); 

    OstTraceFunctionExit0( GLXDETAILSVIEW_INITIALIZEVIEW_EXIT );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//activate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::activate()
{
    OstTraceFunctionEntry0( GLXDETAILSVIEW_ACTIVATE_ENTRY );

    mFavIconEnabled = HbIcon(GLXICON_ADD_TO_FAV);
    mFavIconDisabled = HbIcon(GLXICON_REMOVE_FAV);

    if (getSubState() != IMAGEVIEWER_DETAIL_S) {
        //set the favorite model to check if this image is in favourites folder or not
        setFavModel();
        mFavIcon->setItemIcon(mFavIconDisabled);
    }
    else {
        mFavIcon->hide();
    }

    //Initialze the Model
    initializeNewModel();

    //make the connections
    setConnections();

    //Set context mode to fetch descriptions
    mModel->setData(QModelIndex(), (int) GlxContextComment, GlxContextRole);
    
    //Set the Layout Correspondingly.
    updateLayout(mWindow->orientation());

    OstTraceFunctionExit0( GLXDETAILSVIEW_ACTIVATE_EXIT );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//resetView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::resetView()
{
    GLX_LOG_INFO("GlxDetailsView::resetView");

    //Remove the context created to update details
    if (mModel) {
        mModel->setData(QModelIndex(), (int) GlxContextComment, GlxRemoveContextRole);
    }

    //clear the connections
    clearConnections();
    //clear the model
    clearCurrentModel();

    delete mFavIcon;
    mFavIcon = NULL;

    delete mFavModel;
    mFavModel = NULL;

    delete mShareButton;
    mShareButton = NULL;

    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_RESETVIEW, "GlxDetailsView::resetView" );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//deActivate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::deActivate()
{
    GLX_LOG_INFO("GlxDetailsView::deActivate");
    OstTraceFunctionEntry0( GLXDETAILSVIEW_DEACTIVATE_ENTRY );

    mImageBackGround->hide();
    mDetailsIcon->hide();
    cleanUp();

    OstTraceFunctionExit0( GLXDETAILSVIEW_DEACTIVATE_EXIT );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setModel(QAbstractItemModel *model)
{
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETMODEL, "GlxDetailsView::setModel" );
    if (mModel == model) {
        return;
    }
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//getAnimationItem
//--------------------------------------------------------------------------------------------------------------------------------------------
QGraphicsItem * GlxDetailsView::getAnimationItem(GlxEffect transtionEffect)
{
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_GETANIMATIONITEM, "GlxDetailsView::getAnimationItem" );
    if (transtionEffect == FULLSCREEN_TO_DETAIL || transtionEffect == DETAIL_TO_FULLSCREEN) {
        return this;
    }
    return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//cleanUp
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::cleanUp()
{
    delete mListModel;
    mListModel = NULL;

    delete mListView;
    mListView = NULL;

    takeWidget();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//showImage
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::showImage()
{
    QVariant variant = mModel->data(mModel->index(0, 0), GlxFocusIndexRole);
    if (variant.isValid() && variant.canConvert<int> ()) {
        mSelIndex = variant.value<int> ();
    }

    variant = mModel->data(mModel->index(mSelIndex, 0), GlxFsImageRole);

    if (variant.isValid() && variant.canConvert<HbIcon> ()) {
        mDetailsIcon->setIcon(variant.value<HbIcon> ());
        mImageBackGround->setIcon(HbIcon(":/data/details.svg"));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setFavModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFavModel()
{
    GlxModelParm modelParm;
    QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
        GlxFocusIndexRole).value<int> (), 0), GlxUriRole)).value<QString> ();

    if (mFavModel == NULL) {
        modelParm.setCollection(KGlxCollectionPluginFavoritesAlbumId);
        modelParm.setContextMode(GlxContextFavorite);
        modelParm.setPath(imagePath);
        mFavModel = new GlxFavMediaModel(modelParm);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeNewModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeNewModel()
{
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_INITIALIZENEWMODEL, "GlxDetailsView::initializeNewModel" );

    if (mModel) {
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this,
            SLOT(rowsRemoved(QModelIndex,int,int)));
        connect(mModel, SIGNAL( updateDetailsView() ), this, SLOT( refreshDetailsview() ));
        connect(mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearCurrentModel()
{
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_CLEARCURRENTMODEL, "GlxDetailsView::clearCurrentModel" );

    if (mModel) {
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this,
            SLOT(rowsRemoved(QModelIndex,int,int)));
        disconnect(mModel, SIGNAL( updateDetailsView() ), this, SLOT( refreshDetailsview() ));
        disconnect(mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ));
        mModel = NULL;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setConnections
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setConnections()
{

    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(updateLayout(Qt::Orientation)));

    connect(mShareButton, SIGNAL(clicked()), this, SLOT(launchShareDialog()));

    if (getSubState() != IMAGEVIEWER_DETAIL_S) {

        connect(mFavIcon, SIGNAL(updateFavourites()), this, SLOT(updateFavourites()));

        connect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this,
            SLOT( dataChanged(QModelIndex,QModelIndex) ));

        //Connect the signals only in case of details view launched from photos app
        // and dont connect the signals if the details view is launched from MMS,File Manager
        // or other apps.
        connect((mListView->itemPrototypes()).at(0), SIGNAL(signalFromCustomWidget(int)), this,
            SLOT(triggerTheCommand(int)));

    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearConnections
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearConnections()
{
    GLX_LOG_INFO("GlxDetailsView::clearConnections ");

    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(updateLayout(Qt::Orientation)));

    if (mModel && getSubState() != IMAGEVIEWER_DETAIL_S) {

        disconnect((mListView->itemPrototypes()).at(0), SIGNAL(signalFromCustomWidget(int)), this,
            SLOT(triggerTheCommand(int)));

        disconnect(mShareButton, SIGNAL(released()), this, SLOT(launchShareDialog()));

        disconnect(mFavIcon, SIGNAL(updateFavourites()), this, SLOT(updateFavourites()));

        disconnect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this,
            SLOT( dataChanged(QModelIndex,QModelIndex) ));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//getSubState
//--------------------------------------------------------------------------------------------------------------------------------------------
int GlxDetailsView::getSubState()
{
    int substate = NO_DETAIL_S;

    if (mModel) {
        QVariant variant = mModel->data(mModel->index(0, 0), GlxSubStateRole);

        if (variant.isValid() && variant.canConvert<int> ()) {
            substate = variant.value<int> ();
        }
    }
    return substate;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateLayout
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateLayout(Qt::Orientation orient)
{
    GLX_LOG_INFO("GlxDetailsView::updateLayout ");
    bool loaded = false;
    QString section;

    //Load the widgets accroding to the current Orientation
    if (orient == Qt::Horizontal) {
        section = GLX_DETAILSVIEW_LSSECTION;
    }
    else {
        section = GLX_DETAILSVIEW_PTSECTION;
    }

    //Load the Sections
    mDocLoader->load(GLX_DETAILSVIEW_DOCMLPATH, section, &loaded);

    showImage();
    updateGeometry();

    GLX_LOG_INFO1("GlxDetailsView::updateLayout =%d\n",loaded);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//rowsRemoved
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_ROWSREMOVED, "GlxDetailsView::rowsRemoved" );

    if (mModel->rowCount() <= 0) {
        return emit actionTriggered(EGlxCmdEmptyData);
    }

    if (start <= mSelIndex && end >= mSelIndex) {
        return emit actionTriggered(EGlxCmdBack);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//modelDestroyed
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::modelDestroyed()
{
    mModel = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//dataChanged
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    Q_UNUSED(endIndex);

    QVariant variant = mFavModel->data(startIndex, GlxFavorites);
    if (variant.isValid() && variant.canConvert<bool> ()) {
        if (variant.value<bool> ()) {
            mFavIcon->setItemIcon(mFavIconEnabled);
        }
        else {
            mFavIcon->setItemIcon(mFavIconDisabled);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//refreshDetailsview
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::refreshDetailsview()
{
    GLX_LOG_INFO("GlxDetailsView::refreshDetailsview ");
    mListModel->refreshCustomWidget();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//triggerTheCommand
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::triggerTheCommand(int row)
{
    GLX_LOG_INFO("GlxDetailsView::triggerTheCommand ");

    if (row == GlxDetailsViewItemProtoType::widgetNameIndex) {
        emit actionTriggered(EGlxCmdRenameFile);
    }
    else {
        emit actionTriggered(EGlxCmdComment);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateFavourites
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateFavourites()
{
    QVariant variant = mFavModel->data(mFavModel->index(0, 0), GlxFavorites);
    if (variant.isValid() && variant.canConvert<bool> ()) {
        if (variant.value<bool> ()) {
            emit actionTriggered(EGlxCmdRemoveFromFav);
        }
        else {
            emit actionTriggered(EGlxCmdAddToFav);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//launchShareDialog
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::launchShareDialog()
{
    GLX_LOG_INFO("GlxDetailsView::launchShareDialog ");
    emit actionTriggered(EGlxCmdSend);
}

