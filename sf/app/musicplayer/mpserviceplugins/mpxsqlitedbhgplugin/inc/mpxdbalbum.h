/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is responsible for all category tables (Artist,
*                Album, Genre, Composer).
*
*
*/


#ifndef MPXDBALBUM_H
#define MPXDBALBUM_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS DECLARATION

class MMPXDbAlbumObserver
    {
    public:
        /**
        * HandleAlbumartForAlbumL.
        * @param aId, The album ID.
        * @param aArt, The albumart uri.
        * @returns alternative albumart retrieved in the specified Album.
        */
        virtual HBufC* HandleAlbumartForAlbumL( const TUint32 aId, TPtrC aArt ) = 0;
               
        /**
        * Called when album artist is removed from an album
        * @param aCategoryId category item ID
        * @param aDrive drive the category item is on
        * @param aItemChangedMessages changed mesages array to be updated or NULL
        */
        virtual void DeleteAlbumForArtistL(TUint32 aCategoryId,
            TInt aDrive, CMPXMessageArray* aItemChangedMessages) = 0;

        /**
        * Called when album artist is added to an album
        * @param aName name string
        * @param aArt Album art path
        * @param aDriveId drive to add the category to
        * @return the ID of the category item (new or existing)
        */
        virtual TUint32 AddAlbumArtistL(const TDesC& aName, const TDesC& aArt, TInt aDriveId) = 0;
    };
/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbAlbum :
    public CMPXDbCategory
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance.
        */
        static CMPXDbAlbum* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance on the cleanup stack.
        */
        static CMPXDbAlbum* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Destructor
        */
        virtual ~CMPXDbAlbum();

    public:
        /**
        * Add an album item. If the record already exists, its counter will
        * be incremented.
        * @param aCategory category type
        * @param aMedia: The media object
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aNewRecord: indicates to the caller if a new record is created.
        *        ETrue if a new row is created in the table; otherwise EFalse.
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return The unique id of the row added.
        */
        virtual TUint32 AddItemL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = ETrue);

        /**
        * Decrement the number of songs for the item. If the count gets to 0, remove
        * the item.
        * @param aId: The ID of the category to delete.
        * @param aDriveId: The drive Id the name (category) belongs to. Songs on different
        *                  drives may belong to the same album or artist; consequently, one
        *                  row for each artist/album id and drive ID pair will exist in the
        *                  lookup table with a count existing for each row that tracks the
        *                  number number of songs on that drive with that album/artist.
        * @param aItemChangedMessages if valid on return contains a deleted message if the
        *                  category was deleted
        * @param aItemExist Out parameter, ETrue if the category is not deleted after the delete,
        *        EFalse otherwise
        * @param aArt: The albumart uri
        */
        void DecrementSongsForAlbumL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, const TDesC& aArt );

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsMediaWallL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets details for the category items under a parent category item.
        * This applies to albums for a given artist. The method panics if the
        * category or parent category are invalid.
        * @param aParentCategory identifies the parent category
        * @param aParentId identifies the parent item
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        virtual void GetSubCategoryItemsL(TMPXGeneralCategory aParentCategory, TUint32 aParentId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);
        
        /**
        * Update a category item.
        * @param aId: The ID of the category to update
        * @param aMedia: The media data
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aItemChangedMessages: if valid on return contains a updated message if the
        *        category was updated
        */
        void UpdateItemL(TUint32 aId, const CMPXMedia& aMedia, TInt aDriveId, CMPXMessageArray* aItemChangedMessages);

    public:        
        /**
        * Get songs count for a specified album and a specified artist
        * @param aArtistId: The ID of the artist
        * @param aAlbumId: The ID of the album
        */
        TInt GetSongsCountInAlbumMatchingArtistL(TUint32 aArtistId, TUint32 aAlbumId);
       /**
        * Check if the specified album ID is an unknown album.
        * An “Unknown album” is one where the album name and album artist are unknown.
        * @param aId: The ID of the album
        * @return ETrue if it's an unknown album
        */
        TBool IsUnknownAlbumL(const TUint32 aId);
        
        /**
        * Generate Unique ID for Album
        * @param aMedia media object
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return the ID of album
        */
        TUint32 GenerateUniqueIdL(const CMPXMedia& aMedia);

    private:
        /**
        * Updates the media with information from the table
        * @param aRecord record containing the source fields
        * @param aAttrs attributes to be returned
        * @param aMedia returns the requested attributes
        */
        void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

         /**
        * Generate searching criteria for album table from the given criteria
        * @param aMedia media data
        * @param aFields fields of Album table
        * @param aValues values of each field of Album table
        * @return a string containing the selection criteria. The ownership is passed to the caller.
        */
        void GenerateAlbumFieldsValuesL(const CMPXMedia& aMedia,
            CDesCArray& aFields, CDesCArray& aValues);
        
        /**
        * Get the Albumart of song which belongs to the specified Album.
        * @param aId The ID of the album
        * @param aArt albumart with the song is deleted.
        * @returns alternative albumart retrieved in the specified Album.
        */
        HBufC* AlbumartForAlbumL(const TUint32 aId, TPtrC aArt);
        
        /**
        * Processes a recordset by constructing media instances and adding them
        * to the specified array.
        * @param aAttrs attributes to be returned
        * @param aRecordset recordset to be processed
        * @param aMediaArray returns the media instances
        */
        void ProcessAlbumRecordSetL(const TArray<TMPXAttribute>& aAttrs,
            RSqlStatement& aRecordset, CMPXMediaArray& aMediaArray);
        
        /**
        * Processes a recordset by constructing media instances and adding them
        * to the specified array.
        * @param aAttrs attributes to be returned
        * @param aRecordset recordset to be processed
        * @param aMediaArray returns the media instances
        */
        void ProcessMediaWallAlbumRecordSetL(const TArray<TMPXAttribute>& aAttrs,
            RSqlStatement& aRecordset, CMPXMediaArray& aMediaArray);
        
        /**
        * Retrieve AlbumArtist name from media object
        * @param aMedia media object
        * @param aName returned name
        * @return ETrue if name is available
        */   
        TBool RetrieveAlbumArtistL(const CMPXMedia& aMedia, TPtrC& aName);

        /**
        * Retrieve Artist name from media object
        * @param aMedia media object
        * @param aName returned name
        */   
        void RetrieveArtist(const CMPXMedia& aMedia, TPtrC& aName);

        /**
        * Check if Art needs to be updated
        * @param aDeletedSongArt deleted song's art
        * @param aCurrentAlbumArt Album's current art
        * @return ETrue if aDeletedSongArt is not non-embedded album art and 
        *         aDeleteSongArt and aCurrentAlbumArt are the same and known
        */   
        TBool NeedToUpdateArt(const TDesC& aDeletedSongArt, const TDesC& aCurrentAlbumArt);
        
    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbAlbum(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory, MMPXDbAlbumObserver& aObserver);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    public:

        /**
        * Column indexes in the category tables
        */
        enum TAlbumColumns
            {
            EAlbumUniqueId = KMPXTableDefaultIndex,
            EAlbumName,
            EAlbumSongCount,      
            EAlbumArtist,
            EAlbumArtistName,
            EAlbumArt,
            EAlbumFieldCount
            };

    private:    // Data
        MMPXDbAlbumObserver& iObserver;
        RPointerArray<CMPXMedia> iUnknownArtists;
        RPointerArray<CMPXMedia> iUnknownAlbums;
    };
    
#endif // MPXDBALBUM_H

// End of File
