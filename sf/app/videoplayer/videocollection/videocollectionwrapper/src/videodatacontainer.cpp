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
* Description:   VideoDataContainer class declaration*
*/

// Version : %version: 15 %

// INCLUDE FILES
#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>

#include "videodatacontainer.h"
#include "videocollectionutils.h"
#include "videocollectiontrace.h"

const int INVALID_INDEX = -1;
const TMPXItemId INVALID_ID = TMPXItemId::InvalidId();


/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// -----------------------------------------------------------------------------
// VideoDataContainer
// -----------------------------------------------------------------------------
//
VideoDataContainer::VideoDataContainer()
{
	FUNC_LOG;
}
    
// -----------------------------------------------------------------------------
// VideoDataContainer
// -----------------------------------------------------------------------------
//
VideoDataContainer::~VideoDataContainer()
{
	FUNC_LOG;
    clear();
    clearRemoved();
}

// -----------------------------------------------------------------------------
// clear
// -----------------------------------------------------------------------------
//
void VideoDataContainer::clear()
{
	FUNC_LOG;
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator i = mMediaData.begin();
    while(i != mMediaData.end())
    {
        delete (*i).second;
        ++i;
    }
    mMediaData.clear();
    mMediaIds.clear();
}

// -----------------------------------------------------------------------------
// remove
// -----------------------------------------------------------------------------
//
void VideoDataContainer::remove(const TMPXItemId &id)
{   
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator removeIter = mMediaData.find(id);
    if(removeIter == mMediaData.end())
    {
        return;
    }
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator hashIter;     
    mMediaIds.removeAt(removeIter->first);    
    // sync item indexes whose ids exist in id- list after
    // recently removoved. 
    decIndexesAfter(removeIter->first);
    
    delete removeIter->second;
    mMediaData.erase(removeIter);
}   

// -----------------------------------------------------------------------------
// append
// -----------------------------------------------------------------------------
//
void VideoDataContainer::append(CMPXMedia *media)
{
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(media, KMPXMediaGeneralId, mediaId );

    if(mediaId == TMPXItemId::InvalidId())
    {
        // could not get id or id does not match ==> NOP
        return;       
    }
    
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator iter = mMediaData.find(mediaId);
    // if item is in the removal list, not allowed to append
    if(mRemovedMedia.contains(mediaId))
    {
        delete media;
        return;    
    }
    
    // if item exist, do not add into container
    if(iter != mMediaData.end())
    {
        delete media;
        return;
    }      
    mMediaIds.append(mediaId);
    mMediaData[mediaId] = qMakePair( mMediaIds.count() - 1, media);     
}


// -----------------------------------------------------------------------------
// fromIndex
// -----------------------------------------------------------------------------
//
CMPXMedia* VideoDataContainer::fromIndex(const int &index) const
{   
    if(index >= 0 && index < mMediaIds.count() && mMediaData.contains(mMediaIds[index]))
    {   
        return (mMediaData.find(mMediaIds[index]))->second;
    }
    return 0;  
}
 
// -----------------------------------------------------------------------------
// indexOfId
// -----------------------------------------------------------------------------
//
const int& VideoDataContainer::indexOfId(const TMPXItemId &id) const 
{
    QHash<TMPXItemId, QPair<int, CMPXMedia*> >::const_iterator iter = mMediaData.find(id);
    if( iter != mMediaData.constEnd())
    {
        return iter->first;
    }
    return INVALID_INDEX;
}

// -----------------------------------------------------------------------------
// idFromIndex
// -----------------------------------------------------------------------------
//
const TMPXItemId& VideoDataContainer::idFromIndex(const int &index) const
{   
    if(index >= 0 && index < mMediaIds.count())
    {
        return mMediaIds.at(index);
    }
    return INVALID_ID;    
}

// -----------------------------------------------------------------------------
// count
// -----------------------------------------------------------------------------
//
int VideoDataContainer::count() const
{
    return mMediaData.count();
}

