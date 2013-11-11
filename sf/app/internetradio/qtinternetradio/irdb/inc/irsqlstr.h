/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Description: DDL(Data Definition Language) of Internet Radio Database 
*
*/

#ifndef IRSQLSTR_H_
#define IRSQLSTR_H_

#include <QString>

const QString IRDBName = "IRDB.db";

#ifdef PLATSIM_DEBUG_CONF
    const QString IRDBFile = IRDBName;
#else
    const QString IRDBFile = "c:\\private\\2002FFAC\\IRDB.db";    
#endif



const QString IRDBConnectionName("IR");
const QString IRDBSrhCIdCntFrmChannelInfo("select count(channelId) from channelInfo where channelId =");
const QString IRDBSrhCIdCntFrmFavorites("select count(channelId) from favorites where channelId =");
const QString IRDBSrhCIdFrmChannelInfo("select channelid from IRVIEW_CHANNELINFO ");
const QString IRDBSrhCIdFrmView("select channelId from IRVIEW_ALL ");
const QString IRDBSrhAllFrmView("select * from IRVIEW_ALL ");
const QString IRDBInsertIRBuff("insert into IRBuff (channelid, datasource, opt) values("); 
const QString IRDBSrhCIdCntFrmSongHistory("select count(channelId) from songHistory where songName =");
const QString IRDBSrhCIdCntFrmChannelHistory("select count(channelId) from channelHistory where channelId =");
const QString IRDBSrhCIdMaxFrmChannelInfo("select max(channelId) from channelInfo;");
const QString IRDBDltRowFrmUrlInfoByCId("delete from urlInfo where channelId =");
const QString IRDBSrhUserCid("select channelId from IR_VIEW_SRH_USERCID ");
const QString IRDBSrhRecordCntFrmSongHistory("select count(songName) from songHistory ");
/*
 * notes
 * About two columns in IRBuff table;
 * opt:
 -- opt = 1, insert
 -- opt = 2, update
 -- opt = 3, invoke the actions in update trigger;
 
 * dataSource: 
 -- dataSource = 1, data for channelHistory
 -- dataSource = 2, data for searchRlt
 -- dataSource = other, data dont belong to both channelHistory and searchRlt , 
              come from go to station view or synchronization with iSDS side;

 * trigger recursion
 -- until now the sqlite don't support recursive trigger, 
 -- !!! if in future, the sqlite support the trigger recursion function, 
 -- the trigger recursion function must be disabled. 
 *
*/


/************************************************
* define for table IRBUFF and its triggers;
************************************************/
/*
* Define for IRBuff table;
* it is a temporary table for storage the row about IRDB;
*/
const QString CREATE_TABEL_IRBUFF("CREATE TABLE [IRBuff] (   \
[channelID] integer(4) NOT NULL UNIQUE,   \
[channelName] nvarchar(255),   \
[channelNickName] nvarchar(255),   \
[genreName] nvarchar(255),   \
[genreId] nvarchar(64),     \
[languageName] nvarchar(255),   \
[languageCode] nvarchar(8),   \
[countryName] nvarchar(255),   \
[countryCode] INTEGER(2),     \
[description] nvarchar(255),   \
[shortDesc] nvarchar(255),   \
[lastModified] datetime DEFAULT (datetime(current_timestamp,'localtime')),   \
[musicStoreStatus] nvarchar(6),   \
[imgUrl] nvarchar(255),   \
[bIcon] BLOB,    \
[sIcon] BLOB,    \
[advertisementUrl] nvarchar(255),   \
[advertisementInUse] nvarchar(255),   \
[expiration] datetime DEFAULT (datetime(current_timestamp,'+10 day','localtime')),   \
[dataSource] numeric(1) NOT NULL,   \
[opt] NUMERIC(1) DEFAULT (1), \
PRIMARY KEY ([channelID]));");


