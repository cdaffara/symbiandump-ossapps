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
* Description:   VideoCollectionClient class definition*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

#include <vcxmyvideosdefs.h>
#include <QList>
#include "videocollectioncommon.h"

// FORWARD DECLARATIONS

class MMPXCollectionUtility;
class VideoDataSignalReceiver;
class VideoCollectionListener;

// CLASS DECLARATION

/**
 * Client class for My Videos MPX Collection.
 *
 * @lib videocollectionwrapper.dll
 */
class VideoCollectionClient 
{

public: 
    
    /**
     * Constructor
     */
    VideoCollectionClient();

    /**
     * Destructor.
     */
    virtual ~VideoCollectionClient();
    
    /**
     * Initializes object and allocates members
     * 
     * @param signal receiver
     * 
     * @return 0 if ok, < 0 in case of error
     */
    int initialize(VideoDataSignalReceiver *signalReceiver);

public:
        
    /**
     * Collection opening status
     */
    enum TCollectionOpenStatus
    {        
        ECollectionNotOpen = 0,
        ECollectionOpening,
        ECollectionOpened            
    };  

    /**
     * Connects videodata signals to provided object
     * If method fails, client should deallocate it's 
     * VideoDataSignalReceiver object to make sure thee are
     * no unused signal connections.
     * 
     * @param aVideoModelObserver Observer.
     * 
     * @return int -1 in case of failure
     */
    int connectCollectionSignalReceiver(
            VideoDataSignalReceiver *signalReceiver);
    
    /**
     * Returns the current level of collection.
     * 
     * @return some type of TCollectionLevels or -1 in case of error
     */
    int getCollectionLevel();
    
    /**
     * Returns the current category requested to open
     * 
     */
    void getCategoryId(TMPXItemId &id);

    /**
     * Returns reference to collection open status. Client can use
     * this to get and change the status
     * 
     * @return some type of TCollectionLevels or -1 in case of error
     */
    int getOpenStatus();
    
    /**
     * Sets open status, if given status is ECollectionOpened calls 
     * startOpenCurrentState to start collection into current state. 
     * 
     */
    void setOpenStatus(int status,  bool startOpening = true);
        
    /**
     * Starts opening of collection to the all videos category
     * 
     * @return 0 startup ok
     */
    int startOpenCollection(int level);     
    
    /**
     * Calls open to collection to get current
     * state to be opened. 
     * 
     * @return 0 startup ok
     */
    int startOpenCurrentState();
    
    /** 
     * calls collection to deleteFile(s) mpx media object defined by the given ids
     *
     * @param mediaIds pointerr to the list of items to be removed
     * @return 0 if ok
     */
    int deleteVideos(QList<TMPXItemId> *mediaIds);
    
    /**
     * Calls collection to open item defined by the given id.
     * 
     * @param aMediaId id of the item
     * @return KErrNone if ok  
     */
    int openItem(const TMPXItemId &mediaId);

    /**
     * calls collection to go back to collection level
     * 
     */
    int back();

    /**
     * Fetches MPX Media object based on MPX ID. Object can be requested later
     * with method GetLatestFetchedMpxMediaL(). Note, there is no indication
     * when the object becomes available.
     * 
     * @param aMpxId MPX ID.
     * 
     * @return 0 media fetchingstarted ok 
     */
    int fetchMpxMediaByMpxId(const TMPXItemId &aMpxId);    
    
    /**
     * Gets all details for the selected MPX Media object.
     * Result will be returned in CVideoCollectionListener::HandleCollectionMediaL().
     * 
     * @param aMediaId meida id of the item (TMPXItemId.iId1) 
     * 
     * @return 0 if detail fetching started ok
     */
    int getVideoDetails(const TMPXItemId& aMediaId);
    
    /**
     * Add a new album.
     * 
     * @param title, Album title.
     * @return TMPXItemId id of created album TMPXItemId:::InvalidId() in case of failure
     */
    TMPXItemId addNewAlbum(const QString &title);

