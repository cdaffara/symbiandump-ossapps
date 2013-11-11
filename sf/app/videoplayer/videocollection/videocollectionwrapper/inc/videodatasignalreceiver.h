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
* Description:   Signal receiver interface for video data changes.*
*/



#ifndef __VIDEOMODELOBSERVER_H
#define __VIDEOMODELOBSERVER_H

// INCLUDES
#include <QObject>
#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>
#include <QList>

// FORWARD DECLARATIONS
class CMPXMediaArray;
class CMPXMedia;


// CLASS DECLARATION

/**
 * Observer interface for video data changes.
 *
 * @lib vcxnsmyvideos.lib
 */
class  VideoDataSignalReceiver : public QObject
{
    Q_OBJECT
    
public slots:

    /**
     * Signaled when a list of videos is available.
     * Ownership of array is not transferred.
     * 
     * @param aVideoList List of videos
     */
    virtual void newVideoListSlot( CMPXMediaArray* aVideoList ) = 0;
    
    /**
     * Signaled when a list of videos is appended from collection.
     * Ownership of array is not transferred.
     * 
     * @param videoList List of videos
     */
    virtual void appendVideoListSlot( CMPXMediaArray* videoList ) = 0;
          
    
    /**
     * Signaled when a single video item has fetched.
     * Ownership of video is transferred.
     * 
     * @param aVideo Video item.
     */
    virtual void newVideoAvailableSlot( CMPXMedia* aVideo ) = 0;
    
    /**
     * Signaled when received an avent about externally removed item.
     * 
     * @param itemId item id
     */
    virtual void itemDeletedSlot( TMPXItemId &itemId ) = 0;
           
    /**
     * Signaled when multiple video deletion command is completed.
     * 
     * @param overallCount count of ALL videos in this delete procedure 
     * @param failedMediaIds media-array containing possible failed medias
     */
    virtual void videoDeleteCompletedSlot( int overallCount, QList<TMPXItemId> *failedMediaIds ) = 0;
    
    /**
     * Signaled when album remove command is completed and some albums were not removed.
     * 
     * @param failedMediaIds media-array containing failed medias
     */
    virtual void albumRemoveFailureSlot( QList<TMPXItemId> *failedMediaIds ) = 0;
    
    /**
     * Signaled when all details for a video have been fetched.
     * 
     * @param media MPX Media that contains all details.
     */
    virtual void videoDetailsCompletedSlot( CMPXMedia* media ) = 0;
    
    /**
     * Signalled when album items are recieved.
     * 
     * @param albumId, Album which items are received.
     * @param albumItems, Items belonging to the current album.
     */
    virtual void albumListAvailableSlot(TMPXItemId &albumId,
        CMPXMediaArray *albumItems) = 0;
    
    /**
     * Signalled when item has been mofidied.
     * 
     * @param itemId, item that has been modified.
     */
    virtual void itemModifiedSlot(const TMPXItemId &itemId) = 0;
    
    /**
     * Signalled when video list is complete.
     */
    virtual void videoListCompleteSlot() = 0;
    
    /**
     * Signalled when album list is complete.
     */
    virtual void albumListCompleteSlot() = 0;
};

#endif // __VIDEOMODELOBSERVER_H