/*
* Define for IRBuff insert trigger;
*/
const QString TRI_INSERT_IRBUFF("CREATE TRIGGER [TRI_INSERT_IRBuff] \
AFTER INSERT ON [IRBuff] FOR EACH ROW \
BEGIN \
 \
insert into channelInfo \
(channelID,   channelName,  channelNickName, genreName, \
 genreId,     languageName, languageCode,    countryName, \
 countryCode, description, shortDesc, lastModified, \
 musicStoreStatus) \
select \
 channelID,   channelName,  channelNickName, genreName, \
 genreId,     languageName, languageCode,    countryName, \
 countryCode, description,  shortDesc, lastModified, \
 musicStoreStatus \
from IRBuff \
where channelId = new.channelId \
AND new.opt =1;\
\
insert into channelHistory(channelID)  \
select channelID from IRBuff  \
where channelId = new.channelID \
AND new.dataSource = 1 \
AND new.opt = 1;  \
\
insert into \
searchRlt(channelID) \
select channelID from IRBuff \
where channelId = new.channelID \
AND new.dataSource = 2 \
AND new.opt = 1; \
\
insert into \
img(channelID, imgUrl, bIcon, sIcon) \
select channelID, imgUrl, bIcon, sIcon from IRBuff \
where channelId = new.channelId \
AND new.opt =1;\
\
insert into \
advertisement(channelID, advertisementUrl, advertisementInUse, expiration) \
select channelID, advertisementUrl, advertisementInUse, expiration from IRBuff \
where channelId = new.channelId \
AND new.opt =1;\
\
\
\
update IRBuff \
set \
imgUrl = (select imgUrl from img where channelid = new.channelid), \
bIcon = (select bIcon from img where channelid = new.channelid), \
sIcon = (select sIcon from img where channelid = new.channelid), \
advertisementUrl = (select advertisementUrl from advertisement where channelid = new.channelid), \
advertisementInUse = (select advertisementInUse from advertisement where channelid = new.channelid), \
expiration = (select expiration from advertisement where channelid = new.channelid), \
channelName = (select channelName from channelinfo where channelid = new.channelid), \
channelNickName = (select channelNickName from channelinfo where channelid = new.channelid), \
genreName = (select genreName from channelinfo where channelid = new.channelid), \
genreId = (select genreId from channelinfo where channelid = new.channelid), \
languageName = (select languageName from channelinfo where channelid = new.channelid), \
languageCode = (select languageCode from channelinfo where channelid = new.channelid), \
countryName = (select countryName from channelinfo where channelid = new.channelid), \
countryCode = (select countryCode from channelinfo where channelid = new.channelid), \
description = (select description from channelinfo where channelid = new.channelid), \
shortDesc = (select shortDesc from channelinfo where channelid = new.channelid), \
musicStoreStatus = (select musicStoreStatus from channelinfo where channelid = new.channelid) \
where channelId = new.channelID \
AND opt = 2;  \
 \
\
END;");


