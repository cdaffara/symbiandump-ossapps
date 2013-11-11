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

// FORWARD DECLARATIONS

class MMPXCollectionUtility;
class VideoDataSignalReceiver;
class VideoCollectionListener;


// CLASS DECLARATION

/**
 * Client class for My Videos MPX Collection.
 *
 * @lib videocollectionwrapper.dll
 */
class VideoCollectionClient 
{
        
public: 
    
    /**
     * Constructor
     */
    VideoCollectionClient() {};

    /**
     * Destructor.
     */
    virtual ~VideoCollectionClient(){};
    
   

public:
   
    /**
     * Collection level app browsed to
     */
    enum TCollectionLevels
    {
        ELevelInvalid = -1,
    	ELevelCategory = 2,
        ELevelVideos   = 3,
        ELevelAlbum   = 4
    };
            
    /**
     * Returns the current level of collection.
     * 
     * @return some type of TCollectionLevels or -1 in case of error
     */
    int getCollectionLevel(){ return mCollectionPathLevel; };
    
   
    /**
     * Variable for storing My Videos collection path level.
     */
    static int mCollectionPathLevel;
 };

#endif // __VIDEOCOLLECTIONCLIENT_H
