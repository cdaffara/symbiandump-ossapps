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
* Description:  This class is responsible for the auto playlist functionality.
*
*/


#ifndef MPXDBAUTOPLAYLIST_H
#define MPXDBAUTOPLAYLIST_H

// INCLUDES
#include <e32base.h>
#include <mpxattribute.h>

// CLASS FORWARDS
class CMPXMedia;
class CMPXMediaArray;
class CMPXDbManager;

// CLASS DECLARATION

/**
* Responsible for managing the music table
*
* @lib MPXDbPlugin.lib
*/
class CMPXDbAutoPlaylist :
    public CBase
    {
    public:

        /**
        * Two-phased constructor.
        * @return new CMPXDbAutoPlaylist instance.
        */
        static CMPXDbAutoPlaylist* NewL(CMPXDbManager& aDbManager, RFs& aFs, CMPXResource& aResource);

        /**
        * Two-phased constructor.
        * @return new CMPXDbAutoPlaylist instance on the cleanup stack.
        */
        static CMPXDbAutoPlaylist* NewLC(CMPXDbManager& aDbManager, RFs& aFs, CMPXResource& aResource);

        /**
        * Destructor
        */
        virtual ~CMPXDbAutoPlaylist();

    public:

        /**
        * Get the playlist ID of an auto-playlist by type
        * @param aPlaylistType type of the auto-playlist
        * @return auto playlist ID
        * @leave KErrNotSupported if invalid playlist type
        */
        TUint32 AutoPlaylistIdL(TMPXAutoPlaylistType aPlaylistType);

        /**
        * Get the name of an auto playlist
        * @param aPlaylistId identifies the playlist
        * @return the name of the playlist
        * @leave KErrNotSupported if invalid playlist ID
        */
        TPtrC AutoPlaylistNameL(TUint32 aPlaylistId);

        /**
        * Checks whether the playlist ID corresponds to an auto playlist or not.
        * @param aPlaylistId identifies the playlist
        * @return Auto playlist type or EMPXNoAutoPlaylist if not found
        */
        TMPXAutoPlaylistType AutoPlaylistTypeL(TUint32 aPlaylistId);

        /**
        * Get information on a specified playlist
        * @param aPlaylistId identifies the playlist
        * @param aAttrs the attributes to include for the playlists found.
        * @param aMedia returns the playlist information
        * @leave KErrNotSupported if invalid playlist ID
        */
        void GetPlaylistL(TUint32 aPlaylistId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

        /**
        * Get information on all auto playlists
        * @param aMediaArray returns information on all auto playlists
        */
        void GetAllPlaylistsL(CMPXMediaArray& aMediaArray);

    private:

        /**
        * Get count and duration information of the auto-playlist
        * @param aType the type of the auto-playlist
        * @param aCount reference to count to be returned
        * @param aDuration reference to duration to be returned
        */
        void GetPlaylistDetailsL(TMPXAutoPlaylistType aType, TInt& aCount, TInt& aDuration);

        /**
        * Update the media with information about the auto-playlist
        * @param aPlaylistId Id of the auto-playlist
        * @param aAttrs the attributes to include for the playlist matching the
        *        selection criteria
        * @param aMedia the playlist details object
        */
        void UpdateMediaL(TUint32 aPlaylistId, const TArray<TMPXAttribute>& aAttrs,
            CMPXMedia& aMedia);

    private:

        /**
        * C++ constructor
        */
        CMPXDbAutoPlaylist(CMPXDbManager& aDbManager);

        /**
        * Second phase constructor
        */
        void ConstructL(RFs& aFs, CMPXResource& aResource);

    private:    // Data
        TUint32             iRecentlyPlayedPlaylistId; // recently played playlist ID
        TUint32             iMostPlayedPlaylistId;     // most played playlist ID
        TUint32             iRecentlyAddedPlaylistId;  // recently added playlist ID

        // owned
        HBufC*              iRecentlyPlayedPlaylist;   // recently played playlist name
        HBufC*              iMostPlayedPlaylist;       // most played playlist name
        HBufC*              iRecentlyAddedPlaylist;    // recently added playlist name
        CMPXDbManager&      iDbManager;
    };

#endif // MPXDBAUTOPLAYLIST_H

// End of File