/*
* Define for IRBuff update trigger;
*/
const QString TRI_UPDATE_IRBUFF("CREATE TRIGGER [TRI_UPDATE_IRBUFF] \
AFTER UPDATE ON [IRBuff] FOR EACH ROW \
BEGIN \
\
update channelHistory \
set \
channelLatestPlayTime = datetime(current_timestamp,'localtime')  \
where \
new.opt = 3 \
AND Channelid = new.channelid \
AND new.datasource = 1; \
\
update searchRlt \
set \
channelLatestSrhTime = datetime(current_timestamp,'localtime') \
where \
new.opt = 3 \
AND Channelid = new.channelid \
AND new.datasource = 2; \
\
\
insert into channelHistory(channelID)  \
select channelID from IRBuff  \
where \
new.opt = 3 \
AND channelId = new.channelID \
AND new.dataSource = 1 \
AND not exists \
(select channelID from channelHistory where channelID = new.channelID);  \
\
insert into \
searchRlt(channelID) \
select channelID from IRBuff \
where \
new.opt = 3 \
AND channelId = new.channelID \
AND new.dataSource = 2 \
AND not exists \
(select channelID from searchRlt where channelID = new.channelID);   \
\
\
UPDATE channelInfo \
SET \
channelName = (select channelName from IRBuff where channelId = new.channelId), \
channelNickName = (select channelNickName from IRBuff where channelId = new.channelId), \
genreName = (select genreName from IRBuff where channelId = new.channelId), \
genreId = (select genreId from IRBuff where channelId = new.channelId), \
languageName = (select languageName from IRBuff where channelId = new.channelId), \
languageCode = (select languageCode from IRBuff where channelId = new.channelId), \
countryName = (select countryName from IRBuff where channelId = new.channelId), \
countryCode = (select countryCode from IRBuff where channelId = new.channelId), \
description = (select description from IRBuff where channelId = new.channelId), \
shortDesc = (select shortDesc from IRBuff where channelId = new.channelId), \
lastModified = (select lastModified from IRBuff where channelId = new.channelId), \
musicStoreStatus = (select musicStoreStatus from IRBuff where channelId = new.channelId) \
where \
new.opt = 3 \
AND channelId = new.channelId; \
\
update img  \
set \
imgUrl = (select imgUrl from IRBuff where channelId = new.channelId), \
bIcon = (select bIcon from IRBuff where channelId = new.channelId), \
sIcon = (select sIcon from IRBuff where channelId = new.channelId) \
where \
new.opt = 3 \
AND channelId = new.channelId; \
\
update advertisement \
set advertisementUrl = (select advertisementUrl from IRBuff where channelId = new.channelId), \
advertisementInUse = (select advertisementInUse from IRBuff where channelId = new.channelId), \
expiration = (select expiration from IRBuff where channelId = new.channelId) \
where \
new.opt = 3 \
AND channelId = new.channelId; \
\
END;");

//above trigger should add handle the case that the channelId exist in channelinfo but unexist in channelHistory.
/*
insert into channelHistory(channelID)  \
select channelID from IRBuff  \
where channelId = new.channelID \
AND new.dataSource = 1 \
AND new.opt = 3  \
AND not exists \
(select channelID from channelHistory where channelID = new.channelID);  \
*/


/**************************************************
* define for table channelHistory and its triggers;
***************************************************/
/*
* Define for channelHistory table;
*
* it stores the channels played by IR,the row counts is no more than 100;
*/
const QString CREATE_TABEL_CHANNELHISTORY("CREATE TABLE [channelHistory] (   \
[SID] integer NOT NULL PRIMARY KEY UNIQUE, \
[channelID] integer(4) NOT NULL UNIQUE, \
[channelLatestPlayTime] datetime DEFAULT (datetime(current_timestamp, 'localtime')));");


/*
* Define for channelHistory insert trigger;
*
* if rows counts > 100, it will trigger delete action.
*
* if one row is added here, 
* the corresponsive row with the same channelId may be inserted to channelinfo.
* 
* other:
* from pragram logic judge, 
* the probability of insert a row into IRBuff 
* when the row's opt = 1 while the row's channelId is in channelInfo table
* equals zero
*/
const QString TRI_INSERT_CHANNELHISTORY("CREATE TRIGGER [TRI_INSERT_channelHistory] \
AFTER INSERT ON [channelHistory] FOR EACH ROW \
BEGIN \
\
UPDATE channelInfo \
SET \
refCnt = refCnt+1 \
where channelId = new.channelId; \
\
UPDATE channelInfo \
SET channelType = 0 \
WHERE channelId = new.channelID \
AND new.channelID >= 4026531840; \
\
delete from channelHistory \
where channelLatestPlayTime = ( select min(channelLatestPlayTime) from channelHistory) \
AND (select count(*) from channelHistory)> 100; \
\
END;");


/*
* Define for channelHistory delete trigger;
*
* if one row is deleted, 
* the refcnt of corresponsive row, it has same channelId in channelInfo,
* will be decreased.
*/
const QString TRI_DELETE_CHANNELHISTORY("CREATE TRIGGER [TRI_DELETE_channelHistory] \
AFTER DELETE ON [channelHistory] \
BEGIN 	\
UPDATE channelInfo   \
SET refCnt = refCnt-1  \
where       \
channelId = OLD.channelId; \
END;");


