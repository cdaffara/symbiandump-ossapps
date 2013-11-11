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
* Description:
*
*/

// System includes
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <HbMainWindow>
#include <QTimer>
#include <QMessageBox>
#include <QComboBox>
#include <QLocalSocket>
#include <QDir>
#include <QStringListModel>
#include <QSettings>
#include <HbInstance>
#include <QLabel>

// User includes
#include "testwindow_win32.h"
#include "radioapplication.h"
#include "radiologger.h"
#include "radio_global.h"
#include "radioenginewrapper_win32_p.h"

const int WINDOW_WIDTH = 360;
const int WINDOW_HEIGHT = 640;
const int TOOLBAR_HEIGHT = 120;
const int WINDOW_EXTRA_WIDTH = 5;

const QString KBtnDisconnectHeadset = "Disconnect Headset";
const QString KBtnConnectHeadset = "Connect Headset";

const QString KBtnGoOffline = "Go Offline";
const QString KBtnGoOnline = "Go Online";

struct Song
{
    const char* mArtist;
    const char* mTitle;
};

const Song KRecognizedSongs[] = {
    { "Red Hot Chili Peppers", "Under The Bridge" },
    { "Queens Of The Stone Age", "No One Knows" },
    { "The Presidents of the United States of America", "Dune Buggy" },
    { "System of a Down", "Aerials" },
    { "The White Stripes", "Seven Nation Army" },
    { "Alice In Chains", "When The Sun Rose Again" },
    { "Bullet For My Valentine", "Tears Don't Fall" }
};
const int KSongsCount = sizeof( KRecognizedSongs ) / sizeof( KRecognizedSongs[0] );

/*!
 *
 */
Win32Window::Win32Window() :
    QWidget( 0 ),
    mOrientationButton( new QPushButton( "Change Orientation", this ) ),
    mVolUpButton( new QPushButton( "Volume Up", this ) ),
    mVolDownButton( new QPushButton( "Volume Down", this ) ),
    mAddSongButton( new QPushButton( "Add Song", this ) ),
    mClearSongButton( new QPushButton( "Clear Song", this ) ),
    mHeadsetButton( new QPushButton( KBtnDisconnectHeadset, this ) ),
    mHeadsetConnected( true ),
    mOfflineButton( new QPushButton( KBtnGoOffline, this ) ),
    mThemeBox( new QComboBox( this ) ),
    mToolbarLayout( 0 ),
    mVolume( 5 ),
    mRadioWindow( 0 ),
    mOrientation( Qt::Vertical ),
    mSongIndex( 0 )
{
    mThemeBox->setEditable( false );
    initThemes();    

    Radio::connect( mOrientationButton, SIGNAL(clicked()), this, SLOT(changeOrientation()) );
    Radio::connect( mVolUpButton, SIGNAL(clicked()), this, SLOT(volumeUp()) );
    Radio::connect( mVolDownButton, SIGNAL(clicked()), this, SLOT(volumeDown()) );
    Radio::connect( mHeadsetButton, SIGNAL(clicked()), this, SLOT(toggleHeadsetStatus()) );
    Radio::connect( mAddSongButton, SIGNAL(clicked()), this, SLOT(addSong()) );
    Radio::connect( mClearSongButton, SIGNAL(clicked()), this, SLOT(clearSong()) );
    Radio::connect( mOfflineButton, SIGNAL(clicked()), this, SLOT(toggleOffline()) );
    Radio::connect( mThemeBox, SIGNAL(activated(QString)), this, SLOT(changeTheme(QString)) );

    QTimer::singleShot( 0, this, SLOT(updateWindowSize()) );
}

/*!
 *
 */
Win32Window::~Win32Window()
{
}

/*!
 *
 */
void Win32Window::addHbWindow( HbMainWindow* radioWindow )
{
    mRadioWindow = radioWindow;
    mOrientation = mRadioWindow->orientation();
    updateWindowSize();

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    mToolbarLayout = new QGridLayout( this );
    mToolbarLayout->setHorizontalSpacing( 5 );
    mToolbarLayout->setVerticalSpacing( 5 );

    mToolbarLayout->addWidget( mOrientationButton, 0, 0 );
    mToolbarLayout->addWidget( mVolUpButton, 0, 1 );
    mToolbarLayout->addWidget( mVolDownButton, 1, 1 );
    mToolbarLayout->addWidget( mHeadsetButton, 1, 0 );
    mToolbarLayout->addWidget( mAddSongButton, 2, 0 );
    mToolbarLayout->addWidget( mClearSongButton, 2, 1 );
    mToolbarLayout->addWidget( mOfflineButton, 3, 0 );

    QGridLayout* themeLayout = new QGridLayout( this );
    themeLayout->addWidget( new QLabel( "Theme:", this ), 0, 0 );
    themeLayout->addWidget( mThemeBox, 0, 1 );
    themeLayout->setColumnStretch( 1, 2 );

    mToolbarLayout->addLayout( themeLayout, 3, 1 );
    mToolbarLayout->setColumnStretch( 0, 1 );
    mToolbarLayout->setColumnStretch( 1, 1 );

    layout->addItem( mToolbarLayout );
    layout->addWidget( radioWindow );

    setLayout( layout );
}

/*!
 *
 */
void Win32Window::init()
{
    RadioEngineWrapperPrivate* wrapper = RadioEngineWrapperPrivate::instance();
    if ( wrapper ) {
        QString error = wrapper->dataParsingError();
        if ( !error.isEmpty() ) {
            QMessageBox msg( QMessageBox::Warning, "Unable to parse radio settings", error, QMessageBox::Ok );
            msg.exec();
        }

        if ( wrapper->isOffline() ) {
            mOfflineButton->setText( KBtnGoOnline );
        }
    }
    updateWindowSize();
}

