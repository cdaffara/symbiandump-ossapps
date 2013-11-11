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
*                Album, Genre, Composer, AbstractAlbum).
*
*
*/


#ifndef MPXDBABSTRACTALBUM_H
#define MPXDBABSTRACTALBUM_H

// INCLUDES
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include "mpxdbcategory.h"


// CLASS FORWARDS

// CLASS DECLARATION

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbAbstractAlbum :
    public CMPXDbCategory,
    public MThumbnailManagerObserver
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance.
        */
        static CMPXDbAbstractAlbum* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, RFs& aFs);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance on the cleanup stack.
        */
        static CMPXDbAbstractAlbum* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory, RFs& aFs);

        /**
        * Destructor
        */
        virtual ~CMPXDbAbstractAlbum();

        /**
        * Find the drive Id for a abstractalbum
        * @param aAbstractAlbumId identifies the abstractalbum
        * @return corresponding drive id
        */
        TInt GetDriveIdL(TUint32 aAbstractAlbumId);

        /**
        * Add a category item. If the record already exists, its counter will
        * be incremented.
        * @param aName: This is the name to be entered into the row
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aNewRecord: indicates to the caller if a new record is created.
        *        ETrue if a new row is created in the table; otherwise EFalse.
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return The unique id of the row added.
        */
        TUint32 AddItemL( const TDesC& aUri, const TDesC& aName, const TDesC& aAlbumArtist, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = EFalse);

        /**
        *  Add existing item to table with same entry fields values except new Uri.
        * @param aOldId: existing item Id
        * @param newUri: new Uri
        * @return The unique id of the row added.
        */      
        TUint32 AddUpdatedItemL(TUint32 aOldId, const TDesC& newUri);

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
        */
        void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, TBool aMtpInUse);
        
        /**
        * Remove a abstractalbum, remove entry from AbstractAlbum table, 
        * Thumbnail table, corresponding alb from file system
        * @param aAbstractAlbumId ID of the abstractalbum to remove
        * @param aItemChangedMessages a list of change events as a result of the
        *        abstractalbum removal
        * @param aFileDeleted indicate if alb file already deleted from file system
        */
        void RemoveAbstractAlbumL(TUint32 aAbstractalbumId, 
            CMPXMessageArray& aItemChangedMessages, TBool aFileDeleted);
        
        /**
        * Update a category item.
        * @param aId: The ID of the category to update
        * @param aMedia: The media data
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aItemChangedMessages: if valid on return contains a updated message if the
        *        category was updated
        */
        void UpdateItemL(TUint32 aId, const CMPXMedia& aMedia, TInt aDriveId, CMPXMessageArray* aItemChangedMessages);
          
        /**
        * Delete or Rename Thumbnail from TN DB by calling Thumbnail manager
        * Rename Thumbnail if aNewPath is not NULL, otherwise Delete Thumbnail
        * @param aOldPath: old Uri
        * @param aNewPath: new Uri
        * @param aPriority: priority pass to Thumbnail manager
        */
        void HandleTNL( const TDesC& aOldPath, const TDesC& aNewPath = KNullDesC, TInt aPriority =0 );

        /**
        * Get the uri field for a given ID.
        * @param aId identifies the abstractalbum item
        * @return uri that matches aId. Ownership is abandoned.
        */
        HBufC* GetUriL(TUint32 aId);

        /**
        * Get Ids of albstractalbum with no song associated.
        * Rename Thumbnail if aNewPath is not NULL, otherwise Delete Thumbnail
        * @param aItemsIds: array of Ids for abstractalbums which have no song associated.
        */
        void GetAllItemsWithNoSongL(RArray<TUint32>& aItemsIds);

        /**
        * Create Thumbnail Manager session.
        * Create Thumbnail Manager session when needed: rename/delete entries in Thumbnail table.
        */
        void CreateTNMSessionL();
        
        /**
        * Remove all abstractalbum items with no songs associated,
        * TN entry and .alb files also removed.
        *
        */
        void RemoveAbstractAlbumsWithNoSongL();
        
        /**
        /* When refresh library, Remove .alb entry from AbstractAlnum table, TN table if .alb files already deleted 
        /* from file system via file manager.
        */
        void AbstractAlbumCleanUpL();
       
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
        void GenerateAbstractAlbumFieldsValuesL(const CMPXMedia& aMedia,
            CDesCArray& aFields, CDesCArray& aValues);
        
        /**
        * Remove a abstractalbum from AbstractAlbum table
        * @param aAbstractAlbumId identifies the abstractalbum
        * @param aDriveId drive Id
        */
        void DeleteAbstractAlbumEntryL(TUint32 aAbstractAlbumId,  TInt aDriveId = 0);
        

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbAbstractAlbum(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory, RFs& aFs);

        /**
        * Second phase constructor.
        */
        void ConstructL();

        /**
        * MThumbnailManagerObserver
        */
        void ThumbnailPreviewReady(
            MThumbnailData& aThumbnail, TThumbnailRequestId aId );
        /**
        * MThumbnailManagerObserver
        */
        void ThumbnailReady(
            TInt aError,
            MThumbnailData& aThumbnail, TThumbnailRequestId aId );

    public:

        /**
        * Column indexes in the category tables
        */
        enum TAbstractAlbumColumns
            {
            EAbstractAlbumUniqueId = KMPXTableDefaultIndex,
            EAbstractAlbumUri,
            EAbstractAlbumName,
            EAbstractAlbumArtist,
            EAbstractAlbumSongCount,
            EAbstractAlbumVolumeId,
            EAbstractAlbumFieldCount
            };

    private:    // Data
        CThumbnailManager*        iTNManager;
        TBool                     iRenameTN;
        RFs&                      iFs;

    };
#endif // MPXDBAbstractAlbum_H

// End of File