/**************************************************
* define for table searchRlt and its triggers;
***************************************************/
/*
* Definition for searchRlt table;
*
* it stores the channels searched by IR user,the row counts is no more than 100;
*/
const QString CREATE_TABEL_SEARCHRLT("CREATE TABLE [searchRlt] (  \
[SID] integer NOT NULL PRIMARY KEY UNIQUE,  \
[channelID] integer(4) NOT NULL UNIQUE,   \
[channelLatestSrhTime] DATETIME DEFAULT (datetime(current_timestamp, 'localtime')));");


/*
* Define for searchRlt insert trigger;
*
* if rows counts > 100, it will trigger delete action.
*
* if one row is added here, 
* the corresponsive row with the same channelId may be inserted to channelinfo.
*
*/
const QString TRI_INSERT_SEARCHRLT("CREATE TRIGGER [TRI_INSERT_searchRlt] \
AFTER INSERT ON [searchRlt] FOR EACH ROW \
BEGIN  \
\
UPDATE channelInfo \
SET refCnt = refCnt+1 \
where \
channelId = new.channelId;  \
  \
UPDATE channelInfo \
SET channelType = 0 \
WHERE channelId = new.channelID \
AND new.channelID >= 4026531840; \
\
delete from searchRlt \
where SID = ( select min(SID) from searchRlt) AND (select count(*) from searchRlt)> 100; \
\
END;");


/*
* Define for searchRlt delete trigger;
* 
* if one row is deleted, 
* the refcnt of corresponsive row, it has same channelId in channelInfo,
* will be decreased.
*/
const QString TRI_DELETE_SEARCHRLT("CREATE TRIGGER [TRI_DELETE_searchRlt] \
AFTER DELETE ON [searchRlt] \
BEGIN 	\
UPDATE channelInfo   \
SET refCnt = refCnt-1  \
where channelId = OLD.channelId; \
\
END;");


/**************************************************
* define for table channelInfo and its triggers;
***************************************************/
/*
* Definition for channelInfo table;
* it stores the channels infomation;
*/
const QString CREATE_TABEL_CHANNELINFO("CREATE TABLE channelInfo( \
[channelID] integer(4) PRIMARY KEY UNIQUE NOT NULL ,\
[channelName] nvarchar(255) ,\
[channelNickName] nvarchar(255) ,\
[genreName] nvarchar(255) ,\
[genreId] nvarchar(64),\
[languageName] nvarchar(255) ,\
[languageCode] nvarchar(8) ,\
[countryName] nvarchar(255) ,\
[countryCode] nvarchar(255) ,\
[description] nvarchar(255) ,\
[shortDesc] nvarchar(255) ,\
[lastModified] datetime DEFAULT (datetime(current_timestamp,'localtime')), \
[channelType] numeric(1) DEFAULT (1) ,\
[musicStoreStatus] nvarchar(6) ,\
[refCnt] numeric(1) DEFAULT (0));");




/*
* Define for channelInfo update trigger;
* if refCnt default is 0 after update, it will trigger delete action,
* all infomation about this row in  IRDB will be removed.
*/
const QString TRI_UPDATE_CHANNELINFO("CREATE TRIGGER [TRI_UPDATE_channelInfo] \
AFTER UPDATE ON [channelInfo] FOR EACH ROW \
BEGIN 	\
delete from channelInfo \
where \
channelId = new.channelID \
AND refCnt = 0; \
END;");


/*
* Definition for channelInfo delete trigger;
* the refCnt default value is 1;
*/
const QString TRI_DELETE_CHANNELINFO("CREATE TRIGGER [TRI_DELETE_channelInfo] \
AFTER DELETE ON [channelInfo] FOR EACH ROW \
BEGIN  \
delete from advertisement where channelID = old.channelID;  \
delete from img where channelID = old.channelID;  \
delete from urlInfo where channelID = old.channelID; \
END;");



/**************************************************
* define for table img and its triggers;
***************************************************/
/*
* Definition for img table;
* it stores the channels logo img infomation;
*/
const QString CREATE_TABEL_IMG("CREATE TABLE [img] (  \
[SID] integer NOT NULL PRIMARY KEY UNIQUE,   \
[channelID] integer(4) NOT NULL,   \
[imgUrl] nvarchar(255) DEFAULT ('unavailable'),   \
[bIcon] BLOB,    \
[sIcon] BLOB); ");

