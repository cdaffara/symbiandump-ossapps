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
#ifndef MPQUERYMANAGER_H_
#define MPQUERYMANAGER_H_

#include <QPointer>
#include <QNetworkReply>
#include <QDomDocument>
#include <QMap>
#include <HbIcon>

#include "mpviewbase.h"

class QNetworkAccessManager;
class QSignalMapper;
class ThumbnailManager;

/*!
  MpQueryManager class is responsible for performing music player
  related network queries, including retrieving the URL for the
  music.

  It emits searchUrlRetrieved signal when it has received the
  URL.
*/
class MpQueryManager : public QObject
{
    Q_OBJECT

public:
    MpQueryManager();
    ~MpQueryManager();
    
public:
    void reset();
    void queryInspireMeItems( QString mArtist,QString mAlbum,QString mTitle );
    void queryLocalMusicStore();
    bool isLocalMusicStore();
    
    int recommendationsCount() const;
    QString recommendedSong(int index) const;
    QString recommendedArtist(int index) const;
    HbIcon recommendedAlbumArt(int index) const;

private slots:
    void retrieveInformationFinished( QNetworkReply* reply );
    void retrieveInformationNetworkError( QNetworkReply::NetworkError error );
    void retrieveInformationSslErrors( const QList<QSslError> &error );
    void albumArtDownloaded( int index );    
                  
private:
    void clearRecommendations();    
    void clearNetworkReplies();    
    void signalError();
    void constructRequest( QString &uri );     
    // retrieve URI from Ovi music server
    void retrieveInformation( const QString &urlEncoded );
    
    QString keyValues( QStringList keys, QStringList values ) const;
    void handleParsedXML();

    
signals:
    void inspireMeItemAlbumArtReady();
    void localMusicStoreRetrieved(bool storeUpdated);
    void localMusicStoreRetrievalError();
    void inspireMeItemsRetrievalError();
    void inspireMeItemsMetadataRetrieved();
            
private:    
   
    QNetworkAccessManager   	*mManager;
    QNetworkAccessManager   	*mAlbumArtDownloader;
    
    int                     	mDownloadedAlbumArts;
  
    QList<QNetworkReply *>  	mReplys;
    QList<int>                  mThumbnailRequests;
    
    QDomDocument            	mDomDocument; 
    
    QString                 	mArtist;
    QString                 	mAlbum;
    QString                 	mTitle;
    QString                 	mMusicStore;
    
    QStringList          		mRecommendationSongs;
    QStringList          		mRecommendationArtists;
    QStringList          		mRecommendationAlbumArtsLink;
    QMap<QString, HbIcon>  		mRecommendationAlbumArtsMap;
    int                     	mAlbumArtsReadyCount;
    HbIcon                 		mDefaultRecommendationAlbumArt;
       
    enum RequestType { NoRequest, InspireMeItemsMetadataRequest, InspireMeItemsAlbumArtRequest, LocalStoreRequest };
    RequestType         		mRequestType;
    int                         mRecommendationCount;
 
    QSignalMapper               *mDownloadSignalMapper;

};
#endif /* MPQUERYMANAGER_H_ */

