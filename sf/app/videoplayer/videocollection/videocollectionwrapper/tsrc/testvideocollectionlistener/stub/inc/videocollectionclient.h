
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
* Description:   stub collection client for testing VideoCollectionListener
* 
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>



class VideoCollectionClient
{
public:
    
    
    enum TCollectionOpenStatus
    {
        ECollectionInvalid,
        ECollectionOpening,
        ECollectionOpened
    };
    /**
     * constructor
     */
    VideoCollectionClient();
    
    /**
     * destructor
     */
    virtual ~VideoCollectionClient();
    
    /**
     * sets mLevel
     */
    void setCollectionLevel(int level);
        
    /**
     * return mLevel
     */
    int getCollectionLevel();
    
    /**
     * return mOpenStatus
     */
    int getOpenStatus();
    
    /**
     * return id and type
     */
    void getCategoryId(TMPXItemId &id);


    /**
     * sets mOpenStatus
     */
    void setOpenStatus(int status, bool startOpening = true);
    
    /**
     * sets mLatestItemId
     */
    void fetchMpxMediaByMpxId(TMPXItemId &eventData);
    
    /**
     * gets mLatestItemId
     */
    TMPXItemId getLatestMPXId();
    
    /**
     * startOpenCurrentState
     */
    int startOpenCurrentState();
    
    /**
     * sets mLatestItemId
     */
    void openItem(TMPXItemId id);
    
    /**
     * count of openItemcalls
     */
    int mOpenItemCallCount;
    
private:
    
    /**
     * value returned from collectionLevel
     */
    int mLevel;
    
    /**
     * value simulating open status
     */
    int mOpenStatus;
    
    /**
     * value fecthed from fetchMpxMediaByMpxId or 
     * setted in openItem
     */
    TMPXItemId mLatestItemId;
    
public:
	static int mStartOpenCurrentStateCallCount;
	
	static bool mSetOpenStatusStartOpening;
    
};

#endif


