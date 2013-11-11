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
* Description: MpMainWindow implementation.
*
*/


#include <hbapplication.h>
#include <hbview.h>
#include <mpxviewpluginqt.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>
#include <xqsharablefile.h>
#include <afactivitystorage.h>
#include <afactivation.h>

#include "mpmainwindow.h"
#include "mpviewbase.h"
#include "musicservices.h"
#include "mpenginefactory.h"
#include "mpsettingsmanager.h"
#include "mpglobalpopuphandler.h"
#include "mpmediacontroller.h"
#include "mptrace.h"

const QString MUSIC_MAIN_VIEW = "MusicMainView";
const QString MUSIC_NOW_PLAYING_VIEW = "MusicNowPlayingView";

/*!
   \class MpMainWindow
   \brief The MpMainWindow class provides Main Window functionalities.

   MpMainWindow handles activation of views

*/

/*!
    Constructs the main winsdow.
 */

MpMainWindow::MpMainWindow()
    : MpxViewFramework(),
      mCollectionViewPlugin(0),
      mPlaybackViewPlugin(0),
      mSettingsViewPlugin(0),
      mDetailsViewPlugin(0),
      mMediaWallViewPlugin(0),
      mCurrentViewPlugin(0),
      mVerticalViewType( CollectionView ),
      mPreviousVerticalViewType( NoView ),
      mMusicServices(0),
      mPopupHandler(0),
      mUserExit( false ),
      mMpMediaController(0),
      mActivityStorage(0)
{
    TX_LOG
}

/*!
 Destructs the main window.
 */
MpMainWindow::~MpMainWindow()
{
    TX_ENTRY
    if(mMusicServices){
        delete mMusicServices;
    }
    
    if (mCollectionViewPlugin) {
        mCollectionViewPlugin->destroyView();
        delete mCollectionViewPlugin;
    }
    if (mPlaybackViewPlugin) {
        mPlaybackViewPlugin->destroyView();
        delete mPlaybackViewPlugin;
    }
    if (mSettingsViewPlugin) {
        mSettingsViewPlugin->destroyView();
        delete mSettingsViewPlugin;
    }
    if (mDetailsViewPlugin) {
        mDetailsViewPlugin->destroyView();
        delete mDetailsViewPlugin;
    }
    if (mMediaWallViewPlugin) {
        mMediaWallViewPlugin->destroyView();
        delete mMediaWallViewPlugin;
    }

    delete mMpMediaController;
    delete mActivityStorage;

    MpEngineFactory::close();


    TX_EXIT
}


/*!
Initialize and activate the collection view
 */
void MpMainWindow::initialize()
{
    TX_ENTRY

#ifdef _DEBUG
    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);
    TX_LOG_ARGS("Available plugins:");
    for ( int i = 0; i < impls.size(); i++ ) {
        TX_LOG_ARGS("Plugin " << i << ": " << impls[i].uid() << ", " << impls[i].dllName() );
    }