// -----------------------------------------------------------------------------
// decIndexesAfter
// -----------------------------------------------------------------------------
//
void VideoDataContainer::decIndexesAfter(const int &fromIndex)
{
    int count = mMediaIds.count();
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator hashIter;   
    for(int i = fromIndex; i < count; ++i)
    {
        hashIter = mMediaData.find(mMediaIds[i]);
        if(hashIter != mMediaData.end())
        {
            hashIter->first--;
        }
    }   
}

// -----------------------------------------------------------------------------
// markItemsRemoved
// -----------------------------------------------------------------------------
//
const TMPXItemId VideoDataContainer::markItemRemoved(const int &itemIndex)
{
    // for all provided indexes:
    // - get item address from mMediaData
    // - get item index from mMediaData
    // - remove item from mMediaData, do not deallocate object
    // - remove item's id from mMediaIds -list 
    // - append item into mRemovedMedia
    // - append item's id into returned id -list
    CMPXMedia *media = 0;
    // get copy of id of item to be removed
    const TMPXItemId id = idFromIndex(itemIndex);
    media = fromIndex(itemIndex);    
    if(id == TMPXItemId::InvalidId() || !media)
    {
        return id;
    }
    if(!mRemovedMedia.contains(id))
    {
        mRemovedMedia[id] = media;  
    }
    mMediaData.remove(id);
    mMediaIds.removeAt(itemIndex);
    decIndexesAfter(itemIndex);
    return id;
}

// -----------------------------------------------------------------------------
// clearRemoved
// -----------------------------------------------------------------------------
//
int VideoDataContainer::clearRemoved(QList<TMPXItemId> *itemIds)
{
	FUNC_LOG;
    int count = 0;
    QList<TMPXItemId> ids;

    QList<TMPXItemId>::const_iterator iterEnd;
    if(!itemIds)
    {
        ids = mRemovedMedia.keys();
    }
    else
    {
        ids = *itemIds;
    }
    QList<TMPXItemId>::const_iterator idIter = ids.constBegin();
    QHash<TMPXItemId, CMPXMedia*>::iterator iter;
    while(idIter != ids.constEnd())
    {
        iter = mRemovedMedia.find((*idIter));
        if(iter != mRemovedMedia.end())
        {
            delete (*iter);
            mRemovedMedia.remove((*idIter));
            count++;
        }        
        ++idIter;
    }
    return count;
}

// -----------------------------------------------------------------------------
// restoreRemovedItems
// -----------------------------------------------------------------------------
//
int VideoDataContainer::restoreRemovedItems(QList<TMPXItemId> *itemIds)
{  
	FUNC_LOG;
    
    int count = 0;
    QList<TMPXItemId> ids;

    QList<TMPXItemId>::const_iterator iterEnd;
    if(!itemIds)
    {
        ids = mRemovedMedia.keys();
    }
    else
    {
        ids = *itemIds;
    }
    
    QList<TMPXItemId>::const_iterator idIter = ids.constBegin();
    QHash<TMPXItemId, CMPXMedia*>::iterator iter;
    while(idIter != ids.constEnd())
    {
        iter = mRemovedMedia.find((*idIter));        
        if(iter != mRemovedMedia.constEnd() && !mMediaData.contains(iter.key()))
        {
            // append data to actual containers and remove item from deleted hash
            mMediaIds.append(iter.key());
            mMediaData[iter.key()] = qMakePair(mMediaIds.count() - 1, iter.value());
            mRemovedMedia.remove((*idIter));
            count++;    
        }
        ++idIter;
    }    
    return count;
}

// -----------------------------------------------------------------------------
// getRemovedMedia
// -----------------------------------------------------------------------------
//
CMPXMedia* VideoDataContainer::getRemovedMedia(TMPXItemId &itemId)
{
    QHash<TMPXItemId, CMPXMedia*>::const_iterator itemIter = 
                                            mRemovedMedia.constFind(itemId);
    if(itemIter != mRemovedMedia.constEnd())
    {
        return itemIter.value();
    }
    return 0;
}

// end of file
