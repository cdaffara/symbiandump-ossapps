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
* Description: VideoThumbnailFetcher class definition
*
*/

#ifndef __VIDEOTHUMBNAILDATAFETCHER_H__
#define __VIDEOTHUMBNAILDATAFETCHER_H__

// INCLUDES
#include <qobject.h>
#include <qicon.h>
#include <qlist.h>
#include <qhash.h>
#include <mpxitemid.h>
#include <thumbnailmanager_qt.h>

// FORWARD DECLARATIONS

class VideoThumbnailFetcher : public QObject
{
    /**
     * defined to be able to use signals and slots
     */
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    VideoThumbnailFetcher();
    
    /**
     * Desctructor
     */
    ~VideoThumbnailFetcher();

    /**
     * Adds thumbnail fetch to the fetch list. pauseFetching should be called 
     * before adding fetches for performance reasons. After all fetches have
     * been added, call continueFetching to start the fetch process which passes 
     * the internal data to thumbnail manager. Signal thumbnailReady is emitted 
     * when fetch is complete. 
     * 
     * @param fileName path to the media.
     * @param mediaId mpx id for the media.
     * @param priority priority for the fetch.
     * 
     */
    void addFetch(const QString fileName, const TMPXItemId &mediaId, int priority);    
    
    /**
     * Empties fetch list. This does not cancel the possible ongoing fetch on
     * thumbnail manager side.
     * 
     */
    void cancelFetches();
    
    /**
     * Returns count of the fetches.
     * 
     */
    int fetchCount();
    
    /**
     * Pauses thumbnail fetching process. This does not pause the possible 
     * ongoing fetch on thumbnail manager side. 
     * 
     */
    void pauseFetching();
    
    /**
     * Continues the fetching process. All fetches added with addFetch are 
     * started without create thumbnail flag. If there's not any of those, 
     * starts creating thumbnails for fetches that have no thumbnail yet. 
     * Signal allThumbnailsFetched is emitted if there's nothing to do.
     *
     * @param cancelOngoingFetches if true then all ongoing thumbnail fetches
     *                             will be canceled.
     *                             
     */
    void continueFetching(bool cancelOngoingFetches);
    
    /**
     * Enables or disables the thumbnail creation for videos that do  
     * not have thumbnail already in the database.
     * 
     * @param enable true enables thumbnail creation, false disables. 
     * 
     */
    void enableThumbnailCreation(bool enable);

private:

    class ThumbnailFetchData
    {
        public:         
            QString mFileName;
            int mPriority;
            TMPXItemId mMediaId;
            int mRequestId;
    };
    
protected:
    
    /**
     * Starts fetching all the thumbnails in fetch list. Create flag is 
     * disabled. Thumbnail manager signals to thumbnailReadySlot. If thumbnail 
     * fetch fails with -1 the fetch is added to thumbnail creation list, 
     * otherwise signal thumbnailReady signal emitted.
     *
     * @param cancelOngoingFetches if true then all ongoing thumbnail fetches
     *                             will be canceled.
     * 
     */
    void startThumbnailFetches(bool cancelOngoingFetches);
    
    /**
     * Starts fetching thumbnail with highest priority from creation list. 
     * Create flag is enabled. Thumbnail manager signals to thumbnailReadySlot,
     * from there signal thumbnailReady is emitted.
     *  
     */
    void startThumbnailCreation();
    
signals:
    
    /**
     * Signaled after signal from thumbnail manager has been processed and
     * thumbnail fetch process is complete.
     *
     * @param tnData thumbnail
     * @param mediaId mpx id for the media. 
     * @param error possible error code from thumbnail manager ( 0 == ok )
     * 
     */
    void thumbnailReady(QPixmap tnData, const TMPXItemId &mediaId, int error);

    /**
     * Signaled when all the fetches have been done.
     * 
     */
    void allThumbnailsFetched();
    
private slots:
    
    /**
     * Thumbnail manager signals this slot when thumbnail is ready
     * for some item.
     *
     * @param tnData thumbnail
     * @param internal internal data to identify the request
     * @param requestId thumbnail manager request id
     * @param error: possible error code from thumbnail manager ( 0 == ok )
     *
     */
    void thumbnailReadySlot(QPixmap tnData, void *internal, int requestId, int error);

private: // Data

    /**
     * Thumbnail manager object.
     */
    ThumbnailManager                        *mThumbnailManager;
    
    /**
     * List containing not started thumbnail fetches.
     * 
     * key is mpx id.
     * value is thumbnail fetch data.  
     */
    QHash<TMPXItemId, ThumbnailFetchData *> mFetchList;
    
    /**
     * Hash containing ongoing thumbnail fetches.
     * 
     * key is mpx id.
     * value is thumbnail fetch data.
     */
    QHash<TMPXItemId, ThumbnailFetchData *> mStartedFetchList;
    
    /**
     * List containing thumbnails that have not been created yet.
     */
    QList<ThumbnailFetchData *>             mCreationList;
    
    /**
     * Flag indicating if fetching has been paused.
     */
    bool                                    mPaused;
    
    /**
     * Flag indicating if thumbnail creation is enabled.
     */
    bool                                    mTbnCreationEnabled;
};

#endif // __VIDEOTHUMBNAILDATAFETCHER_H__
