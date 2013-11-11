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
* Description:  Helper class for creating mediaobjects for testing CVideoListDataModel class methods*
*/


#ifndef __TESTVIDEOLISTMEDIAFACTORY_H__
#define __TESTVIDEOLISTMEDIAFACTORY_H__


#include <qtemporaryfile.h>

class CMPXMediaArray;
class CMPXMedia;

/**
 * flag values for selecting wich details are created for items
 */
enum MediaDataSelection
{
    MediaDataNone = 0x00,    
    MediaDataRatingFlag = 0x01,
    MediaDataDateFlag = 0x02,
    MediaDataSizeFlag  = 0x04,
    MediaDataDurationFlag = 0x08,
    MediaDataAgeProfile = 0x10,
    MediaDataFilePath = 0x20,
    MediaDataName = 0x40,
    MediaDataId   = 0x80,
    MediaDataDesc = 0x100,
    MediaDataModified = 0x200,
    MediaDataAuthor = 0x400,
    MediaDataCopyright = 0x800,
    MediaDataLanguage = 0x1000,
    MediaDataFormat = 0x2000,
    MediaDetailCategoryVideoCount = 0x4000,
    MediaDataResolution = 0x8000,
    MediaDataBitrate = 0x10000,
    MediaDataAll = 0xffffffff
};

// const used for media creation
_LIT( KMediaTestNamePrefix, "MEDIATESTNAME_%d" );
const QString gQTMediaNamePrefix = "MEDIATESTNAME_";

_LIT( KMediaTestPathPrefix, "c:\\MEDIATESTPATH_%d\\" );
const QString gQTMediaPathPrefix = "MEDIATESTPATH_";

_LIT( KMediaTestDescPrefix, "MEDIATESTDESC_%d" );
const QString gQTMediaDescPrefix = "MEDIATESTDESC_";

_LIT( KMediaTestAuthorPrefix, "MEDIATESTAUTHOR_%d" );
const QString gQTMediaAuthorPrefix = "MEDIATESTAUTHOR_";

_LIT( KMediaTestCopyrightPrefix, "MEDIATESTCOPYRIGHT_%d" );
const QString gQTMediaCopyrightPrefix = "MEDIATESTCOPYRIGHT_";

_LIT( KMediaTestLanguagePrefix, "MEDIATESTLANGUAGE_%d" );
const QString gQTMediaLanguagePrefix = "MEDIATESTLANGUAGE_";

_LIT( KMediaTestFormatPrefix, "MEDIATESTFORMAT_%d" );
const QString gQTMediaFormatPrefix = "MEDIATESTFORMAT_";

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
     * creates a copy of media-array and transfers ownership to caller 
     */
    CMPXMediaArray* copyOfMediaArray();
    
    /**
     * creates wanted amount of items with selected details
     * 
     * @param count
     * @param MediaDetailSelection flags
     */
    void createMediaItems(int count, int flags = MediaDataAll);
       
    /**
     * creates CMPXMediaArray of category items
     * 
     */
    void createCollectionItems();

    /**
     * deletes media-array 
     */
    void removeArray();
    
    /**
     * creates media-item
     * 
     * @index used as part of data values
     * @MediaDetailSelection
     * 
     * @return CMPXMedia 
     */
    CMPXMedia* getMedia(int index, int dataSelectionFlags  = MediaDataAll );
    
    /**
     * creates media-item
     * 
     * @index used as part of data values
     * @MediaDetailSelection
     * 
     * @return CMPXMedia 
     */
    CMPXMedia* getMediaWithWantedIds(int id1, int id2 );
    
private:
    
    /**
     * fills data for media
     */
    bool fillData(CMPXMedia *media, int index, int dataSelectionFlags  = MediaDataAll );
    
    /**
     * array of medias
     */
    CMPXMediaArray *mArray;
    
    /**
     * list of temp files
     */
    QList<QTemporaryFile*> mTmpFiles;
};


#endif  // __TESTVIDEOLISTMEDIAFACTORY_H__

// End of file
    


