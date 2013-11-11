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
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxyFactory>
#include <qmobilityglobal.h>
#include <QNetworkSession>
#include <QDomElement>
#include <QList>
#include <QUrl>
#include <QSslError>
#include <XQSysInfo>
#include <QSignalMapper>
#include <QSettings>

#include "mptrace.h"

const int KRecommendationCount = 2;

MpQueryManager::MpQueryManager()
    : mManager(0),
      mAlbumArtDownloader(0),
      mDefaultRecommendationAlbumArt("qtg_large_album_art"),
      mRequestType(NoRequest),
      mRecommendationCount(0)                               
{
    TX_ENTRY
    mManager = new QNetworkAccessManager( this );      
    // A second intance is necessary to reduce complexity.
    // Otherwise, we would have to shoot async events when we want to receive inspire me items' album art
    // and that may not always work.
    mAlbumArtDownloader = new QNetworkAccessManager( this ); 

    mDownloadSignalMapper = new QSignalMapper(this);
    TX_EXIT
}

MpQueryManager::~MpQueryManager()
{
    TX_ENTRY
    reset();
    if ( mManager ) {
       mManager->deleteLater();
    }
    if ( mAlbumArtDownloader ) {
       mAlbumArtDownloader->deleteLater();
    }
    delete mDownloadSignalMapper;
    TX_EXIT
}


void MpQueryManager::clearNetworkReplies()
{
    mRequestType = NoRequest; 
    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    TX_ENTRY_ARGS( "Reply count = " << mReplys.count() );    
    for ( int i = 0; i < mReplys.count(); ++i ) {
        QNetworkReply *reply = mReplys.at( i );
        disconnect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );        
        if ( reply != NULL ) {
            reply->close();
            reply->deleteLater();
            reply = NULL;
        }
    }
    mReplys.clear();
    TX_EXIT
}

    
void MpQueryManager::queryInspireMeItems(QString artist,QString album,QString title)
{
    TX_ENTRY    
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    // start querying inspire me items
    QString queryRecommendation("http://api.music.ovi.com/1.0/" + mMusicStore + "/releases/recommend/?");
    constructRequest( queryRecommendation );
    // TODO: Tokens change per new ovi api release. 
	//       Need to figure out a way to get them updated on the fly
    queryRecommendation.append("&Token=03574704-e3d1-4466-9691-e0b34c7abfff");

    TX_LOG_ARGS( queryRecommendation );
    retrieveInformation( queryRecommendation );
    mRequestType = InspireMeItemsMetadataRequest;
    TX_EXIT
}

void MpQueryManager::clearRecommendations()
{
    TX_ENTRY
    mRecommendationCount = 0;
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    mRecommendationSongs.clear();
    mRecommendationArtists.clear();
    mRecommendationAlbumArtsLink.clear();
    mRecommendationAlbumArtsMap.clear();
    TX_EXIT    
}



/*!
 Returns the Local Music Store(if available) to be used while querying "Inspire Me" Items
 */
