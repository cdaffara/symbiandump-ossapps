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
* Description:  This header file contains macro definitions used by the database subsystem.
*
*/

#ifndef MPXPODCASTCOLLECTIONDBDEF_H
#define MPXPODCASTCOLLECTIONDBDEF_H

// INCLUDES
#include <mmfmeta.h>

// CONSTANTS

// ================================== DATABASE ================================

// Db filename
_LIT(KMCDbFile, "pcv6_1.db");
_LIT(KMCDbFileEMMC, "pcv6_1i.db");
// Collection Db resource file
_LIT(KMPXCollectionDbResourceFile,  "PCRes.rsc");

// == TABLE NAMES ===============================================
// The Auxiliary table stores general database metadata like last refresh time etc...
_LIT(KMCAuxiliaryTable,         "Auxiliary");
// The Podcast table stores information about the episodes stored within the collection
_LIT(KMCPodcastTable,           "Podcast");
// The author table stores information about the authors of the music stored in the collection
_LIT(KMCAuthorTable,            "Artist"); // podcast author = media artist
// The Title table stores information about the podcast titles of the episodes stored in the collection
_LIT(KMCTitleTable,             "Album");  // podcast title = media album
// The Category table stores information about the category of the episodes stored in the collection
_LIT(KMCCategoryTable,          "Genre");  // podcast category = media genre
// The Composer table stores information about the composer of the episodes stored in the collection
_LIT(KMCComposerTable,          "Composer");

// == Podcast TABLE FIELDS ======================================================
// Uniqe Id (32 bits). Combination of 4bits of table Id and 28bits of MD5 hashed code
// (from Path or Name string)
_LIT(KMCPodcastUniqueId,            "UniqueId");
_LIT(KMCPodcastVolumeId,            "VolumeId");
_LIT(KMCPodcastSync,                "Sync");        // Ever SYNCed with PC
_LIT(KMCPodcastMod,                 "Modified");    // Modified in db
_LIT(KMCPodcastIsPlaying,           "IsPlaying");   // Episode is currently playing
_LIT(KMCPodcastDel,                 "Deleted");     // File deleted already
_LIT(KMCPodcastDuration,            "Duration");
_LIT(KMCPodcastTimeAdded,           "TimeAdded");
_LIT(KMCPodcastPlayCount,           "PlayCount");
_LIT(KMCPodcastTimePlayed,          "TimePlayed");
_LIT(KMCPodcastRating,              "Rating");
// NOTE: Location will store the path and file name, without drive-letter
_LIT(KMCPodcastLocation,            "Location");
_LIT(KMCPodcastArt,                 "Art");
_LIT(KMCPodcastDbFlag,              "DbFlag");
_LIT(KMCPodcastUrl,                 "Url");
_LIT(KMCPodcastDRM,                 "DRM");
_LIT(KMCPodcastReleaseDate,         "ReleaseDate");
_LIT(KMCPodcastPublishDate,         "PublishDate");
_LIT(KMCPodcastLastPlayPosition,    "LastPlayPosition");
_LIT(KMCPodcastSampleRate,          "SampleRate");
_LIT(KMCPodcastBitRate,             "BitRate");
// Number of channels: required by MTP to support Play4sure
_LIT(KMCPodcastNumChannels,         "NumChannels");
// Audio Codec: required by MTP to support Play4sure
_LIT(KMCPodcastCodec,               "Codec");
_LIT(KMCPodcastMimeType,            "MimeType");

// From MMF (MmfMeta.h)
#define KMCPodcastName             KMMFMetaEntrySongTitle           // "title"
// podcast author = media artist
#define KMCPodcastAuthor           KMMFMetaEntryArtist              // "artist"
// podcast title = media album
#define KMCPodcastTitle            KMMFMetaEntryAlbum               // "album"
#define KMCPodcastTrackNumber      KMMFMetaEntryAlbumTrack          // "albumtrack"
// podcast category = media genre
#define KMCPodcastGenre            KMMFMetaEntryGenre               // "genre"
#define KMCPodcastComposer         KMMFMetaEntryComposer            // "composer"
#define KMCPodcastComment          KMMFMetaEntryComment             // "comment"
#define KMCPodcastCopyright        KMMFMetaEntryCopyright           // "copyright"
#define KMCPodcastOrigArtist       KMMFMetaEntryOriginalArtist      // "originalartist"
_LIT(KMCPodcastCategory,           "PodcastCategory");
// end of MMF support data

// == Category TABLE FIELDS ===================================================
_LIT(KMCCategoryUniqueId,           "UniqueId");
_LIT(KMCCategoryName,               "Name");
_LIT(KMCCategoryCountEpisodes,      "EpisodeCount");

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
_LIT(KMCPodcastCorruptDB,           "Corrupt");

// Number of records that have been marked as deleted
_LIT(KMCSavedDeletedRecordCount,    "SaveDeletedRecordCount");


#endif // MPXPODCASTCOLLECTIONDBDEF_H

//End of File
