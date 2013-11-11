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
* Description:   Videolist view class definition
*
*/

#ifndef VIDEOLISTVIEW_H
#define VIDEOLISTVIEW_H

#include <hbview.h>
#include <qnamespace.h>
#include <hbaction.h>

class QGraphicsItem;
class QVariant;
class HbToolBarExtension;
class TMPXItemId;
class VideoListWidget;
class VideoCollectionViewUtils;
class VideoCollectionWrapper;
class VideoCollectionUiLoader;
class VideoServices;
class VideoListToolbar;
class VideoListMenu;

/**
 * Class acts as an container for widgets that are used to display different
 * data: all videos, video collections or video services.
 *
 * Class is also responsible to mainatain correct widget active selected by the user's
 * tap from the toolbar and creating and maintaining main menu commonly used by all widgets.
 *
 */
class VideoListView : public HbView
{
    Q_OBJECT

public:

    /**
     * Contructor.
     * @param uiLoader VideoCollectionUiLoader instance for this view
     * @param parent parent of this view
     */
    VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent = 0);

    /**
     * Destructor.
     *
     */
    ~VideoListView();

    /**
     * Creates and initializes view objects, toolbar and menu and connects signals
     *
     * @return 0 if ok, < 0 if intialisation fails
     */
    int initializeView();

    /**
     * Activates current widget to be shown, enables menu and connects
     * orientation change signals.
     *
     * @param itemId, Id of the widget which is to be activated
     * @return 0 activation ok, < 0 if activation fails
     */
    int activateView(TMPXItemId &itemId);

    /**
     * Deactivates current widget, disables menu and disconnects
     * orientation change signals.
     *
     */
    void deactivateView();

    /**
     * Back implementation.
     * Not used at the moment
     *
     */
    void back();
    
    /**
     * Returns pointer to currently active VideoListWidget. Null if no 
     * active widget yet.
     * 
     * @return Currently active VideoListWidget, or null if no widget active.
     */
    VideoListWidget* getCurrentList();

signals:

    /**
     * Command signal. Should be connected to the
     * plugin interface's command -signal in order to
     * get the emitted commands transported uotside plugin.
     *
     * @param int command id
     */
    void command(int);
    
    /**
     * Signaled to do delayed loading of components not loaded initially at start up phase
     *
     */
    void doDelayeds();
    
    /**
     * Signaled when view is ready.
     */
    void viewReady();

private slots:

    /**
     * slot is connected to model's modelReady -signal
     */
    void modelReadySlot();
    
    /**
     * slot is connected to model's albumListReady -signal
     */
    void albumListReadySlot();
    
    /**
     * slot is connected to plugin's doDelayeds -signal
     *
     */
    void doDelayedsSlot();
    
    /**
     * slot is connected to service's titleReady -signal
     */
	void titleReadySlot(const QString& title);

     /**
     * Slot is connected to model slots informing changes in item count
     * Method checks the current model state and updates accrodingly
     *
     */
	void layoutChangedSlot();

    /**
     * Slot is connected into toolbar's all videos tab's
     * triggered signal.
     *
     * Activates all videos widget by calling changeWidget.
     *
     */
    void openAllVideosViewSlot();  

    /**
     * Slot is connected into toolbar's video collection tab's
     * triggered signal.
     *
     * Activates video collection widget by calling changeWidget.
     *
     */
    void openCollectionViewSlot();
    
    /**
     * Slot is connected into hbInstance's primary window's
     * aboutToChangeOrientation -signal. This is called when
     * orientation is to be change.
     */
    void aboutToChangeOrientationSlot();

    /**
     * Slot is connected into hbInstance's primary window's
     * aboutToChangeOrientation -signal. This is called when
     * orientation is changed.
     *
     * @param orientation new orientation
     */
    void orientationChangedSlot( Qt::Orientation orientation );

    /**
     * Slot is connected into viewdollectionwrapper's asyncStatus -signal
     * If status is failed delete, refiltering is called to model before
     * error message is shown.
     *
     * @param statusCode code of error
     * @param additional additional data of status
     */
    void handleAsyncStatusSlot(int statusCode, QVariant &additional);

    /**
     * Slot is connected to videolistwidgets collectionOpened -signal
     *
     * @param openingCollection
     * @param collection contains the name of the collection opened
     */
    void collectionOpenedSlot(bool openingCollection,
        const QString& collection,
        const TMPXItemId &collectionId);
        
    /**
     * Slot which is called when an object has been loaded.
     */
    void objectReadySlot(QObject *object, const QString &name);
    
    /**
     * Slot which is called when actions in toolbar needs to be changed.
     */
    void toolbarActionsChanged(QList<QAction*> newActions);
    
    /**
     * Slot which is called when toolbar extension needs to be changed.
     */
    void toolbarExtensionChanged(HbToolBarExtension* newExtension);
    
private:
    /**
     * Convenience method that modelReadySlot and albumListReadySlot calls.
     */
    void modelReady();
    
    /**
     * Cleans all possibly created objects from this. In some cases there are no quarantees
     * that they were created correctly, and thus is better to start again from clean slate.
     */
    void cleanup();

    /**
    * Method creates 3 tabs for the view's toolbar: all videos, collections and Services.
    * Tabs' icons are loaded and theiur triggered signals are connected into corresponding slots.
    *
    * @return 0 creation ok, < 0 creation fails
    */
    int createToolbar();

    /**
     * Shows or hides the hint. Only shows the hint if model does not have any
     * items.
     * 
     * @param show Set this to false if you want to force hide the hint.
     */
    void showHint(bool show = true);
    
    /**
     * Updates the sublabel text.
     */
    void updateSubLabel();
    
    /**
     * Activates all videos or collections -list.
     * 
     * @return int 0 ok 
     */
    int activateMainView();
    
    /**
     * Activate to collection content view when servicing.
     * Only default collections are supported. If some other
     * id is provided, all videos  -list will be activated 
     * 
     * @param itemId. Id of collection to activate
     * @return int 
     */
    int activateCollectionContentView(const TMPXItemId &itemId);
    
private:

    /**
     * Reference to video collection view utils
     */
    VideoCollectionViewUtils &mUiUtils;

    /**
     * Reference to videocollectionwrapper
     */
    VideoCollectionWrapper &mWrapper;

    /**
     * Pointer to the XML UI (DocML) loader, not owned
     */
    VideoCollectionUiLoader* mUiLoader;
    
    /**
     * Toolbar handler.
     */
    VideoListToolbar* mToolbar;
    
    /**
     * Menu handler.
     */
    VideoListMenu* mMenu;
    
    /**
     * Boolean for knowing when the model is ready.
     */
    bool mModelReady;
    
    /**
     * Boolean for knowing when the view is ready.
     */
    bool mViewReady;

    /**
     * Pointer to videoservices instance
     * if exists, app has started as service
     */
    VideoServices* mVideoServices;

    /**
     * Currently used list
     */
    VideoListWidget* mCurrentList;

    /**
     * String containing the name of the currently open collection
     */
    QString mCollectionName;
};

#endif // VIDEOLISTVIEW_H
