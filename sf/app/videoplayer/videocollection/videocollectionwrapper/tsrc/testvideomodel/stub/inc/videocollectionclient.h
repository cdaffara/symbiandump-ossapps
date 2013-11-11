

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
* Description:  stubclass of videocollection for testing VideoListDataModel class methods*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

#include <mpxmedia.h>
#include <QList>


class VideoDataSignalReceiver;


class  VideoCollectionClient 
{

public: // Constructors and destructor

    /**
     * constructor.
     */
    VideoCollectionClient();

    /**
     * Destructor.
     */
    virtual ~VideoCollectionClient();

    /**
     * dummy impl
     */
    int initialize( VideoDataSignalReceiver* signalReceiver);
    
    /**
     * return 0 always, increases global counter by one
     * 
     * @param TMPXItemId not used
     * 
     * @return 0
     */
    int deleteVideos( QList<TMPXItemId> *ids );
    
    /**
     * returns mRemoveItemsReturn
     */
    int removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &items);
    
    /**
     * if true, initializate fails
     */
    static bool mFailInitialize;
    
    /**
     * if true, connectCollectionSignalReceiver fails
     */
    static bool mFailConnectSignals ;
 
    /**
     * count of initialize -calls
     */
    static int mInitializeCount;
    
    /**
     * returned from removeItemsFromAlbum
     */
    static int mRemoveItemsReturn;
    
  
};

#endif // __VIDEOCOLLECTIONCLIENT_H