/*!
 * Private slot
 */
void Win32Window::changeOrientation()
{
    if ( mOrientation == Qt::Horizontal ) {
        mOrientation = Qt::Vertical;
    } else {
        mOrientation = Qt::Horizontal;
    }

    mRadioWindow->setOrientation( mOrientation );
    updateWindowSize();
}

/*!
 * Private slot
 */
void Win32Window::volumeUp()
{
    if ( ++mVolume > MAXIMUM_VOLUME_LEVEL ) {
        mVolume = MAXIMUM_VOLUME_LEVEL;
    }
    RadioEngineWrapperPrivate::instance()->setVolume( mVolume );
}

/*!
 * Private slot
 */
void Win32Window::volumeDown()
{
    if ( --mVolume < 0 ) {
        mVolume = 0;
    }
    RadioEngineWrapperPrivate::instance()->setVolume( mVolume );
}

/*!
 * Private slot
 */
void Win32Window::toggleHeadsetStatus()
{
    mHeadsetConnected = !mHeadsetConnected;
    if ( mHeadsetConnected ) {
        mHeadsetButton->setText( KBtnDisconnectHeadset );
    } else {
        mHeadsetButton->setText( KBtnConnectHeadset );
    }
    RadioEngineWrapperPrivate::instance()->setHeadsetStatus( mHeadsetConnected );
}

/*!
 * Private slot
 */
void Win32Window::updateWindowSize()
{
    if ( mOrientation == Qt::Horizontal ) {
        resize( WINDOW_HEIGHT + WINDOW_EXTRA_WIDTH, WINDOW_WIDTH + TOOLBAR_HEIGHT );
    } else {
        resize( WINDOW_WIDTH + WINDOW_EXTRA_WIDTH, WINDOW_HEIGHT + TOOLBAR_HEIGHT );
    }
}

/*!
 * Private slot
 */
void Win32Window::addSong()
{
    Song song = KRecognizedSongs[mSongIndex++];
    mSongIndex %= KSongsCount;

    RadioEngineWrapperPrivate::instance()->addSong( song.mArtist, song.mTitle );
}

/*!
 * Private slot
 */
void Win32Window::clearSong()
{
    RadioEngineWrapperPrivate::instance()->clearSong();
}

/*!
 * Private slot
 */
void Win32Window::toggleOffline()
{
    bool offline = !RadioEngineWrapperPrivate::instance()->isOffline();
    RadioEngineWrapperPrivate::instance()->setOffline( offline );
    if ( offline ) {
        mOfflineButton->setText( KBtnGoOnline );
    } else {
        mOfflineButton->setText( KBtnGoOffline );
    }
}

/*!
 * Private slot
 */
void Win32Window::changeTheme( const QString& theme )
{
    LOG_FORMAT( "Changing to theme %s", GETSTRING( theme ) );
    QLocalSocket socket;
    socket.connectToServer( "hbthemeserver" );
    if ( socket.waitForConnected( 3000 ) ) {
        QByteArray outputByteArray;
        QDataStream outputDataStream( &outputByteArray, QIODevice::WriteOnly );
        outputDataStream << 4; // EThemeSelection from HbThemeServerRequest in hbthemecommon_p.h
        outputDataStream << theme;
        socket.write( outputByteArray );
        socket.flush();
    }
}

/*!
 *
 */
void Win32Window::initThemes()
{
    QStringList themeList;
    foreach ( const QString& themeRootPath, themeRootPaths() ) {
        QDir dir( themeRootPath ) ;
        QStringList list = dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name );

        if ( list.contains( "themes", Qt::CaseSensitive ) ) {
            QDir root = themeRootPath;
            dir.setPath( root.path() + "/themes/icons/" );
            QStringList iconthemeslist = dir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name );
            foreach ( QString themefolder, iconthemeslist ) {
                QDir iconThemePath( root.path() + "/themes/icons/" + themefolder );
                if ( iconThemePath.exists( "index.theme" ) ) {
                    QSettings iniSetting( iconThemePath.path() + "/index.theme", QSettings::IniFormat );
                    iniSetting.beginGroup( "Icon Theme" );
                    QString hidden = iniSetting.value( "Hidden" ).toString();
                    QString name = iniSetting.value( "Name" ).toString();
                    iniSetting.endGroup();
                    if ( (hidden == "true") || ( hidden == "" ) || ( name != themefolder ) ) {
                        iconthemeslist.removeOne( themefolder );
                    }
                }
                else {
                     iconthemeslist.removeOne( themefolder );
                }

            }

            themeList.append( iconthemeslist );
        }
    }

    if ( themeList.count() == 0 ) {
        themeList.insert( 0, "hbdefault" ); //adding one default entry
        mThemeBox->setEnabled( false );
    }

    mThemeBox->setModel( new QStringListModel( themeList, mThemeBox ) );

    for ( int i = 0; i < themeList.count(); ++i ) {
        QString theme = themeList.at( i );
        if ( theme == HbInstance::instance()->theme()->name() ) {
            mThemeBox->setCurrentIndex( i );
        }
    }
}

/*!
 *
 */
QStringList Win32Window::themeRootPaths()
{
    QStringList rootDirs;
    QString envDir = qgetenv( "HB_THEMES_DIR" );
    if ( !envDir.isEmpty() ) {
        rootDirs << envDir;
    }

    QString resourcesDir = HB_RESOURCES_DIR;
    if ( resourcesDir.isEmpty() ) {
        resourcesDir = "/hb_dev/src/hbcore/resources";
    }
    rootDirs << resourcesDir;

    return rootDirs;
}
