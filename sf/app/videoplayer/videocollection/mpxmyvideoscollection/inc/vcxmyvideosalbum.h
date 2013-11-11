/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*

* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Class representing album in My Videos collection.
*/


#ifndef CVCXMYVIDEOSALBUM_H
#define CVCXMYVIDEOSALBUM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class CMPXMedia;
class CVcxMyVideosCollectionPlugin;

// CLASS DECLARATION

/**
 * Contains information about video which belongs to some album.
 * These are stored to CVcxMyVideosAlbum::iVideoList.
 */
NONSHARABLE_CLASS(TVcxMyVideosAlbumVideo)
    {
    public:

        /**
         * Constructor.
         */
        TVcxMyVideosAlbumVideo();

        /**
        * = operator.
        */        
        TVcxMyVideosAlbumVideo& operator=( const TVcxMyVideosAlbumVideo& aVideo );
    private:
        
        /**
        * Set values.
        */
        void Set( TUint32 aMdsId, TUint32 aRelationMdsId, CMPXMedia* aVideo );

    public:
        TUint32    iMdsId;         //Video object ID in MDS.
        TUint32    iRelationMdsId; //Relation ID in MDS, this is used when removing items from album.
        CMPXMedia* iMedia;
    };

/**
 * Contains list of videos belonging to an album. Videos can be searched, added and
 * removed. Videos are kept in sorting order by MDS ID to speed up access. Pointers
 * to media objects in collection cache are also stored to speed up access.
 * Contains also logic for keeping album media attributes up to date.
 */
NONSHARABLE_CLASS(CVcxMyVideosAlbum) : public CBase
    {    
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CVcxMyVideosAlbum();

    /**
     * Two-phased constructor.
     */
    static CVcxMyVideosAlbum* NewL( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

    /**
     * Two-phased constructor.
     */
    static CVcxMyVideosAlbum* NewLC( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

    // new methods
    
    /**
     * Sets up this album.
     * 
     * @param aAlbum  Media class to set from.
     */
    void SetL( /*CMPXMedia& aAlbum*/ );
    
    /**
     * Sorts iVideoList by MDS ID.
     */
    void Sort();
    
    /**
     * Checks if aMdsId belongs to this album. Checking is
     * done using bisection method.
     * 
     * @param aMdsId  Id to check.
     */
    TBool BelongsToAlbum( TUint32 aMdsId );

    /**
     * Finds video from iVideoList. Uses bisection method, ie
     * is fast.
     * 
     * @param aMdsId  Video to find.
     * @param aVideo  If found, this is filled with data.
     * @param aIndex  Position in iVideoList where from the video
     *                was found.
     * @return        System-wide error code. If != KErrNone, then
     *                aVideo and aIndex won't contain valid data.
     */
    TInt Video( TUint32 aMdsId, TVcxMyVideosAlbumVideo& aVideo, TInt& aIndex );

    /**
     * Adds aVideo to this album. The id array is kept in ascending order.
     * 
     * @param aVideo  Video to add.
     */
    void AddL( TVcxMyVideosAlbumVideo aVideo );

    /**
     * Removes videos from this album.
     * 
     * @param aMdsIds Array containing IDs of the videos to remove.
     */
    void Remove( RArray<TUint32>& aMdsIds );

    /**
     * Removes aMdsId from this album.
     * 
     * @param aMdsId    ID to remove.
     * @param aCompress If ETrue, then Compress is called to iVideoList.
     *                  ETrue as default.
     */
    void Remove( TUint32 aMdsId, TBool aCompress = ETrue );
    
    /**
     * Used for keeping RArray<TVcxMyVideosAlbumVideo> in integer order by
     * TVcxMyVideosAlbumVideo::iMdsId.
     * 
     * @param aVideo1 Video to compare
     * @param aVideo2 Video to compare
     * @return -1 if aVideo1 is smaller than aVideo2, 1 if aVideo1 is larger than
     *         aVideo2.
     */
    static TInt CompareVideosByMdsId( const TVcxMyVideosAlbumVideo& aVideo1,
            const TVcxMyVideosAlbumVideo& aVideo2 );

    /**
     * Creates video list belonging to this album. Ownership goes to caller.
     * 
     * @return Media containing media array. Array items are videos.
     */
    CMPXMedia* CreateVideoListL();

    /**
     * Appends items from aFromVideoList to aToVideoList if they belong to
     * this album.
     * 
     * @param aFromVideoList     From list.
     * @param aToVideoList       To List.
     * @param aNewItemStartIndex Start index of the copy. Items in aFromVideoList before this
     *                           value are ignored.
     */
    void AppendToVideoListL( CMPXMedia& aFromVideoList,
            CMPXMedia& aToVideoList, TInt aNewItemStartIndex );

    /**
     * Calculates KVcxMediaMyVideosCategoryItemCount, KVcxMediaMyVideosCategoryNewItemCount,
     * KVcxMediaMyVideosCategoryNewItemName and KMPXMediaGeneralDate attributes to iMedia.
     * 
     * @return Returns ETrue if album attributes were modified, EFalse otherwise.
     */
    TBool CalculateAttributesL();
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CVcxMyVideosAlbum( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private: //data
    CVcxMyVideosCollectionPlugin& iCollection;
    
public: //data
    
    /**
     * MDS ID of this album.
     */
    TUint32 iMdsId;

    /**
     * MPX media object representing this album in collection, not own.
     * The item is owned by CVcxMyVideosAlbums::iAlbumList
     */
    CMPXMedia* iMedia;

    /**
     * Array containing info about videos belonging to this album.
     */
    RArray<TVcxMyVideosAlbumVideo> iVideoList;
            
    };

#endif // CVCXMYVIDEOSALBUM_H
