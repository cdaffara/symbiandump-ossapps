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
* Description:  This class is responsible for Artist table.
*
*
*/


#ifndef MPXDBARTIST_H
#define MPXDBARTIST_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS DECLARATION

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbArtist :
    public CMPXDbCategory
    {
    public:
        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbArtist instance.
        */
        static CMPXDbArtist* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbArtist instance on the cleanup stack.
        */
        static CMPXDbArtist* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);
        
        /**
        * Destructor
        */
        virtual ~CMPXDbArtist();

    public:
        /**
        * Add a artist item. If the record already exists, its counter will
        * be incremented.
        * @param aCategory category type
        * @param aMedia: This is the media object
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aNewRecord: indicates to the caller if a new record is created.
        *        ETrue if a new row is created in the table; otherwise EFalse.
        * @return The unique id of the row added.
        */
        virtual TUint32 AddItemL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia, TInt aDriveId, TBool& aNewRecord,
            TBool aCaseSensitive = ETrue);

        /**
        * Gets the details for all category items.
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the requested attributes for all items
        */
        void GetAllCategoryItemsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Update a category item.
        * @param aId: The ID of the category to update
        * @param aMedia: The media data
        * @param aDriveId: The Drive Id the name (category) belongs
        * @param aItemChangedMessages: if valid on return contains a updated message if the
        *        category was updated
        */
        virtual void UpdateItemL(TUint32 aId, const CMPXMedia& aMedia, TInt aDriveId, CMPXMessageArray* aItemChangedMessages);
        
        /**
        * Decrement the number of songs for the item. If the song and album count gets to 0, 
        * remove the item.
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
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        virtual void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, TBool aMtpInUse = EFalse); 
#else
        virtual void DecrementSongsForCategoryL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TBool& aItemExist);
#endif //ABSTRACTAUDIOALBUM_INCLUDED            

        /**
        * Add a AlbumArtist. If the record already exists, its counter will
        * be incremented.
        * @param aName: This is the name to be entered into the row
        * @param aArt: Album art
        * @param aDriveId: The Drive Id the name (category) belongs
        * @return The unique id of the row added.
        */
        TUint32 AddAlbumArtistL(const TDesC& aName, const TDesC& aArt, TInt aDriveId);
        
        /**
        * Decrement the number of albums for the item. If the song and album count gets to 0, 
        * remove the item.
        * @param aId: The ID of the category to delete.
        * @param aDriveId: The drive Id the name (category) belongs to. Songs on different
        *                  drives may belong to the same album or artist; consequently, one
        *                  row for each artist/album id and drive ID pair will exist in the
        *                  lookup table with a count existing for each row that tracks the
        *                  number number of songs on that drive with that album/artist.
        * @param aItemChangedMessages if valid on return contains a deleted message if the
        *                  category was deleted
        */
        void DecrementAlbumsForArtistL(TUint32 aId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages);       

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
        * Generate searching criteria for artist table from the given criteria
        * @param aMedia media data
        * @param aFields fields of Artist table
        * @param aValues values of each field of Artist table
        * @return a string containing the selection criteria. The ownership is passed to the caller.
        */
        void GenerateArtistFieldsValuesL(const CMPXMedia& aMedia,
            CDesCArray& aFields, CDesCArray& aValues);
       
        /**
        * Gets the Songs count and Albums count for a Artist item
        * @param aDrivceId The Drive Id the name (category) belongs
        * @param aId identifies the Artist item
        * @param aSongsCount songs count
        * @param aAlbumsCount albums count
        */
        void GetSongsAndAlbumsCountL(TInt aDriveId, TUint32 aId, TInt& aSongsCount, TInt& aAlbumsCount);
        
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
        CMPXDbArtist(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the category tables
        */
        enum TArtistColumns
            {
            EArtistUniqueId = KMPXTableDefaultIndex,
            EArtistName,
            EArtistSongCount,
            EArtistAlbumCount,
            EArtistArt,
            EArtistFieldCount
            };
    };
    
#endif // MPXDBARTIST_H

// End of File