/*
* Definition for trigger img table;
* when imgurl is updated(old.value isn't same with new.value),
* the logo info in database will be remvoed;
*/
const QString TRI_UPDATE_IMG("CREATE TRIGGER [IRI_UPDATE_img] \
AFTER UPDATE \
ON [img] \
FOR EACH ROW \
BEGIN \
update img \
set sIcon = NULL, bIcon = NULL \
where new.imgurl <> old.imgurl; \
\
END; ");


/**************************************************
* define for table urlInfo and its triggers;
***************************************************/
/*
* Definition for urlInfo table;
* it stores the channels url information;
*/
const QString CREATE_TABEL_URLINFO("CREATE TABLE [urlInfo] (  \
[SID] integer NOT NULL PRIMARY KEY UNIQUE,   \
[channelUrl] nvarchar(255) DEFAULT ('unavailable'),   \
[channelID] integer(4) NOT NULL,  \
[bitRate] integer(4));");


/**************************************************
* define for table advertisement and its triggers;
***************************************************/
/*
* Definition for advertisement table;
* it stores the advertisement information;
*/
const QString CREATE_TABEL_ADVERTISEMENT("CREATE TABLE [advertisement] (  \
[SID] integer NOT NULL PRIMARY KEY UNIQUE,   \
[channelID] integer(4) NOT NULL,   \
[advertisementUrl] nvarchar(255),   \
[advertisementInUse] nvarchar(255),   \
[expiration] datetime);");


/*
* Definition for advertisement update trigger;
* if there is row in IRbuff, it will be deleted,
* here is last defender for remove rows in IRbuff;
*/
const QString TRI_INSERT_ADVERTISEMENT("CREATE TRIGGER [TRI_INSERT_advertisement] \
AFTER INSERT ON [advertisement] FOR EACH ROW \
BEGIN 	\
DELETE FROM IRBuff; \
END;");


/*
* Definition for advertisement update trigger;
* update channelid in advertisement is forbidden; 
*
* if there is row in IRbuff, it will be deleted,
* here is last defender for remove rows in IRbuff;
* 
*/
const QString TRI_UPDATE_ADVERTISEMENT("CREATE TRIGGER [TRI_UPDATE_advertisement] \
AFTER UPDATE ON advertisement FOR EACH ROW \
BEGIN \
update advertisement set channelid = old.channelid where channelid = new.channelid;  \
DELETE FROM IRBuff; \
END;");



/**************************************************
* define for table songHistory and its triggers;
***************************************************/
/*
* Definition for songHistory table;
* it stores the song played information;
* the rows in this table are no more than 100;
*/
const QString CREATE_TABEL_SONGHISTORY("CREATE TABLE [songHistory]( \
[SID] integer PRIMARY KEY UNIQUE NOT NULL ,\
[songName] nvarchar(64) NOT NULL ,\
[artistName] nvarchar(64) ,\
[channelID] integer(4) NOT NULL ,\
[songPlaySeq] LARGEINT DEFAULT (0), \
[songLatestPlayTime] DATETIME DEFAULT (datetime(current_timestamp,'localtime')) );");


/*
* Definition for songHistory insert trigger;
* 
* if the rows are more than 100, the oldest row will be removed;
* after a row insert, the channel's refCnt in channelInfo will be increase;
*/
const QString TRI_INSERT_SONGHISTORY("CREATE TRIGGER [TRI_INSERT_songHistory] \
AFTER INSERT ON [songHistory] FOR EACH ROW \
BEGIN 	\
UPDATE channelInfo   \
SET refCnt = refCnt+1   \
where channelId = new.channelId;   \
 \
update songHistory \
SET \
songName = new.songName,  \
channelId = new.channelId, \
artistName = new.artistName \
where \
songName = new.songName \
AND channelId = new.channelId \
AND artistName = new.artistName; \
 \
delete from songHistory   \
where songPlaySeq = ( select min(songPlaySeq) from songHistory)    \
AND (select count(*) from songHistory)> 100;   \
END;");


