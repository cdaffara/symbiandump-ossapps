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
* Description:  SQLite queries for MPX collection db
*
*/


#ifndef MPXDBPLUGINQUERIES_H
#define MPXDBPLUGINQUERIES_H

// SQLite queries

// == Music table queries =====================================================
//
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KMusicCreateTable, "CREATE TABLE Music("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"DbFlag INTEGER,"
    L"VolumeId INTEGER,"
    L"Title TEXT COLLATE NOCASE,"
    L"Artist INTEGER,"
    L"Art TEXT,"
    L"Deleted INTEGER DEFAULT 0,"
    L"Location TEXT,"
    L"AlbumTrack INTEGER,"
    L"PlayCount INTEGER DEFAULT 0,"
    L"TimeAdded TEXT,"
    L"TimePlayed TEXT DEFAULT '',"
    L"Duration INTEGER,"
    L"Sync INTEGER DEFAULT 0,"
    L"Modified INTEGER DEFAULT 0,"
    L"Album INTEGER,"
    L"Genre INTEGER,"
    L"Composer INTEGER,"
    L"ReleaseDate TEXT DEFAULT '',"
    L"Rating INTEGER,"
    L"Comment TEXT,"
    L"Copyright TEXT,"
    L"Url TEXT,"
    L"DRM INTEGER,"
    L"LastPlayPosition INTEGER DEFAULT 0,"
    L"SampleRate INTEGER,"
    L"BitRate INTEGER,"
    L"NumChannels INTEGER,"
    L"Codec INTEGER,"
    L"MimeType TEXT,"
    L"MTPDrmStatus INTEGER," 
    L"AlbumArtist TEXT,"
    L"ContainEmbeddedArt INTEGER DEFAULT 0,"
    L"AbstractAlbum INTEGER)");

#else
_LIT(KMusicCreateTable, "CREATE TABLE Music("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"DbFlag INTEGER,"
    L"VolumeId INTEGER,"
    L"Title TEXT COLLATE NOCASE,"
    L"Artist INTEGER,"
    L"Art TEXT,"
    L"Deleted INTEGER DEFAULT 0,"
    L"Location TEXT,"
    L"AlbumTrack INTEGER,"
    L"PlayCount INTEGER DEFAULT 0,"
    L"TimeAdded TEXT,"
    L"TimePlayed TEXT DEFAULT '',"
    L"Duration INTEGER,"
    L"Sync INTEGER DEFAULT 0,"
    L"Modified INTEGER DEFAULT 0,"
    L"Album INTEGER,"
    L"Genre INTEGER,"
    L"Composer INTEGER,"
    L"ReleaseDate TEXT DEFAULT '',"
    L"Rating INTEGER,"
    L"Comment TEXT,"
    L"Copyright TEXT,"
    L"Url TEXT,"
    L"DRM INTEGER,"
    L"LastPlayPosition INTEGER DEFAULT 0,"
    L"SampleRate INTEGER,"
    L"BitRate INTEGER,"
    L"NumChannels INTEGER,"
    L"Codec INTEGER,"
    L"MimeType TEXT,"
    L"MTPDrmStatus INTEGER)");
#endif // ABSTRACTAUDIOALBUM_INCLUDED

_LIT(KMusicDropTable,"DROP TABLE Music");
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KMusicCheckTable,"SELECT UniqueId,Title,Location,Artist,Album,AlbumTrack,Genre,Composer,Duration,ReleaseDate,TimeAdded,PlayCount,TimePlayed,Rating,Comment,Copyright,Art,DbFlag,Sync,Modified,Deleted,Url,DRM,LastPlayPosition,SampleRate,BitRate,VolumeId,NumChannels,Codec,MimeType,MTPDrmStatus,AlbumArtist,ContainEmbeddedArt,AbstractAlbum FROM Music");
#else
_LIT(KMusicCheckTable,"SELECT UniqueId,Title,Location,Artist,Album,AlbumTrack,Genre,Composer,Duration,ReleaseDate,TimeAdded,PlayCount,TimePlayed,Rating,Comment,Copyright,Art,DbFlag,Sync,Modified,Deleted,Url,DRM,LastPlayPosition,SampleRate,BitRate,VolumeId,NumChannels,Codec,MimeType,MTPDrmStatus FROM Music");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
// indexes
_LIT(KMusicDeletedIdIndex,"CREATE INDEX IndexMusicDeletedId ON Music(Deleted,UniqueId)");
_LIT(KMusicDeletedTitleIndex,"CREATE INDEX IndexMusicDeletedTitle ON Music(Deleted,Title)");
_LIT(KMusicDeletedArtistTitleIndex,"CREATE INDEX IndexMusicArtistDeletedTitle ON Music(Artist,Deleted,Title)");
_LIT(KMusicDeletedAlbumTitleIndex,"CREATE INDEX IndexMusicAlbumDeletedTitle ON Music(Album,Deleted,Title)");
_LIT(KMusicDeletedAlbumTrackTitleIndex,"CREATE INDEX IndexMusicAlbumDeletedTrackTitle ON Music(Album,Deleted,AlbumTrack,Title)");
_LIT(KMusicDeletedGenreTitleIndex,"CREATE INDEX IndexMusicGenreDeletedTitle ON Music(Genre,Deleted,Title)");
_LIT(KMusicDeletedComposerTitleIndex,"CREATE INDEX IndexMusicComposerDeletedTitle ON Music(Composer,Deleted,Title)");

