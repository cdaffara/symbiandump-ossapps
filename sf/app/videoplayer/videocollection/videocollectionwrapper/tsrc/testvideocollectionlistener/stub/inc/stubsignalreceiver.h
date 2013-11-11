/**
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
* Description:   stub signal receiver for testing VideoCollectionListener
* 
*/

#ifndef __CSTUBSIGNALRECEIVER_H__
#define __CSTUBSIGNALRECEIVER_H__

#include <QObject>
#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <Qlist>
#include "videodatasignalreceiver.h"

class  StubSignalReceiver : public VideoDataSignalReceiver
{
    Q_OBJECT
    
    Q_DISABLE_COPY(StubSignalReceiver)
 
public:
    
    /**
     * contructor
     */
    StubSignalReceiver();
    
    /**
     * destructor
     */
    virtual ~StubSignalReceiver();
    
    /**
     * clears all members
     */
    void resetLatestItems();
    
    /**
     * return mLatestPtr
     */
    void* getLatestPointerAddr();
    
    /**
     * return mLatestItemId
     */
    TMPXItemId getLatestItemId();
    
    /**
     * return mLatestModifiedItemId
     */
    TMPXItemId getLatestModifiedItemId();

    /**
     * return mLatesListData
     */
    QList<TMPXItemId>& getLatestListData();
    
    /**
     * return mLatestInteger
     */
    int getLatestIntegerData();
    
    /**
     * return mVideoListComplete
     */
    bool getVideoListComplete();
    
    /**
     * return mAlbumListComplete
     */
    bool getAlbumListComplete();
    
public slots:

    /**
     * Saves provided address to mLatestPtr
     */
    void newVideoListSlot(CMPXMediaArray* aVideoList);
    
    /**
     * Saves provided address to mLatestPtr
     */
    void appendVideoListSlot( CMPXMediaArray* videoList );
    
    /**
     * Saves provided address to mLatestPtr 
     */
    void videoListAppendedSlot(CMPXMediaArray* aVideoList);
      
    /**
     * Saves provided address to mLatestPtr
     */
    void newVideoAvailableSlot(CMPXMedia* aVideo);
    
    /**
     * Saves provided integer to mLatestInt
     */
    void videoDeletedSlot(TMPXItemId videoId);
     
    /**
     * copies provided list data to mLatesListData
     */
    void videoDeleteCompletedSlot(int count, QList<TMPXItemId> *failedMediaIds);
    
    /**
     * Saves provided media to mLatestPtr
     */
    void videoDetailsCompletedSlot(CMPXMedia* media);
    
    /**
     * Saves provided integer to mLatestItemId and provided pointer to
     * mLatestPtr
     */
    void albumListAvailableSlot(TMPXItemId &albumId,
            CMPXMediaArray *albumItems);
    
    /**
     * copies provided list data to mLatesListData
     */
    void albumRemoveFailureSlot(QList<TMPXItemId> *items);

    /**
     * saves provided item id into mLatestItemId
     */
    void itemDeletedSlot(TMPXItemId &id);
    
    /**
     * saves provided item id into mLatestModifiedItemId
     */
    virtual void itemModifiedSlot(const TMPXItemId &itemId);
    
    /**
     * Sets boolean mVideoListComplete to true;
     */
    void videoListCompleteSlot();
    
    /**
     * Sets boolean mAlbumListcomplete to true;
     */
    void albumListCompleteSlot();
    
private:
    /**
     * Contains address of latest pointer passed to object
     */
    void *mLatestPtr;
    
    /**
     * contains latest integer value sent
     */
    int mLatestInteger;
    
    /**
     * contains value if the latest integer passed to object
     */
    TMPXItemId mLatestItemId;
    
    /**
     * contains value if the latest integer passed to object at EMPXItemModified event 
     */
    TMPXItemId mLatestModifiedItemId;

    /**
     * contains contents of list received from signal
     */
    QList<TMPXItemId> mLatesListData;
    
    /**
     * True if videoListCompleteSlot has been called.
     */
    bool mVideoListComplete;

    /**
     * True if albumListCompleteSlot has been called.
     */
    bool mAlbumListComplete;
};

#endif
