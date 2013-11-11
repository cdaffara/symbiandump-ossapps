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
* Description: VideoThumbnailDataPrivate class definition
*
*/

#ifndef __VIDEOTHUMBNAILDATAPRIVATE_H__
#define __VIDEOTHUMBNAILDATAPRIVATE_H__

// INCLUDES
#include <qobject.h>
#include <qpointer.h>
#include <qcache.h>
#include <qicon.h>
#include <qset.h>
#include <qlist.h>
#include <qhash.h>
#include <e32const.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class VideoCollectionWrapper;
class VideoProxyModelGeneric;
class QModelIndex;
class QTimer;
class VideoThumbnailFetcher;
class HbIcon;

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
    const QIcon* getThumbnail(const TMPXItemId &mediaId);

    /**
     * Method removes thumbnail data from the icon cache.
     *
     * @param mediaId id for media whose thumbnail is to be removed
     *
     * @return bool: true if item removed correctly, false if not.
     *
     */
    bool removeThumbnail(const TMPXItemId &mediaId);

    /**
     * Starts background thumbnail fetching from the given fetch index.
     *
     * @param model model of the items for the fetching.  
     * @param fetchIndex index where to start the background thumbnail fetching.
     */
    void startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex);
    
    /**
     * Enables or disables thumbnail background fetching.
     * 
     * @param enable true enables and false disables thumbnail background fetching.
     */
    void enableBackgroundFetching(bool enable);

    /**
     * Enables or disables thumbnail creation. Default is enabled. Thumbnails
     * that have been already generated are fetched still.  
     * 
     * @param enable true enables and false disables thumbnail creation.
     */
    void enableThumbnailCreation(bool enable); 
    
    /**
     * Returns if background fetching is enabled.
     * 
     * @return bool: true if background fetching is enabled, otherwise false.
     */
    bool backgroundFetchingEnabled();
    
    /**
     * Frees allocated data for thumbnails and cancels ongoing fetches.
     */
    void freeThumbnailData();
    
protected:    
    
    /**
     * Method creates member objects and connects signals.
     *
     * @return int 0 initialization ok, -1 initialization failed
     *
     */
    int initialize();

    /**
     * Cleanup internal data. This must be called before UI environment is destroyed.
     */
    void cleanup();

    /**
     * Connects the signals.
     *
     * @return int 0 connecting ok, -1 connecting failed.
     */
    int connectSignals();

    /**
     * Disconnects the signals.
     *
     */
    void disconnectSignals();
    
    /**
     * Starts fetching thumbnails for medias in the model defined by the indexes 
     * parameter. Priority for the thumbnails is ascending starting from the 
     * defined priority.
     *
     * param @indexes indexes of the medias in the model
     * param @priority starting priority for the thumbnails
     *
     * @return int count of fetches started or -1 in case of error.
     */
    int startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority);

    /**
     * Method starts fetching thumbnail for the video specified by given mediaId
     * Media IDs are used as a key identification in the thumbnail map where data is
     * collected.
     *
     * @param mediaId media id for which to start fetching thumbnail
     * @param priority priority for the fetch
     *
     * @return int: thumbnail id or -1 if fetch starting fails.
     *
     */
    int startFetchingThumbnail(const TMPXItemId &mediaId, int priority);
    
    /**
     * Appends indexes in the source model to the list between start and end.
     *
     * @param indexes list of indexes
     * @param startIndex start index
     * @param endIndex end index
     */
    void getModelIndexes(QList<QModelIndex> &indexes, int &startIndex, int &endIndex);

    /**
     * Method returns default thumbnail data.
     *
     * @param mediaId media id for which to get thumbnail
     * 
     * @return QIcon: reference to default thumbnail data
     *
     */
    const QIcon* defaultThumbnail(const TMPXItemId &mediaId);
    
    /**
     * Loads icon from file or resource, the icon is scaled to size of the 
     * video list view thumbnail.
     * 
     * @param iconName name of the file or resource.
     * 
     * @return QIcon the icon load from resource or file.
     * 
     */
    QIcon *loadIcon(QString iconName);

    /**
     * Starts timer that continues the background thumbnail fetching at timeout.
     */
    void continueBackgroundFetch();

signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when
     *
     * @param mediaIds: media ids of the videos whose thumbnail is ready.
     *
     */
    void thumbnailsFetched(QList<TMPXItemId>& mediaIds);

private slots:

    /**
     * Thumbnail fetcher signals this slot when thumbnail is ready
     * for some item. Thumbnail data is saved to icon cache.
     *
     * @param tnData: thumbnail
     * @param mediaId: pointer to the media id used in emitted signal
     * @param error: possible error code from thumbnail manager ( 0 == ok )
     *
     */
    void thumbnailReadySlot(QPixmap tnData, const TMPXItemId &mediaId, int error);

    /**
     * Fetches thumbnails around of mCurrentFetchIndex in batches of THUMBNAIL_BACKGROUND_FETCH_AMOUNT.
     * The fetch index is set by startFetchingThumbnails which also resets
     * mCurrentBackgroundFetchCount. Maximum of THUMBNAIL_MAX_BACKGROUND_FETCH thumbnails are
     * fetched between the count resets.
     *
     */
    void doBackgroundFetching();
    
    /**
     * Signaled when UI environment is about to be destroyed. Thumbnails must be cleaned before that.
     * 
     */
    void aboutToQuitSlot();
    
    /**
     * Emits thumbnailReady signals for list of thumbnails. 
     * 
     */
    void reportThumbnailsReadySlot();

    /**
     * Thumbnail fetcher signals this when all thumbnails have been fetched.
     */
    void allThumbnailsFetchedSlot();
    
private: // Data

    VideoThumbnailFetcher           *mThumbnailFetcher;
    
    /**
     * Local cache for thumbnail data:
     * - key is the video's media id
     * - data is the actual thumbnail data.
     * If thumbnail data is not yet fetched, QIcon is default thumbnail.
     */
    QCache<TMPXItemId, QIcon>       mThumbnailData;

    /**
     * Local hash for default thumbnails.
     * - key is mpx item id defining default tn type 
     * - data is the actual thumbnail data.
     */
    QHash<TMPXItemId, QIcon *>      mDefaultThumbnails;

    /**
     * Current model, set at call to startBackgroundFetching.
     */
    QPointer<VideoProxyModelGeneric>     mCurrentModel;
    
    /**
     * Current index for background thumbnail fetching.
     */
    int                             mCurrentFetchIndex;

    /**
     * Current count of background thumbnails fetched around the fetch index.
     */
    int                             mCurrentBackgroundFetchCount;

    /**
     * Timer for background fetching.
     */
    QTimer                          *mBgFetchTimer;

    /**
     * Timer for thumbnail ready signaling.
     */
    QTimer                          *mTbnReportTimer;
    
    /**
     * List of media ids for thumbnails that are ready.
     */
    QList<TMPXItemId>               mReadyThumbnailMediaIds;
    
    /**
     * Flag for signal connections.
     */
    bool                            mSignalsConnected;
    
    /**
     * Indicates if background thumbnail fetching is enabled.
     */
    bool                            mBackgroundFetchingEnabled;
};

#endif  // __VIDEOTHUMBNAILDATAPRIVATE_H__

// End of file
