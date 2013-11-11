/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides the functions to manipulate the music collection
*                database.
*
*/


#ifndef MPXDBHANDLER_H
#define MPXDBHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

#include <mpxattribute.h>
#include <mpxmessage2.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>

#include "mpxcollectiondbstd.h"
#include "mpxcollectiondb.hrh"
#include "mpxdbactivetask.h"
#include "mpxdbmusic.h"     // for MMPXDbMusicObserver
#include "mpxdbartist.h"    // for MMPXDbArtistObserver
#include "mpxdbalbum.h"     // for MMPXDbAlbumObserver
#include "mpxdbplaylist.h"	// for MMPXDbPlaylistObserver
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
#include "mpxdbabstractalbum.h"
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;
class CMPXCollectionDbManager;
class CMPXDbPlaylist;
class CMPXDbCategory;
class CMPXDbAuxiliary;
class CMPXDbAutoPlaylist;
class CMPXDbArtist;
class CMPXDbAlbum;
class CMPXDbGenre;
class CMPXDbComposer;

// CLASS DECLARATION

/**
*  Music collection database handler
*
*  @lib MPXDbPlugin.lib
*/
class CMPXDbHandler :
    public CBase,
    public MMPXDbMusicObserver,
    public MMPXDbAlbumObserver,
	public MMPXDbPlaylistObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return a new CMPXDbHandler instance
        */
        static CMPXDbHandler* NewL(RFs& aFs, CMPXResource& aResource);

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return a new CMPXDbHandler instance
        */
        static CMPXDbHandler* NewLC(RFs& aFs, CMPXResource& aResource);

        /**
        * Destructor.
        */
        virtual ~CMPXDbHandler();

    public:

        /**
        * Add a song to the music collection database
        * @param aMedia media object which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aMessageArray change event message to notify client of the changes
        * @return TUint32 id of the item added
        */
        TUint32 AddSongL(const CMPXMedia& aMedia, CMPXMessageArray* aMessageArray=NULL);

        /**
        * Add a song to the music collection without a database transaction; it
        * is assumed the transaction is created at a higher leve
        * @param aMedia media object which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aMessageArray change event message to notify client of the changes
        * @return TUint32 id of the item added
        */
        TUint32 AddSongWithNoTransactionL(const CMPXMedia& aMedia, CMPXMessageArray* aMessageArray=NULL);

        /**
        * Add a playlist to the collection
        * @param aMedia playlist media object with title and URI of the playlist.
        * @return playlist Id of the playlist created
        */
        TUint32 AddPlaylistL(const CMPXMedia& aMedia);

        /**
        * Add songs to the specified playlist
        * @param aMedia playlist media object with playlist Id.
        * @return playlist Id of the playlist
        */
        TUint32 AddSongToPlaylistL(const CMPXMedia& aMedia);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Add an abstractalbum to the collection
        * @param aMedia abstractalbum media object with URI of the abstractalbum.
        * @param aMessageArray change event message to notify client of the changes
        * @return abstractalbum Id of the abstractalbum created
        */
        TUint32 AddAbstractAlbumL(const CMPXMedia& aMedia, CMPXMessageArray* aMessageArray=NULL);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Update song info for a song in the music collection database
        * @param aMedia songDetails object  which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aItemChangedMessages a list of change events as a result of the
        *           song update
        * @return ETrue if UI visible change else EFalse
        */
        CMPXDbActiveTask::TChangeVisibility UpdateSongL(const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Update abstractalbum info to AbstractAlbum table and Songs associated with abstractalbum
        *           in the music collection database
        * @param aMedia media object         
        * @param aItemChangedMessages a list of change events as a result of the
        *           song update
        * @return ETrue if UI visible change else EFalse
        */
        CMPXDbActiveTask::TChangeVisibility UpdateAbstractAlbumL(const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Update all songs which associate with AbstractAlbum to new AbstractAlbum info 
        * in the collection
        * @param aMedia media object  which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aItemChangedMessages a list of change events as a result of the
        *           song update
        * @return ETrue if UI visible change else EFalse
        */
        CMPXDbActiveTask::TChangeVisibility UpdateSongsAbstractAlbumInfoL(const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Update playlist info in the music collection database
        * @param aMedia playlist details object which contains file path,
        *           timestamps etc.
        * @param aMessageArray change event messages for the playlist update
        */
        void UpdatePlaylistL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray);

        /**
        * Update the songs for a playlist in the music collection database
        * @param aMedia playlist details object which contains the playlist songs.
        * @param aMessage change event message for the playlist update
        */
        void UpdatePlaylistSongsL(const CMPXMedia& aMedia, CMPXMessage& aMessage);

        /**
        * Reorder the specified song in the specified playlist
        * @param aPlaylistId id of the playlist which contains the song to be reordered
        * @param aSongId id of the song to be reordered
        * @param aOriginalOrdinal the original ordinal of the song within the playlist.
        *        Ordinal starts from 0.
        * @param aNewOrdinal the new ordinal of the song within the playlist. Ordinal
        *        starts from 0.
        * @param aMessage change event message for the playlist update
        */
        void ReorderPlaylistL(const TMPXItemId& aPlaylistId, const TMPXItemId& aSongId,
            TUint aOriginalOrdinal, TUint aNewOrdinal, CMPXMessage& aMessage);

        /**
        * Remove all songs within music collection database
        */
        void RemoveEntireCollectionL();

        /**
        * Remove a song from the music collection database
        * @param aSongId ID of the song to be removed
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aItemChangedMessages a list of change events as a result of the song removal
        * @param aDeleteRecord indicates whether the client has explictly requested
        *                to delete the records from the database. If ETrue, records
        *                associated with the media will be deleted from the database;
        *                if EFalse, whether to mark the records as deleted or to delete
        *                the records will be based on other deciding factors.
        */
        void RemoveSongL(TUint32 aSongId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages, TBool aDeleteRecord = EFalse);

        /**
        * Removes a category of songs from the music collection, and its
        * corresponding category in the lookup table
        * @param aCategory the category to remove
        * @param aCategoryId ID of the category to remove
        * @param aUriArray a descriptor array to contain the URI of the deleted songs
        * @param aItemChangedMessages a list of change events as a result of the song removal
        */
        void RemoveSongsMatchingCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages);

        /**
        * Removes songs from the music collection belonging to the
        * specified artist and album as well as the actual category
        * in the lookup table
        * @param aArtistId id of the artist
        * @param aAlbumId id of the album
        * @param aUriArray a descriptor array to contain the URI of the deleted file(s)
        * @param aItemChangedMessages a list of change events as a result of the song removal
        */
        void RemoveSongsMatchingArtistAndAlbumL(TUint32 aArtistId, TUint32 aAlbumId,
            CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages);

        /**
        * Remove all playlists from the collection
        */
        void RemoveAllPlaylistsL();

        /**
        * Remove a playlist This will remove all the songs from the playlist
        * @param aPlaylistId ID of the playlist to remove
        * @param aUriArray a descriptor array to contain the URI of the deleted file
        * @param aItemChangedMessages a list of change events as a result of the
        *        playlist removal
        */
        void RemovePlaylistL(TUint32 aPlaylistId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Remove a abstractalbum, remove entry from AbstractAlbum table, 
        * Thumbnail table, corresponding alb from file system
        * @param aAbstractAlbumId ID of the abstractalbum to remove
        * @param aItemChangedMessages a list of change events as a result of the
        *        abstractalbum removal
        * @param aFileDeleted indicate if alb file already deleted from file system
        */
        void RemoveAbstractAlbumL(TUint32 aAbstractAlbumId,
            CMPXMessageArray& aItemChangedMessages, TBool aFileDeleted); 
#endif // ABSTRACTAUDIOALBUM_INCLUDED

        /**
        * Remove a song from the specified playlist.
        * @param aPlaylistId ID of the playlist to remove the song from
        * @param aSongId Id of the song in the playlist to be removed
        * @param aOrdinal the ordinal of the song in the playlist. This uniquely
        *        identifies which song in the playlist to be removed; whereas song
        *        Id doesn't because duplicate tracks in the playlist are allowed.
        *        The position is relative to zero, i.e. zero implies that the song
        *        at the beginning of the playlist is to be removed.
        * @param aItemChangedMessages a list of change events as a result of the
        *        song removal
        */
        void RemoveSongFromPlaylistL(TUint32 aPlaylist, const TMPXItemId& aSongId,
            TInt aOrdinal, CMPXMessageArray& aItemChangedMessages);

        /**
        * Delete records in all databases which have been marked as deleted
        */
        void CleanupDeletedRecordsL();

        /**
        * Get all songs from the music collection databasee
        * @param aMediaArray Array to place all the songs' required info
        * @param aAttrs required attributes
        */
        void GetAllSongsL(CMPXMediaArray* aMediaArray,
            const TArray<TMPXAttribute>& aAttrs);

        /**
        * Get a limited number of songs from the music collection databasee
        * @param aMediaArray Array to place all the songs' required info
        * @param aAttrs required attributes
        * @param aLimit maximum number of songs to retrieve
        */
        void GetAllSongsLimitedL(const TArray<TMPXAttribute>& aAttrs,
                                 CMPXMediaArray& aMediaArray, TInt aLimit);

        /**
        * Get songs from the music collection database incrementally
        * @param aMediaArray Array to place all the songs' required info
        * @param aAttrs required attributes
        * @param aTitle title to use as a key
        * @param aNumOfSongs number of songs to read
        * @param aAsc ascending or descending data read
        */
        void GetSongsInBlockL(CMPXMediaArray* aMediaArray,
            const TArray<TMPXAttribute>& aAttrs,
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
        void GetSongsAtOffsetL( CMPXMediaArray* aMediaArray,
                                const TArray<TMPXAttribute>& aAttrs,
                                TInt aOffset,
                                TInt aCount );

        /**
        * Get all the songs that match the given artist ID
        * @param aArtistId ID of the artist to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching songs' required
        *        attributes
        */
        void GetSongsMatchingArtistL(TUint aArtistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get all the songs that match the given album ID
        * @param aAlbumId ID of the album to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching songs' required info
        */
        void GetSongsMatchingAlbumL(TUint aAlbumId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get all the songs that match the given artist and album ID
        * @param aArtistId ID of the artist to match
        * @param aAlbumId ID of the album to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching songs' required info
        */
        void GetSongsMatchingArtistAndAlbumL(TUint aArtistId, TUint aAlbumId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get all the songs that match the given genre ID
        * @param aGenreId ID of the genre to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching songs' required info
        */
        void GetSongsMatchingGenreL(TUint aGenreId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get all the songs that match the given composer ID
        * @param aComposerId ID of the composer to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching songs' required info
        */
        void GetSongsMatchingComposerL(TUint aComposerId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get all the songs that match the given playlist ID
        * @param aPlaylistId ID of the playlist to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the required song info
        */
        void GetSongsMatchingPlaylistL(TUint aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray* aMediaArray);

        /**
        * Get the song info that matches the given song ID from Songs table.
        * @param aSongId ID of the song
        * @param aAttrs required attributes
        * @param aMedia returns the song info
        */
        void GetSongL(TUint32 aSongId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Appends the media object with the song information to the specified array.
        * @param aSongId ID of the song
        * @param aAttrs required attributes
        * @param aMediaArray on return the song info is appended to this array
        */
        void GetSongL(TUint32 aSongId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get the song info that matches the given song ID from Songs table.
        * The song info will first be populated from Playlist table and
        * if the Songs table exists in the drive where the song is located,
        * song info will be overwritten from Songs table. This enables the
        * support for displaying song title in a playlist when the drive
        * where the song is located is not present.
        * @aSongId ID of the song
        * @aPlaylistId ID of the playlist the song belongs to.
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the required song info
        * @leave KErrNotFound if the song could not be found in the
        *       PlaylistSongs table
        */
        void GetPlaylistSongL(TUint32 aSongId, TUint32 aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

        /**
        * Appends the a media object with the song information to the specified array.
        * @param aSongId ID of the song
        * @param aPlaylistId ID of the playlist
        * @param aAttrs required attributes
        * @param aMediaArray on return the song info is appended to this array
        */
        void GetPlaylistSongL(TUint32 aSongId, TUint32 aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get the song ID of the song that matches the given URI
        * @param aUri URI to match
        * @return song ID
        */
        TUint32 GetSongIdMatchingUriL(const TDesC& aUri);

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Get the abstractalbum ID of the abstractalbum that matches the given URI
        * @param aUri URI to match
        * @return abstractalbum ID
        */
        TUint32 GetAbstractAlbumIdMatchingUriL(const TDesC& aUri);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * Get all the artist names from the music collection database sorted by name.
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the artists' required info
        */
        void GetAllArtistsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);

        /**
        * Get all the album names from the music collection database sorted by name.
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the albums' required info
        */
        void GetAllAlbumsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);

        /**
        * Get all the album names from the music collection database sorted by artist.
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the albums' required info
        */
        void GetAllAlbumsMediaWallL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);
        
        /**
        * Get all the albums that match the given artist ID sorted by name.
        * @param aArtistId ID of the artist to match
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the matching albums' required info
        */
        void GetAlbumsMatchingArtistL(TUint aArtistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get all the genre names from the collection
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the genres' required info
        */
        void GetAllGenresL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);

        /**
        * Get all the composer names from the collection
        * @param aAttrs required attributes
        * @param aMediaArray Array to place all the composers' required info
        */
        void GetAllComposersL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);

        /**
        * Get the playlist ID of the playlist that matches the given URI
        * @param aUri URI to match
        * @return playlist ID to return
        */
        TUint32 GetPlaylistIdMatchingUriL(const TDesC& aUri);

        /**
        * Checks if the specified ID corresponds to an auto playlist.
        * @param aPlaylistId ID to be checked
        * @return ETrue if it corresponds to an auto playlist, EFalse otherwise
        */
        TBool IsAutoPlaylistL(TUint32 aPlaylistId);

        /**
        * Get all the playlist names from the collection
        * @param aMediaArray Array to place all the playlists' required info
        * @param aAttrs required attributes
        */
        void GetAllPlaylistsL(CMPXMediaArray* aMediaArray,
            const TArray<TMPXAttribute>& aAttrs);

        /**
        * Get all the system playlist names from the collection
        * @param aMediaArray Array to place all the system playlists' required info
        */
        void GetAllSystemPlaylistNamesL(CMPXMediaArray* aMediaArray);

        /**
        * Get title from the Id
        * @param Id to search for
        * @return name matching the ID
        */
        HBufC* GetNameMatchingIdL(const TUint32 aId) const;

        /**
        * Get URI from the Id
        * @param Id to search for
        * @return URI matching the ID
        * @leave KErrNotSupported if the ID refers to an invalid category
        */
        HBufC* GetUriMatchingIdL(const TUint32 aId) const;

        /**
        * Retrieve category record with information as specified in the given
        * attributes
        * @param aCategoryId Id of the record to be retrieved
        * @param aCategory category type
        * @param aAttrs information to return about the specified category
        * @param aMedia returns the category information
        */
        void GetCategoryL(const TUint32 aCategoryId, TMPXGeneralCategory aCategory,
                const TArray<TMPXAttribute>& aAttrs, CMPXMedia* aMedia);

        /**
        * Get the duration for all songs
        * @return duration
        */
        TInt GetAllSongsDurationL();

        /**
        * Get the duration for all songs for a specified artist
        * @param aArtistId identifies the artist
        * @return duration
        */
        TInt GetArtistDurationL(TInt aArtistId);

        /**
        * Get the duration for all songs in a specified album
        * @param aAlbumId identifies the album
        * @return duration
        */
        TInt GetAlbumDurationL(TInt aAlbumId);

        /**
        * Get the duration for all songs for a specified artist and album
        * @param aArtistId identifies the artist
        * @param aAlbumId identifies the album
        * @return duration
        */
        TInt GetArtistAlbumDurationL(TInt aArtistId, TInt aAlbumId);

        /**
        * Get the duration for all songs for a specified composer
        * @param aComposerId identifies the composer
        * @return duration
        */
        TInt GetComposerDurationL(TInt aComposerId);

        /**
        * Get the duration for all songs for a specified genre
        * @param aGenreId identifies the genre
        * @return duration
        */
        TInt GetGenreDurationL(TInt aGenreId);

        /**
        * Get the duration for all songs in a user playlist
        * @param aPlaylistId identifies the playlist
        * @return duration
        */
        TInt GetUserPlaylistDurationL(TInt aPlaylistId);

        /**
        * Get the duration for all songs for a specified playlist
        * @param aPlaylistId identifies the playlist
        * @return duration
        */
        TInt GetPlaylistDurationL(TInt aPlaylistId);

        /**
        * Fetch the total number of items in the specified category
        * @param aCategory category to be queried
        * @return number of items.
        */
        TInt NumberOfItemsL(TMPXGeneralCategory aCategory);

        /**
        * Find media(s) from the collection
        * @param aCriteria selection criteria
        * @param aAttrs attributes for the matching media
        * @return matching media. Ownership is transferred.
        * @leave KErrArgument if the criteria do not include a type
        *           or category
        */
        CMPXMedia* FindAllLC(const CMPXMedia& aCriteria,
            const TArray<TMPXAttribute>& aAttrs);

        /**
        * Set the last refreshed time
        * @param aTime the updated last refresh time
        */
        void SetLastRefreshedTimeL(TTime aTime);

        /**
        * Get the last refreshed time
        * @return last refreshed time
        */
        TTime GetLastRefreshedTimeL();

        /**
        * Set the db corrupted bit in the aux table for all drives
        * @param aCorrupted flag indicating whether or not the db is corrupted
        */
        void SetDBCorruptedL(TBool aCorrupted);

        /**
        * Get the db corrupted bit in the aux table ( or of all drives )
        * @return ETrue if database is corrupted, else EFalse
        */
        TBool IsDBCorruptedL();

        /**
        * Checks if the database has been created
        * @return ETrue if file has been created
        */
        TBool DatabaseCreated();

        /**
        * Close and re-open the music collection database stored on the
        * specified drive
        * @param aDrive drive containing the music collection database to re-open
        */
        void OpenDatabaseL(TInt aDrive);

        /**
        * Close a database for a particular drive
        * @param aDrive drive to close
        */
        void CloseDatabaseL(TInt aDrive);

        /**
        * Recreate all databases.
        */
        void ReCreateDatabasesL();

        /**
        * Starts a refresh operation.
        */
        void RefreshStartL();

        /**
        * Ends a refresh operation.
        */
        void RefreshEndL();

        /**
        * Notification of mtp start
        */
        void MtpStartL();

        /**
        * Notification of mtp end
        */
        void MtpEndL();
        /**
        * Get total record count for music in db
        * @param aDrive, specifies drive number for db selection
        * @return count
        */
        TUint GetMusicCountL(TInt aDrive);

        /**
        * Get total record count for playlists in db
        * @param aDrive, specifies drive number for db selection
        * @return count
        */
        TUint GetPlaylistCountL(TInt aDrive);

        /**
        * Get total record count for music and playlists in db
        * @param aDrive, specifies drive number for db selection
        * @return count
        */
        TUint GetTotalCountL(TInt aDrive);

        /**
        * Get URIs for a given amount of songs
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies unique ID of last record retrieved
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain unique ID of a last record in the array
        */
        void GetMusicUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                               CDesCArray& aUriArr, TInt& aLastID);

        /**
        * Get URIs for a given amount of playlists
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies unique ID of last record retrieved
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain unique ID of a last record in the array
        */
        void GetPlaylistUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                  CDesCArray& aUriArr, TInt& aLastID);

        /**
        * Starts a transaction on all open databases.
        */
        void BeginTransactionL();

        /**
        * Commits or rolls back a transaction.
        * @param aError if error then rolls back the transaction and leaves
        *               if KErrNone it commits the transaction
        */
        void EndTransactionL(TInt aError);

        /**
        * Checks if the database is currently in a transaction.
        *
        * @return ETrue if database is currently in a transaction, EFalse otherwise
        */
        TBool InTransaction();

        /**
         * Notifies the handler that the collection will be closed.
         * It is called before closing the collection.
         */
        void PreCloseCollectionL();

         /**
          * Checks if the spefified drive is a remove drive
          */
         TBool IsRemoteDrive(TDriveNumber aDrive);

         /**
          * Checks if there is a drive that has a low disk space
          * @leave KErrDiskFull if the space on one of the drives is below the
          *         critical level
          */
         void CheckDiskSpaceOnDrivesL();

    private:

        /**
        * Add a song to the music collection database
        * @param aMedia: media object which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aMessage change event message to notify client of the changes
        * @return Id of the song added
        * @leave KErrArgument if the media doe snot include the song URI
        */
        TUint32 DoAddSongL(const CMPXMedia& aMedia, CMPXMessageArray* aMessageArray);

        /**
        * Add a playlist to the collection
        * @param aMedia playlist media object with title and URI of the playlist.
        * @return ID of the playlist created
        */
        TUint32 DoAddPlaylistL(const CMPXMedia& aMedia);

        /**
        * Add songs to the specified playlist
        * @param aMedia playlist media object with playlist Id.
        * @return playlist Id of the playlist
        */
        TUint32 DoAddSongToPlaylistL(const CMPXMedia& aMedia);

        /**
        * Update song info for a song in the music collection database
        * @param aMedia songDetails object  which contains file path,
        *           artist, composer, name(title), track, etc...
        * @param aItemChangedMessages a list of items changed
        * @return ETrue if UI visible change
        * @leave KErrNotSupported if the song ID could not be constructed
        */
        CMPXDbActiveTask::TChangeVisibility DoUpdateSongL(const CMPXMedia& aMedia,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Update playlist info in the music collection database
        * @param aMedia playlist details object which contains file path,
        *           timestamps etc...
        * @param aMessage change event message for the playlist update
        */
        void DoUpdatePlaylistL(const CMPXMedia& aMedia, CMPXMessageArray& aMessageArray);

        /**
        * Replace all songs for a playlist. This method is called when harvester
        * scans an updated playlist. This will also be called if MTP wants to
        * replace the contents of an existing playlist. The implementation deleted the
        * existing songs first and then inserts the new ones.
        * @param aMedia playlist and songs details object which contains file path,
        *           timestamps etc...
        * @param aMessage change event message for the playlist update
        */
        void DoUpdatePlaylistSongsL(const CMPXMedia& aMedia, CMPXMessage& aMessage);

        /**
        * Reorder the specified song in the specified playlist
        * @param aPlaylistId id of the playlist which contains the song to be reordered
        * @param aSongId id of the song to be reordered
        * @param aOriginalOrdinal the original ordinal of the song within the playlist.
        *        Ordinal starts from 0.
        * @param aNewOrdinal the new ordinal of the song within the playlist. Ordinal
        *        starts from 0.
        * @param aMessage change event message for the playlist update
        */
        void DoReorderPlaylistL(const TMPXItemId& aPlaylistId, const TMPXItemId& aSongId,
            TUint aOriginalOrdinal, TUint aNewOrdinal, CMPXMessage& aMessage);

        /**
        * Deletes a song. In addition to deleting the song record from the music
        * table takes care of updating the category and playlist tables.
        * @param aSongId song to be deleted
        * @param aUriArray on return contains the URIs of the song deleted.
        * @param aItemChangedMessages on return contains changed messages for all
        *        affected items
        * @param aDeleteRecord ETrue if the record is to be deleted, EFalse if it is
        *        to be marked as deleted.
        */
        void DoRemoveSongL(TUint32 aSongId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages, TBool aDeleteRecord);

        /**
         * Deletes the song  from the playlist tables
         * @param aSongId song to be deleted
         * @param aItemChangedMessages on return contains changed messages for all
         *        affected items
         */
        void DoRemoveSongFromPlaylistL(TUint32 aSongId,CMPXMessageArray& aItemChangedMessages);

        /**
        * Deletes all the songs for a given category. The implementation uses
        * DoRemoveSongL for the business logic.
        * @param aCategory category type (artist/album/genre/composer)
        * @param aCategoryId identifies the category item
        * @param aUriArray on return contains the URIs of the songs deleted.
        * @param aItemChangedMessages on return contains changed messages for all
        *        affected items
        * @leave KErrNotSupported if invalid category
        */
        void DoRemoveSongsMatchingCategoryL(TMPXGeneralCategory aCategory,
            TUint32 aCategoryId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);

        /**
        * Deletes all the songs for a given artist and album.
        * The implementation uses DoRemoveSongL for the business logic.
        * @param aArtistId identifies the artist
        * @param aAlbumId identifies the album
        * @param aUriArray on return contains the URIs of the songs deleted.
        * @param aItemChangedMessages on return contains changed messages for all affected items
        */
        void DoRemoveSongsMatchingArtistAndAlbumL(TUint32 aArtistId, TUint32 aAlbumId,
            CDesCArray& aUriArray, CMPXMessageArray& aItemChangedMessages);

        /**
        * Deletes all the playlists in all databases. Note that the songs are not affected.
        */
        void DoRemoveAllPlaylistsL();

        /**
        * Deletes a specified playlist.
        * @param aPlaylistId the playlist to be deleted.
        * @param aUriArray on return contains the URIs of the items deleted.
        * @param aItemChangedMessages on return contains changed messages for all
        *        affected items
        */
        void DoRemovePlaylistL(TUint32 aPlaylistId, CDesCArray& aUriArray,
            CMPXMessageArray& aItemChangedMessages);


        /**
        * Deletes a song from a playlist.
        * @param aPlaylistId identifies the playlist.
        * @param aSongId song to be deleted.
        * @param aOrdinal song ordinal in case there are multiple instances of the same
        *        song in the playlist.
        * @param aItemChangedMessages on return contains changed messages for all
        *        affected items
        */
        void DoRemoveSongFromPlaylistL(TUint32 aPlaylistId, const TMPXItemId& aSongId,
            TInt aOrdinal, CMPXMessageArray& aItemChangedMessages);



        /**
        * Deletes all song records marked as deleted.
        */
        void DoCleanupDeletedRecordsL();

        /**
        * Find items in the collection
        * @param aCriteria selection criteria
        * @param aAttrs attributes for the matching media
        * @param aMediaArray returns the matching entries
        */
        void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray* aMediaArray);

        /**
        * Find a song.
        * @param aCriteria selection criteria
        * @param aAttrs attributes for the matching media
        * @param aMediaArray returns the matching entries
        */
        void FindSongL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Gets the songs for a specified playlist
        * @param aPlaylistId identifies the playlist
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the matching entries
        */
        void GetPlaylistSongsL(TUint32 aPlaylistId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Find an album
        * @param aCriteria selection criteria
        * @param aAttrs attributes to be returned
        * @param aMediaArray returns the matching entries
        */
        void FindAlbumL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Extracts the playlist ID and drive ID from the media object.
        * @param aMedia playlist information
        * @param aPlaylistId returns the playlist ID
        * @param aPlaylistDriveId returns the drive ID
        * @leave KErrArgument if the media does not include the ID and URI
        */
        void ProcessPlaylistMediaL(CMPXMedia& aMedia, TUint32& aPlaylistId,
            TInt& aPlaylistDriveId);

        /**
        * Updates all the song entries in the playlist media with extra
        * information from the music table.
        * @param aMedia inout, contains the playlist/songs information
        * @leave KErrArgument if the song does not exist in the music table and
        *        the media does not include a URI
        */
        void UpdatePlaylistSongInfoL(CMPXMedia& aMedia);

        /**
        * Makes sure all the folders specified in the array parameter are created,
        * otherwise it tries to create them. Updates the iDbDrives member variable
        * with the drives corresponding to the folders.
        * @param aFolders contains all the folders to be processed
        */
        void ProcessMusicFoldersL(const CDesCArray& aFolders);

        /**
        * Returns the CMPXDbCategory instance associated with the specified
        * category type
        * @param aCategory category type
        * @return corresponding CMPXDbCategory instance
        * @leave KErrNotSupported if invalid category
        */
        CMPXDbCategory* DbCategoryL(TMPXGeneralCategory aCategory) const;

        /**
        * Verify the volume id for the databases
        */
        void VerifyVolumeIdL( TInt aDrive );
        void VerifyVolumeIdL();


#if defined (__MTP_PROTOCOL_SUPPORT)
        /**
        * Reads the "save deleted records" MTP flag from CenRep and returns
        * the value.
        * @return value of the "save deleted records" MTP flag
        */
        TBool SaveDeletedSongs();
#endif

#ifdef RD_MULTIPLE_DRIVE
        /**
        * Retrieve all visible music folder locations.
        */
        CDesCArrayFlat* GetMusicFoldersL();
#endif // RD_MULTIPLE_DRIVE

    private:    // From MMPXDbMusicObserver

        /**
        * @see MMPXDbMusicObserver
        */
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
       virtual TUint32 AddAbstractAlbumItemL(const TDesC& aName, 
            TInt aDriveId, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist,
            const TDesC& aUri, const TDesC& aAlbumArtist=KNullDesC);
#endif // ABSTRACTAUDIOALBUM_INCLUDED

       virtual TUint32 AddCategoryItemL(TMPXGeneralCategory aCategory, const CMPXMedia& aMedia,       
            TInt aDrive, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist);
       
       virtual void UpdateCategoryItemL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            const CMPXMedia& aMedia, TInt aDrive, CMPXMessageArray* aItemChangedMessages);

       virtual TUint32 GenerateUniqueIdForAlbumL(const CMPXMedia& aMedia);
       
       virtual TBool IsUnknownAlbumL(const TUint32 aId);

        /**
        * @see MMPXDbMusicObserver
        */
        virtual void DeleteSongForCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            TInt aDrive, CMPXMessageArray* aItemChangedMessages, TBool& aItemExist, const TDesC& aArt = KNullDesC);

        /**
        * @see MMPXDbMusicObserver
        */
        virtual void HandlePlayCountModifiedL(CMPXMessageArray& aItemChangedMessages);

        /**
        * @see MMPXDbMusicObserver
        */
        virtual void HandlePlaybackTimeModifiedL(CMPXMessageArray& aItemChangedMessages);
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        /**
        * @see MMPXDbMusicObserver
        */
        virtual HBufC* HandleGetAlbumNameFromIdL(TUint32 aId);
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        
    private:    // From MMPXDbAlbumObserver   
        /**
        * Get the Albumart of song which belongs to the specified Album.     
        * @param aId, The Album ID.
        * @param aArt, the AlbumArt uri.
	    * @returns alternative albumart retrieved in the specified Album.
        */
        virtual HBufC* HandleAlbumartForAlbumL(const TUint32 aAlbumId, TPtrC aArt);
       
        /**
        * @see MMPXDbAlbumObserver
        */
        virtual void DeleteAlbumForArtistL(TUint32 aCategoryId,
            TInt aDrive, CMPXMessageArray* aItemChangedMessages);

        /**
        * @see MMPXDbAlbumObserver
        */
        virtual TUint32 AddAlbumArtistL(const TDesC& aName, const TDesC& aArt, TInt aDriveId);       

    private:	// From MMPXDbPlaylistObserver
        /**
        * @see MMPXDbPlaylistObserver
        */
        virtual TInt HandlePlaylistDurationL(TUint32 aPlaylistId);
        virtual void HandlePlaylistInfoL(TUint32 aPlaylistId, TInt& aCount, TInt& aDuration);

    private:

        /**
        * C++ constructor.
        * @param file session
        */
        CMPXDbHandler(RFs& aFs, CMPXResource& aResource);

        /**
        * Symbian 2nd phase constructor..
        */
        void ConstructL();

    private:    // Data

        // owned member variables
        CMPXCollectionDbManager* iDbManager;
        CMPXDbMusic* iDbMusic;
        CMPXDbPlaylist* iDbPlaylist;
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
        CMPXDbAbstractAlbum* iDbAbstractAlbum;
#endif // ABSTRACTAUDIOALBUM_INCLUDED
        CMPXDbArtist* iDbArtist;
        CMPXDbAlbum* iDbAlbum;
        CMPXDbGenre* iDbGenre;
        CMPXDbComposer* iDbComposer;
        CMPXDbAuxiliary* iDbAuxiliary;

        CMPXDbAutoPlaylist* iAutoPlaylist;

        CDesCArrayFlat* iMimeTypes;         // MIME types supported
        CDesCArrayFlat* iExtensions;        // file extensions supported
        CDesCArrayFlat* iExtensionsMime;    // file extensions to check MIME
        CDesCArrayFlat* iExtensionsDrm;     // file extensions to check DRM type
        RArray<TInt> iDbDrives;

        // not owned member variables
        RFs& iFs;
        CMPXResource& iResource;
        TBool iOutOfDisk;                   // Are we in out of disk mode?
        TBool iRefresh;                     // Refresh operation currently performed
        TBool iMtpInUse;                    // Flag for MTP operation
        TInt  iOpOnDbCount;                 // Count to provide a min op amount in one transaction
    };

#endif      // MPXDBHANDLER_H

// End of File
