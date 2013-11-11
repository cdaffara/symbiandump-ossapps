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
* Description: Manipulate Interface of Internet Radio Database 
*
*/

#ifndef _IRDB_VIEW_STRUCT_H_
#define _IRDB_VIEW_STRUCT_H_

#include <QString>
#include <QMap>
#include <QByteArray>


/* 
* pair node for column-value;
* uint:    column enum;
* QString: column value;
*/
typedef QMap<uint, QString>  columnMap;
typedef QMap<uint, QByteArray> logoMap;

enum EColumnLogo{bLogo, sLogo};
const QString colNameLogo[] = {"bIcon", "sIcon"};
const QString strBLogo = ":bIcon";
const QString strSLogo = ":sIcon";

const int IRDB_BOTH_LOGO = 0x03;// both logo, big and small is in logodata;
const int IRDB_BIG_LOGO = 0x01; //big logo is in logodata;
const int IRDB_SMALL_LOGO = 0x02; //small logo is in logodata

const int IRDB_ALL_COLUMN = 25;
const int IRDB_OPT_INSERT = 1;
const int IRDB_OPT_UPDATE = 2;
const int IRDB_OPT_TRIGGER = 3;
const int IRDB_CHANNELINFO_COLUMN = 20;
const int IRDB_CHANNELHISTORY_COLUMN = 20;
const int IRDB_SEARCHRLT_COLUMN = 20;
const int IRDB_SONGHISTORY_COLUMN = 22;
const int IRDB_FAVORITES_COLUMN = 20;
const int IRDB_IMG_COLUMN = 4;

enum EColumnView{channelId,        channelName,   channelNickName,  
                 genreName,        genreId,       languageName, languageCode,  
                 countryName,      countryCode,   description,  shortDesc,    
                 lastModified,     channelType,   musicStoreStatus, 
                 imgUrl,           bLogoGet, sLogoGet,   
                 advertisementUrl, advertisementInUse, expiration,
                 songName, artistName, channelUrl, bitRate, opt, dataSource};

const QString colNameView[] = {"channelId",        "channelName",       "channelNickName",  
                               "genreName",        "genreId",           "languageName",     "languageCode",  
                               "countryName",      "countryCode",       "description",      "shortDesc",    
                               "lastModified",     "channelType",       "musicStoreStatus", 
                               "imgUrl",           "bIcon",            "sIcon",     
                               "advertisementUrl", "advertisementInUse", "expiration",
                               "songName", "artistName", "channelUrl", "bitRate", "opt", "dataSource"};

//flag of data belong to channelHistory;
const int DATA_OF_CHANNELHISTORY = 1;

//flag of data belong to searchRlt;
const int DATA_OF_SEARCHRLT = 2;

/*
* for urlinfo table;
*/
//columns count of urlinfo;
const int IRDB_URLINFO_COLUMN = 3;

//urlInfo's column enum;
enum EColumnUrlInfo{channelUrl_URL, channelId_URL, bitRate_URL};

//urlInfo's column name;
const QString colNameUrlInfo[] = {"channelUrl", "channelId", "bitRate" };

//for urlInfo insert 
typedef QMap<QString, uint>  columnUrlInfoInsertMap;


/*
* for advertisement table;
*/
/*
* Due to the advertisement wrapper is unavailable, the structure is masked temporarily.
*/
//columns count of advertisement;
//const int IRDB_ADVERTISEMENT_COLUMN = 5;

//advertisement's column enum;
//enum EColumnAdvertisement{channelId_AD, advertisementUrl_AD, advertisementInUse_AD, expiration_AD};

//advertisement's column name;
//const QString colNameAdvertisement[] = {"channelId", "advertisementUrl", "advertisementInUse", "expiration"};


#endif


