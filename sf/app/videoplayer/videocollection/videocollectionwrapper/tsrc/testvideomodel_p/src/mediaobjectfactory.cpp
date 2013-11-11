
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

// INCLUDES

#include <mpxmediavideodefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qdatetime.h>
#include <xqconversions.h>
#include "vcxmyvideosdefs.h"
#include "mediaobjectfactory.h"

// -----------------------------------------------------------------------------
// MediaObjectFactory
// -----------------------------------------------------------------------------
//
MediaObjectFactory::MediaObjectFactory() :
    mArray(0)
{
    
}

// -----------------------------------------------------------------------------
// ~MediaObjectFactory
// -----------------------------------------------------------------------------
//
MediaObjectFactory::~MediaObjectFactory()
{
    removeArray();
}
 
// -----------------------------------------------------------------------------
// mediaArray
// -----------------------------------------------------------------------------
//
CMPXMediaArray* MediaObjectFactory::mediaArray()
{
    return mArray;
}

// -----------------------------------------------------------------------------
// copyOfMediaArray
// -----------------------------------------------------------------------------
//
CMPXMediaArray* MediaObjectFactory::copyOfMediaArray()
{
    CMPXMediaArray *copyArray = 0;
    if(mArray)
    {
        TRAP_IGNORE(copyArray = CMPXMediaArray::NewL(*mArray));
    }
    return copyArray;
}
 
// -----------------------------------------------------------------------------
// createMediaItems
// -----------------------------------------------------------------------------
//
void MediaObjectFactory::createMediaItems(int count, int flags)
{
    if(!mArray)
    {
        TRAPD(error, mArray = CMPXMediaArray::NewL()  )
        if(!mArray)
        {
            return;
        }
    }
    int nextPossibleIndex = mArray->Count();
    
    CMPXMedia *media;

    for(int i = 0; i < count; ++i)
    {
        media = getMedia(i, flags);
        if(!media)
        {
            delete mArray;
            mArray = 0;
            return;
        }
        
        TRAPD(error, mArray->AppendL(media));
        if(error != KErrNone)
        {
            delete mArray;
            mArray = 0;
            return;
        }
    }
}


// -----------------------------------------------------------------------------
// createCollectionItems
// -----------------------------------------------------------------------------
//
void MediaObjectFactory::createCollectionItems()
{
    if(!mArray)
    {
        TRAPD(error, mArray = CMPXMediaArray::NewL()  )
        if(!mArray)
        {
            return;
        }
    }
    int nextPossibleIndex = mArray->Count();
    
    for(int i = KVcxMvcCategoryIdDownloads; i <= KVcxMvcCategoryIdOther; ++i)
    {
        CMPXMedia *media = NULL;
        TRAP_IGNORE(media = CMPXMedia::NewL());
        if(!media)
        {
            return;
        }
        TMPXItemId itemId;
        
        // set media id
		itemId.iId1 = i;
		itemId.iId2 = 1;
		TRAPD(error, media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId));
		if(error != KErrNone)
		{
			delete media;
			return;
		}
		TBuf<65> videoname;
		videoname.Format(KMediaTestNamePrefix, index);              
        TRAP(error, media->SetTextValueL( KMPXMediaGeneralTitle, videoname));
        if(error != KErrNone)
        {
           delete mArray;
           mArray = 0;
           return;
        }
    	if(!media)
        {
            delete mArray;
            mArray = 0;
            return;
        }
        
        TRAP(error, mArray->AppendL(media));
        if(error != KErrNone)
        {
            delete mArray;
            mArray = 0;
            return;
        }
    }
}

CMPXMedia* MediaObjectFactory::getMediaWithWantedIds(int id1, int id2)
{
    CMPXMedia *media = NULL;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    TMPXItemId itemId;
    if(!media)
    {
        return 0;
    }
    itemId.iId1 = id1;//category all
    itemId.iId2 = id2;//iId2 = 1 for category item

    TRAPD(error, media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId));
    if(error != KErrNone)
    {
        delete media;
        return 0;
    }
    
    TBuf<65> videoname;
    videoname.Format(KMediaTestNamePrefix, id1);              
    TRAP(error, media->SetTextValueL( KMPXMediaGeneralTitle, videoname));
    if(error != KErrNone)
    {
       delete media;
       return 0;
    }

    return media;
}

