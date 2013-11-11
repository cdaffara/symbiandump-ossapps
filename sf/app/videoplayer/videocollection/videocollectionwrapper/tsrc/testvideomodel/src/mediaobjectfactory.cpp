
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

// INCLUDES

#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qdatetime.h>
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
// createMediaItems
// -----------------------------------------------------------------------------
//
void MediaObjectFactory::createMediaItems(int count, int mediaType, MediaDetailSelection flags)
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
    
    // - create 10 media objects and add them into array
    CMPXMedia *media;
    
    
    for(int i = 0; i < count; ++i)
    {
        media = NULL;
        TRAPD(error, media = CMPXMedia::NewL());
        if(!media)
        {
            delete mArray;
            mArray = 0;
            return;
        }
        
        if(!fillMediaDatas(media, i, mediaType, flags))
        {
            delete media;
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

// -----------------------------------------------------------------------------
// fillMediaDatas
// -----------------------------------------------------------------------------
//
bool MediaObjectFactory::fillMediaDatas(CMPXMedia* media, int index, int mediaType, MediaDetailSelection detailSelectionFlag)
{
    if(!media)
    {
        return false;
    }
    TBuf<65> videoname;
    TMPXItemId itemId;
    // set media id
    itemId.iId1 = index;
    itemId.iId2 = mediaType;
    TRAPD(error, media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId));
    if(error != KErrNone)
    {
        return false;
    }
   
    // set media name
    videoname.Format(KMediaTestNamePrefix, index);              
    TRAP(error, media->SetTextValueL( KMPXMediaGeneralTitle, videoname));
    if(error != KErrNone)
    {
        return false;
    }
    
    // set media rating
    if(detailSelectionFlag & MediaDetailRatingFlag)
    {
        int rating = (index%5) +1;
        TRAPD(error, media->SetTObjectValueL<TInt>(KVcxMediaMyVideosRating, rating));
        if(error != KErrNone)
        {
            return false;
        }
    }
    
    // set media date
    if(detailSelectionFlag & MediaDetailDateFlag)
    {
        TDateTime dateTime;
        dateTime.Set(2009, TMonth(index), index, index+1, index+1, index+1, index+1);
        TTime ttimeDate(dateTime);
        TInt64 temp= ttimeDate.Int64();
        TRAPD(error, media->SetTObjectValueL<TInt64>(KMPXMediaGeneralDate, ttimeDate.Int64()));
        if(error != KErrNone)
        {    
            return false;
        }
    }
    
    // set duration
    if(detailSelectionFlag & MediaDetailDurationFlag)
    {
        float duration = 1.0 * index;
        TRAPD(error, media->SetTObjectValueL<float>(KMPXMediaGeneralDuration, duration));
        if(error != KErrNone)
        { 
            return false;
        }
    }
    
    // set size
    if(detailSelectionFlag & MediaDetailSizeFlag)
    {
        quint32 size = 1.0 + index;
        TRAPD(error, media->SetTObjectValueL<quint32>(KMPXMediaGeneralSize, size));
        if(error != KErrNone)
        { 
            return false;
        }
    }

    // set file path
    if(detailSelectionFlag & MediaDetailFilePathFlag)
    {
        videoname.Format(KMediaTestFilePathPrefix, index);              
        TRAP(error, media->SetTextValueL( KMPXMediaGeneralUri, videoname));
        if(error != KErrNone)
        {
            return false;
        }
    }
    
    // set video count
    if(detailSelectionFlag & MediaDetailCategoryVideoCount)
    {  
        int count = 10 + index;
        TRAPD(error, media->SetTObjectValueL<quint32>(KVcxMediaMyVideosCategoryItemCount, count));
        if(error != KErrNone)
        {
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
    delete mArray;
    mArray = 0;
}

// End of file
    


