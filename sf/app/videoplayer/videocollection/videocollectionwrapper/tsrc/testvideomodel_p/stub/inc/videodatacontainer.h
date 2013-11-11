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
* Description: stub videodatacontainer class definition*
*/

#ifndef __VIDEODATACONTAINER_H__
#define __VIDEODATACONTAINER_H__

// INCLUDES
#include <qhash.h>
#include <qlist.h>
#include <qpair.h>
#include <mpxitemid.h>
#include <qabstractitemmodel.h>


// FORWARD DECLARATIONS
class CMPXMedia;

class VideoDataContainer
{
public:
    
    /**
     * contructor
     */
    VideoDataContainer();
    
    /**
     * destructor
     */
    virtual ~VideoDataContainer();
    
public:  // from QHash
    
    /**
     * Deallocates all CMPXMedia objects and clear containers
     */
    void clear();

    /**
     * Deallocates and removes item of provided id.
     * 
     * @param id media item id
     */
    void remove(const TMPXItemId &id);
     
    /**
     * Appends provided media object to container.
     * If item with same id exists allready, old item is being
     * removed and replaced by the new item
     * 
     * @param media - media object to append
     */
    void append(CMPXMedia *media);

    /**
     * Returns a media object from provided index
     * 
     * @param index position of wanted object
     * 
     * @return CMPXMedia pointer to media object or null if 
     *                   object is not found from wanted index
     * 
     */
    CMPXMedia* fromIndex(const int &index) const;
  
    /**
     * Returns an index of id.
     * 
     * @param id item id
     * 
     * @return int index of item or -1 if item with provided id is not found
     */
    const int& indexOfId(const TMPXItemId &id) const;

    /**
     * Returns id of item from provided index
     * 
     * @param index potisiotn where to look for item
     * 
     * @return id of item or invalid TMPXItemId if item is not found from provided index
     */
    const TMPXItemId& idFromIndex(const int &index) const;

    /**
     * returns count of items
     * 
     * @return int 
     */
    int count() const;
    
    /**
    * Method removes item from data container at provided index and 
    * appends it into removed buffer
    * 
    * @param inteIndex index of item
    * @return TMPXItemId id of the item marked as removed
    */
    const TMPXItemId markItemRemoved(const int &itemIndex);
   
    /**
     * Method removes provided items from mRemovedMedia
     * 
     * @param itemIds ids of items to be removed. If null, removes all
     * @return int count of items actually removed
     */
    int clearRemoved(QList<TMPXItemId> *itemIds = 0);
    
    /**
     * Method removed provided items from mRemovedMedia and returns them
     * int actual container
     * 
     * @param itemIds ids of items to be restored. If null, restores all
     * 
     * @return int count of items actually restored
     */
    int restoreRemovedItems(QList<TMPXItemId> *itemIds = 0);
    
    /**
     * Returns item from removed buffer
     * 
     * @param itemId id of item to be returned
     */
    CMPXMedia* getRemovedMedia(TMPXItemId &itemId);
    
    /**
     * decrements indexes of items after provided index by one.
     */
    void decHashIndexesAfter(const int &fromIndex);
    

public: // data
    
    /**
     * list of media ids used to fetch item thought index.
     */
    QList<TMPXItemId> mMediaIds;
    
    /**
     * lookup hash for fetching correct item based on id.
     * Multihash needed in stub to be able to make sure appending 
     * works ok
     * Key: item id
     * value: pair, where first is item index and second is item data
     */
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> > mMediaData;
    
    /**
     * lookup hash for media items that are deleted, but not yet completely
     * removed from the filesystem.
     */
    QHash<TMPXItemId, CMPXMedia*> mRemovedMedia;

};

#endif  // __VIDEODATACONTAINER_H__


    


