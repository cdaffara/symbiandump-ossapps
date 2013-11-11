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
* Description:  This header file contains macro definitions used by the
*                database subsystem.
*
*/

#ifndef MPXCOLLECTIONDBDEF_H
#define MPXCOLLECTIONDBDEF_H

// INCLUDES
#include <mmfmeta.h>

// CONSTANTS

// ================================== DATABASE ================================

// Db filename
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KMCDbFile, "mpxv3_2.db");
_LIT(KMCDbFileEMMC, "mpxv3_2i.db");
#else 
_LIT(KMCDbFile, "mpxv3_2n.db");
_LIT(KMCDbFileEMMC, "mpxv3_2in.db");
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// Collection Db resource file
_LIT(KMPXCollectionDbResourceFile,  "mpxcollectiondbhgres.rsc");

// == TABLE NAMES =============================================================
// The Auxiliary table stores general database metadata
_LIT(KMCAuxiliaryTable,             "Auxiliary");
// The Music table stores songs information
_LIT(KMCMusicTable,                 "Music");
// The Playlist table stores playlists information
_LIT(KMCPlaylistTable,              "Playlist");
// The PlaylistSongs table stores the song instances for each playlist
_LIT(KMCPlaylistSongsTable,         "PlaylistSongs");
// The PlaylistSongInfo table stores the songs that are included in playlists
_LIT(KMCPlaylistSongInfoTable,      "PlaylistSongInfo");
// The Artist table stores artist information
_LIT(KMCArtistTable,                "Artist");
// The Album table stores albums information
_LIT(KMCAlbumTable,                 "Album");
// The Genre table stores genres information
_LIT(KMCGenreTable,                 "Genre");
// The Composer table stores composers information
_LIT(KMCComposerTable,              "Composer");
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// The AbstractAlbum table stores Abstract Album Art information
_LIT(KMCAbstractAlbumTable,              "AbstractAlbum");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
// == Music TABLE FIELDS ======================================================
_LIT(KMCMusicUniqueId,          "UniqueId");

// From MMF (MmfMeta.h)
#define KMCMusicName                KMMFMetaEntrySongTitle           // "title"
#define KMCMusicArtist              KMMFMetaEntryArtist              // "artist"
#define KMCMusicAlbum               KMMFMetaEntryAlbum               // "album"
#define KMCMusicTrackNumber         KMMFMetaEntryAlbumTrack          // "albumtrack"
#define KMCMusicGenre               KMMFMetaEntryGenre               // "genre"
#define KMCMusicComposer            KMMFMetaEntryComposer            // "composer"
#define KMCMusicComment             KMMFMetaEntryComment             // "comment"
#define KMCMusicCopyright           KMMFMetaEntryCopyright           // "copyright"
#define KMCMusicOrigArtist          KMMFMetaEntryOriginalArtist      // "originalartist"
//end of MMF support data

// NOTE: Location will store the path without drive letter
_LIT(KMCMusicLocation,              "Location");
_LIT(KMCMusicDuration,              "Duration");
_LIT(KMCMusicReleaseDate,           "ReleaseDate");
_LIT(KMCMusicTimeAdded,             "TimeAdded");
_LIT(KMCMusicPlayCount,             "PlayCount");
_LIT(KMCMusicTimePlayed,            "TimePlayed");
_LIT(KMCMusicRating,                "Rating");
_LIT(KMCMusicArt,                   "Art");
_LIT(KMCMusicDbFlag,                "DbFlag");
_LIT(KMCMusicSync,                  "Sync");
_LIT(KMCMusicMod,                   "Modified");
_LIT(KMCMusicDel,                   "Deleted");
_LIT(KMCMusicUrl,                   "Url");
_LIT(KMCMusicDRM,                   "DRM");
_LIT(KMCMusicLastPlayPosition,      "LastPlayPosition");
_LIT(KMCMusicSampleRate,            "SampleRate");
_LIT(KMCMusicBitRate,               "BitRate");
_LIT(KMCMusicVolumeId,              "VolumeId");

// Number of channels: required by MTP to support Play4sure
_LIT(KMCMusicNumChannels,           "NumChannels");

// Audio Codec: required by MTP to support Play4sure
_LIT(KMCMusicCodec,                 "Codec");
_LIT(KMCMusicMimeType,              "MimeType");
_LIT(KMCMusicMTPDrmStatus,          "MTPDrmStatus");
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KMCMusicAlbumArtist,           "AlbumArtist");
_LIT(KMCMusicContainEmbeddedArt,    "ContainEmbeddedArt");
_LIT(KMCMusicAbstractAlbum,         "AbstractAlbum");
#endif // ABSTRACTAUDIOALBUM_INCLUDED

// == Category TABLE FIELDS ===================================================
_LIT(KMCCategoryUniqueId,           "UniqueId");
_LIT(KMCCategoryName,               "Name");
_LIT(KMCCategorySongCount,          "SongCount");

// == Playlist TABLE FIELDS ===================================================
_LIT(KMCPlaylistUniqueId,           "UniqueId");
_LIT(KMCPlaylistName,               "Name");
_LIT(KMCPlaylistUri,                "Uri");
_LIT(KMCPlaylistVolumeId,           "VolumeId");
_LIT(KMCPlaylistSync,               "Sync");
_LIT(KMCPlaylistDbFlag,             "DbFlag");
_LIT(KMCPlaylistTime,               "Time");

// == PlaylistSongs TABLE FIELDS ==============================================
_LIT(KMCPlaylistSongsUniqueId,      "UniqueId");
_LIT(KMCPlaylistSongsSongId,        "SongId");
_LIT(KMCPlaylistSongsPlaylistId,        "PlaylistId");
_LIT(KMCPlaylistSongsOrdinal,   "Ordinal");

// == PlaylistSongInfo TABLE FIELDS ===========================================
_LIT(KMCPlaylistSongInfoSongId,     "SongId");
_LIT(KMCPlaylistSongInfoUri,        "Uri");
_LIT(KMCPlaylistSongInfoTitle,      "Title");
_LIT(KMCPlaylistSongInfoVolumeId,   "VolumeId");
_LIT(KMCPlaylistSongInfoDbFlag,     "DbFlag");

// == Album TABLE FIELDS ==============================================
_LIT(KMCAlbumArtistName, "ArtistName");
// == Auxiliary TABLE FIELDS ==================================================
// Version of database
// The version integer looks like:
// 31     23      15           0 bit
// -----------------------------
// |Major |Minor  |Build       |
// -----------------------------
_LIT(KMCAuxVersion,                 "Version");

// Last time db was refreshed
_LIT(KMCAuxTimeRefresh,             "TimeRefreshed");

// Last time db was synced
_LIT(KMCAuxTimeSync,                "TimeSynced");

// Database Corrupted bit
_LIT(KMCMusicCorruptDB,             "Corrupt");

// Number of records that have been marked as deleted
_LIT(KMCSavedDeletedRecordCount,    "SaveDeletedRecordCount");

#endif // MPXCOLLECTIONDBDEF_H

// End of File