/*
* Definition for songHistory delete trigger;
* 
* after row is removed, the channel's refCnt in channelInfo will be decreased;
*/
const QString TRI_DELETE_SONGHISTORY("CREATE TRIGGER [TRI_DELETE_songHistory] \
AFTER DELETE ON [songHistory] FOR EACH ROW \
BEGIN 	\
UPDATE channelInfo   \
SET refCnt = refCnt-1   \
where channelId = OLD.channelId; \
END;");


/*
* Definition for songHistory update trigger;
*/
const QString TRI_UPDATE_SONGHISTORY("CREATE TRIGGER [TRI_UPDATE_songHistory] \
AFTER UPDATE ON songHistory FOR EACH ROW \
BEGIN \
 \
update songHistory \
set songPlaySeq = (select max(songPlaySeq) from songHistory) + 1  \
where songName = new.songName  \
AND channelId = new.channelId  \
AND artistName = new.artistName; \
 \
END");


/**************************************************
* define for table favorites and its triggers;
***************************************************/
/*
* Definition for favorites table;
* it stores the  information of song user's favorites;
* the rows in this table are no more than 100;
*/
const QString CREATE_TABEL_FAVORITES("CREATE TABLE [favorites]( \
[SID] integer PRIMARY KEY UNIQUE NOT NULL ,\
[ChannelID] integer(4) UNIQUE NOT NULL ,\
[PlayCount] integer(4) DEFAULT (1),\
[FavSeq] LARGEINT DEFAULT (0) );");


/*
* Definition for favorites insert trigger;
* 
* if the rows are more than 100, the oldest row will be removed;
* after a row insert, the channel's refCnt in channelInfo will be increase;
*/
const QString TRI_INSERT_FAVORITES("CREATE TRIGGER TRI_INSERT_favorites \
AFTER INSERT On favorites \
BEGIN 	\
\
update favorites \
set channelId = new.channelid \
where channelid = new.channelid; \
\
UPDATE channelInfo   \
SET refCnt = refCnt+1   \
where channelId = new.channelId;   \
 \
delete from favorites   \
where \
FavSeq = ( select min(FavSeq) from favorites )  \
AND (select count(*) from favorites)> 100; \
 \
END;");


/*
* Definition for songHistory update trigger;
*/
const QString TRI_UPDATE_FAVORITES("CREATE TRIGGER [TRI_UPDATE_favorites] \
AFTER UPDATE ON favorites FOR EACH ROW \
BEGIN \
update favorites \
set \
PlayCount = PlayCount+1, \
FavSeq = (select max(FavSeq) from favorites) +1  \
where channelid = old.channelID; \
 \
update favorites \
set channelid = old.channelid \
where channelid = new.channelid;  \
END;");


/*
* Definition for favorites delete trigger;
* 
* after row is removed, the channel's refCnt in channelInfo will be decreased;
*/
const QString TRI_DELETE_FAVORITES("CREATE TRIGGER TRI_DELETE_favorites \
AFTER DELETE On favorites \
BEGIN 	\
UPDATE channelInfo   \
SET refCnt = refCnt-1   \
where       \
channelId = OLD.channelId; \
END;");



/**************************************************
* define for IRDB VIEW
* channelId is the join key;
***************************************************/
const QString IRVIEW_CHANNELINFO1(" Create  View [IRVIEW_CHANNELINFO1] As \
select \
channelInfo.[channelID], \
channelInfo.[channelName], channelInfo.[channelNickName], channelInfo.[genreName],   \
channelInfo.[genreId],     channelInfo.[languageName],    channelInfo.[languageCode], \
channelInfo.[countryName], channelInfo.[countryCode],     channelInfo.[description],  \
channelInfo.[shortDesc],   channelInfo.[lastModified],    channelInfo.[channelType], \
channelInfo.[musicStoreStatus], \
img.[imgUrl], img.[bIcon], img.[sIcon] \
from  \
 channelInfo LEFT JOIN img ON channelInfo.channelID = img.channelID  ");



