/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoListToolbar class definition
*
*/

#ifndef VIDEOLISTTOOLBAR_H
#define VIDEOLISTTOOLBAR_H

#include <qobject.h>
#include <hbaction.h>
#include <videocollectioncommon.h>
#include <videocollectionviewutils.h>

class QActionGroup;
class QAction;
class HbToolBarExtension;
class VideoServices;
class VideoOperatorService;
class VideoCollectionUiLoader;
class VideoListView;

/**
 * Class controls the visible toolbar actions.
 */
class VideoListToolbar: public QObject
{
    Q_OBJECT

public:

    /**
     * Contructor.
     */
    VideoListToolbar(VideoCollectionUiLoader* uiLoader, VideoListView* parent);

    /**
     * Destructor.
     */
    ~VideoListToolbar();
    
    /**
     * Creates needed internal variables. Should be called before this class is used.
     * Note that the service information should be set in the VideoCollectionViewUtils
     * before this is called for correct operation.
     */
    void initialize();

signals:
    
    /**
     * Emitted when actions for toolbar have been changed.
     */
    void actionsChanged(QList<QAction*> actions);
    
    /**
     * Emitted when need changes to toolbar extension.
     */
    void toolbarExtensionChanged(HbToolBarExtension* extension);
    
    /**
     * Emitted when all videos action is triggered.
     */
    void allVideosActionTriggered();
    
    /**
     * Emitted when collection view action is triggered.
     */
    void collectionViewActionTriggered();
    
public slots:
    
    /**
     * Should be called, whenever state of the view changes, that
     * could affect the toolbar actions.
     * 
     * @param currentLevel current view level.
     * @param noVideos if list has videos or not.
     * @param modelReady if the model is ready or not.
     */
    void viewStateChanged(VideoCollectionCommon::TCollectionLevels currentLevel, 
        bool noVideos, bool modelReady);

    /**
     * Slot is connected into "Add videos" signal
     *
     */
    void addVideosToCollectionSlot();
    
    /**
     * Slot is connected into "remove videos" signal
     */
    void removeVideosFromCollectionSlot();
    
private:
    
    /**
     * Creates actions for toolbar.
     */
    void createToolbarActions();
    
    /**
     * Loads video services from central respository and creates toolbar buttons for them.  
     */
    void createOperatorServicesToolbarActions();
    
    /**
     * Loads video service from Central Repository and stores it into member array.
     * 
     * @param titleKey CenRep key for service title.
     * @param iconKey CenRep key for icon resource.
     * @param uriKey CenRep key for service URI.
     * @param uidKey CenRep key for service application UID.
     */
    void loadOperatorService(int titleKey, int iconKey, int uriKey, int uidKey);
    
    /**
     * Creates action with given parameters. createActionGroup() must be called successfully
     * before using this method.
     *
     * @param tooltip Tooltip text for the action.
     * @param icon Filepath for the icon file.
     * @param actionGroup Actiongroup for created action.
     * @param slot Slot for the triggered signal of the action.
     * @return HbAction pointer if creation ok, otherwise 0
     */
    HbAction* createAction(QString icon, QActionGroup* actionGroup, const char *slot);
    
private:

    /**
     * Actions ids used in tool bar
     */
    enum TViewActionIds
    {
        ETBActionAllVideos     = 10,
        ETBActionCollections   = 11,
        ETBActionServices      = 12,
        ETBActionAddVideos     = 13,
        ETBActionRemoveVideos  = 14
    };
    
    /**
     * Holds the current level state.
     */
    VideoCollectionCommon::TCollectionLevels mCurrentLevel;
    
    /**
     * Reference to video collection view utils
     */
    VideoCollectionViewUtils &mUiUtils;
    
    /**
     * Pointer to the XML UI (DocML) loader.
     * Not owned
     */
    VideoCollectionUiLoader* mUiLoader;
    
    /**
     * Pointer to parent VideoListView.
     * Not own.
     */
    VideoListView* mListView;
    
    /**
     * Pointer to videoservices instance
     * if exists, app has started as service
     */
    VideoServices* mVideoServices;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarViewsActionGroup;

    /**
     * Action group for the toolbar.
     */
    QActionGroup* mToolbarCollectionActionGroup;

    /**
     * Map containing toolbar actions
     */
    QMap<TViewActionIds, HbAction*> mToolbarActions;
    
    /**
     * Actions for different view states
     */
    QHash<VideoCollectionCommon::TCollectionLevels, QList<QAction*> > mViewStateActions;

    /**
     * Toolbar extension for operator services when there's more than
     * one of them.
     */
    HbToolBarExtension *mToolbarServiceExtension;
    
    /**
     * List of operator services.
     */
    QList<VideoOperatorService *> mVideoOperatorServices;
};

#endif // VIDEOLISTTOOLBAR_H
