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
* Description: Unit Test for mpquerymanager.
*
*/

#include <QDebug>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxyFactory>
#include <qmobilityglobal.h>
#include <QNetworkSession>
#include <QDomElement>
#include <QList>
#include <QFile>
#include <QUrl>
#include <QSslError>

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <HbApplication>
#include "unittest_mpquerymanager.h"
#include "mpcommondefs.h"

// Do this so we can access all member variables.
#define private public
#include "mpquerymanager.h"
#undef private



/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpQueryManager tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_testmpquerymanager.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpQueryManager::TestMpQueryManager()
    : mTest(0)
{

}

//Destructor
TestMpQueryManager::~TestMpQueryManager()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpQueryManager::initTestCase()
{
    
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpQueryManager::cleanupTestCase()
{

}

/*!
 Called before each testfunction is executed.
 */
void TestMpQueryManager::init()
{
    mTest = new MpQueryManager();
}

/*!
 Called after every testfunction.
 */
void TestMpQueryManager::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*
void TestMpQueryManager::testrecommendationSongs()
{
    // TODO: when this test gets executed, it crashes at cleanup() where QueryManager destructor
    // refers to the QNetworkAccessManager instances. Fishy.. Do a deaper investigation    
    QStringList recommendedSongs;
    QStringList recommendedSongsRecd;
    
    QString recommendedSong1("Song1");
    QString recommendedSong2("Song2");
        
    recommendedSongs.append(recommendedSong1);
    recommendedSongs.append(recommendedSong2);
    
    mTest->mRecommendationSongs.append(recommendedSongs);
    
    recommendedSongsRecd << mTest->recommendationSongs();
    
    QCOMPARE(recommendedSongs,recommendedSongsRecd);
}

*/
void TestMpQueryManager::testrecommendationArtists()
{
    QStringList recommendedArtists;
    QStringList recommendedSongs;
    QStringList recommendedArtistsRecd;
        
    QString recommendedArtist1("Artist1");
    QString recommendedArtist2("Artist2");
    QString song1("Song1");
    QString song2("Song2");
           
    recommendedArtists.append(recommendedArtist1);
    recommendedArtists.append(recommendedArtist2);
    
    recommendedSongs.append(song1);
    recommendedSongs.append(song2);
        
    mTest->mRecommendationArtists.append(recommendedArtists);
    mTest->mRecommendationSongs.append(recommendedSongs);
    
    for(int i = 0; i < mTest->recommendationsCount(); ++i) {
        recommendedArtistsRecd << mTest->recommendedArtist(i);
    }    
        
    QCOMPARE(recommendedArtists,recommendedArtistsRecd);
}

/*
void TestMpQueryManager::testrecommendationAlbumArtsLink()
{
    // TODO: when this test gets executed, it crashes at cleanup() where QueryManager destructor
    // refers to the QNetworkAccessManager instances. Fishy.. Do a deaper investigation    
    QStringList recommendedAlbumArtsLink;
    QStringList recommendedAlbumArtsLinkRecd;
        
    QString recommendedAlbumArtsLink1("AlbumArtsLink1");
    QString recommendedAlbumArtsLink2("AlbumArtsLink2");
            
    recommendedAlbumArtsLink.append(recommendedAlbumArtsLink1);
    recommendedAlbumArtsLink.append(recommendedAlbumArtsLink2);
        
    mTest->mRecommendationAlbumArtsLink.append(recommendedAlbumArtsLink);
        
    recommendedAlbumArtsLinkRecd << mTest->recommendationAlbumArtsLink();
        
    QCOMPARE(recommendedAlbumArtsLink,recommendedAlbumArtsLinkRecd);
}

*/