const QString IRVIEW_CHANNELINFO(" Create  View [IRView_channelinfo] As \
select \
IRView_channelinfo1.*, \
advertisement.[advertisementUrl], advertisement.[advertisementInUse],advertisement.[expiration] \
from \
IRView_channelinfo1 LEFT JOIN advertisement ON IRView_channelinfo1.[channelId] = advertisement.[channelID]  ");


const QString IRVIEW_CHANNELHISTORY(" Create  View [IRVIEW_channelHistory] As \
select \
IRView_channelinfo.* \
from \
channelHistory \
LEFT JOIN \
IRView_channelinfo ON channelHistory.[channelId] = IRView_channelinfo.[channelID] \
order by channelHistory.[channelLatestPlayTime] desc ");

const QString IRVIEW_FAVORITES(" Create  View [IRVIEW_favorites] As \
select \
IRView_channelinfo.* \
from \
favorites LEFT JOIN IRView_channelinfo ON favorites.[channelId] = IRView_channelinfo.[channelID] \
order by favorites.[FavSeq] desc ");


const QString IRVIEW_SEARCHRLT(" Create  View [IRVIEW_searchRlt] As \
select \
IRView_channelinfo.* \
from \
searchRlt LEFT JOIN IRView_channelinfo ON searchRlt.[channelId] = IRView_channelinfo.[channelID] \
order by searchRlt.[channelLatestSrhTime] asc ");

const QString IRVIEW_SONGHISTORY("Create  View [IRVIEW_songHistory] As \
select \
IRView_channelinfo.*, songHistory.[songName], songHistory.[artistName] \
from \
songHistory LEFT JOIN IRView_channelinfo ON songHistory.[channelId] = IRView_channelinfo.[channelID] \
order by songHistory.[songPlaySeq] desc ");


const QString IR_VIEW_SRH_USERCID(" CREATE VIEW [IR_VIEW_SRH_USERCID] As \
select channelinfo.[channelID], channelNickName, channelUrl, bitRate \
FROM channelinfo LEFT JOIN urlInfo ON channelinfo.channelID = urlInfo.channelID ");


const QString IR_VIEW_ALL("Create  View [IRVIEW_ALL] As  \
select \
IRView_channelinfo.*,urlInfo.[channelUrl],  urlInfo.[bitRate] \
from \
IRView_channelInfo \
LEFT JOIN \
urlinfo ON IRView_channelinfo.[channelID] = urlInfo.[channelID] ");

/**************************************************
* define for drop IRDB VIEW, TABLE AND TRIGGER
***************************************************/
//drop view;
const QString IR_VIEW1_DROP("Drop View If Exists MAIN.[IR_VIEW1];"); 
const QString IR_VIEW2_DROP("Drop View If Exists MAIN.[IR_VIEW2];"); 
const QString IR_VIEW3_DROP("Drop View If Exists MAIN.[IR_VIEW3];"); 
const QString IR_VIEW4_DROP("Drop View If Exists MAIN.[IR_VIEW4];"); 
const QString IR_VIEW5_DROP("Drop View If Exists MAIN.[IR_VIEW5];"); 
const QString IR_VIEW6_DROP("Drop View If Exists MAIN.[IR_VIEW6];"); 
const QString IR_VIEW_ALL_DROP("Drop View If Exists MAIN.[IR_VIEW_ALL];"); 

//drop table;
const QString IR_IRBUFF_DROP("Drop table if exists main.[IRBUFF];");
const QString IR_CHANNELHISTORY_DROP("Drop table if exists main.[channelHistory];");
const QString IR_SEARCHRLT_DROP("Drop table if exists main.[searchRlt];");
const QString IR_CHANNELINFO_DROP("Drop table if exists main.[channelInfo];");
const QString IR_IMG_DROP("Drop table if exists main.[img];");
const QString IR_URLINFO_DROP("Drop table if exists main.[urlInfo];");
const QString IR_ADVERTISEMENT_DROP("Drop table if exists main.[advertisement];");
const QString IR_SONGHISTORY_DROP("Drop table if exists main.[songHistory];");
const QString IR_FAVORITES_DROP("Drop table if exists main.[favorites];");



#endif /* IRSQLSTR_H_ */
































