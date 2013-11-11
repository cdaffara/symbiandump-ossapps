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
* Description:   Stub class for VideoThumbnailDataPrivate
*/

#ifndef __VIDEOTHUMBNAILDATAPRIVATE_H__
#define __VIDEOTHUMBNAILDATAPRIVATE_H__

// INCLUDES
#include <QObject>
#include <QPointer>
#include <QCache>
#include <QIcon>
#include <QSet>
#include <QList>
#include <e32const.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class VideoProxyModelGeneric 
{
public:
    int ooo;
};

class VideoThumbnailDataPrivate : public QObject
{
    /**
     * defined to be able to use signals and slots
     */
    Q_OBJECT

public:
    
    
    /**
     * Default constructor
     */
    VideoThumbnailDataPrivate();

    /**
     * Destructor
     */
    virtual ~VideoThumbnailDataPrivate();

    /**
     * Starts fetching thumbnails for medias in the model defined by the indexes parameter.
     * Priority for the thumbnails is ascending starting from the defined priority. Uses method
     * startFetchingThumbnail for the actual fetching.
     *
     * Clears all other thumbnail fetches.
     *
     * param @indexes indexes of the medias in the model
     * param @priority starting priority for the thumbnails
     * param @setFetchIndex whether index for background thumbnail fetcing is set
     *
     * @return int count of fetches started or -1 in case of error.
     */
    int startFetchingThumbnails(const QList<int> &indexes, int priority);

    /**
     * Method starts fetching thumbnail for the video specified by given mediaId
     * Media IDs are used as a key identification in the thumbnail map where data is
     * collected. Id is passed to thumbnail manager as internal data and when
     * thumbnail is fetched and thumbnailReadySlot is signaled, id is used to notify
     * client about which item's thumbnail is ready.
     *
     * If thumbnail fetching is started succesfully, method saves thumbnail item
     * into local repository with default thumbnail data.
     *
     * @param mediaId media id for which to start fetching thumbnail
     * @param priority priority for the fetch
     *
     * @return int: thumbnail id or -1 if fetch starting fails.
     *
     */
    int startFetchingThumbnail(TMPXItemId mediaId, int priority);

    /**
     * Method returns a pointer to video's thumbnail.
     *
     * If thumbnail fetching is ongoing or not yet started, pointer to default thumbnail
     * is returned.
     *
     * @param mediaId media id for which to get thumbnail
     * @param tnData reference to thumbnail data where data is ocpied
     *
     * @return QIcon* thumbnail pointer
     *
     */
    const QIcon* getThumbnail( TMPXItemId mediaId );

    /**
     * Method removes thumbnail data from the local repository.
     *
     * @param mediaId id for media whose thumbnail is to be removed
     *
     * @return bool: true if item removed correctly, false if not.
     *
     */
    bool removeThumbnail( TMPXItemId mediaId );

    /**
     * Enables or disables thumbnail background fetching. Default is enabled.
     * 
     * @param enable true enables and false disables thumbnail background fetching.
     */
    void enableBackgroundFetching(bool enable);
    
    /**
     * Frees allocated data for thumbnails and cancels ongoing fetches.
     */
    void freeThumbnailData();
    
    /**
     * Starts background thumbnail fetching from the given fetch index.
     *
     * @param model
     * @param fetchIndex index where to start the background thumbnail fetching.
     */
    void startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex);    
    
    /**
     * Enables thumbnail creation.
     */
    void enableThumbnailCreation(bool enable);
    
    /**
     * Returns background fetching enabled status. 
     * 
     * @return bool: true if background fetching is enabled, otherwise false.
     */
    bool backgroundFetchingEnabled();
    
signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when
     *
     * @param mediaIds: media ids of the videos whose thumbnail is ready.
     *
     */
    void thumbnailsFetched(QList<TMPXItemId>& mediaIds);
    
public: // Data

    static int mConstructCallCount;
    static int mDestructCallCount;
    static int mStartFetchingThumbnailsCallCount;
    static int mStartFetchingThumbnailCallCount;
    static int mGetThumbnailCallCount;
    static int mRemoveThumbnailCallCount;
    static int mFreeThumbnailDataCallCount;
    static bool mBackgroundThumbnailFetchingEnabled;
    static int mStartBackgroundFetchingCallCount;
    static int mEnableThumbnailCreationCallCount;
};

#endif  // __VIDEOTHUMBNAILDATAPRIVATE_H__

// End of file