// -----------------------------------------------------------------------------
// getMedia
// -----------------------------------------------------------------------------
//
CMPXMedia* MediaObjectFactory::getMedia(int index, int dataSelectionFlags)
{
    CMPXMedia *media = NULL;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    if(!media)
    {
        return 0;
    }
    
    TMPXItemId itemId = TMPXItemId::InvalidId();
    
    // set media id
    if(dataSelectionFlags & MediaDataId)
    {
        itemId.iId1 = index;//category all
        itemId.iId2 = 0;//iId2 = 1 for category item
    }
    TRAPD(error, media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId));
    if(error != KErrNone)
    {
        delete media;
        return 0;
    }
    if(!fillData(media, index, dataSelectionFlags))
    {
        return 0;
    }
    return media;
}
// -----------------------------------------------------------------------------
// fillData
// -----------------------------------------------------------------------------
//
bool MediaObjectFactory::fillData(CMPXMedia *media, int index, int dataSelectionFlags) 
{    
    TBuf<65> videoname;
    // set media name
    if(dataSelectionFlags & MediaDataName)
    {
        videoname.Format(KMediaTestNamePrefix, index);              
        TRAPD(error, media->SetTextValueL( KMPXMediaGeneralTitle, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set media rating
    if(dataSelectionFlags & MediaDataRatingFlag)
    {
        // index: 0 1 2 3 4 5 6 7 8 9 ...
        // value: 1 2 3 4 5 1 2 3 ...
        int rating = (index%5) + 1;
        TRAPD(error, media->SetTObjectValueL<TInt>(KVcxMediaMyVideosRating, rating));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set media date
    if(dataSelectionFlags & MediaDataDateFlag)
    {
        TDateTime dateTime;
        dateTime.Set(2009, TMonth(index), index, index+1, index+1, index+1, index+1);
        TTime ttimeDate(dateTime);
        TInt64 temp= ttimeDate.Int64();
        TRAPD(error, media->SetTObjectValueL<TInt64>(KMPXMediaGeneralDate, ttimeDate.Int64()));
        if(error != KErrNone)
        {    
            delete media;
            return false;
        }
    }
    
    // set duration
    if(dataSelectionFlags & MediaDataDurationFlag)
    {
        float duration = 1.0 + index;
        TRAPD(error, media->SetTObjectValueL<float>(KVcxMediaMyVideosDuration, duration));
        if(error != KErrNone)
        { 
            delete media;
            return false;
        }
    }
    
    // set size
    if(dataSelectionFlags & MediaDataSizeFlag)
    {
        quint32 size = 1.0 + index;
        TRAPD(error, media->SetTObjectValueL<quint32>(KMPXMediaGeneralSize, size));
        if(error != KErrNone)
        { 
            delete media;
            return false;
        }
    }
    
    // set age profile
    if(dataSelectionFlags & MediaDataAgeProfile)
    {
        // index: 0 1 2 3 4 5 6 7 8 9 ...
        // value: 1 2 3 4 5 1 2 3 ...
        quint32 profile = (index%5) + 1;
        TRAPD(error, media->SetTObjectValueL<quint32>(KVcxMediaMyVideosAgeProfile, profile));
        if(error != KErrNone)
        { 
            delete media;
            return false;
        }
    }
    
    // set "filepath"
    if(dataSelectionFlags & MediaDataFilePath)
    {
        mTmpFiles.append(new QTemporaryFile());
        mTmpFiles.last()->open();
        
        HBufC *fileName = XQConversions::qStringToS60Desc(mTmpFiles.last()->fileName());

        TRAPD(error, media->SetTextValueL( KMPXMediaGeneralUri, *fileName));
        delete fileName;
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set description
    if(dataSelectionFlags & MediaDataDesc)
    {
        videoname.Format(KMediaTestDescPrefix, index);              
        TRAPD(error, media->SetTextValueL( KMPXMediaGeneralComment, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set author
    if(dataSelectionFlags & MediaDataAuthor)
    {
        videoname.Format(KMediaTestAuthorPrefix, index);              
        TRAPD(error, media->SetTextValueL( KVcxMediaMyVideosAuthor, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set copyright
    if(dataSelectionFlags & MediaDataCopyright)
    {
        videoname.Format(KMediaTestCopyrightPrefix, index);              
        TRAPD(error, media->SetTextValueL( KMPXMediaGeneralCopyright, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set language
    if(dataSelectionFlags & MediaDataLanguage)
    {
        videoname.Format(KMediaTestLanguagePrefix, index);              
        TRAPD(error, media->SetTextValueL( KVcxMediaMyVideosAudioLanguage, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set format
    if(dataSelectionFlags & MediaDataFormat)
    {
        videoname.Format(KMediaTestFormatPrefix, index);              
        TRAPD(error, media->SetTextValueL( KMPXMediaGeneralMimeType, videoname));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set media modified date
    if(dataSelectionFlags & MediaDataModified)
    {
        QDateTime date = QDateTime::currentDateTime();
        date = date.addDays((index+1)*2);
        
        TDateTime dateTime;

        dateTime.Set(date.date().year(), TMonth(date.date().month() - 1), date.date().day() - 1, 
                          date.time().hour(), date.time().minute(), 
                              date.time().second(), date.time().msec());
        TTime ttimeDate(dateTime);
        TInt64 temp= ttimeDate.Int64();
        TRAPD(error, media->SetTObjectValueL<TInt64>(KVcxMediaMyVideosModifiedDate, ttimeDate.Int64()));
        if(error != KErrNone)
        {    
            delete media;
            return false;
        }
    }
    
    // set video count
    if(dataSelectionFlags & MediaDetailCategoryVideoCount)
    {  
        int count = (index%5) + 1;
        TRAPD(error, media->SetTObjectValueL<quint32>(KVcxMediaMyVideosCategoryItemCount, count));
        if(error != KErrNone)
        {
            return false;
        }
    }
    
    // set resolution
    if(dataSelectionFlags & MediaDataResolution)
    {
        int width = index + 1;
        int height = index + 2;
        
        TRAPD(error, media->SetTObjectValueL<quint16>(KMPXMediaVideoWidth, width));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
        TRAP(error, media->SetTObjectValueL<quint16>(KMPXMediaVideoHeight, height));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    // set bitrate
    if(dataSelectionFlags & MediaDataBitrate)
    {
        int bitrate = (index+1) * 800;
        TRAPD(error, media->SetTObjectValueL<qint16>(KMPXMediaVideoBitRate, bitrate));
        if(error != KErrNone)
        {
            delete media;
            return false;
        }
    }
    
    return true;
}

// -----------------------------------------------------------------------------
// removeArray
// -----------------------------------------------------------------------------
//
void MediaObjectFactory::removeArray()
{
    while(!mTmpFiles.isEmpty())
    {
        mTmpFiles.first()->close();
        delete mTmpFiles.first();
        mTmpFiles.removeFirst();
    }
    
    delete mArray;
    mArray = 0;
}

// End of file
    