_LIT(KQueryMusicInsert, "INSERT INTO :dbname.Music(%S) VALUES(%S)");
_LIT(KQueryMusicUpdate, "UPDATE :dbname.Music SET %S WHERE UniqueId=%u");
_LIT(KQueryMusicDelete, "DELETE FROM :dbname.Music WHERE UniqueId=%u");
_LIT(KQueryMusicDeleteUpdate, "UPDATE :dbname.Music SET Deleted=1 WHERE UniqueId=%u");
_LIT(KQueryMusicDeleteForCategory, "UPDATE :dbname.Music SET Deleted=0 WHERE %S=%u");
_LIT(KQueryMusicCleanup, "DELETE FROM :dbname.Music WHERE Deleted=1");

// Note: The no categories queries below only retrieve a limited number of fields from the Music table
//       as opposed to *. This is an optimization that matches the DBMS version of the plugin and assumes
//       that the client only requests those fields. From the C++ code perspective there is no difference
//       in processing as the five fields in question reside at the beginning of the table. If the client
//       is modified to request more attributes then the queries below will have to be changed to return *.
//
//       As far as performance goes, the queries that only return six fields are a little faster than
//       the ones that return *, for example, for a * query that takes 2.1 seconds the equivalent
//       optimized query is faster by more than 100ms.