void MpQueryManager::queryLocalMusicStore()
{
    TX_ENTRY
    QString imsi,mcc;

    XQSysInfo sysInfo( this );
    imsi = sysInfo.imsi();
    mcc= imsi.left(3);
    TX_LOG_ARGS( "mcc : " << mcc );
    
    QString queryLocalMusicStore("http://api.music.cq1.brislabs.com/1.0/?mcc=" + mcc + "&token=06543e34-0261-40a4-a03a-9e09fe110c1f");
    TX_LOG_ARGS( "queryLocalMusicStore : " << queryLocalMusicStore );
    retrieveInformation( queryLocalMusicStore );
    mRequestType = LocalStoreRequest;
    TX_EXIT
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
    TX_LOG_ARGS ("recommendedSong: " << result);    
    return result; 
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
 Slot to call when getting response
 */
void MpQueryManager::retrieveInformationFinished( QNetworkReply* reply )
{
    TX_ENTRY
    QString errorStr;
    int errorLine;
    int errorColumn;
    bool parsingSuccess;
        
    if ( reply->error() != QNetworkReply::NoError ) {
		TX_LOG_ARGS("reply->error(): " << reply->error());
        signalError();
        return;
    }
        
    parsingSuccess = mDomDocument.setContent( reply, true, &errorStr, &errorLine, &errorColumn );
    if ( !parsingSuccess ) {
		TX_LOG_ARGS("Parsing Received Content Failed");
        signalError();
        return;
    }

    handleParsedXML();  //CodeScanner throws a warning mis-interpreting the trailing 'L' to be a leaving function.
    
    mReplys.removeAll(reply); // remove it so that we wont process it again
    reply->deleteLater(); // make sure reply is deleted, as we longer care about it
    
    TX_EXIT
}

void MpQueryManager::signalError()
{
    TX_ENTRY; 
    switch(mRequestType) {
        case InspireMeItemsMetadataRequest:
            TX_LOG_ARGS("emit inspireMeItemsRetrievalError");
            emit inspireMeItemsRetrievalError();
            break;
        case InspireMeItemsAlbumArtRequest:
            TX_LOG_ARGS("Warning: InspireMeItemsAlbumArtRequestError, will keep using the default AA icon");            
            break;    
        case LocalStoreRequest:
            TX_LOG_ARGS("emit localMusicStoreRetrievalError");            
            emit localMusicStoreRetrievalError();
            break;
        case NoRequest:   
        default:
            TX_LOG_ARGS("Warning!! Possible uninitialized mRequestType");            
            break;                                 
    }
    TX_EXIT    
}

/*!
 Slot to call when there is network error
 */
void MpQueryManager::retrieveInformationNetworkError( QNetworkReply::NetworkError error )
{
    TX_ENTRY_ARGS( "Network error for retrieving Information" << error);    

	Q_UNUSED(error)

    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    signalError();
    TX_EXIT
}

/*!
 Slot to call when there is ssl error
 */
void MpQueryManager::retrieveInformationSslErrors( const QList<QSslError> &/*error*/ )
{
    TX_ENTRY_ARGS( "SSL error for retrieving Information" );
    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    signalError();    
    TX_EXIT
}

/*!
 Slot to call when downloading finished
 */
void MpQueryManager::albumArtDownloaded( int index )
{
    TX_ENTRY_ARGS( "mDownloadedAlbumArts = " << mDownloadedAlbumArts << "index = " << index);
    
    QNetworkReply* reply = qobject_cast<QNetworkReply*> ( qobject_cast<QSignalMapper*>( sender() )->mapping( index ) );
    // It seems we get several finished signals for the same reply obj
    // do nothing if we get a second signal
    if( mReplys.indexOf(reply) == -1 ) {
        TX_LOG_ARGS("Warning: QNetworkReply AA request may have been processed in previous call: " << reply );
        return;
    }
       
    if ( reply->error() == QNetworkReply::NoError ) {
        QPixmap albumart;
        bool result = albumart.loadFromData( reply->readAll() );
        if ( result ) {
            mRecommendationAlbumArtsMap.insert( mRecommendationAlbumArtsLink.at( index ), HbIcon( QIcon( albumart ) ) );

        } else {
            mRecommendationAlbumArtsMap.insert( mRecommendationAlbumArtsLink.at( index ), mDefaultRecommendationAlbumArt );        
        }

        ++mDownloadedAlbumArts;
        mReplys.removeAll(reply); // remove it so that we wont process it again
        reply->deleteLater(); // make sure reply is deleted, as we longer care about it
    }
    else {
        TX_LOG_ARGS( "Error: Downloading album art failed! Will keep using the default AA" );
        mRecommendationAlbumArtsMap.insert( mRecommendationAlbumArtsLink.at( index ), mDefaultRecommendationAlbumArt );  
    }
    mDownloadSignalMapper->removeMappings( reply );
    
    if( mDownloadedAlbumArts == mRecommendationCount) {
        // no need to be informed anymore
        mDownloadSignalMapper->disconnect(this);
        emit inspireMeItemAlbumArtReady();    
    }
    
    TX_EXIT
}


/*!
 Get Atom response from Ovi server based on query
 */
void MpQueryManager::retrieveInformation( const QString &urlEncoded )
{
    TX_ENTRY_ARGS( "urlEconded = " << urlEncoded)
    connect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ), Qt::UniqueConnection );
    QNetworkReply *reply = mManager->get( QNetworkRequest( QUrl( urlEncoded ) ) );
    mReplys.append( reply );
    
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
    TX_EXIT
}


/*!
 Find the most suitable link based on Atom response from Ovi music server
 */