#endif // _DEBUG

    if ( XQServiceUtil::isService() ) {
        // Music Service mode
        // Set the Collection View and Playback View to fetcher mode
        mMusicServices = new MusicServices();
        int err = connect(mMusicServices, SIGNAL(serviceActive( quint32 )), this, SLOT(initializeServiceView( quint32 )));
        TX_LOG_ARGS("connection error: " << err);
        XQServiceUtil::toBackground( false );
    }
    AfActivation *activation = new AfActivation( this );
    

    mActivityStorage = new AfActivityStorage;
    if ( !mMusicServices ) {
        QVariantHash params = activation->parameters();
        ActivityMode mode = !params.value( "activityname" ).toString().compare( MUSIC_NOW_PLAYING_VIEW ) ? MpMainWindow::MusicNowPlayingView : MpMainWindow::MusicMainView; 
        
        MpEngineFactory::createSharedEngine();
        mPopupHandler = new MpGlobalPopupHandler( this );
        if( activation->reason() == Af::ActivationReasonActivity ) {
            // Restoring an activity, not a fresh startup or a service
            // Activities from Task switcher only have one parameter
            if( params.count() == 1 ) {
                 loadActivity( mActivityStorage->activityData( activation->name() ) );
            }
        }
        if ( orientation() == Qt::Vertical ) {
            // If first startup ignore shuffleAll and send to collection view to refresh library
            if ( mode == MusicMainView  || MpSettingsManager::firstStartup() ) {
                loadView( CollectionView );
                activateView( CollectionView );
                loadView( MediaWallView );
                loadView( PlaybackView );
            } else if ( mode == MusicNowPlayingView ) {
                if( params.contains( "shuffle" ) ) {
                    if( !params.value( "shuffle" ).toString().compare( "yes" ) ) {
                        MpEngineFactory::sharedEngine()->shuffleAll();
                    }
                }
                loadView( PlaybackView );
                activateView( PlaybackView );
                loadView( CollectionView );
                loadView( MediaWallView );
            }
        }
        else {
            // If first startup ignore shuffleAll and send to refresh library
            if( mode == MusicNowPlayingView && !MpSettingsManager::firstStartup() ) {
                if( params.contains( "shuffle" ) ) {
                    if( !params.value( "shuffle" ).toString().compare( "yes" ) ) {
                        MpEngineFactory::sharedEngine()->shuffleAll();
                    }
                }
                mVerticalViewType = PlaybackView;
            }
            loadView( MediaWallView );
            activateView( MediaWallView );
            loadView( CollectionView );
            loadView( PlaybackView );
        }
        connect(this, SIGNAL( orientationChanged( Qt::Orientation ) ), SLOT( switchView( Qt::Orientation ) ) );
        connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryUpdated() ), SLOT( handleLibraryUpdated() ) );
        MpEngineFactory::sharedEngine()->checkForSystemEvents();
        //Register to application manager to wait for activities and clear previous activities on the task switcher
        qRegisterMetaType<Af::ActivationReason>( "Af::ActivationReason" );
        connect( activation, SIGNAL( activated( Af::ActivationReason, QString, QVariantHash ) ), this, SLOT( loadActivityData( Af::ActivationReason, QString, QVariantHash ) ) );
        connect( MpEngineFactory::sharedEngine(), SIGNAL( restorePathFailed() ), this, SLOT( handleRestorePathFailed() ) );
        mActivityStorage->removeActivity( MUSIC_MAIN_VIEW );
        mActivityStorage->removeActivity( MUSIC_NOW_PLAYING_VIEW );
        connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( saveActivity() ) );
        mMpMediaController = new MpMediaController();
        emit applicationReady();
        
    }
    else {
        setOrientation(Qt::Vertical, true);//This sould prevent media wall activation.
    }
    //since we only have one landscape view, media wall, disable automatic 
    //orientation transitions, they look like a flicker.
    //This will make the view switch faster.
    setAutomaticOrientationEffectEnabled( false );    
    TX_EXIT
}

/*!
 Slot to be called to handle the \a commandCode.
 */