_LIT(KQueryMusicGetSong, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.UniqueId=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId");

// LTAN-79N8ND: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
/*
_LIT(KQueryMusicGetAllSongs, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.Title");
_LIT(KQueryMusicGetSongsForArtist, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Artist=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.Title");
_LIT(KQueryMusicGetSongsForAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.AlbumTrack, Music.Title");
_LIT(KQueryMusicGetSongsForUnknownAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.Title");
_LIT(KQueryMusicGetSongsForArtistAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Artist=%u AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.AlbumTrack, Music.Title");
_LIT(KQueryMusicGetSongsForGenre, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Genre=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.Title");
_LIT(KQueryMusicGetSongsForComposer, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Composer=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.Title");
_LIT(KQueryMusicGetRecentlyPlayed, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.TimePlayed<>'' AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetMostPlayed, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.PlayCount<>0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY PlayCount DESC, TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetRecentlyAdded, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND (julianday(Music.TimeAdded)>julianday('now','-7 days')) AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY Music.TimeAdded DESC");
*/

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicGetAllSongsMinimum, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,0,Music.Art,Music.Deleted,Music.Location,0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY MUSIC.Title");
_LIT(KQueryMusicGetAllSongsMinimumLimited, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,0,Music.Art,Music.Deleted,Music.Location,0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY MUSIC.Title LIMIT %u");
#else
_LIT(KQueryMusicGetAllSongsMinimum, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,0,Music.Art,Music.Deleted,Music.Location,0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY MUSIC.Title");
_LIT(KQueryMusicGetAllSongsMinimumLimited, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,0,Music.Art,Music.Deleted,Music.Location,0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY MUSIC.Title LIMIT %u");
#endif

_LIT(KQueryMusicGetAllSongs, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 4");
_LIT(KQueryMusicGetSongsForArtist, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Artist=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 4");
_LIT(KQueryMusicGetSongsForAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 9, 4");
_LIT(KQueryMusicGetSongsForUnknownAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 4");
_LIT(KQueryMusicGetSongsForArtistAlbum, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Artist=%u AND Music.Album=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 9, 4");
_LIT(KQueryMusicGetSongsForGenre, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Genre=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 4");
_LIT(KQueryMusicGetSongsForComposer, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.Composer=%u AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 4");
_LIT(KQueryMusicGetRecentlyPlayed, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.TimePlayed<>'' AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 12 DESC LIMIT %u");
_LIT(KQueryMusicGetMostPlayed, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND Music.PlayCount<>0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY PlayCount DESC, TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetRecentlyAdded, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Music.Deleted=0 AND (julianday(Music.TimeAdded)>julianday('now','-7 days')) AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId ORDER BY 11 DESC");
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicGetSongsForAbstractAlbum,"SELECT Music.*,AbstractAlbum.Uri FROM :dbname.Music,:dbname.AbstractAlbum WHERE Music.Deleted=0 AND Music.AbstractAlbum=AbstractAlbum.UniqueId AND AbstractAlbum.UniqueId=%u");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicSong, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE %S");
_LIT(KQueryMusicFindAll, "SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE %S AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId %S");

// LTAN-79N8ND: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
/*
_LIT(KQueryMusicFindAllSortOrderTrack, "ORDER BY Music.AlbumTrack,Music.Title");
_LIT(KQueryMusicFindAllSortOrderTitle, "ORDER BY Music.Title");
*/
_LIT(KQueryMusicFindAllSortOrderTrack, "ORDER BY 9, 4");
_LIT(KQueryMusicFindAllSortOrderTitle, "ORDER BY 4");

// no categories queries
_LIT(KQueryMusicGetSongNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryMusicGetAllSongsNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 ORDER BY Title");
_LIT(KQueryMusicGetSongsForArtistNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 AND Artist=%u ORDER BY Title");
_LIT(KQueryMusicGetSongsForAlbumNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location,AlbumTrack FROM :dbname.Music WHERE Deleted=0 AND Album=%u ORDER BY AlbumTrack,Title");
_LIT(KQueryMusicGetSongsForUnknownAlbumNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 AND Album=%u ORDER BY Title");
_LIT(KQueryMusicGetSongsForArtistAlbumNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location,AlbumTrack FROM :dbname.Music WHERE Deleted=0 AND Artist=%u AND Album=%u ORDER BY AlbumTrack,Title");
_LIT(KQueryMusicGetSongsForGenreNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 AND Genre=%u ORDER BY Title");
_LIT(KQueryMusicGetSongsForComposerNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location FROM :dbname.Music WHERE Deleted=0 AND Composer=%u ORDER BY Title");
_LIT(KQueryMusicGetRecentlyPlayedNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND TimePlayed<>'' ORDER BY TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetMostPlayedNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location,PlayCount,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND PlayCount<>0 ORDER BY PlayCount DESC, TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetRecentlyAddedNoCategories, "SELECT UniqueId,DbFlag,VolumeId,Title,0,'',0,Location,TimeAdded FROM :dbname.Music WHERE Deleted=0 AND (julianday(TimeAdded)>julianday('now','-7 days')) ORDER BY TimeAdded DESC");

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicGetSongsInBlockAsc, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId AND Title > :title ORDER BY 4 ASC LIMIT :limit");
_LIT(KQueryMusicGetSongsInBlockDsc, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId AND Title < :title ORDER BY 4 DESC LIMIT :limit");
_LIT(KQueryMusicGetSongsAtOffset, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY 4 LIMIT :limit OFFSET :offset");
_LIT(KQueryMusicGetSongsLimited, "Select Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,'',0,0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY 4 LIMIT %u");
#else
_LIT(KQueryMusicGetSongsInBlockAsc, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId AND Title > :title ORDER BY 4 ASC LIMIT :limit");
_LIT(KQueryMusicGetSongsInBlockDsc, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId AND Title < :title ORDER BY 4 DESC LIMIT :limit");
_LIT(KQueryMusicGetSongsAtOffset, "SELECT Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY 4 LIMIT :limit OFFSET :offset");
_LIT(KQueryMusicGetSongsLimited, "Select Music.UniqueId,Music.DbFlag,Music.VolumeId,Music.Title,Music.Artist,Music.Art,0,'',0,0,'','',0,0,0,0,0,0,0,0,'','','',0,0,0,0,0,0,'',0,Artist.Name FROM :dbname.Music,:dbname.Artist WHERE Deleted=0 AND Music.Artist=Artist.UniqueId ORDER BY 4 LIMIT %u");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicGetUri, "SELECT Location,VolumeId FROM :dbname.Music WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryMusicGetTitle, "SELECT Title FROM :dbname.Music WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryMusicGetMusicUris, "SELECT UniqueId,Location FROM :dbname.Music WHERE Deleted=0 ORDER BY UniqueId ASC LIMIT %u");
_LIT(KQueryMusicGetMusicUrisFrom, "SELECT UniqueId,Location FROM :dbname.Music WHERE Deleted=0 AND UniqueId > %u ORDER BY UniqueId ASC LIMIT %u");

_LIT(KQueryMusicCount, "SELECT count(*) FROM :dbname.Music WHERE Deleted=0");
_LIT(KQueryMusicAlbum, "SELECT DISTINCT Album FROM :dbname.Music WHERE Deleted=0 AND Artist=%u ORDER BY Album");  // deprecated, not maintained
_LIT(KQueryMusicVolume, "SELECT VolumeId FROM :dbname.Music WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryMusicVolumeAll, "SELECT VolumeId FROM :dbname.Music WHERE UniqueId=%u");
_LIT(KQueryMusicInfo, "SELECT * FROM :dbname.Music WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryOtherSongsInArtistAlbum, "SELECT count(*) FROM :dbname.Music WHERE Deleted=0 AND UniqueId<>%u AND Artist=%u AND Album=%u");  // deprecated, not maintained

_LIT(KQueryMusicDurationAll, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0");
_LIT(KQueryMusicDurationArtist, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND Artist=%u");
_LIT(KQueryMusicDurationAlbum, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND Album=%u");
_LIT(KQueryMusicDurationArtistAlbum, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND Artist=%u AND Album=%u");
_LIT(KQueryMusicDurationGenre, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND Genre=%u");
_LIT(KQueryMusicDurationComposer, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND Composer=%u");
_LIT(KQueryMusicDurationRecentlyPlayed, "SELECT sum(Duration) FROM (SELECT Duration,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND TimePlayed<>'' ORDER BY TimePlayed DESC LIMIT %u)");
_LIT(KQueryMusicDurationMostPlayed, "SELECT sum(Duration) FROM (SELECT Duration,PlayCount,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND PlayCount<>0 ORDER BY PlayCount DESC,TimePlayed DESC LIMIT %u)");
_LIT(KQueryMusicDurationRecentlyAdded, "SELECT sum(Duration) FROM :dbname.Music WHERE Deleted=0 AND (julianday(TimeAdded) > julianday('now','-7 days'))");

_LIT(KQueryMusicGetDurationRecentlyPlayed, "SELECT Duration,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND TimePlayed<>'' ORDER BY TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetDurationMostPlayed, "SELECT Duration,PlayCount,TimePlayed FROM :dbname.Music WHERE Deleted=0 AND PlayCount<>0 ORDER BY PlayCount DESC,TimePlayed DESC LIMIT %u");
_LIT(KQueryMusicGetDurationRecentlyAdded, "SELECT Duration FROM :dbname.Music WHERE Deleted=0 AND (julianday(TimeAdded) > julianday('now','-7 days'))");

// Select songs information from music, playlistsongs and playlistsonginfo via playlistid order by playlistsongs's ordinal
_LIT(KQueryMusicGetAllSongsbyPl,"select mu.UniqueId,mu.DbFlag,mu.VolumeId,mu.Title,mu.Artist,mu.Art,0,mu.Location,pl.UniqueId as PlUId, pl.ordinal from :dbname.music as mu,:pldbname.playlistsongs as pl where mu.Deleted=0 AND mu.uniqueid = pl.songid and pl.playlistid =%u ORDER BY 10");  // deprecated, not maintained
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryMusicGetAllSongsInfobyPl,"SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name,pl.UniqueId as PlUId,pl.ordinal FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer,:pldbname.playlistsongs as pl WHERE Music.Deleted=0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId AND Music.UniqueId=pl .songid and pl .PlaylistId=%u ORDER BY 40");
#else
_LIT(KQueryMusicGetAllSongsInfobyPl,"SELECT Music.*,Artist.Name,Album.Name,Genre.Name,Composer.Name,pl.UniqueId as PlUId,pl.ordinal FROM :dbname.Music,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer,:pldbname.playlistsongs as pl WHERE Music.Deleted=0 AND Music.Album=Album.UniqueId AND Music.Artist=Artist.UniqueId AND Music.Genre=Genre.UniqueId AND Music.Composer=Composer.UniqueId AND Music.UniqueId=pl .songid and pl .PlaylistId=%u ORDER BY 37");
#endif
_LIT(KQuerySongsInArtistAlbum, "SELECT count(*) FROM :dbname.Music WHERE Deleted=0 AND Artist=%u AND Album=%u");
_LIT(KQueryMusicGetArtistForAlbum, "SELECT Artist FROM :dbname.Music,:dbname.Artist WHERE Music.Deleted=0 AND Music.Artist=Artist.UniqueId AND Music.Album=%u");
_LIT(KQueryMusicGetAlbumsforArtist, "SELECT Album.* FROM :dbname.Album,:dbname.Music WHERE Music.Deleted=0 AND Music.Artist=%u AND Album.UniqueId=Music.Album ORDER BY Name, UniqueId");
_LIT(KQueryMusicGetAlbumartForAlbum, "SELECT Art FROM :dbname.Music WHERE Music.Deleted=0 AND Music.Album=%u");
// == Category table queries ==================================================
//
_LIT(KCategoryTablePlaceholder, "##table##");

_LIT(KCategoryCreateTable,"CREATE TABLE ##table##("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Name TEXT COLLATE NOCASE,"
    L"SongCount INTEGER)");

_LIT(KCategoryDropTable,"DROP TABLE ##table##");
_LIT(KCategoryCheckTable,"SELECT UniqueId,Name,SongCount FROM ##table##");
_LIT(KCategoryNameIndex,"CREATE INDEX Index##table##Name ON ##table##(Name)");

_LIT(KQueryCategoryInsert, "INSERT INTO :dbname.##table##(UniqueId,Name,SongCount) VALUES(%u,'%S',%u)");
_LIT(KQueryCategoryDelete, "DELETE FROM :dbname.##table## WHERE UniqueId=%u");

_LIT(KQueryCategoryCount, "SELECT count(*) FROM :dbname.##table##");
_LIT(KQueryCategoryItem, "SELECT * FROM :dbname.##table## WHERE UniqueId=%u");
_LIT(KQueryCategoryItemBySongId, "SELECT ##table##.* FROM :dbname.##table##,:dbname.Music WHERE Music.Deleted=0 AND Music.UniqueId=%u AND ##table##.UniqueId=Music.%S");
_LIT(KQueryCategoryAll, "SELECT * FROM :dbname.##table## ORDER BY Name");
_LIT(KQueryCategoryItems, "SELECT * FROM :dbname.##table## WHERE %S ORDER BY Name ASC");
_LIT(KQueryCategorySongCount, "SELECT SongCount FROM :dbname.##table## WHERE UniqueId=%u");

_LIT(KQueryCategoryIncrementSongCount, "UPDATE :dbname.##table## SET SongCount=SongCount+1 WHERE UniqueId=%u");
_LIT(KQueryCategoryDecrementSongCount, "UPDATE :dbname.##table## SET SongCount=SongCount-1 WHERE UniqueId=%u");
_LIT(KQueryCategoryGetSongCount, "SELECT SongCount FROM :dbname.##table## WHERE UniqueId=%u");

// == Auxiliary table queries =================================================
//
_LIT(KAuxiliaryCreateTable, "CREATE TABLE Auxiliary("
    L"Id INTEGER,"
    L"Version TEXT,"
    L"TimeRefreshed TEXT,"
    L"TimeSynced TEXT,"
    L"Corrupt INTEGER DEFAULT 0,"
    L"SaveDeletedRecordCount INTEGER DEFAULT 0)");
_LIT(KAuxiliaryDropTable,"DROP TABLE Auxiliary");
_LIT(KAuxiliaryCheckTable, "SELECT Id,Version,TimeRefreshed,TimeSynced,Corrupt,SaveDeletedRecordCount FROM AUXILIARY");
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryAuxiliaryInsert, "INSERT INTO Auxiliary(Id,Version,Corrupt) VALUES(0,'6.8.0',%u)");
#else
_LIT(KQueryAuxiliaryInsert, "INSERT INTO Auxiliary(Id,Version,Corrupt) VALUES(0,'6.8.1',%u)");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
_LIT(KQueryAuxiliarySetTime, "UPDATE :dbname.Auxiliary SET TimeRefreshed='%S', Corrupt=0");
_LIT(KQueryAuxiliaryGetTime, "SELECT TimeRefreshed FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetCorrupt, "UPDATE :dbname.Auxiliary SET Corrupt=%u");
_LIT(KQueryAuxiliaryGetCorrupt, "SELECT Corrupt FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliaryGetCount, "SELECT SaveDeletedRecordCount FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetCount, "UPDATE :dbname.Auxiliary SET SaveDeletedRecordCount=%u");
_LIT(KQueryAuxiliaryGetId, "SELECT Id FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetId, "UPDATE :dbname.Auxiliary SET Id=%u");

// == Playlist table queries ==================================================
//
_LIT(KPlaylistCreateTable, "CREATE TABLE Playlist("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"VolumeId INTEGER,"
    L"DbFlag INTEGER,"
    L"Sync INTEGER,"
    L"Name TEXT COLLATE NOCASE,"
    L"Uri TEXT,"
    L"Time TEXT)");
_LIT(KPlaylistDropTable,"DROP TABLE Playlist");
_LIT(KPlaylistCheckTable, "SELECT UniqueId,Name,Uri,VolumeId,Sync,DbFlag,Time FROM Playlist");
_LIT(KPlaylistNameIndex,"CREATE INDEX IndexPlaylistName ON Playlist(Name)");

_LIT(KQueryPlaylistInsert, "INSERT INTO :dbname.Playlist(UniqueId,Name,Uri,VolumeId,Sync,DbFlag,Time) VALUES(%u,'%S','%S',%u,0,%u,'%S')");
_LIT(KQueryPlaylistUpdate, "UPDATE :dbname.Playlist SET %S WHERE UniqueId=%u");
_LIT(KQueryPlaylistUpdateTime, "UPDATE :dbname.Playlist SET Time='%S' WHERE UniqueId=%u");
_LIT(KQueryPlaylistUpdateTimeForSong, "UPDATE :dbname.Playlist SET Time='%S' WHERE UniqueId IN (SELECT DISTINCT PlaylistId FROM :dbname.PlaylistSongs WHERE SongId=%u)");
_LIT(KQueryPlaylistDeleteAll, "DELETE FROM :dbname.Playlist");
_LIT(KQueryPlaylistDelete, "DELETE FROM :dbname.Playlist WHERE UniqueId=%u");

_LIT(KQueryPlaylistCount, "SELECT count(*) FROM :dbname.Playlist");
_LIT(KQueryPlaylistGetAll, "SELECT * FROM :dbname.Playlist ORDER BY Name ASC");
_LIT(KQueryPlaylistGet, "SELECT * FROM :dbname.Playlist WHERE UniqueId=%u");
_LIT(KQueryPlaylistDuration, "SELECT sum(Music.Duration) FROM :dbname.Music,:dbname.Playlist,:dbname.PlaylistSongs WHERE Music.UniqueId=PlaylistSongs.SongId AND PlaylistSongs.PlaylistId=%u");
_LIT(KQueryPlaylistId, "SELECT UniqueId FROM :dbname.Playlist WHERE Uri='%S'");
_LIT(KQueryLikePlaylistId, "SELECT UniqueId FROM :dbname.Playlist WHERE Uri LIKE '%%%%%S%%%%' ESCAPE '\\'");
_LIT(KQueryPlaylistDriveId, "SELECT VolumeId FROM :dbname.Playlist WHERE UniqueId=%u");
_LIT(KQueryPlaylistItems, "SELECT * FROM :dbname.Playlist WHERE %S ORDER BY Name ASC");
_LIT(KQueryPlaylistGetForSong, "SELECT DISTINCT PlaylistId FROM :dbname.PlaylistSongs WHERE SongId=%u");
_LIT(KQueryPlaylistFileCount, "SELECT count(*) FROM :dbname.Playlist WHERE Uri LIKE '%%.m3u'");
//see "Efficient Use of Symbinan SQL How-To Document" for details about following queries:
_LIT(KQueryPlaylistGetFileUris, "SELECT UniqueId,Uri FROM :dbname.Playlist WHERE Uri LIKE '%%%%.m3u' ORDER BY UniqueId ASC LIMIT %u");
_LIT(KQueryPlaylistGetFileUrisFrom, "SELECT UniqueId,Uri FROM :dbname.Playlist WHERE Uri LIKE '%%%%.m3u' AND UniqueId > %u ORDER BY UniqueId ASC LIMIT %u");

// == PlaylistSongs table queries =============================================
//
_LIT(KPlaylistSongsCreateTable, "CREATE TABLE PlaylistSongs("
    L"UniqueId INTEGER PRIMARY KEY AUTOINCREMENT,"
    L"SongId INTEGER,"
    L"PlaylistId INTEGER,"
    L"Ordinal INTEGER)");
_LIT(KPlaylistSongsDropTable,"DROP TABLE PlaylistSongs");
_LIT(KPlaylistSongsCheckTable, "SELECT UniqueId,SongId,PlaylistId,Ordinal FROM PlaylistSongs");

_LIT(KQueryPlaylistSongsInsert, "INSERT INTO :dbname.PlaylistSongs(PlaylistId,SongId,Ordinal) VALUES(%u,%u,%u)");
_LIT(KQueryPlaylistSongsUpdate, "UPDATE :dbname.PlaylistSongs SET SongId=%u WHERE SongId=%u");
_LIT(KQueryPlaylistSongsUpdatePlaylistId, "UPDATE :dbname.PlaylistSongs SET PlaylistId=%u WHERE PlaylistId=%u");
_LIT(KQueryPlaylistSongsDelete, "DELETE FROM :dbname.PlaylistSongs WHERE SongId=%u");
_LIT(KQueryPlaylistSongsDeleteAll, "DELETE FROM :dbname.PlaylistSongs");
_LIT(KQueryPlaylistSongsDeletePlaylist, "DELETE FROM :dbname.PlaylistSongs WHERE PlaylistId=%u");
_LIT(KQueryPlaylistSongsDeleteSong, "DELETE FROM :dbname.PlaylistSongs WHERE PlaylistId=%u AND SongId=%u");
_LIT(KQueryPlaylistSongsDeleteSongOrdinal, "DELETE FROM :dbname.PlaylistSongs WHERE PlaylistId=%u AND SongId=%u AND Ordinal=%u");
_LIT(KQueryPlaylistSongsDeleteForCategory, "DELETE FROM :dbname.PlaylistSongs WHERE PlaylistSongs.SongId=Music.UniqueId AND Music.%S=%u");
_LIT(KQueryPlaylistSongsDeleteForArtistAlbum, "DELETE FROM :dbname.PlaylistSongs WHERE PlaylistSongs.SongId=Music.UniqueId AND Music.Artist=%u AND Music.Album=%u");

_LIT(KQueryPlaylistSongsCount, "SELECT count(*) FROM :dbname.music as m,:pldbname.Playlistsongs as pl WHERE pl.PlaylistId=%u AND m.UniqueId=pl.SongId AND m.Deleted=0");
_LIT(KQueryPlaylistSongsPlaylistSongCount, "SELECT count(*) FROM :dbname.PlaylistSongs WHERE PlaylistId=%u AND SongId=%u");
_LIT(KQueryPlaylistSongsSongCount, "SELECT count(*) FROM :dbname.PlaylistSongs WHERE SongId=%u");
_LIT(KQueryPlaylistSongsGetSongs, "SELECT UniqueId,SongId,Ordinal FROM :dbname.PlaylistSongs WHERE PlaylistId=%u ORDER BY Ordinal");

// LTAN-79N8ND: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
//_LIT(KQueryPlaylistSongsGetSong, "SELECT PlaylistSongs.UniqueId,PlaylistSongs.SongId,PlaylistSongs.PlaylistId,PlaylistSongs.Ordinal,PlaylistSongInfo.VolumeId,PlaylistSongInfo.DbFlag,PlaylistSongInfo.Uri,PlaylistSongInfo.Title FROM :dbname.PlaylistSongs,:dbname.PlaylistSongInfo WHERE PlaylistSongs.PlaylistId=%u AND PlaylistSongs.SongId=%u AND PlaylistSongs.SongId=PlaylistSongInfo.SongId ORDER BY PlaylistSongs.Ordinal");
_LIT(KQueryPlaylistSongsGetSong, "SELECT PlaylistSongs.UniqueId,PlaylistSongs.SongId,PlaylistSongs.PlaylistId,PlaylistSongs.Ordinal,PlaylistSongInfo.VolumeId,PlaylistSongInfo.DbFlag,PlaylistSongInfo.Uri,PlaylistSongInfo.Title FROM :dbname.PlaylistSongs,:dbname.PlaylistSongInfo WHERE PlaylistSongs.PlaylistId=%u AND PlaylistSongs.SongId=%u AND PlaylistSongs.SongId=PlaylistSongInfo.SongId ORDER BY 4");
_LIT(KQueryPlaylistSongsGetSongsInfo, " SELECT PlaylistSongs.*,PlaylistSongInfo.VolumeId,PlaylistSongInfo.DbFlag,PlaylistSongInfo.Uri,PlaylistSongInfo.Title FROM :dbname.PlaylistSongs,:dbname.PlaylistSongInfo WHERE PlaylistSongs.SongId=PlaylistSongInfo.SongId AND PlaylistSongs.PlaylistId=%u ORDER BY 4");

_LIT(KQueryPlaylistSongsGetSongInfo, "SELECT PlaylistSongs.UniqueId,PlaylistSongs.SongId,PlaylistSongs.PlaylistId,PlaylistSongs.Ordinal,PlaylistSongInfo.VolumeId,PlaylistSongInfo.DbFlag,PlaylistSongInfo.Uri,PlaylistSongInfo.Title FROM :dbname.PlaylistSongs,:dbname.PlaylistSongInfo WHERE PlaylistSongs.UniqueId=%u AND PlaylistSongs.SongId=%u AND PlaylistSongs.SongId=PlaylistSongInfo.SongId");

_LIT(KQueryPlaylistSongsUpdateSongOrdinalAfterDelete, "UPDATE :dbname.PlaylistSongs SET Ordinal=Ordinal-1 WHERE PlaylistId=%u AND Ordinal>%u");
_LIT(KQueryPlaylistSongsUpdateSongOrdinal, "UPDATE :dbname.PlaylistSongs SET Ordinal=%u WHERE UniqueId=%u");
_LIT(KQueryPlaylistSongsMoveSongUp, "UPDATE :dbname.PlaylistSongs SET Ordinal=Ordinal+1 WHERE PlaylistId=%u AND Ordinal>=%u AND Ordinal<%u");
_LIT(KQueryPlaylistSongsMoveSongDown, "UPDATE :dbname.PlaylistSongs SET Ordinal=Ordinal-1 WHERE PlaylistId=%u AND Ordinal<=%u AND Ordinal>%u");

// == PlaylistSongInfo table queries ==========================================
//
_LIT(KPlaylistSongInfoCreateTable, "CREATE TABLE PlaylistSongInfo("
    L"SongId INTEGER PRIMARY KEY,"
    L"VolumeId INTEGER,"
    L"DbFlag INTEGER,"
    L"Uri TEXT,"
    L"Title TEXT COLLATE NOCASE)");
_LIT(KPlaylistSongInfoDropTable,"DROP TABLE PlaylistSongInfo");
_LIT(KPlaylistSongInfoCheckTable, "SELECT SongId,VolumeId,DbFlag,Uri,Title FROM PlaylistSongInfo");

_LIT(KQueryPlaylistSongInfoInsert, "INSERT INTO :dbname.PlaylistSongInfo(SongId,Uri,Title,VolumeId,DbFlag) VALUES(%u,'%S','%S',%u,%u)");
_LIT(KQueryPlaylistSongInfoDelete, "DELETE FROM :dbname.PlaylistSongInfo WHERE SongId=%u");
_LIT(KQueryPlaylistSongInfoDeleteAll, "DELETE FROM :dbname.PlaylistSongInfo");
_LIT(KQueryPlaylistSongInfoCleanup, "DELETE FROM :dbname.PlaylistSongInfo WHERE SongId NOT IN (SELECT DISTINCT SongId FROM :dbname.PlaylistSongs)");
_LIT(KQueryPlaylistSongInfoUpdateTitle, "UPDATE :dbname.PlaylistSongInfo SET Title='%S' WHERE SongId=%u");
_LIT(KQueryPlaylistSongInfoUpdate, "UPDATE :dbname.PlaylistSongInfo SET SongId=%u,Uri='%S',VolumeId=%u WHERE SongId=%u");
_LIT(KQueryPlaylistSongInfoUpdateFlags, "UPDATE :dbname.PlaylistSongInfo SET DbFlag=%u WHERE SongId=%u");

_LIT(KQueryPlaylistSongInfoExists, "SELECT SongId FROM :dbname.PlaylistSongInfo WHERE SongId=%u");
_LIT(KQueryPlaylistSongInfoGet, "SELECT * FROM :dbname.PlaylistSongInfo WHERE SongId=%u");

// == Criteria strings ========================================================
//
// Music criteria
_LIT(KCriterionMusicUniqueId, "Music.UniqueId=%u");
_LIT(KCriterionMusicNotDeleted, "Music.Deleted=0");
_LIT(KCriterionMusicArtist, "Music.artist=%u");
_LIT(KCriterionMusicAlbum, "Music.album=%u");
_LIT(KCriterionMusicGenre, "Music.genre=%u");
_LIT(KCriterionMusicComposer, "Music.composer=%u");
// SQLite requires LIKE statement to include an ESCAPE clause if an escaped percentage
// or underscore is to be used as regular search text instead of pattern matching
// characters. See http://www.sqlite.org/lang_expr.html for an explanation. To supprt
// partial title matching, our goal is to have the following statement as an example:
//      SELECT * FROM Music WHERE title LIKE '%xyz%' ESCAPE '\'
// To achieve this, we specify the format to be %%%%%S%%%%. The 1st percentage is the
// escape character for string formatter to output the 2nd percentage sign. Similiarly
// for the 3rd, 4th, 6th, 7th, 8th, and 9th. The 5th percentage instructs string formatter
// to substitute a string, i.e. %S. After subsititution, we will have, for example, %%xyz%%.
// Because this substituted string will be substituted again into a WHERE clause, we need
// to escape the percentage signs in order to get %xyz% as the end result.
_LIT(KCriterionMusicTitle, "Music.title LIKE '%%%%%S%%%%' ESCAPE '\\'");
_LIT(KCriterionMusicVolume, "Music.VolumeId=%u");
_LIT(KCriterionMusicLocation, "Music.Location='%S'");
_LIT(KCriterionMusicSync, "Music.Sync=%u");
_LIT(KCriterionMusicDeleted, "Music.Deleted=%u");
_LIT(KCriterionMusicModified, "Music.Modified=%u");

// Category criteria
_LIT(KCriterionCategoryUniqueId, "##table##.UniqueId=%u");
_LIT(KCriterionCategoryName, "##table##.Name LIKE '%%%%%S%%%%' ESCAPE '\\'");

// Playlist criteria
_LIT(KCriterionPlaylistUniqueId, "UniqueId=%u");
_LIT(KCriterionPlaylistVolumeId, "VolumeId=%u");
_LIT(KCriterionPlaylistTitle, "Name='%S'");
_LIT(KCriterionPlaylistLikeTitle, "Name LIKE '%%%%%S%%%%' ESCAPE '\\'");
_LIT(KCriterionPlaylistUri, "Uri='%S'");
_LIT(KCriterionPlaylistSync, "Sync=%u");
_LIT(KCriterionPlaylistTime, "Time='%S'");

// PlaylistSongs criteria
_LIT(KCriterionPlaylistSongId, "PlaylistSongs.SongId=%u");

// == Album table queries ==========================================
//
_LIT(KAlbumCreateTable,"CREATE TABLE Album("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Name TEXT COLLATE NOCASE,"
    L"SongCount INTEGER,"
    L"Artist INTEGER,"
    L"ArtistName TEXT COLLATE NOCASE,"
    L"Art TEXT)");

_LIT(KAlbumCheckTable,"SELECT UniqueId,Name,SongCount,Artist,ArtistName,Art FROM Album");

_LIT(KQueryAlbumInsert, "INSERT INTO :dbname.Album(UniqueId,Name,SongCount,Artist,ArtistName,Art) VALUES(%u,'%S',%u,%u,'%S','%S')");
_LIT(KQueryAlbumUpdate, "UPDATE :dbname.Album SET %S WHERE UniqueId=%u");

_LIT(KQueryAlbumAll, "SELECT Album.* FROM :dbname.Album ORDER BY Name, ArtistName");
_LIT(KQueryAlbumAllMediaWall, "SELECT Album.* FROM :dbname.Album ORDER BY ArtistName, Name");
_LIT(KCriterionArtistName, "ArtistName='%S'");
_LIT(KCriterionArt, "Art='%S'");

// == Artist table queries ==========================================
//
_LIT(KArtistCreateTable,"CREATE TABLE Artist("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Name TEXT COLLATE NOCASE,"
    L"SongCount INTEGER,"
    L"AlbumCount INTEGER,"
    L"Art TEXT)");

_LIT(KArtistCheckTable,"SELECT UniqueId,Name,SongCount,AlbumCount,Art FROM Artist");

_LIT(KQueryArtistInsert, "INSERT INTO :dbname.Artist(UniqueId,Name,SongCount,AlbumCount,Art) VALUES(%u,'%S',%u,%u,'%S')");
_LIT(KQueryArtistUpdate, "UPDATE :dbname.Artist SET %S WHERE UniqueId=%u");

_LIT(KQueryArtistAll, "SELECT Artist.* FROM :dbname.Artist WHERE SongCount > 0 ORDER BY 2");
_LIT(KQueryArtistName, "SELECT Artist.Name FROM :dbname.Artist WHERE UniqueId=%u");

_LIT(KQueryArtistIncrementAlbumCount, "UPDATE :dbname.Artist SET AlbumCount=AlbumCount+1 WHERE UniqueId=%u");
_LIT(KQueryArtistDecrementAlbumCount, "UPDATE :dbname.Artist SET AlbumCount=AlbumCount-1 WHERE UniqueId=%u");
_LIT(KQueryArtistGetSongCountAndAlbumCount, "SELECT SongCount,AlbumCount FROM :dbname.Artist WHERE UniqueId=%u");
_LIT(KQueryArtistItems, "SELECT * FROM :dbname.Artist WHERE %S AND SongCount > 0 ORDER BY Name ASC");

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
// == AbstractAlbum table queries ==========================================
//
_LIT(KAbstractAlbumCreateTable,"CREATE TABLE AbstractAlbum("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Uri TEXT,"
    L"Name TEXT COLLATE NOCASE,"
    L"AlbumArtist TEXT,"
    L"SongCount INTEGER,"
    L"VolumeId INTEGER)");

_LIT(KAbstractAlbumCheckTable,"SELECT UniqueId,Uri,Name,AlbumArtist,SongCount,VolumeId FROM AbstractAlbum");
_LIT(KCriterionAbstractAlbumVolumeId, "VolumeId=%u");
_LIT(KQueryAbstractAlbumInsert, "INSERT INTO :dbname.AbstractAlbum(UniqueId,Uri,Name,AlbumArtist,SongCount,VolumeId) VALUES(%u,'%S','%S','%S',%u,%u)");
_LIT(KQueryAbstractAlbumUpdate, "UPDATE :dbname.AbstractAlbum SET %S WHERE UniqueId=%u");
_LIT(KCriterionCategoryVolumeId, "SELECT VolumeId FROM :dbname.AbstractAlbum WHERE UniqueId=%u");
_LIT(KQueryAbstractAlbumsWithNoSong, "SELECT * FROM :dbname.AbstractAlbum WHERE SongCount=0");
#endif // ABSTRACTAUDIOALBUM_INCLUDED
#endif // MPXDBPLUGINQUERIES_H