void MpQueryManager::handleParsedXML()
{
    TX_ENTRY
    QDomElement rootElement = mDomDocument.documentElement();
    
    if ( rootElement.attribute( "type" ) == "recommendedTracks" ) {
        TX_LOG_ARGS( "Recommendation response" )
        QDomElement entry = rootElement.firstChildElement( "entry" );
        mRecommendationCount = 0;
        while ( !entry.isNull() && mRecommendationCount < KRecommendationCount ) {
            if ( entry.attribute( "type" ) == "musictrack" ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() ) {
                    if ( link.attribute( "title" ) == "albumart100" ) {
                        mRecommendationAlbumArtsLink.append( link.attribute( "href" ) );
                        mRecommendationAlbumArtsMap.insert( link.attribute( "href" ), mDefaultRecommendationAlbumArt );                      
                        break;
                    }
                    else {
                        link = link.nextSiblingElement( "link" );
                    }
                }
                QDomElement metadata = entry.firstChildElement( "metadata" );
                mRecommendationSongs.append( metadata.firstChildElement( "name" ).text() );
                mRecommendationArtists.append( metadata.firstChildElement( "primaryartist" ).text() );
                ++mRecommendationCount;
            }
            entry = entry.nextSiblingElement( "entry" );
        }
 
        emit inspireMeItemsMetadataRetrieved();

        QNetworkReply *reply = 0;
        // we need to channel the retrieved album arts to albumArtDownloaded slot only
        disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );        
        for (int i = 0; i < mRecommendationCount; i++ ) {
            TX_LOG_ARGS( "song name: " << mRecommendationSongs.at(i) );
            TX_LOG_ARGS( "Artist name: " << mRecommendationArtists.at(i) );
            TX_LOG_ARGS( "Album art link: " << mRecommendationAlbumArtsLink.at(i) );
            mRequestType = InspireMeItemsAlbumArtRequest;
            if ( mRecommendationAlbumArtsLink.at( i ).contains( "http", Qt::CaseInsensitive ) ) {
                reply = mAlbumArtDownloader->get( QNetworkRequest( QUrl( mRecommendationAlbumArtsLink.at(i) ) ) );
                mReplys.append( reply );
                connect( reply, SIGNAL( finished() ), mDownloadSignalMapper, SLOT( map() ) );
                mDownloadSignalMapper->setMapping( reply, i );

                connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
                connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
            }
        }
       // we have queried for album arts for inspire me items. Now, time to wait for a response        
        connect( mDownloadSignalMapper, SIGNAL( mapped( int ) ), this, SLOT( albumArtDownloaded( int ) ) );

    }
    else if ( rootElement.attribute( "type" ) == "storeList" ) {
		TX_LOG_ARGS( "Music Store List" )
        QDomElement entry = rootElement.firstChildElement( "workspace" );
        QString previousMusicStore = mMusicStore;
        mMusicStore = entry.attribute( "countryCode" );
        if( !mMusicStore.isEmpty() ) {
            bool musicStoreUpdated = ( previousMusicStore != mMusicStore );
            TX_LOG_ARGS("Music Store" << mMusicStore );
            emit localMusicStoreRetrieved( musicStoreUpdated );
            if( musicStoreUpdated ) {
                QSettings settings;
                TX_LOG_ARGS( "Storing music store value: " << mMusicStore );
                settings.setValue( "LocalMusicStore", QVariant( mMusicStore ) );
            }    
        }
        else {
            emit localMusicStoreRetrievalError();   
        }		
    }
    else {
        TX_LOG_ARGS( "Not supported response" )
    }
    TX_EXIT
}


void MpQueryManager::reset()
{
    TX_ENTRY
    mManager->disconnect(this);
    mAlbumArtDownloader->disconnect(this);
    clearNetworkReplies();
    clearRecommendations();
    mRecommendationAlbumArtsMap.clear();
    TX_EXIT
}
    
/*!
 Construct the query for fetching URI & recommendations
 */
void MpQueryManager::constructRequest( QString &uri )
{
    TX_ENTRY_ARGS( "uri =" << uri)
    
    QStringList keys;
    keys << "artist" << "albumtitle" << "tracktitle" << "orderby";
    
    // "relevancy" is the selected sort order
    // sort order types can be relevancy, alltimedownloads, streetreleasedate, sortname, recentdownloads
    QStringList values;
    values << mArtist << mAlbum << mTitle << QString("relevancy");
    TX_LOG_ARGS( "Artist: " << mArtist ); 
    TX_LOG_ARGS( "Album: " << mAlbum );
    TX_LOG_ARGS( "Title: " << mTitle );
    
    uri += keyValues( keys, values );

    QUrl url(uri);
    uri = url.toEncoded();
    TX_EXIT
}

/*!
 Make a key & value pair string for querying
 */
QString MpQueryManager::keyValues( QStringList keys, QStringList values ) const
{
    TX_ENTRY
    QString str;
    if ( keys.length() != values.length() ) {
        TX_LOG_ARGS( "Error: keys length is not equal to values length" ); 
    }
    else {
        for ( int i = 0; i < keys.length(); i++ ) {
            QString tValue = values.at( i );
            if ( 0 != tValue.length() ) {
                str += keys.at( i ) + "=" + values.at( i ) + "&";
            }
        }
    }
    TX_EXIT
    return str.left( str.length() - 1 );
}

bool MpQueryManager::isLocalMusicStore()
{
    if( mMusicStore.isEmpty() ) {
        QSettings settings;
        QVariant settingsvariant = settings.value( "LocalMusicStore", "" );
        mMusicStore = settingsvariant.toString(); 
        TX_LOG_ARGS( "Got local music store from settings:" << mMusicStore );
    }
    TX_LOG_ARGS( "isLocalMusicStore = " << !mMusicStore.isEmpty() )
    return !mMusicStore.isEmpty();
}

HbIcon MpQueryManager::recommendedAlbumArt(int index) const
{
    TX_LOG_ARGS( "index = " << index )
    return mRecommendationAlbumArtsMap.value( mRecommendationAlbumArtsLink.at( index ) );
}
