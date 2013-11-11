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
* Contributors:
*
* Description: Music Player Query Manager.
*
*/

#include "mpquerymanager.h"
#include <QObject>
#include <QList>
#include <QFile>
#include <QUrl>

#include "mptrace.h"

const int KRecommendationNum = 2;

MpQueryManager::MpQueryManager(QObject *parent)
                                :QObject(parent)
{
    TX_ENTRY_ARGS("STUB")
    mRecommendationAlbumArtsName << "C:\\Data\\albumOne.png" << "C:\\Data\\albumTwo.png";
    TX_EXIT_ARGS("STUB")
}

MpQueryManager::~MpQueryManager()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}


void MpQueryManager::clearNetworkReplies()
{
    TX_ENTRY_ARGS("STUB Reply count = ");
    TX_EXIT_ARGS("STUB")
}


void MpQueryManager::queryLocalMusicStore(QString artist,QString album,QString title)
{
    TX_ENTRY_ARGS("STUB")
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    TX_EXIT_ARGS("STUB")    
}

void MpQueryManager::queryLocalMusicStore()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")    
}

void MpQueryManager::reset()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")    
}

bool MpQueryManager::isLocalMusicStore() const
{
    TX_ENTRY_ARGS("STUB - will always return TRUE")
    TX_EXIT_ARGS("STUB") 
    return true;
}

    
void MpQueryManager::queryInspireMeItems(QString artist,QString album,QString title)
{
    TX_ENTRY_ARGS("STUB")    
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    TX_EXIT_ARGS("STUB")
}

void MpQueryManager::clearRecommendations()
{
    TX_ENTRY_ARGS("STUB")    
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    for ( int i = 0; i < KRecommendationNum; i++ ) {
       mRecommendationSongs.clear();
       mRecommendationArtists.clear();
       mRecommendationAlbumArtsLink.clear();
       mRecommendationAlbumArtsMap.clear();      
       QFile file( mRecommendationAlbumArtsName.at( i ) );        
       if ( file.exists() ) {
           if ( file.remove() ) {
               TX_LOG_ARGS( "File removed - " << file.fileName() );
           }
           else {
               TX_LOG_ARGS( "Cannot remove file - " << file.fileName() );
           }
       } else {
           TX_LOG_ARGS( "File doesn't exist - " << file.fileName() );
       }
    }
    TX_EXIT_ARGS("STUB")    
}


/*!
 Return recommendation artists
 */
QString MpQueryManager::recommendedArtist(int index) const
{
    QString result;
    if( (0 <= index) && (index < mRecommendationArtists.count())) {
        result = mRecommendationArtists.at(index);
    }
    TX_LOG_ARGS ("recommendedArtist: " << result);    
    return result;
}

/*!
 Return the number of ready album arts
 */
int &MpQueryManager::albumArtsReadyCount()
{
    TX_LOG  
    return mAlbumArtsReadyCount;
}

int MpQueryManager::recommendationsCount() const
{
    TX_LOG_ARGS ("count: " << mRecommendationSongs.count());
    return mRecommendationSongs.count();
}

QString MpQueryManager::recommendedSong(int index) const
{
    QString result;
    if( (0 <= index) && (index < mRecommendationSongs.count())) {
        result = mRecommendationSongs.at(index);
    }
    return result; 
}

HbIcon MpQueryManager::recommendedAlbumArt(int index) const
{
    TX_LOG_ARGS( "index = " << index )
    return mRecommendationAlbumArtsMap.value( mRecommendationAlbumArtsLink.at( index ) );
}
