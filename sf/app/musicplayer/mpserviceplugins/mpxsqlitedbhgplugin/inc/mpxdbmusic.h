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
* Description:  This class is responsible for all music table functionality.
*                The Music table contains one record for each song on the database
*                drive.
*
*
*/


#ifndef MPXDBMUSIC_H
#define MPXDBMUSIC_H

// INCLUDES
#include "mpxdbtable.h"
#include "mpxdbcommonstd.h"
#include "mpxdbactivetask.h"

// CLASS FORWARDS
class CMPXMedia;
class CMPXMediaArray;

// CLASS DECLARATION

/**
* Observer interface for handling the category ID functionality.
* This is needed because the incoming CMPXMedia parameters in Add and Update methods
* contain the names for the category items and the music table has to use IDs linking
* to the category tables. Using this interface removes the dependency to the category
* class.
*
* Note that this interface is only used when information is written to the Music table,
* when information is retrieved the query takes care of also retrieving the names from
* the corresponding category tables so the outgoing CMPXMedia will contain the correct
* information.
*/
class MMPXDbMusicObserver
    {
    public:
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Called when a new category item has to be added to a category table.
        * @param aName Abstract Album art name
        * @param aDrive drive to add the category to
        * @param aItemChangedMessages changed mesages array to be updated or NULL
        * @param aItemExist Out parameter, ETrue if the item already exist before the add,
        *        EFalse otherwise
        * @param aUri Abstract Album art Uri
        * @param aAlbumArtist AlbumArtist string
        * @return the ID of the category item (new or existing)
        */
        virtual TUint32 AddAbstractAlbumItemL(const TDesC& aName, 
            TInt aDriveId, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist,
            const TDesC& aUri, const TDesC& aAlbumArtist=KNullDesC) = 0;
#endif // ABSTRACTAUDIOALBUM_INCLUDED

        /**
        * Called when a new category item has to be added to a category table.
        * @param aCategory category type
        * @param aMedia media object
        * @param aDrive drive to add the category to
        * @param aItemChangedMessages changed mesages array to be updated or NULL
        * @param aItemExist Out parameter, ETrue if the item already exist before the add,
        *        EFalse otherwise
        * @return the ID of the category item (new or existing)
        */
        virtual TUint32 AddCategoryItemL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia,       
            TInt aDrive, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist) = 0;

        /**
        * Called when the ID of a category item changed for a Music record, for example when
        * the artist name changed for a song. The implementation is supposed to update the
        * category record and add corresponding item changed messages to the array parameter.
        * @param aCategory category type
        * @param aCategoryId old category item ID
        * @param aDrive drive the category item is on
        * @param aItemChangedMessages changed mesages array to be updated or NULL
        * @param aItemExist Out parameter, ETrue if the category is not deleted after the delete,
        *        EFalse otherwise
        * @param aArt Art of the music record. 
        *             Art needs to be given so that the cateogry can switch over to different art 
        *             if necessary
        * 
        */
        virtual void DeleteSongForCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            TInt aDrive, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, const TDesC& aArt = KNullDesC) = 0;

        /**
        * Called when the play count attribute of a song is changed. Should add an item changed
        * message to the array.
        * @param aItemChangedMessages array of item changed messages to add to
        */
        virtual void HandlePlayCountModifiedL(CMPXMessageArray& aItemChangedMessages) = 0;

        /**
        * Called when the playback time attribute of a song is changed. Should add an item changed
        * message to the array.
        * @param aItemChangedMessages array of item changed messages to add to
        */
        virtual void HandlePlaybackTimeModifiedL(CMPXMessageArray& aItemChangedMessages) = 0;

        /**
         * Called when category item is changed. Should add an item changed
         * message to the array.
         * @param aCategory category type
         * @param aCategoryId old category item ID
         * @param aDrive drive the category item is on
         * @param aMedia media data
         * @param aItemChangedMessages changed mesages array to be updated or NULL
         */
        virtual void UpdateCategoryItemL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
             const CMPXMedia& aMedia, TInt aDrive, CMPXMessageArray* aItemChangedMessages) = 0;

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Get title from the Id
        * @param Id to search for
        * @return name matching the ID
        */
        virtual HBufC* HandleGetAlbumNameFromIdL( TUint32 aId ) = 0;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        
        /**
        * Generate Unique ID for Album
        * @param aMedia media object
        * @param aCaseSensitive indicates whether case sensitivity should be taken
        *        into consideration when generating the unique row id
        * @return the ID of album
        */
        virtual TUint32 GenerateUniqueIdForAlbumL(const CMPXMedia& aMedia) = 0;

        /**
        * Check if the specified album ID is Unknown album
        * @param aId: The ID of the album
        * @return ETrue if it's Unknown album
        */
        virtual TBool IsUnknownAlbumL(const TUint32 aId) = 0;
    };

