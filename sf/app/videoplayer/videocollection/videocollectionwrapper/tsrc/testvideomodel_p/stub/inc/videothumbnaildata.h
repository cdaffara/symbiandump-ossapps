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
* Description:   CVideoThumbnailData stub -class definition for unit testing CVideoListData*
*/

#ifndef __VIDEOTHUMBNAILDATA_H__
#define __VIDEOTHUMBNAILDATA_H__


// INCLUDES
#include <QObject>
#include <QMap>
#include <QPair>
#include <QIcon>
#include <QList>
#include <e32const.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS


class VideoThumbnailData : public QObject 
{
    Q_OBJECT

public:

    /* 
     * Thumbnail fetch priorities
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
     * stub:
     * increases gStartFetchingCounter by the count of indexes.     
     * return -1 if gFailFetchTn is true
     * return  0 if gFailFetchTn is false 
     */
    int startFetchingThumbnails(const QList<int> &indexes, VideoThumbnailPriority priority, bool setFetchIndex = true);
    
    /**
     * stub:
     * increases gStartFetchingCounter by one
     * return -1 if gFailFetchTn is true
     * return  0 if gFailFetchTn is false
     * 
     */
    int startFetchingThumbnail(int mediaId, int priority);
    
    /**
     * 
     * 
     * 
     */
    const QIcon* getThumbnail( TMPXItemId mediaId );
    
    /**
    * stub:
    * return -1 if gFailRemoveTn is true
    * return  0 if gFailRemoveTn is false
    * 
    */
    int removeThumbnail( TMPXItemId mediaId );
    
    /**
     * 
     * 
     */
    const QIcon* defaultThumbnail();
    
private:
    
    /**
    * stub:
    * return -1 if gFailInitilize is true
    * return  0 if gFailInitilize is false
    * 
    */
    int initialize();
     
    /**
     * Default constructor
     */
    VideoThumbnailData();  
    
    /**
     * Destructor
     */
    ~VideoThumbnailData(); 
    
public:
    
    // Testing methods.
    
    /**
     * sets gFailInitilize 
     */
    static void setInitializationFail(bool fail);
    
    /**
     * zeroes gStartFetchingCounter 
     */
    static void resetStartFetchingCount();
    
    /**
     * returns gStartFetchingCounter 
     */
    static int getStartFetchingCount();
    
    /**
     * zeroes gRemoveCounter 
     */
    static void resetRemoveCount();
    
    /**
     * returns gRemoveCounter 
     */
    static int getRemoveCount();
   	 
signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when 
     * thumbnail generatio nis ready from the tn manager.
     * 
     * Media id for the video is passed along with the signal.
     * 
     * 
     * @param mediaId: id of the video whose thumbnail is ready.
     * 
     */
    void thumbnailsFetched( QList<TMPXItemId>& mediaId );  

private:
    
    /**
     * empty icon 
     */
    QIcon emptyIcon;
    
};
#endif  // __VIDEOTHUMBNAILDATA_H__

// End of file