    /**
     * Add a new album.
     * 
     * @param mediaIds, list of album id's to be removed.
     * @return 0 if no errors.
     */
    int removeAlbums(const QList<TMPXItemId> &mediaIds);

    /**
     * Add items in existing album.
     * 
     * @param albumId, Album where to add items.
     * @param mediaIds, Items which to add.
     * @return 0 if no errors.
     */
    int addItemsInAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);
    
    /**
     * Removes items from existing album.
     * 
     * @param albumId, Album from where to remove items.
     * @param mediaIds, Items which to remove.
     * @return 0 if no errors.
     */
    int removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);
    
    /**
     * Renames a video or user defined collection item.
     * 
     * @param itemId, id of item to be renamed.
     * @param newTitle, New title for the album.
     * @return 0 if no errors.
     */
    int renameItem(const TMPXItemId &itemId, const QString &newTitle);

private:
    
    /**
     * Private implementation to handle leaving code of collection opening.
     * 
     */
    void startOpenCollectionL(int level);
    
    /**
     * Private implementation to handle leaving code of video deletion.
     * 
     * @param mediaIds list of ides to remove
     */
    void deleteVideosL(QList<TMPXItemId> &mediaIds);
    
    /**
     * Private implementation to handle leaving code of video opening.
     * 
     * @param videoId id of the video to open
     */
    void openVideoL(const TMPXItemId &videoId);

    /**
     * Private implementation to handle leaving code of collection opening.
     * 
     * @param id id of the collection to open
     */
    void openCategoryL(const TMPXItemId &id);

    /**
     * Private implementation to handle leaving code of collection back stepping.
     * 
     */
    void backL();

    /**
     * Private implementation to handle leaving code while starting to get video 
     * details
     * 
     * @param videoId id of the video to get details from 
     */
    void getVideoDetailsL(const TMPXItemId &videoId);
        
    /**
     * Removes user created albums.
     * 
     * @param mediaIds, list of album id's to be removed
     */
    void removeAlbumsL(const QList<TMPXItemId> &mediaIds);

    /**
     * Private implementation to handle leaving code while starting to fetch video 
     * object from collection
     * 
     * @param mpxId mpxif for the video object to fetch from collection
     */
    void fetchMpxMediaByMpxIdL(const TMPXItemId &mpxId);    
    
    /**
     * Create a new album.
     * 
     * @param title, Album title.
     * @return album id.
     */
    TMPXItemId createAlbumL(const QString &title);
    
    /**
     * Add items in an album.
     * 
     * @param albumId, Album where to add items.
     * @param mediaIds, Items to add in the album.
     * @return None.
     */
    void addItemsInAlbumL(const TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);
    
    /**
     * Removes items from an album.
     * 
     * @param albumId, Album where to remove items.
     * @param mediaIds, Items to remove from album.
     * @return None.
     */
    void removeItemsFromAlbumL(const TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds);
    
    /**
     * Renames a media item.
     * 
     * @param itemId, Album to be renamed.
     * @param newTitle, New title for the album.
     * @return None.
     */
    void renameL(const TMPXItemId &itemId, const QString newTitle);
    
private:

    /**
     * Pointer to MPX Collection utility.
     * Own.
     */
    MMPXCollectionUtility* mCollectionUtility;
    
    /**
     * Variable for storing My Videos collection opening status.
     */
    int mCollectionOpenStatus;     

    /**
     * Collectionlistener
     */
    VideoCollectionListener *mCollectionListener;
    
    /**
     * Variable for storing opened My Videos category and album ids.
     */
	TMPXItemId mOpenCategoryAlbum;

    /**
     * Variable for storing My Videos collection path level.
     */
    VideoCollectionCommon::TCollectionLevels mCollectionPathLevel;
 };

#endif // __VIDEOCOLLECTIONCLIENT_H
