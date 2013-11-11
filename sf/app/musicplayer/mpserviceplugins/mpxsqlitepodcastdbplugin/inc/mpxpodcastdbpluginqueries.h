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
* Description:  Hard coded queries for Podcast collection db
*
*/


#ifndef MPXPODCASTDBPLUGINQUERIES_H
#define MPXPODCASTDBPLUGINQUERIES_H

// Various Collection view duration queries
_LIT(KPodcastCreateTable, "CREATE TABLE Podcast("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Deleted INTEGER DEFAULT 0,"
    L"DbFlag INTEGER,"
    L"VolumeId INTEGER,"
    L"IsPlaying INTEGER DEFAULT 0,"
    L"Title TEXT COLLATE NOCASE,"
    L"Location TEXT,"
    L"PublishDate TEXT DEFAULT '',"
    L"AlbumTrack INTEGER,"
    L"PlayCount INTEGER DEFAULT 0,"
    L"TimeAdded TEXT,"
    L"TimePlayed TEXT DEFAULT '',"
    L"Duration INTEGER,"
    L"Sync INTEGER DEFAULT 0,"
    L"Modified INTEGER DEFAULT 0,"
    L"Artist INTEGER,"
    L"Album INTEGER,"
    L"Genre INTEGER,"
    L"Composer INTEGER,"
    L"ReleaseDate TEXT DEFAULT '',"
    L"Rating INTEGER,"
    L"Comment TEXT,"
    L"Copyright TEXT,"
    L"Art TEXT,"
    L"Url TEXT,"
    L"DRM INTEGER,"
    L"LastPlayPosition INTEGER DEFAULT 0,"
    L"SampleRate INTEGER,"
    L"BitRate INTEGER,"
    L"NumChannels INTEGER,"
    L"Codec INTEGER,"
    L"MimeType TEXT)");

_LIT(KPodcastDropTable,"DROP TABLE Podcast");
_LIT(KPodcastCheckTable,"SELECT UniqueId,Deleted,DbFlag,VolumeId,IsPlaying,Title,Location,PublishDate,AlbumTrack,PlayCount,TimeAdded,TimePlayed,Duration,Sync,Modified,Artist,Album,Genre,Composer,ReleaseDate,Rating,Comment,Copyright,Art,Url,DRM,LastPlayPosition,SampleRate,BitRate,NumChannels,Codec,MimeType FROM Podcast");

_LIT(KQueryPodcastInsert, "INSERT INTO :dbname.Podcast(%S) VALUES(%S)");
_LIT(KQueryPodcastUpdate, "UPDATE :dbname.Podcast SET %S WHERE UniqueId=%u");
_LIT(KQueryPodcastSetIsPlaying, "UPDATE :dbname.Podcast SET IsPlaying=%u WHERE UniqueId=%u");
_LIT(KQueryPodcastDelete, "DELETE FROM :dbname.Podcast WHERE UniqueId=%u");
_LIT(KQueryPodcastDeleteUpdate, "UPDATE :dbname.Podcast SET Deleted=1 WHERE UniqueId=%u");
_LIT(KQueryPodcastDeleteForCategory, "UPDATE :dbname.Podcast SET Deleted=0 WHERE %S=%u");
_LIT(KQueryPodcastCleanup, "DELETE FROM :dbname.Podcast WHERE Deleted=1");