void MpMainWindow::handleCommand( int commandCode )
{
    TX_ENTRY_ARGS("commandCode=" << commandCode );
    
    switch ( commandCode ) {
        case MpCommon::Exit:
            if ( mCurrentViewPlugin ) {
                disconnectView();
            }
            mUserExit = true;
            qApp->quit();
            break;
        case MpCommon::SendToBackground:
            lower();
            break;
        case MpCommon::ActivateCollectionView:
            activateView( CollectionView );
            break;
        case MpCommon::ActivatePlaybackView:
            activateView( PlaybackView );
            break;
        case MpCommon::ActivateSettingsView:
            activateView( SettingsView );
            break;
        case MpCommon::ActivateDetailsView:
            activateView( DetailsView );
            break;
        case MpCommon::ActivatePreviousView:
            if ( orientation() == Qt::Vertical 
                    && mPreviousVerticalViewType != NoView ) {
                activateView( mPreviousVerticalViewType );
            }
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called to switch view based on \a orientation.
 */
void MpMainWindow::switchView( Qt::Orientation orientation )
{
        if (orientation == Qt::Vertical) {
            activateView( mVerticalViewType );   
        }
        else {
            activateView( MediaWallView );
        }
}

/*!
 Activate the \a viewType view.
 */
void MpMainWindow::activateView(MpMainWindow::ViewType viewType)
{
    TX_ENTRY_ARGS("viewType=" << viewType );

    bool doTransition = true;
    Hb::ViewSwitchFlags  transitionFlags = Hb::ViewSwitchCachedFullScreen;
    
    if ( mCurrentViewPlugin ) {     
        disconnectView();
        mCurrentViewPlugin->deactivateView();
        if ( viewType  == MediaWallView || mCurrentViewPlugin == mMediaWallViewPlugin ) {
            doTransition = false;
        }
        else if ( viewType  == CollectionView && mCurrentViewPlugin == mPlaybackViewPlugin ||
                  viewType  == PlaybackView && mCurrentViewPlugin == mDetailsViewPlugin ||
                  viewType  == PlaybackView && mCurrentViewPlugin == mSettingsViewPlugin ) {
            transitionFlags |= Hb::ViewSwitchUseBackAnim;
        }
        mCurrentViewPlugin = 0;
    }

    // loadView will not reload the view if is already loaded.
    mCurrentViewPlugin = loadView( viewType );
    Q_ASSERT( mCurrentViewPlugin );

    if ( mCurrentViewPlugin ) {
        if ( viewType != MediaWallView ) {
            //storing previous vertical view type only if it is not the same
            //view, this to be able to keep track of the previously activated 
            //view excluding media wall switching.
            if ( mVerticalViewType != viewType ) {
                mPreviousVerticalViewType = mVerticalViewType;
            }
            mVerticalViewType = viewType;
        }
        addView( reinterpret_cast<HbView*>( mCurrentViewPlugin->getView() ) );
        setCurrentView( reinterpret_cast<HbView*>( mCurrentViewPlugin->getView() ) , doTransition , transitionFlags);
        mCurrentViewPlugin->activateView();
        connectView();
    }
    TX_EXIT
}


/*!
 Connect the signals to the current active view.
 */
void MpMainWindow::connectView()
{
    TX_ENTRY

	QObject::connect(mCurrentViewPlugin,
					 SIGNAL(command(int)),
					 this,
					 SLOT(handleCommand(int)));
    TX_EXIT
}

/*!
 Disonnect the signals from the current active view.
 */
void MpMainWindow::disconnectView()
{
    TX_ENTRY

	QObject::disconnect(mCurrentViewPlugin,
					 SIGNAL(command(int)),
					 this,
					 SLOT(handleCommand(int)));
    TX_EXIT
}

/*!
 Slot to be called when Library has changed.
 */
void MpMainWindow::handleLibraryUpdated()
{
    TX_ENTRY

    // Data might have changed, so other views than Collection or MediaWall are not valid any more.
    if ( mCurrentViewPlugin == mMediaWallViewPlugin ) {
        if ( mVerticalViewType != CollectionView ) {
            mVerticalViewType = CollectionView;
        }
    }
    else if ( mCollectionViewPlugin && mCurrentViewPlugin != mCollectionViewPlugin ) {
        activateView( CollectionView );

    }

    TX_EXIT
}

/*!
 Reimp.
 */
void 	MpMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    //RND feature to rotate on emulator.
#ifdef __WINS__   
    case 16842753:
    case Qt::Key_Call:
        if (orientation () == Qt::Vertical) {
            setOrientation(Qt::Horizontal, false);
        }
        else {
            setOrientation(Qt::Vertical, false);
        }
        break;
#endif // __WINS__
    default:
        HbMainWindow::keyPressEvent (event);
        break;          
    }
}

/*!
  Slot to initialize the view that corresponds to the requested service  
 */
