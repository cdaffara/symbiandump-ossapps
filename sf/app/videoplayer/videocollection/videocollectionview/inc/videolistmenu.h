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
* Description:   VideoListMenu class definition
*
*/

#ifndef VIDEOLISTMENU_H
#define VIDEOLISTMENU_H

#include <qobject.h>
#include <qmap.h>
#include <hbaction.h>

class TMPXItemId;
class VideoListView;
class VideoCollectionViewUtils;
class VideoProxyModelGeneric;
class VideoCollectionUiLoader;
class VideoListSelectionDialog;
class VideoServices;

/**
 * Class handles the menu for VideoListView.
 */
class VideoListMenu : public QObject
{
    Q_OBJECT

public:

    /**
     * Contructor.
     * @param uiLoader VideoCollectionUiLoader instance
     * @param parent VideoListView parent
     */
    VideoListMenu(VideoCollectionUiLoader* uiLoader, VideoListView* parent);

    /**
     * Destructor.
     *
     */
    ~VideoListMenu();

    /**
     * Creates and initializes menu and connects signals
     *
     * @return 0 if ok, < 0 if initialisation fails
     */
    int initializeMenu();

private:
    
    /**
     * Shows or hides a menu action.
     */
    void showAction(bool show, const QString &name);
    
private slots:
    
    /**
     * Slot is connected into main menus sort -items
     * Method checks sorting role based on active menu item and initiates sorting
     *
     */
    void startSorting();

    /**
     * Method checks the sorting role and starts sorting
     *
     */
    void doSorting(int role);
    
    /**
     * Slot is connected into main menus "delete items" (delete...) signal
     *
     * Calls ui utils to show multiple delete dialog for current widget
     *
     */
    void deleteItemsSlot();

    /**
     * Slot is connected into main menus "Create new collection..." signal
     * Shows a selection dialog for creating a new collection
     *
     */
    void createCollectionSlot();

    /**
     * Slot is connected into toolbar's  "Add videos" signal
     *
     */
    void addVideosToCollectionSlot();
    
    /**
     * Slot is connected into main menus aboutToShow -signal
     *
     */
    void aboutToShowMainMenuSlot();
    
    /**
     * Prepare menu when videos used through browsing service.
     */
    void prepareBrowseServiceMenu();

    /**
     * Slot which is called when an object has been loaded.
     */
    void objectReadySlot(QObject *object, const QString &name);
    
private:

    /**
     * Reference to video collection view utils
     */
    VideoCollectionViewUtils &mUiUtils;

    /**
     * Pointer to the XML UI (DocML) loader, not owned
     */
    VideoCollectionUiLoader* mUiLoader;
    
    /**
     * Pointer to VideoListView.
     * Not own.
     */
    VideoListView* mListView;
    
    /**
     * Pointer to videoservices instance
     * if exists, app has started as service
     */
    VideoServices* mVideoServices;

    /**
     * Sorting roles mapped to appropriate actions.
     */
    QMap<HbAction*, int> mSortingRoles;

};

#endif // VIDEOLISTMENU_H
