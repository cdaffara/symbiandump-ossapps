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
* Description:   stub VideoCollectionClient class definition*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

#include <vcxmyvideosdefs.h>
#include <QList>


// CLASS DECLARATION


class VideoCollectionClient 
{
        
public: 
    
    /**
     * Constructor
     */
    VideoCollectionClient():mDeleteFails(false) {};

    /**
     * Destructor.
     */
    virtual ~VideoCollectionClient(){};
    
    /**
     * returns 0 if mDeleteFails is false
     */
    int deleteVideos(QList<TMPXItemId> *mediaIds)
    {
        Q_UNUSED(mediaIds);
        if(mDeleteFails)
        {
            return -1;
        }
        return 0;
    }

    /**
     * if true, deletevideos -call fails
     */
    bool mDeleteFails;
   
 };

#endif // __VIDEOCOLLECTIONCLIENT_H