void MpMainWindow::initializeServiceView( quint32 clientSecureId )
{
    switch (mMusicServices->currentService()) {
 
    case MusicServices::EUriFetcher:
        {
            MpEngineFactory::createSharedEngine( clientSecureId , MpEngine::Fetch );
            mPopupHandler = new MpGlobalPopupHandler( this );
            loadView( CollectionView, MpCommon::FetchView );
            MpViewBase* collectionView = reinterpret_cast<MpViewBase*>(mCollectionViewPlugin->getView());
            connect(collectionView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            activateView( CollectionView );           
            loadView(PlaybackView, MpCommon::FetchView );
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(playbackView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryUpdated() ), this, SLOT( handleLibraryUpdated() ) );
            MpEngineFactory::sharedEngine()->checkForSystemEvents();
            break;
        }
    case MusicServices::EPlayback:
        {
            MpEngineFactory::createSharedEngine( clientSecureId , MpEngine::Embedded );
            loadView(PlaybackView, MpCommon::EmbeddedView );
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(mMusicServices, SIGNAL(playReady(QString)), MpEngineFactory::sharedEngine(), SLOT(playEmbedded(QString)));
            connect(mMusicServices, SIGNAL(playReady(const XQSharableFile&)), MpEngineFactory::sharedEngine(), SLOT(playEmbedded(const XQSharableFile&)));
            connect(playbackView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            activateView( PlaybackView );
            break;
        }
    default:
        Q_ASSERT_X(false, "MpMainWindow::initializeServiceView", "undefined service");
        break;
    }
    mMpMediaController = new MpMediaController();
    emit applicationReady();
}

/*!
  Slot to handle activity switching once the stand alone instance is running and registered 
  in the activity manager to wait for activities.
  Only running activity supported at the moment is "MusicNowPlayingView"
 */

void MpMainWindow::loadActivityData( Af::ActivationReason reason, const QString &name, QVariantHash parameter )
{
    TX_ENTRY
    if( reason == Af::ActivationReasonActivity ) {
        QString activityId = name;
        QVariantHash params = parameter;
        if( !activityId.compare( MUSIC_NOW_PLAYING_VIEW ) ) {
            if( params.contains( "shuffle" ) ) {
                if( !params.value( "shuffle" ).toString().compare( "yes" ) ) {
                    MpEngineFactory::sharedEngine()->shuffleAll();
                }
            }
            if ( orientation() == Qt::Vertical ) {
                if( mVerticalViewType != PlaybackView ) {
                    activateView( PlaybackView );
                }
            }
        }
    }
    TX_EXIT
}

/*!
  Slot to save activity upon exiting application
 */
void MpMainWindow::saveActivity()
{
    TX_ENTRY
    QString activity = ( mVerticalViewType == CollectionView ) ? MUSIC_MAIN_VIEW : MUSIC_NOW_PLAYING_VIEW;
    //Get data from engine
    QByteArray serializedRestorePath;
    if ( mUserExit ) {
        //Internal exit will always return to the main view
        activity = MUSIC_MAIN_VIEW;
    }
    else {
        //Only saved data if exited via task switcher or GOOM
        MpEngineFactory::sharedEngine()->saveActivityData( serializedRestorePath );
    }
    QVariantHash activityParameters;
    activityParameters.insert( "screenshot", QPixmap::grabWidget( this, this->rect() ) );
    QVariantHash activityData;
    activityData.insert( "restorePath", serializedRestorePath );
       
    
    bool ok = mActivityStorage->saveActivity( activity , activityData, activityParameters );
    if ( !ok ){
        TX_LOG_ARGS( "Error: Add Failed; should never get here." );
    }
    TX_EXIT
}

/*!
  Slot to handle a failed path restoration. Switch to collection view if not already there.
 */
void MpMainWindow::handleRestorePathFailed()
{
    TX_ENTRY
    if ( orientation() == Qt::Vertical ) {
        if( mVerticalViewType != CollectionView ) {
            activateView( CollectionView );
        }
    }
    TX_EXIT
}

/*!
  loads a view if type \a  type, return the view plugin, if view is already loaded it will not reload the view.
 */
MpxViewPlugin* MpMainWindow::loadView( ViewType type, MpCommon::MpViewMode viewMode )
{
    MpxViewPlugin** plugin = 0;
    long int pluginUid = 0;    
    switch (type) { 
    case CollectionView:
        pluginUid = MpCommon::KCollectionViewUid;
        plugin = &mCollectionViewPlugin;
        break;
    case PlaybackView:
        pluginUid = MpCommon::KPlaybackViewUid;
        plugin = &mPlaybackViewPlugin;
        break;
    case SettingsView:
        pluginUid = MpCommon::KSettingsViewUid;
        plugin = &mSettingsViewPlugin;
        break;
    case DetailsView:
        pluginUid = MpCommon::KDetailsViewUid;
        plugin = &mDetailsViewPlugin;
        break;
    case MediaWallView:
        pluginUid = MpCommon::KMediaWallViewUid;
        plugin = &mMediaWallViewPlugin;
        break;
    default:
        Q_ASSERT_X(false, "MpMainWindow::loadView", "undefined view type");
        break;
    }
    Q_ASSERT( plugin && pluginUid);
    if ( plugin && !(*plugin) ) {
        //plugin was not loded before, loding plugin.
        XQPluginLoader pluginLoader( pluginUid );
        QObject* pluginInstance = pluginLoader.instance();
        (*plugin) = qobject_cast<MpxViewPlugin*>( pluginInstance )->viewPlugin();
        Q_ASSERT( *plugin );
        
        if ( *plugin ) {
            (*plugin)->createView();
            MpViewBase* view = reinterpret_cast<MpViewBase*>( (*plugin)->getView() );
            view->setTitle( mMusicServices ? mMusicServices->contextTitle() : hbTrId("txt_mus_title_music"));
            view->setViewMode( viewMode );
        }
    }
    Q_ASSERT( plugin ? *plugin : 0 );
    return plugin ? *plugin : 0;
}

/*!
  Load activity called via task switcher
  Restore view and playback path if applicable
 */
void MpMainWindow::loadActivity( QVariant data )
{
    TX_ENTRY
    QVariantHash activityData = data.toHash();
    QByteArray serializedRestorePath = activityData.value( "restorePath" ).toByteArray();
    MpEngineFactory::sharedEngine()->loadActivityData( serializedRestorePath );
    TX_EXIT
}

