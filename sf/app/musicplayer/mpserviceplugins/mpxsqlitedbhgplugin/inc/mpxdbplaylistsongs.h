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
* Description:  This class is responsible for the functionality related to
*                playlist songs. The class operates on two tables: PlaylistSongs
*                and PlaylistSongInfo.
*
*                The PlaylistSongs table contains records for all song instances in
*                playlists. For example if a playlist contains 7 songs, there will be 7
*                corresponding records in the PlaylistSongs table, even if one song has
*                multiple instances in the playlist.
*
*                The PlaylistSongInfo table contains one record for each song included
*                in at least one playlist. Redundant song information is stored in this
*                table to help with songs on different drives than the playlist.
*
*
*/


#ifndef MPXDBPLAYLISTSONGS_H
#define MPXDBPLAYLISTSONGS_H

// INCLUDES
#include "mpxdbtable.h"

// CLASS FORWARDS
class CMPXMedia;
class CMPXMediaArray;

// CLASS DECLARATION

/**
* Responsible for managing the music table
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbPlaylistSongs :
    public CMPXDbTable
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbPlaylistSongs instance.
        */
        static CMPXDbPlaylistSongs* NewL(CMPXDbManager& aDbManager);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbPlaylistSongs instance on the cleanup stack.
        */
        static CMPXDbPlaylistSongs* NewLC(CMPXDbManager& aDbManager);

        /**
        * Destructor
        */
        virtual ~CMPXDbPlaylistSongs();

    public:

        /**
        * Add a song/songs to the given playlist
        * @param aPlaylistId playlist id
        * @param aMediaArray media array which contains the tracks to be added to the playlist
        * @param aDriveId Drive Id of corresponding playlist
        */
        void AddSongsL(TUint32 aPlaylistId, const CMPXMediaArray& aMediaArray, TInt aDriveId);

        /**
        * Add a song to playlist songs table for the given playlist
        * @param aPlaylistId id of the playlist to add the song to
        * @param aDriveId drive the song is on
        * @param aMedia song media to be added to the playlist songs table for the playlist
        * @return song ID
        */
        TUint32 AddSongL(TUint32 aPlaylistId, const CMPXMedia& aMedia, TInt aDriveId);

        /**
        * Add a song to playlist songs table for the given playlist
        * @param aPlaylistId id of the playlist to add the song to
        * @param aOrdinal ordinal of the song to be added
        * @param aDriveId drive the song is on
        * @param aMedia song media to be added to the playlist songs table for the playlist
        * @return song ID
        */
        TUint32 AddSongL(TUint32 aPlaylistId, TInt aOrdinal, const CMPXMedia& aMedia,
            TInt aDriveId);

        /**
        * Update song info for a song in the playlist songs table
        * @param aSongId identifies the song
        * @param aMedia song information
        * @param aResetFlags indicates whether the song db flags should be reset or just updated
        * @param aUpdated returns ETrue if the song information has changed
        */
        TBool UpdateSongL(TUint32 aSongId, const CMPXMedia& aMedia, TBool aResetFlags,
            TBool& aUpdated);

        /**
        * Update songs in a playlist to reflect playlist id change
        * @param aPlaylistId identifies the songs in the playlist
        * @param aNewPlaylistId specifies the new playlist id the songs should be associated with
        */
        void UpdateSongsL(TUint32 aPlaylistId, TUint32 aNewPlaylistId);

        /**
        * Reorder the song in the playlist
        * @param aPlaylistId id of the playlist which contains the song to be reordered
        * @param aSongId id of the song to be reordered
        * @param aOriginalOrdinal the original ordinal of the song within the playlist.
        *        Ordinal starts from 0.
        * @param aNewOrdinal the new ordinal of the song within the playlist. Ordinal
        *        starts from 0.
        */
        void ReorderSongL(const TMPXItemId& aPlaylistId, const TMPXItemId& aSongId,
            TUint aOriginalOrdinal, TUint aNewOrdinal);

        /**
        * Remove the given song from all playlists
        * @param aSongId ID of the song to remove
        * @param aDriveId identifies the database to be updated
        */
        void DeleteSongL(TUint32 aSongId);

        /**
        * Remove a song from the playlist.
        * @param aPlaylistId ID of the playlist to remove the song from
        * @param aSongId Id of the song in the playlist to be removed
        * @param aOrdinal the ordinal of the song in the playlist. This uniquely
        *        identifies which song in the playlist to be removed; whereas song
        *        ID doesn't because duplicate tracks in the playlist are allowed.
        *        The position is relative to zero, i.e. zero implies that the song
        *        at the beginning of the playlist is to be removed.
        * @param aDriveId identifies the database to be updated
        */
        void DeleteSongL(TUint32 aPlaylistId, TUint32 aSongId, TInt aOrdinal, TInt aDriveId);

        /**
        * Remove all songs from the specified playlist
        * @param aPlaylistId ID of the playlist.
        * @param aDriveId identifies the database.
        */
        void DeleteSongsL(TUint32 aPlaylistId, TInt aDriveId);

        /**
        * Remove the songs for a given category from all playlists.
        * @param aCategory identifies the category
        * @param aCategoryId identifies the category item
        * @param aDriveId identifies the database to be updated
        */
        void DeleteSongsForCategoryL(TMPXGeneralCategory aCategory, TUint32 aCategoryId,
            TInt aDriveId);

        /**
        * Remove the songs for a given category from all playlists.
        * @param aCategory identifies the category
        * @param aCategoryId identifies the category item
        * @param aDriveId identifies the database to be updated
        */
        void DeleteSongsForArtistAndAlbumL(TUint32 aArtistId, TUint32 aAlbumId, TInt aDriveId);

        /**
        * Remove all songs from the playlist songs table
        */
        void DeleteAllSongsL();

        /**
        * Get ids of all songs in a playlist
        * @param aPlaylistId identifies the playlist
        * @param aMediaArray returns songs' id
        */
        void GetSongsL(TUint32 aPlaylistId, CMPXMediaArray& aMediaArray);

        /**
        * Get ids of all songs in a playlist
        * @param aPlaylistId identifies the playlist
        * @param aAttrs attributes to return
        * @param aMediaArray returns songs' id
        */
        TBool GetSongsL( TUint32 aPlaylistId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMediaArray& aMediaArray);

        /**
        * Get information on a song in a playlist
        * @param aPlaylistId identifies the playlist
        * @param aSongId identifies the song
        * @param aAttrs attributes to return
        * @param aMedia returns song info
        */
        TBool GetSongL(TUint32 aPlaylistId, TUint32 aSongId,
            const TArray<TMPXAttribute>& aAttrs, CMPXMedia& aMedia);

        /**
        * Get information on a song
        * @param aSongId identifies the song
        * @param aAttrs attributes to return
        * @param aMedia returns song info
        */
        TBool GetSongL(const TMPXItemId& aSongId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia*& aMedia);


        /**
        * Get the number of songs for a playlist
        * @param aPlaylistId identifies the playlist
        * @param aDriveId identifies the database to be query
        * @return number of songs
        */
        TInt CountL(TUint32 aPlaylistId, TInt aDriveId);     

    private:

        /**
        * Update the media from the table
        * @param aRecord playlist table view
        * @param aAttrs the attributes to return
        * @param aMedia updated with the playlist song info
        */
        void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Get the number of instances of a song in a playlist
        * @param aPlaylistId identifies the playlist
        * @param aSongId identifies the song
        * @return number of instances
        */
        TInt SongInstanceCountL(TUint32 aPlaylistId, TUint32 aSongId);

        /**
        * Get the number of instances of a song in the PlaylistSongs table.
        * @param aSongId identifies the song
        * @return number of instances
        */
        TInt SongCountL(TUint32 aSongId);

        /**
        * Checks if the specified song exists in the PlaylisySongInfo table.
        * @param aDriveId drive to check or KDbManagerAllDrives
        * @param aSongId identifies the song
        * @return ETrue if it exists, EFalse otherwise
        */
        TBool SongInfoExistsL(TInt aDriveId, TUint32 aSongId);

        /**
        * Deletes all orphaned records in the PlaylistSongInfo table
        * (songs not included in any playlist).
        * @param aDriveId database to cleanup
        */
        void CleanupSongInfoL(TInt aDriveId);

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
        CMPXDbPlaylistSongs(CMPXDbManager& aDbManager);

        /**
        * Safely construct things that can leave
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the playlist songs query
        */
        enum TPlaylistSongsColumns
            {
            EPlaylistSongsUniqueId = KMPXTableDefaultIndex,
            EPlaylistSongsSongId,
            EPlaylistSongsPlaylistId,
            EPlaylistSongsOrdinal,
            EPlaylistSongsVolumeId,
            EPlaylistSongsDbFlag,
            EPlaylistSongsUri,
            EPlaylistSongsTitle
            };

        /**
        * Column indexes in the playlist song info table
        */
        enum TPlaylistSongInfoColumns
            {
            EPlaylistSongInfoSongId = KMPXTableDefaultIndex,
            EPlaylistSongInfoVolumeId,
            EPlaylistSongInfoDbFlag,
            EPlaylistSongInfoUri,
            EPlaylistSongInfoTitle
            };

    };

#endif // MPXDBPLAYLISTSONGS_H

// End of File
