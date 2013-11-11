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
* Description:   VideoThumbnailData singleton class definition
*/

#ifndef __VIDEOTHUMBNAILDATA_H__
#define __VIDEOTHUMBNAILDATA_H__

// INCLUDES
#include <qobject.h>
#include <qicon.h>
#include <qpointer.h>
#include <mpxitemid.h>

#include "videocollectionexport.h"

// FORWARD DECLARATIONS
class VideoThumbnailDataPrivate;
class VideoProxyModelGeneric;

// CLASS DECLARATIONS

class VIDEOCOLLECTION_DLL_EXPORT VideoThumbnailData : public QObject
{
    /**
     * Defined to be able to use signals and slots.
     */
    Q_OBJECT

    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoThumbnailData)

public:

    /*
     * Thumbnail fetch priorities.
     */
    enum VideoThumbnailPriority
    {
        VideoThumbnailPriorityLow = 2000,
        VideoThumbnailPriorityMedium = 3000,
        VideoThumbnailPriorityHigh = 5000
    };    
    
    /**
     * Returns singleton instance for this class.
     *
     * @return The singleton instance.
     *
     */
    static VideoThumbnailData &instance();
    
    /**
     * Method removes thumbnail data from the local repository.
     *
     * @param mediaId id for media whose thumbnail is to be removed
     *
     * @return bool: true if item removed correctly, false if not.
     *
     */
    bool removeThumbnail(const TMPXItemId &mediaId);
    
    /**
     * Method returns a pointer to video's thumbnail.
     *
     * If thumbnail fetching is ongoing or not yet started, pointer to default 
     * thumbnail is returned.
     *
     * @param mediaId media id for which to get thumbnail
     * @param tnData reference to thumbnail data where data is occupied
     *
     * @return QIcon* thumbnail pointer
     *
     */
    const QIcon* getThumbnail(const TMPXItemId &mediaId);

    /**
     * Starts background thumbnail fetching, the model used for the background fetching
     * is also changed.
     *
     * @param model model of the items for the fetching.
     * @param fetchIndex index where to start the background thumbnail fetching.
     *  
     */
    void startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex);
    
    /**
     * Enables or disables thumbnail background fetching. Default is enabled.
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
     * Returns background fetching enabled status. 
     * 
     * @return bool: true if background fetching is enabled, otherwise false.
     */
    bool backgroundFetchingEnabled();
    
    /**
     * Frees allocated data for thumbnails and cancels ongoing fetches.
     * 
     */
    void freeThumbnailData();

signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when
     *
     * @param mediaIds: media ids of the videos whose thumbnail is ready.
     *
     */
    void thumbnailsFetched(QList<TMPXItemId>& mediaIds);
    
private:

    /**
     * Default constructor
     */
    VideoThumbnailData();

    /**
     * Destructor
     */
    ~VideoThumbnailData();
    
    /**
     * Private implementation.
     */
    QPointer<VideoThumbnailDataPrivate> const d_ptr;
};

#endif  // __VIDEOTHUMBNAILDATA_H__

// End of file
