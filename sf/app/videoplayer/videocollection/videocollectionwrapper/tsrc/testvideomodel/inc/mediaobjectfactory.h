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
* Description:  Helper class for creating mediaobjects for testing VideoListDataModel class methods*
*/


#ifndef __TESTVIDEOLISTMEDIAFACTORY_H__
#define __TESTVIDEOLISTMEDIAFACTORY_H__


class CMPXMediaArray;
class CMPXMedia;

/**
 * flag values for selecting wich details are created for items
 */
enum MediaDetailSelection
{
    MediaDetailNone = 0x00,
    MediaDetailRatingFlag = 0x01,
    MediaDetailDateFlag = 0x02,
    MediaDetailSizeFlag  = 0x04,
    MediaDetailDurationFlag = 0x08,
    MediaDetailFilePathFlag = 0x10,
    MediaDetailCategoryVideoCount = 0x20,
    MediaDetailAll = 0xff
};

// const used for media creation
_LIT( KMediaTestNamePrefix, "MEDIATESTFILEPATH_%d" );
const QString gQTMediaNamePrefix = "MEDIATESTFILEPATH_";

_LIT( KMediaTestFilePathPrefix, "MEDIATESTFILEPATH_%d" );
const QString gQTMediaFilePathPrefix = "MEDIATESTFILEPATH_";

class MediaObjectFactory 
{

public:
    /**
     * constructor.
     */
    MediaObjectFactory();
    
    /**
     * destructor
     */
    ~MediaObjectFactory();
    
    /**
     * returns pointer to media-array 
     */
    CMPXMediaArray* mediaArray();
    
    /**
     * creates wanted amount of items with sekected details
     * 
     * @param counr
     * @param MediaDetailSelection flags
     */
    void createMediaItems(int count, int mediaType = 0, MediaDetailSelection flags = MediaDetailAll);
       
    /**
     * deletes media-array 
     */
    void removeArray();
    
private:
    
    /**
     * fills data for single media-item
     * 
     * @param media
     * @index
     * @MediaDetailSelection
     * 
     * @return bool true: succeed
     */
    bool fillMediaDatas(CMPXMedia* media, int index, int mediaType, MediaDetailSelection detailSelectionFlag);
    
    /**
     * array of medias
     */
    CMPXMediaArray *mArray;
};


#endif  // __TESTVIDEOLISTMEDIAFACTORY_H__

// End of file
    