/**
* Responsible for managing the Music table
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbMusic :
    public CMPXDbTable
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aObserver observer reference
        * @return New CMPXDbMusic instance.
        */
        static CMPXDbMusic* NewL(CMPXDbManager& aDbManager, CMPXResource& aResource,
            MMPXDbMusicObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aObserver observer reference
        * @return New CMPXDbMusic instance on the cleanup stack.
        */
        static CMPXDbMusic* NewLC(CMPXDbManager& aDbManager, CMPXResource& aResource,
            MMPXDbMusicObserver& aObserver);

        /**
        * Destructor
        */
        virtual ~CMPXDbMusic();

    public:

        /**
        * Add a song to the music table
        * @param aMedia contains the song details
        * @param aDrive drive to add the song to
        * @param aMessageArray change event message to notify client of the changes
        * @return The unique ID of the song added.
        */
        TUint32 AddSongL(const CMPXMedia& aMedia, TInt aDrive, CMPXMessageArray* aMessageArray=NULL);

        /**
        * Remove a song from the music table
        * @param aSongId the ID of the song to be removed
        * @param aDrive song drive
        * @param aDeleteRecord indicates whether the client has explictly requested
        *                to delete the records from the database. If ETrue, records
        *                associated with the media will be deleted from the database;
        *                if EFalse, whether to mark the records as deleted or to delete
        *                the records will be based on other deciding factors.
        */
        void DeleteSongL(TUint32 aSongId, TInt aDrive, TBool aDeleteRecord = EFalse);

        /**
        * Remove songs matching a category from the music table
        * @param aCategory the category to remove
        * @param aCategoryId ID of the category to remove
        * @param aUriArray on return will be filled with all the paths that were removed
        * @param aItemChangedMessages a list of change events as a result of the song removal
        * @param aDrive drive the category record is on
        */
        void DeleteCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages, TInt aDrive);

        /**
        * Deletes all songs marked as deleted
        */
        void CleanupL();

        /**
        * Update song info for a song in the music table
        * @param aSongId song ID
        * @param aMedia media object which contains file path,
        *                artist, composer, name(title), track, etc...
        * @param aItemChangedMessages a list of change events as a result of the song removal
        * @return ETrue if the change is visible in the UI
        */
        CMPXDbActiveTask::TChangeVisibility UpdateSongL(TUint32 aSongId, const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Get the name of the song that matches the given song ID
        * @param aSongId ID of the song
        * @return Song name, ownership is transferred to the caller.
        */
        HBufC* GetNameL(TUint32 aSongId);

        /**
        * Get URI of the song
        * @param aSongId ID of the song
        * @return Song URI, ownership is transferred to the caller.
        */
        HBufC* GetUriL(TUint32 aSongId);

        /**
        * Get the drive of the song
        * @param aSongId ID of the song
        * @return Song drive
        */
        TInt GetDriveL(TUint32 aSongId);

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Get the drive and category IDs for a specified song
        * @param aSongId song to be retrieved
        * @param aArtistId returns the artist ID
        * @param aAlbumId returns the artist ID
        * @param aGenreId returns the artist ID
        * @param aComposerId returns the artist ID
        * @param aAbstractAlbumId returns the AbstractAlbum ID
        * @param aDriveId returns the song drive
        * @param aArt returns the albumart uri
        * @return song URI. The ownership is transferred.
        */
        HBufC* GetSongInfoL(TUint32 aSongId, TUint32& aArtistId, TUint32& aAlbumId,
            TUint32& aGenreId, TUint32& aComposerId, TUint32& aAbstractAlbumId, TInt& aDriveId, HBufC*& aArt);
#else 
        /**
        * Get the drive and category IDs for a specified song
        * @param aSongId song to be retrieved
        * @param aArtistId returns the artist ID
        * @param aAlbumId returns the artist ID
        * @param aGenreId returns the artist ID
        * @param aComposerId returns the artist ID
        * @param aDriveId returns the song drive
        * @param aArt returns the albumart uri
        * @return song URI. The ownership is transferred.
        */
        HBufC* GetSongInfoL(TUint32 aSongId, TUint32& aArtistId, TUint32& aAlbumId,
            TUint32& aGenreId, TUint32& aComposerId, TInt& aDriveId, HBufC*& aArt);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Get the song Id, title, URI, and general flags from the given song Id and/or URI
        * @param aCriteria search the media by song Id and/or URI
        * @param aMedia returns the song details
        * @return error code KErrNotFound if unable to find the specified song in the
        *          collection; otherwise KErrNone
        */
        TInt GetSongL(const CMPXMedia& aCriteria, CMPXMedia*& aMedia);

        /**
        * Get the most recently played songs. Returns a maximum of KMPXMaxRecentlyPlayedSongs
        * songs.
        * @param aAttrs attributes to include
        * @param aMediaArray returns the songs' info
        */
        void GetRecentlyPlayedSongsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get the most played songs from the music table. Returns a maximum of
        * KMPXMaxMostPlayedSongs songs.
        * @param aAttrs attributes to include
        * @param aMediaArray returns the songs' info
        */
        void GetMostPlayedSongsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get the recently added songs from the music table (7 days)
        * @param aAttrs attributes to include
        * @param aMediaArray returns the songs' info
        */
        void GetRecentlyAddedSongsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

       /**
        * Get the total number of songs
        * @return total number of songs
        */
        TInt CountL();

        /**
        * Get all the albums for a specified artist
        * @param aArtistId artist ID
        * @param aMediaArray returns the album(s) matching the artist
        */
        void GetAlbumsForArtistL(TUint32 aArtistId, CMPXMediaArray& aMediaArray);

        /**
        * Returns details about the specified song.
        * @param aSongId identifies the song
        * @param aAttrs attributes to be retrieved
        * @param aMedia returns the song information
        */
        void GetSongL(TInt aSongId, const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

        /**
        * Returns all songs in the playlist.
        * @param aDrive identifies in db
        * @param aPlaylistId identifies the playlist
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetAllSongsL(TInt aDrive,TInt aPlaylistId,const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Returns all songs in the database.
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetAllSongsL(const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get from all songs with a limit (first N items)
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        * @param aLimit maximum number of songs to retrieve
        */
        void GetAllSongsLimitedL(const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray, TInt aLimit);

        /**
        * Returns a block of songs as specified
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        * @param aTitle title of the song that marks the beginning of the block
        * @param aNumOfSongs the number of songs in the block
        * @param aAsc
        */
        void GetSongsInBlockL( const TArray<TMPXAttribute>& aAttrs,
                               CMPXMediaArray& aMediaArray,
                               TPtrC aTitle,
                               TUint aNumOfSongs,
                               TBool aAsc);


        /**
        * Get songs from the music collection starting at a particular offset
        * ONLY use this as the first query to locate particular item. Should use
        * GetSongsInBlockL() to fetch in blocks with a particular index
        * @param aMediaArray Array to store the results
        * @param aAttrs attributes to fetch
        * @param aOffset offset to start reading
        * @param aCount number of items to get
        */
        void GetSongsAtOffsetL( CMPXMediaArray& aMediaArray,
                                const TArray<TMPXAttribute>& aAttrs,
                                TInt aOffset,
                                TInt aCount );

        /**
        * Returns all songs for a given artist.
        * @param aArtistId artist to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetSongsForArtistL(TUint aArtistId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Returns all songs for a given album.
        * @param aAlbumId album to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetSongsForAlbumL(TUint aAlbumId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Returns all songs for a given artist and album.
        * @param aArtistId artist to get the songs for
        * @param aAlbumId album to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetSongsForArtistAndAlbumL(TUint aArtistId, TUint aAlbumId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Returns all songs for a given genre.
        * @param aGenreId genre to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetSongsForGenreL(TUint aGenreId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Returns all songs for a given composer.
        * @param aComposerId composer to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetSongsForComposerL(TUint aComposerId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);


#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Returns all songs for a given abstractalbum.
        * @param aAbstractAlbumId abstractalbum to get the songs for
        * @param aAttrs attributes to be retrieved
        * @param aMediaArray returns the song attributes.
        */
        void GetAllSongsForAbstractAlbumL(TUint aAbstractAlbumId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Returns the duration of all songs.
        * @return the duration value
        */
        TInt AllSongsDurationL();

        /**
        * Returns the duration for a specified artist.
        * @param aArtistId artist to get the duration for
        * @return the duration value
        */
        TInt ArtistDurationL(TUint aArtistId);

        /**
        * Returns the duration for a specified album.
        * @param aAlbumId album to get the duration for
        * @return the duration value
        */
        TInt AlbumDurationL(TUint aAlbumId);

        /**
        * Returns the duration for a specified artist and album.
        * @param aArtistId artist to get the duration for
        * @param aAlbumId album to get the duration for
        * @return the duration value
        */
        TInt ArtistAlbumDurationL(TUint aArtistId, TUint aAlbumId);

        /**
        * Returns the duration for a specified genre.
        * @param aGenreId genre to get the duration for
        * @return the duration value
        */
        TInt GenreDurationL(TUint aGenreId);

        /**
        * Returns the duration for a specified composer.
        * @param aComposerId composer to get the duration for
        * @return the duration value
        */
        TInt ComposerDurationL(TUint aComposerId);

        /**
        * Returns the duration for the recently played songs.
        * @return the duration value
        */
        TInt RecentlyPlayedDurationL();

        /**
        * Returns the duration for the most played songs.
        * @return the duration value
        */
        TInt MostPlayedDurationL();

        /**
        * Returns the duration for the recently added songs.
        * @return the duration value
        */
        TInt RecentlyAddedDurationL();

        /**
        * Find the songs for the selection criteria
        * @param aGeneralId selection criteria (could be song/album/artist/genre/composer ID)
        * @param aContainerId selection criteria (could be artist/album id)
        * @param aType criteria type (EMPXItem or EMPXGroup)
        * @param aCriteria selection criteria
        * @param aAttrs the attributes to include for the artist(s) matching the
        *        selection criteria
        * @param aMediaArray returns the songs
        * @param aSortByTrackOrder if TRUE sorts tracks according to their track numbers, 
		*        if FALSE using track titles or filename
        */       
        void FindSongsL(TUint32 aGeneralId, TUint32 aContainerId, TMPXGeneralType aType,
            const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray, TBool aSortByTrackOrder = EFalse);
        
        /**
        * Returns the track count for a given drive
        * @param aDrive drive
        * @return the count
        */
        TUint GetDriveTrackCountL(TInt aDrive);

        /**
        * Get URIs for a given number of tracks
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies unique ID of last record retrieved
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain unique ID of a last record in the array
        */
        void GetMusicUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                               CDesCArray& aUriArr, TInt& aLastID);
            
        /**
        * Get the Albumart of song which belongs to the specified Album
        * @param aId, the ID of Album
        * @param aArt, AlbumArt uri
		* @returns alternative albumart retrieved in the specified Album.
        */
        HBufC* AlbumartForAlbumL(const TUint32 aAlbumId, TPtrC aArt);
        

        /*
        * Check the integrity of the music basic table
        */
        void CheckMusicBasicTableL();

    private:

        /**
        * Checks if the specified song exists.
        * @param aId identifies the song
        * @return ETrue if it exists, EFalse otherwise
        */
        TBool SongExistsL(TUint32 aSongId);

        /**
        * Update the media with information found in the Music table
        * @param aMusicTable music table record
        * @param aAttrs the attributes to include
        * @param aMedia returns the song details
        */
        void UpdateMediaL(RSqlStatement& aMusicTable, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Update the media general attributes from the Music table
        * @param aMusicTable music table view
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaGeneralL(RSqlStatement& aMusicTable, const TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Update the media music attributes from the Music table
        * @param aMusicTable music table view
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaMusicL(RSqlStatement& aMusicTable, const TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Update the media audio attributes from the Music table
        * @param aMusicTable music table view
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaAudioL(RSqlStatement& aMusicTable, const TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Update the media MTP attributes from the Music table
        * @param aMusicTable music table view
        * @param aAttrId the required attributes
        * @param aMedia the media object to set
        */
        void UpdateMediaMTPL(RSqlStatement& aMusicTable, const TUint aAttrId,
            CMPXMedia& aMedia);

        /**
        * Adds a song to the Music table
        * @param aSongId identifies the song
        * @param aMedia contains the song attributes
        * @param aDrive drive to add the song to
        * @param aItemChangedMessages item changed messages to update
        */
        TBool DoAddSongL(TUint32 aSongId, const CMPXMedia& aMedia, TInt aDrive,
            CMPXMessageArray* aItemChangedMessages);

        /**
        * Updates a song in the Music table
        * @param aSongId identifies the song
        * @param aMedia contains the song attributes
        * @param aItemChangedMessages item changed messages to update or NULL
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateSongL(TUint32 aSongId, const CMPXMedia& aMedia,
            CMPXMessageArray* aItemChangedMessages);

        /**
        * Updates a song in the Music table
        * @param aSongId identifies the song
        * @param aMedia contains the song attributes
        * @param aDrive drive to add the song to
        * @param aItemChangedMessages item changed messages to update
        * @param aRecordset current song record in the Music table.
        *       Required to detect any field changes.
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateSongL(TUint32 aSongId, const CMPXMedia& aMedia, TInt aDrive,
            CMPXMessageArray* aItemChangedMessages, RSqlStatement& aRecordset);

        /**
        * Get the DRM type of the given file
        * @param aFile file
        * @return DRM type.
        */
        TMCDrmType DRMTypeL(const TDesC& aFile);

        /**
        * Generate searching criteria for music table from the given criteria
        * @param aGeneralId selection criteria (could be song/album/artist/genre/composer Id)
        * @param aContainerId selection criteria (could be artist/album id)
        * @param aType criteria type (EMPXItem or EMPXGroup)
        * @param aCriteria input criteria
        * @return a string containing the selection criteria. The ownership is passed to the caller.
        */
        HBufC* GenerateMusicMatchingCriteriaLC(TUint32 aGeneralId, TUint32 aContainerId,
            TMPXGeneralType aType, const CMPXMedia& aCriteria);

        /**
        * Constructs the fields and corresponding values to be written into the music
        * table for update and insert statements.
        * @param aSongId identifies the song
        * @param aMedia contains the new song attributes
        * @param aItemChangedMessages item changed mesages to add to
        * @param aMusicTable points to the existing record in the Music table for updates or NULL
        *       for inserts
        * @param aFields returns the field names to be updated
        * @param aValues returns the field values to be updated
        * @param aDrive drive the song is on
        * @return ETrue if the change is visible in the UI
        */
        CMPXDbActiveTask::TChangeVisibility GenerateMusicFieldsValuesL(TUint32 aSongId, const CMPXMedia& aMedia,
            CMPXMessageArray* aItemChangedMessages, RSqlStatement* aMusicTable,
            CDesCArray& aFields, CDesCArray& aValues, TInt aDrive);

        /**
        * Generates the full URI from a music record.
        * @param aMusicTable music table record
        * @return Full URI string. The ownership is passed to the caller.
        */
        HBufC* ConstructUriL(RSqlStatement& aMusicTable, TUint32 aMediaId);

        /**
        * Checks if the specified category field (artist/genre/composer/abstractalbum)
        * will change and notifies the observer if so.
        * @param aCategory identifies the category
        * @param aMedia contains the new record attributes
        * @param aAttribute corresponding attribute in the media parameter
        * @param aOldId the current category item ID
        * @param aDriveId drive ID the update is for
        * @param aItemChangedMessages item changed messages to append to
        * @param aItemId returns the category item ID
        * @return ETrue if the field was modified
        */
        TBool UpdateCategoryFieldL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia,
            const TMPXAttribute& aAttribute, TUint32 aOldId, TInt aDriveId,
            CMPXMessageArray* aItemChangedMessages, TUint32& aItemId, const TDesC& aArt = KNullDesC);

        /**
        * Checks if album will change and notifies the observer if so.
        * @param aMedia contains the new record attributes
        * @param aAttribute corresponding attribute in the media parameter
        * @param aOldId the current category item ID
        * @param aDriveId drive ID the update is for
        * @param aItemChangedMessages item changed messages to append to
        * @param aItemId returns the album item ID
		* @param aArt old art of the music record
		* @param aCaseSensitive indicates whether case sensitivity should be taken
        * @return ETrue if the field was modified
        */
        TBool UpdateCategoryFieldForAlbumL(const CMPXMedia& aMedia, 
		    const TMPXAttribute& aAttribute, TUint32 aOldId, TInt aDriveId, 
			CMPXMessageArray* aItemChangedMessages, TUint32& aItemId, const TDesC& aArt = KNullDesC);       
        
        /**
        * Checks if extra attributes are required. The "standard attribute set includes:
        * EMPXMediaGeneralId, EMPXMediaGeneralType, EMPXMediaGeneralCategory,
        * EMPXMediaGeneralTitle, EMPXMediaGeneralUri, and EMPXMediaGeneralFlags
        * @param aAttrs attributes to be checked
        * @return ETrue if extra attributes are required
        */
        TBool ExtraFieldsRequired(const TArray<TMPXAttribute>& aAttrs);

        /**
        * Test whether the given media contains any supported attribute
        * @param aMedia media to be tested
        * @return ETrue if the media contains one or more supported attribute;
        *  otherwise EFalse.
        */
        TBool IsSupported(const CMPXMedia& aMedia);
        
        /**
         * Query all songs from the database and add results to the 
         * iAllSongsQueryResult cache array.
         */
        void ExecuteQueryAllSongsL(const TArray<TMPXAttribute>& aAttrs);

    private:    // from MMPXTable

        /**
        * @see MMPXTable
        */
        virtual void CreateTableL(RSqlDatabase& aDatabase, TBool aCorruptTable);

        /**
        * @see MMPXTable
        */
        virtual void DropTableL(RSqlDatabase& aDatabase);

        /**
        * @see MMPXTable
        */
        virtual TBool CheckTableL(RSqlDatabase& aDatabase);

    private:

        /**
        * C++ constructor
        * @param aDbManager database manager to use for database interactions
        */
        CMPXDbMusic(CMPXDbManager& aDbManager, MMPXDbMusicObserver& aObserver);

        /**
        * Second phase constructor.
        */
        void ConstructL(CMPXResource& aResource);

    private:

        /**
        * Column indexes in the music table
        */
        enum TMusicColumns
            {
            EMusicUniqueId = KMPXTableDefaultIndex,
            EMusicDbFlag,
            EMusicVolumeId,
            EMusicTitle,
            EMusicArtist,
            EMusicArt,
            EMusicDeleted,
            EMusicLocation,
            EMusicAlbumTrack,
            EMusicPlayCount,
            EMusicTimeAdded,
            EMusicTimePlayed,
            EMusicDuration,
            EMusicSync,
            EMusicModified,
            EMusicAlbum,
            EMusicGenre,
            EMusicComposer,
            EMusicReleaseDate,
            EMusicRating,
            EMusicComment,
            EMusicCopyright,
            EMusicUrl,
            EMusicDRM,
            EMusicLastPlayPosition,
            EMusicSampleRate,
            EMusicBitRate,
            EMusicNumChannels,
            EMusicCodec,
            EMusicMimeType,
            EMusicMTPDrmStatus,
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
            EMusicAlbumArtist,
            EMusicContainEmbeddedArt,
            EMusicAbstractAlbum,
#endif // ABSTRACTAUDIOALBUM_INCLUDED
            EMusicArtistName,
            EMusicAlbumName,
            EMusicGenreName,
            EMusicComposerName,
            EMusicFieldCount
            };

         /*
         * Unique ID for Queries with a lifetime
         */
         enum TMusicStatements
             {
             EOffsetQueryBasic = 0xabcd0001,
             EAscQueryBasic = 0xabcd0002,
             EDscQueryBasic = 0xabcd0003,
             EOffSetQuery = 0xabcd0004,
             EAscQuery = 0xabcd0005,
             EDscQuery = 0xabcd0006
             };

    private:    // Data

        // file extensions to check DRM type, owned
        CDesCArrayFlat* iExtensionsDrm;
        MMPXDbMusicObserver& iObserver;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED 
        TBool iArtNeedUpdated;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        RPointerArray<CMPXMedia> iAllSongsQueryResult;
    };

#endif // MPXDBMUSIC_H

// End of File
