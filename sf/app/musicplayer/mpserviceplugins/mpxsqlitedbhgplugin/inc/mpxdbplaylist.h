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
* Description:  This class manages the Playlist table and owns a
*                CMPXPlaylistSongs instance for managing the PlaylistSongs and
*                PlaylistSongInfo tables.
*
*                The Playlist table stores one record for each playlist.
*
*
*/


#ifndef MPXDBPLAYLIST_H
#define MPXDBPLAYLIST_H

// INCLUDES
#include "mpxdbtable.h"

// CLASS FORWARDS
class RSqlStatement;
class CMPXDbPlaylistSongs;
class CMPXMedia;
class CMPXMediaArray;

// CLASS DECLARATION

/**
* Responsible for managing the playlist tables
*
* @lib MPXDbPlugin.lib
*/
class MMPXDbPlaylistObserver
	{
	public:
		virtual TInt HandlePlaylistDurationL(TUint32 aPlaylistId) = 0;
        virtual void HandlePlaylistInfoL(TUint32 aPlaylistId, TInt& aCount, TInt& aDuration) = 0;
	};
class CMPXDbPlaylist : public CMPXDbTable
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbPlaylist instance.
        */
        static CMPXDbPlaylist* NewL(CMPXDbManager& aDbManager, MMPXDbPlaylistObserver& aObserver);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @return New CMPXDbManager instance on the cleanup stack.
        */
        static CMPXDbPlaylist* NewLC(CMPXDbManager& aDbManager, MMPXDbPlaylistObserver& aObserver);

        /**
        * Destructor
        */
        virtual ~CMPXDbPlaylist();

    public:

        /**
        * Add a playlist to the playlist table.  If playlist already exists,
        * it is updated.
        * @param aMedia playlist media to be added to the collection
        * @return The unique id of the playlist
        */
        TUint32 AddPlaylistL(const CMPXMedia& aMedia);

        /**
        * Add one or multiple songs to the playlist.
        * @param aPlaylistId identifies the playlist
        * @param aMediaArray contains the song information to be added
        */
        void AddSongsL(TUint32 aPlaylistId, const CMPXMediaArray& aMediaArray);

        /**
        * Update playlist info
        * @param aMedia playlist object which contains playlist Id, file path,
        *               timestamp, etc... to be updated in the playlist table
        * @param aMessage change event message for the playlist updated
        */
        void UpdatePlaylistL(const CMPXMedia& aMedia, CMPXMessage& aMessage, TInt aDriveId);

        /**
        * Update a playlist song
        * @param aMedia song info
        * @param aResetFlags indicates whether the song db flags should be reset or just updated
        * @param aItemChangedMessages returns the change messages for the playlists updated
        * @return ETrue if the change is visible in the UI
        */
        TBool UpdateSongL(const CMPXMedia& aMedia, TBool aResetFlags,
            CMPXMessageArray* aItemChangedMessages = NULL);

        /**
        * Remove a playlist and return its URI
        * @param aPlaylistId identifies the playlist
        * @return HBufC containing the URI. Ownership is transferred.
        */
        HBufC* DeletePlaylistL(TUint32 aPlaylistId);

        /**
        * Remove a playlist
        * @param aPlaylistId identifies the playlist
        */
        void DeletePlaylistNoUriL(TUint32 aPlaylistId);

        /**
        * Delete a song from the playlist tables.
        * @param aSongId identifies the song
        * @param aItemChangedMessages returns the item changed messages
        */
        void DeleteSongL(TUint32 aSongId, CMPXMessageArray& aItemChangedMessages);

        /**
        * Delete a song from the playlist tables.
        * @param aPlaylistId identifies the playlist
        * @param aSongId identifies the song
        * @param aOrdinal song ordinal
        */
        void DeleteSongL(TUint32 aPlaylistId, TUint32 aSongId, TInt aOrdinal);

        /**
        * Remove all playlists.
        */
        void DeleteAllPlaylistsL();

        /**
        * Gets the total number of playlists.
        * @return number of playlists
        */
        TInt CountL();

        /**
        * Get information on a specified playlist
        * @param aPlaylistId identifies the playlist
        * @param aAttrs the attributes to include for the playlists found.
        * @param aMedia returns the playlist information
        */
        void GetPlaylistL(TUint32 aPlaylistId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Get all playlists
        * @param aAttrs the attributes to include for the playlists found.
        * @param aMediaArray returns the playlist information
        */
        void GetAllPlaylistsL(const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Get the name of a playlist.
        * @param aPlaylistId identifies the playlist
        * @return name matching the specified playlist ID. Ownership is transferred.
        */
        HBufC* GetNameL(TUint32 aPlaylistId);

        /**
        * Get a playlist' uri
        * @param aPlaylistId identifies the playlist
        * @return URI matching the specified playlist ID. Ownership is transferred.
        */
        HBufC* GetUriL(TUint32 aPlaylistId);

        /**
        * Return the playlist ID for a specified URI.
        * @param aUri URI of the playlistId
        * @return playlist ID
        */
        TUint32 GetIdL(const TDesC& aUri);

        /**
        * Find playlist(s) which match(es) the selection criteria
        * @param aCriteria selection criteria
        * @param aAttrs the attributes to include for the playlist(s) matching the
        *        selection criteria
        * @param aMediaArray playlist(s) matching the selection criteria
        */
        void FindAllL(const CMPXMedia& aCriteria, const TArray<TMPXAttribute>& aAttrs,
            CMPXMediaArray& aMediaArray);

        /**
        * Find the drive Id for a playlist
        * @param aPlaylistId identifies the playlist
        * @return corresponding drive id
        */
        TInt GetDriveIdL(TUint32 aPlaylistId);

        /**
        * Provides access to the songs for the current playlist.
        * @return CMPXDbPlaylistSongs reference to be used to access the songs
        */
        CMPXDbPlaylistSongs& Songs();

        /**
        * Returns the playlist count for a given drive
        * @param aDrive drive
        * @return the count
        */
        TUint GetDrivePlaylistCountL(TInt aDrive);

        /**
        * Get URIs for a given number of playlists
        * @param aDrive, specifies drive number for db selection
        * @param aFromID, specifies unique ID of last record retrieved
        * @param aRecords, specifies the number of records to get
        * @param aUriArr, will contain all URIs on return
        * @param aLastID, will contain unique ID of a last record in the array
        */
        void GetPlaylistUriArrayL(TInt aDrive, TInt aFromID, TInt aRecords,
                                  CDesCArray& aUriArr, TInt& aLastID);

    private:

        /**
        * Update the media with information found in the Playlist table
        * @param aRecord playlist table
        * @param aAttrs the attributes to include for the playlist matching the
        *        selection criteria
        * @param aMedia returns playlist details
        */
        void UpdateMediaL(RSqlStatement& aRecord, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Create playlist table view and have the current row point to aPlaylistId
        * @param aPlaylistId ID of the playlist to match
        * @return RSqlStatement with row pointing to selected aPlaylistId
        * Function leaves with KErrNotFound if RSqlStatement is unable to initialize
        * or row cannot be found
        */
        RSqlStatement GetPlaylistRecordL(TUint32 aPlaylistId);

        /**
        * Add a new playlist
        * @param aMedia contains the playlist info
        * @param aDriveId drive to add the playlist to
        * @return ID of the new playlist
        */
        TUint32 DoAddPlaylistL(const CMPXMedia& aMedia, TInt aDriveId);

        /**
        * Update a playlist
        * @param aPlaylistId identifies the playlist
        * @param aMedia contains the playlist info
        * @param aDriveId drive to add the playlist to
        * @param aMessage change event message for the playlist updated
        */
        void DoUpdatePlaylistL(TUint32 aPlaylistId, const CMPXMedia& aMedia, TInt aDriveId,
            CMPXMessage& aMessage);

        /**
        * Sets the Time field for all playlists that contain the specified song to the
        * current time
        * @param aSongId identifies the song
        * @param aItemChangedMessages if not NULL item changed messages are added for all
        *           updated playlists
        * @param aSongInPlaylists out parameter that flags if the song is part of a playlist or not
        */
        void UpdatePlaylistsForSongL(TUint32 aSongId, CMPXMessageArray* aItemChangedMessages,TBool &aSongInPlaylists);

        /**
        * Sets the Time field for a playlist to the current time.
        * @param aPlaylistId identifies the playlist
        * @param aDrive playlist drive
        */
        void UpdatePlaylistTimeL(TUint32 aPlaylistId, TInt aDrive);

        TInt GetDrivePlaylistDuration(TUint32 aPlaylistId);
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
        CMPXDbPlaylist(CMPXDbManager& aDbManager, MMPXDbPlaylistObserver& aObserver);

        /**
        * Second phase constructor
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the playlist table
        */
        enum TPlaylistColumns
            {
            EPlaylistUniqueId = KMPXTableDefaultIndex,
            EPlaylistVolumeId,
            EPlaylistDbFlag,
            EPlaylistSync,
            EPlaylistName,
            EPlaylistUri,
            EPlaylistTime
            };

    private:    // Data

        CMPXDbPlaylistSongs* iPlaylistSongs;
		MMPXDbPlaylistObserver& iObserver;
    };

#endif // MPXDBPLAYLIST_H

// End of File