_LIT(KQueryPodcastGetTitle, "SELECT Title FROM :dbname.Podcast WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryPodcastVolume, "SELECT VolumeId FROM :dbname.Podcast WHERE UniqueId=%u");
_LIT(KQueryPodcastAllVolumeIds, "SELECT VolumeId FROM :dbname.Podcast WHERE Deleted=0");

// LTAN-79N8ND/EVXG-7FABHC: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
/*
_LIT(KQueryPodcastFindAll, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE %S AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.Title");
*/
_LIT(KQueryPodcastFindAll, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE %S AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 6");

_LIT(KQueryPodcastAlbum, "SELECT DISTINCT Album FROM :dbname.Podcast WHERE Deleted=0 AND Artist=%u ORDER BY Album");
_LIT(KQueryPodcastInfo, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryPodcastCategory, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND %S=%u");
_LIT(KQueryPodcastCategoryOrdered, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND %S=%u ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastGetUris, "SELECT UniqueId,Location FROM :dbname.Podcast WHERE Deleted=0 ORDER BY UniqueId ASC LIMIT %u");
_LIT(KQueryPodcastGetUrisFrom, "SELECT UniqueId,Location FROM :dbname.Podcast WHERE Deleted=0 AND UniqueId > %u ORDER BY UniqueId ASC LIMIT %u");

// Note: The no categories queries below only retrieve five fields as opposed to * from the Music table.
//       This is an optimization that matches the DBMS version of the plugin and assumes that the client
//       only requests those fields. From the C++ code perspective there is no difference in processing
//       as the five fields in question reside at the beginning of the table. If the client is modified
//       to request more attributes then the queries below will have to be changed to return *.
//       As far as performance goes, the queries that only return five fields are a little faster than
//       the ones that return *, for example, for a * query that takes 2.1 seconds the equivalent
//       optimized query is faster by more than 100ms.

// categories included
_LIT(KQueryPodcastGetEpisode, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.UniqueId=%u AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId");

// LTAN-79N8ND/EVXG-7FABHC: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
/*
_LIT(KQueryPodcastGetAllEpisodes, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.Title");
*/
_LIT(KQueryPodcastGetAllEpisodes, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 6");

_LIT(KQueryPodcastGetEpisodesForAlbum, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND Podcast.Album=%u AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastNotPlayed, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND Podcast.PlayCount=0 AND Podcast.LastPlayPosition=0 AND Podcast.IsPlaying=0 AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastRecentlyAdded, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.TimeAdded)>=julianday('now','start of day','-7 days') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY TimeAdded DESC,Title");

// LTAN-79N8ND/EVXG-7FABHC: temporary fix.  Symbian 9.4 SQLite cannot resolve column number correctly,
// Use column number instead of column name for ORDER BY, needed when FROM uses more than 1 table
// calculate column number from SELECT fields, first field is column 1 (not 0)
/*
_LIT(KQueryPodcastToday, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of day') AND julianday(Podcast.PublishDate)<julianday('%S','start of day','+1 days') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.PublishDate DESC,Podcast.Title");
_LIT(KQueryPodcastYesterday, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of day','-1 days') AND julianday(Podcast.PublishDate)<julianday('%S','start of day') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.PublishDate DESC,Podcast.Title");
_LIT(KQueryPodcastBetween, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>julianday('%S','start of day','-%u days') AND julianday(Podcast.PublishDate)<=julianday('%S','start of day','-%u days') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.PublishDate DESC,Podcast.Title");
_LIT(KQueryPodcastLastMonth, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of month', '-1 month') AND julianday(Podcast.PublishDate)<julianday('%S','start of month') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.PublishDate DESC,Podcast.Title");
_LIT(KQueryPodcastEarlier, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)<julianday('%S','start of month', '-1 month') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.PublishDate DESC,Podcast.Title");
_LIT(KQueryPodcastUnknown, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND (Podcast.PublishDate='' OR julianday(Podcast.PublishDate)>=julianday('%S','start of day','+1 days')) AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY Podcast.Title");
*/
_LIT(KQueryPodcastToday, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of day') AND julianday(Podcast.PublishDate)<julianday('%S','start of day','+1 days') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 8 DESC, 6");
_LIT(KQueryPodcastYesterday, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of day','-1 days') AND julianday(Podcast.PublishDate)<julianday('%S','start of day') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 8 DESC, 6");
_LIT(KQueryPodcastBetween, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>julianday('%S','start of day','-%u days') AND julianday(Podcast.PublishDate)<=julianday('%S','start of day','-%u days') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 8 DESC, 6");
_LIT(KQueryPodcastLastMonth, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)>=julianday('%S','start of month', '-1 month') AND julianday(Podcast.PublishDate)<julianday('%S','start of month') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 8 DESC, 6");
_LIT(KQueryPodcastEarlier, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND julianday(Podcast.PublishDate)<julianday('%S','start of month', '-1 month') AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 8 DESC, 6");
_LIT(KQueryPodcastUnknown, "SELECT Podcast.*,Artist.Name,Album.Name,Genre.Name,Composer.Name FROM :dbname.Podcast,:dbname.Artist,:dbname.Album,:dbname.Genre,:dbname.Composer WHERE Podcast.Deleted=0 AND (Podcast.PublishDate='' OR julianday(Podcast.PublishDate)>=julianday('%S','start of day','+1 days')) AND Podcast.Album=Album.UniqueId AND Podcast.Artist=Artist.UniqueId AND Podcast.Genre=Genre.UniqueId AND Podcast.Composer=Composer.UniqueId ORDER BY 6");

// no categories queries
_LIT(KQueryPodcastGetEpisodeNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryPodcastGetAllEpisodesNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 ORDER BY Title");
_LIT(KQueryPodcastGetEpisodesForAlbumNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND Album=%u ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastNotPlayedNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND PlayCount=0 AND LastPlayPosition=0 AND IsPlaying=0 ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastRecentlyAddedNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(TimeAdded)>=julianday('now','start of day','-7 days') ORDER BY TimeAdded DESC,Title");
_LIT(KQueryPodcastTodayNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of day') AND julianday(PublishDate)<julianday('%S','start of day','+1 days') ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastYesterdayNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of day','-1 days') AND julianday(PublishDate)<julianday('%S','start of day') ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastBetweenNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>julianday('%S','start of day','-%u days') AND julianday(PublishDate)<=julianday('%S','start of day','-%u days') ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastLastMonthNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of month', '-1 month') AND julianday(PublishDate)<julianday('%S','start of month') ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastEarlierNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)<julianday('%S','start of month', '-1 month') ORDER BY PublishDate DESC,Title");
_LIT(KQueryPodcastUnknownNoCat, "SELECT * FROM :dbname.Podcast WHERE Deleted=0 AND (PublishDate='' OR julianday(PublishDate)>=julianday('%S','start of day','+1 days')) ORDER BY Title");

// count queries
_LIT(KQueryPodcastCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0");
_LIT(KQueryPodcastTodayCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of day') AND julianday(PublishDate)<julianday('%S','start of day','+1 days')");
_LIT(KQueryPodcastYesterdayCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of day','-1 days') AND julianday(PublishDate)<julianday('%S','start of day')");
_LIT(KQueryPodcastBetweenCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>julianday('%S','start of day','-%u days') AND julianday(PublishDate)<=julianday('%S','start of day','-%u days')");
_LIT(KQueryPodcastLastMonthCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)>=julianday('%S','start of month', '-1 month') AND julianday(PublishDate)<julianday('%S','start of month')");
_LIT(KQueryPodcastEarlierCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(PublishDate)<julianday('%S','start of month', '-1 month')");
_LIT(KQueryPodcastUnknownCount, "SELECT count(*) FROM :dbname.Podcast WHERE (PublishDate='' OR julianday(PublishDate)>=julianday('%S','start of day','+1 days')) AND Deleted=0");
_LIT(KQueryPodcastRecentlyAddedCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND julianday(TimeAdded)>=julianday('now','start of day','-7 days')");
_LIT(KQueryPodcastNotPlayedCount, "SELECT count(*) FROM :dbname.Podcast WHERE Deleted=0 AND PlayCount=0 AND LastPlayPosition=0 AND IsPlaying=0");

// duration queries
_LIT(KQueryPodcastEpisodeDuration, "SELECT Duration FROM :dbname.Podcast WHERE Deleted=0 AND UniqueId=%u");
_LIT(KQueryPodcastDurationAll, "SELECT sum(Duration) FROM :dbname.Podcast WHERE Deleted=0");
_LIT(KQueryPodcastDurationAlbum, "SELECT sum(Duration) FROM :dbname.Podcast WHERE Deleted=0 AND Album=%u");
_LIT(KQueryPodcastNotPlayedDuration, "SELECT sum(Duration) FROM :dbname.Podcast WHERE Deleted=0 AND PlayCount=0");
_LIT(KQueryPodcastRecentlyAddedDuration, "SELECT Duration FROM :dbname.Podcast WHERE Deleted=0 AND julianday(TimeAdded)>julianday('now','-7 days')");

// == Category table queries ==================================================
//
_LIT(KCategoryTablePlaceholder, "##table##");

_LIT(KCategoryCreateTable,"CREATE TABLE ##table##("
    L"UniqueId INTEGER PRIMARY KEY,"
    L"Name TEXT COLLATE NOCASE,"
    L"EpisodeCount INTEGER)");

_LIT(KCategoryDropTable,"DROP TABLE ##table##");
_LIT(KCategoryCheckTable,"SELECT UniqueId,Name,EpisodeCount FROM ##table##");
_LIT(KCategoryNameIndex,"CREATE INDEX Index##table##Name ON ##table##(Name)");

_LIT(KQueryCategoryInsert, "INSERT INTO :dbname.##table##(UniqueId,Name,EpisodeCount) VALUES(%u,'%S',%u)");
_LIT(KQueryCategoryDelete, "DELETE FROM :dbname.##table## WHERE UniqueId=%u");

_LIT(KQueryCategoryCount, "SELECT count(*) FROM :dbname.##table##");
_LIT(KQueryCategoryItem, "SELECT * FROM :dbname.##table## WHERE UniqueId=%u");
_LIT(KQueryCategoryItemByEpisodeId, "SELECT ##table##.* FROM :dbname.##table##,:dbname.Podcast WHERE Podcast.Deleted=0 AND Podcast.UniqueId=%u AND ##table##.UniqueId=Podcast.%S");
_LIT(KQueryCategoryAll, "SELECT * FROM :dbname.##table## ORDER BY Name");
_LIT(KQueryCategoryItems, "SELECT * FROM :dbname.##table## WHERE %S ORDER BY Name ASC");

_LIT(KQueryCategoryIncrementEpisodeCount, "UPDATE :dbname.##table## SET EpisodeCount=EpisodeCount+1 WHERE UniqueId=%u");
_LIT(KQueryCategoryDecrementEpisodeCount, "UPDATE :dbname.##table## SET EpisodeCount=EpisodeCount-1 WHERE UniqueId=%u");
_LIT(KQueryCategoryGetEpisodeCount, "SELECT EpisodeCount FROM :dbname.##table## WHERE UniqueId=%u");

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
_LIT(KQueryAuxiliaryInsert, "INSERT INTO Auxiliary(Id,Version,Corrupt) VALUES(0,'6.0.0',%u)");

_LIT(KQueryAuxiliarySetTime, "UPDATE :dbname.Auxiliary SET TimeRefreshed='%S', Corrupt=0");
_LIT(KQueryAuxiliaryGetTime, "SELECT TimeRefreshed FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetCorrupt, "UPDATE :dbname.Auxiliary SET Corrupt=%u");
_LIT(KQueryAuxiliaryGetCorrupt, "SELECT Corrupt FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliaryGetCount, "SELECT SaveDeletedRecordCount FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetCount, "UPDATE :dbname.Auxiliary SET SaveDeletedRecordCount=%u");
_LIT(KQueryAuxiliaryGetId, "SELECT Id FROM :dbname.Auxiliary");
_LIT(KQueryAuxiliarySetId, "UPDATE :dbname.Auxiliary SET Id=%u");

// Podcast table criteria
_LIT(KCriterionPodcastNotDeleted, "Podcast.Deleted=0");
_LIT(KCriterionPodcastUniqueId, "Podcast.UniqueId=%u");
_LIT(KCriterionPodcastAuthor, "Podcast.artist=%u");
_LIT(KCriterionPodcastAlbum, "Podcast.album=%u");
_LIT(KCriterionPodcastGenre, "Podcast.genre=%u");
_LIT(KCriterionPodcastComposer, "Podcast.composer=%u");
// SQLite requires LIKE statement to include an ESCAPE clause if an escaped percentage
// or underscore is to be used as regular search text instead of pattern matching
// characters. See http://www.sqlite.org/lang_expr.html for an explanation. To supprt
// partial title matching, our goal is to have the following statement as an example:
//      SELECT * FROM Podcast WHERE title LIKE '%xyz%' ESCAPE '\'
// To achieve this, we specify the format to be %%%%%S%%%%. The 1st percentage is the
// escape character for string formatter to output the 2nd percentage sign. Similiarly
// for the 3rd, 4th, 6th, 7th, 8th, and 9th. The 5th percentage instructs string formatter
// to substitute a string, i.e. %S. After subsititution, we will have, for example, %%xyz%%.
// Because this substituted string will be substituted again into a WHERE clause, we need
// to escape the percentage signs in order to get %xyz% as the end result.
_LIT(KCriterionPodcastTitle, "Podcast.title LIKE '%%%%%S%%%%' ESCAPE '\\'");
_LIT(KCriterionPodcastVolume, "Podcast.VolumeId=%u");
_LIT(KCriterionPodcastUri, "Podcast.Location='%S'");
_LIT(KCriterionPodcastSynchronized, "Podcast.Sync=%u");
_LIT(KCriterionPodcastDeleted, "Podcast.Deleted=%u");
_LIT(KCriterionPodcastModified, "Podcast.Modified=%u");
_LIT(KCriterionPodcastPublishDateToday, "julianday(Podcast.PublishDate)>=julianday('%S','start of day') AND julianday(Podcast.PublishDate)<julianday('%S','start of day','+1 days')");
_LIT(KCriterionPodcastPublishDateYesterday, "julianday(Podcast.PublishDate)>=julianday('%S','start of day','-1 days') AND julianday(Podcast.PublishDate)<julianday('%S','start of day')");
_LIT(KCriterionPodcastPublishDateBetween, "julianday(Podcast.PublishDate)>julianday('%S','start of day','-%u days') AND julianday(Podcast.PublishDate)<=julianday('%S','start of day','-%u days')");
_LIT(KCriterionPodcastPublishDateLastMonth, "julianday(Podcast.PublishDate)>=julianday('%S','start of month', '-1 month') AND julianday(Podcast.PublishDate)<julianday('%S','start of month')");
_LIT(KCriterionPodcastPublishDateEarlier, "julianday(Podcast.PublishDate)<julianday('%S','start of month', '-1 month')");
_LIT(KCriterionPodcastPublishDateUnknown, "(Podcast.PublishDate='' OR julianday(Podcast.PublishDate)>=julianday('%S','start of day','+1 days'))");

// Category table criteria
_LIT(KCriterionCategoryUniqueId, "##table##.UniqueId=%u");
_LIT(KCriterionCategoryName, "##table##.Name LIKE '%%%%%S%%%%' ESCAPE '\\'");

#endif // MPXPODCASTDBPLUGINQUERIES_H

