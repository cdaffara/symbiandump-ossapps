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
#include <QPixmap>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbmarqueeitem.h>
#include <hbnotificationdialog.h>
#include <QSettings>
#include <QFontMetricsF>
#include <QCoreApplication>
#include <HbColorScheme>
#include <HbEvent>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irqisdsdatastructure.h"
#include "irqmetadata.h"
#include "irqisdsclient.h"
#include "irqfavoritesdb.h"
#include "irlastplayedstationinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqstatisticsreporter.h"
#include "irqenums.h"
#include "irmediakeyobserver.h"
#include "iruidefines.h"
#include "irnowplayingview.h"
#include "irstationshare.h"
#include "irqutility.h"
#include "irqlogger.h"
#include "irplaylist.h"
#include "irstationdetailsview.h"
#include "irservicedef.h"
#include "irqsettings.h"
#include "irqlogger.h"

#ifdef ADV_ENABLED
#include <QTimer>
#include <qgraphicssceneevent.h>
static const QString KDefaultAdvLink(""); // default advertisement link
#endif

const int KNowPlayingLogoSize = 300; // Now playing logo size

static const QString KDefaultStationLogo("qtg_large_internet_radio");
static const QString KPlayButtonIcon("qtg_mono_play");
static const QString KStopButtonIcon("qtg_mono_stop");

static const QString KArtistColor("qtc_lcd_content_normal");
static const QString KSongColor("qtc_lcd_content_normal");
static const QString KStationColor("qtc_lcd_content_normal");

#ifdef STATISTIC_REPORT_TEST_ENABLED    
static const int KDummySongRecognitionUid   = 0xE4EF7D71;
static const int KIRMusicStoreUid           = 0xE609761B;
#else
static const int KIRMusicStoreUid           = 0;
#endif

static void saveStationLogo(const QPixmap &aStationLogo);

/*
 * Description : constructor
 */
IRNowPlayingView::IRNowPlayingView(IRApplication* aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId),
    iStatisticsReporter(NULL),
    iStationShare(NULL),
    iPlayStopAction(NULL),
    iLogoDownloadState(EIdle),    
    iSongNameLabel(NULL),
    iSongNameMarquee(NULL),
    iArtistName(NULL),
    iStationName(NULL),
    iStationLogo(NULL),
    iLogoNeedUpdate(true),    
    iFindinNmsAllowed(false),
    iSongNameAvailable(false)
#ifdef ADV_ENABLED
    ,iAdvImage(NULL)
    ,iAdvImageNeedUpdate(true)
    ,iAdvUrl(KDefaultAdvLink)    
#endif
{
    LOG_METHOD;
    
    iLaunchActionNeeded = (0 == getViewManager()->views().count()); // this is the starting view
        
    initialize();
    
    iStatisticsReporter = IRQStatisticsReporter::openInstance();
        
    connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(updateMetaData(IRQMetaData*)));
    connect(iPlayController, SIGNAL(playingStarted()), this, SLOT(handlePlayStarted()));
    connect(iPlayController, SIGNAL(playingStopped()), this, SLOT(handlePlayStopped()));

    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(handleNetworkEvent(IRQNetworkEvent)));
    connect(iApplication->getMediaKeyObserver(), SIGNAL(playPausePressed()), 
            this, SLOT(handlePlayPauseMediaKey()));
    connect(iApplication->getMediaKeyObserver(), SIGNAL(stopPressed()), 
            this, SLOT(handleStopMediaKey()));
    connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
             this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
    
    setFlag(EViewFlag_StickyViewEnabled);
}

/*
 * Description : destructor
 */
IRNowPlayingView::~IRNowPlayingView()
{
    LOG_METHOD;
    if (iStatisticsReporter)
    {
        iStatisticsReporter->closeInstance();
    }
    
    delete iStationShare;        
}


/*
 * Description: initializing.
 */
void IRNowPlayingView::initialize()
{
    LOG_METHOD;
    setObjectName(NOW_PLAYING_VIEW_OBJECT_NAME);
    QObjectList roots;
    roots.append(this);
    iLoader.setObjectTree(roots);

    // Load XML file
    iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME);
    
    initMenu();
    initToolBar();
    initWidget();    
#ifdef SUBTITLE_STR_BY_LOCID
    setTitle(hbTrId("txt_irad_title_internet_radio"));
#else
    setTitle("Internet radio");      
#endif
    handleOrientationChanged(getViewManager()->orientation());
}

/*
 * Description : create menu for now playing view
 */
void IRNowPlayingView::initMenu()
{
    LOG_METHOD;
    if (iApplication->getSettings()->getIdentifySongEnabled())
    {
        int songRecognitionAppUid = iApplication->getSettings()->getSongRecognitionAppUid();
        if(IRQUtility::findAppByUid(songRecognitionAppUid))
        {
            LOG("Song Recognition is Available");
            iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_YES_SEC);
        }
        else
        {
            LOG("Song Recognition is NOT Available");
            iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_NO_SEC);
        }
    }
    else
    {
        LOG("Song Recognition is Disabled");
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_NO_SEC);
    }
    
    HbAction *openWebAddressAction = qobject_cast<HbAction *> (iLoader.findObject(GO_TO_STATION_ACTION));
    HbAction *shareStationAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_SHARE_STATION_ACTION));
    HbAction *settings = qobject_cast<HbAction *> (iLoader.findObject(SETTINGS_ACTION));
    HbAction *exitAction = qobject_cast<HbAction *> (iLoader.findObject(EXIT_ACTION));
    HbAction *songRecAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_IDENTIFYSONG));

    connect(openWebAddressAction, SIGNAL(triggered()), this, SLOT(handleGoToStationAction()));
    connect(shareStationAction, SIGNAL(triggered()), this, SLOT(handleShareStationAction()));
    connect(settings, SIGNAL(triggered()), this, SLOT(handleSettingAction()));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));  
    connect(songRecAction, SIGNAL(triggered()), this, SLOT(handleIdentifySongAction()));
    
    if(iApplication->isEmbeddedInstance())
    {
        HbMenu *viewMenu = menu();
        disconnect(openWebAddressAction, SIGNAL(triggered()), this, SLOT(handleGoToStationAction()));
        disconnect(settings, SIGNAL(triggered()), this, SLOT(handleSettingAction()));
        disconnect(songRecAction, SIGNAL(triggered()), this, SLOT(handleIdentifySongAction()));
        viewMenu->removeAction(openWebAddressAction);
        viewMenu->removeAction(settings);
        viewMenu->removeAction(songRecAction);
    }
}

/*
 * Description : create tool bar for now playing view
 */
void IRNowPlayingView::initToolBar()
{
    LOG_METHOD;
    HbAction *musicStoreAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_MUSICSTORE));
    iPlayStopAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_PLAYSTOP));
    HbAction *add2FavAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_ADDTOFAV));
    HbAction *flipAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_FLIP));

    connect(musicStoreAction, SIGNAL(triggered()), this, SLOT(handleMusicStoreAction()));
    connect(iPlayStopAction, SIGNAL(triggered()), this, SLOT(handlePlayStopAction()));
    connect(add2FavAction, SIGNAL(triggered()), this, SLOT(handleAddToFavAction()));
    connect(flipAction, SIGNAL(triggered()), this, SLOT(handleDetailInfoAction()));
}

void IRNowPlayingView::initWidget()
{
    LOG_METHOD;
    iStationLogo = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_STATION_LOGO));
    iSongNameLabel = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_SONG_NAME_LABEL));
    iSongNameMarquee = qobject_cast<HbMarqueeItem *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_SONG_NAME_MARQUEE));
    iSongNameMarquee->setLoopCount(-1);
    iArtistName = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_ARTIST_NAME));
    iStationName = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_STATION_NAME));   
    iStationLogo->setIcon(HbIcon(KDefaultStationLogo));   
    iStationName->setPlainText("");
    updateSongName(QString(""));
    iArtistName->setPlainText("");    
    
    connect(iSongNameLabel, SIGNAL(geometryChanged()), this, SLOT(handleGeometryChanged()));
    
    iArtistName->setTextColor(HbColorScheme::color(KArtistColor));
    iSongNameLabel->setTextColor(HbColorScheme::color(KSongColor));
    iSongNameMarquee->setTextColor(HbColorScheme::color(KSongColor));
    iStationName->setTextColor(HbColorScheme::color(KStationColor));
 
#ifdef ADV_ENABLED
    iAdvImage = qobject_cast<HbLabel *> (iLoader.findObject( NOW_PLAYING_VIEW_OBJECT_ADVERTISEMENT_IMAGE));
    iAdvImage->setIcon(HbIcon(KDefaultStationLogo));
#endif    
}

/*
 * Description : update station name and category according to the information in preset
 */
void IRNowPlayingView::updateWidgets()
{
    LOG_METHOD;
    if (iLaunchActionNeeded) // if lunch as starting view, leave the update action to launchAction()
    {
        return;
    }
    
    if(iPlayController->getNowPlayingPreset())
    {
        iStationName->setPlainText(iPlayController->getNowPlayingPreset()->nickName);
    }
    loadStationLogo();
}

void IRNowPlayingView::updateSongName(const QString &aSongName)
{
    LOG_METHOD;
    iSongNameMarquee->stopAnimation();	
    iSongNameLabel->hide();
    iSongNameMarquee->hide();
    
    iSongNameLabel->setPlainText(aSongName);
    iSongNameMarquee->setText(aSongName);
        
    QFontMetricsF metrics(iSongNameLabel->font());
    LOG_FORMAT("song name = %s, song name length = %f, label width = %f",
                STRING2CHAR(aSongName), 
                metrics.boundingRect(aSongName).width(),
                iSongNameLabel->contentsRect().width()
                );
    if (metrics.boundingRect(aSongName).width() > iSongNameLabel->contentsRect().width())
    {
        iSongNameMarquee->show();
        iSongNameMarquee->startAnimation();
    }
    else
    {
        iSongNameLabel->show();
    }
}

void IRNowPlayingView::updateMusicStoreStatus()
{
    LOG_METHOD;
    IRQPreset *preset = iPlayController->getNowPlayingPreset();
    if(preset && (0 == preset->musicStoreStatus.compare("yes",Qt::CaseInsensitive)))
    {
        iFindinNmsAllowed = true;
    }
    else
    {
        iFindinNmsAllowed = false;
    }
}


/*
 * Description : Update logo info
 */
void IRNowPlayingView::updateStationLogo()
{    
    LOG_METHOD;
    if( !iLogoNeedUpdate
        || iLogoDownloadState != EIdle )
    {
        return;
    }
    
    if( !iNetworkController->getNetworkStatus() )
    {
        // when launch as starting view, reach here.
        // but later, will be invoked again via handlePlayStarted()
        return;
    }    

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    if( 1 == nowPlayingPreset->type )
    {
        // isds server channel
        iLogoDownloadState = EDownloadLogo; 
        iIsdsClient->isdsLogoDownSendRequest(nowPlayingPreset, 2, KNowPlayingLogoSize, KNowPlayingLogoSize);    
    }
    else
    {
        iLogoDownloadState = EIdle;
        iLogoNeedUpdate = false; 
#ifdef ADV_ENABLED        
        iAdvImageNeedUpdate = false;  
#endif             
    }
}

#ifdef ADV_ENABLED
/*
 * Description : Update advertisement image
 */
void IRNowPlayingView::updateAdvImage()
{
    LOG_METHOD;
    if( !iAdvImageNeedUpdate
        || iLogoDownloadState != EIdle )
    {
        return;
    }

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    if( (1 == nowPlayingPreset->type)  // isds server channel
        && (nowPlayingPreset->advertisementInUse.size() > 0) )
    {
        IRQPreset advPreset;
        advPreset.imgUrl = nowPlayingPreset->advertisementInUse;
        advPreset.type = 1;
        iLogoDownloadState = EDownLoadAdvImage;        
        iIsdsClient->isdsLogoDownSendRequest(&advPreset);
    }
    else
    {
        iLogoNeedUpdate = false;
        iAdvImageNeedUpdate = false;
        iLogoDownloadState = EIdle;
    }
}
#endif

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRNowPlayingView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    LOG_METHOD;
	LOG_FORMAT("aCommand = %d", (int)aCommand);
    
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;

    switch (aCommand)
    {
    case EIR_ViewCommand_TOBEACTIVATED:       
        handleOrientationChanged(getViewManager()->orientation());                 
        updateMusicStoreStatus();
        updateWidgets();
        ret = EIR_NoDefault;
        break;

    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(handleLogoDownloaded(IRQPreset* )));
        updateStationLogo();
        getViewManager()->saveScreenShot();
        break;
        
    case EIR_ViewCommand_DEACTIVATE:
        if( iLogoDownloadState != EIdle )
        {
            // cancel downloading logo
            iIsdsClient->isdsLogoDownCancelTransaction();
            iLogoDownloadState = EIdle;            
        }        
        disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
                   this, SLOT(handleLogoDownloaded(IRQPreset* )));
        ret = EIR_NoDefault;
        break;
    }

    return ret;
}

/*
 * Description : actions when view is launched.
 */
void IRNowPlayingView::launchAction()
{      
    LOG_METHOD;
    setUseNetworkReason(EIR_UseNetwork_StartingView);
    updateForLauchAction();
#ifdef HS_WIDGET_ENABLED	
    iPlayController->setConnectingStationName(iStationName->plainText());
#endif	
    iApplication->verifyNetworkConnectivity();
    if( iApplication->isEmbeddedInstance())
    {
        getViewManager()->pushViewById(EIRView_PlsView);        
    }
    else
    {
        getViewManager()->pushViewById(EIRView_MainView);         
    }
    
    iLaunchActionNeeded = false;
}


void IRNowPlayingView::updateForLauchAction()
{
    LOG_METHOD;
    //nowplaying view as starting view can have two cases : one is for last played station, the other is for 
    //the only one item in play list file    
    IRPlayList *playList = iApplication->getPlayList();
    if (playList && 1 == playList->getNumberOfEntries())
    {
        IRQPreset *preset = playList->getPresetForEntry(0);
        iStationName->setPlainText(preset->nickName);
        iFindinNmsAllowed = (0 == preset->musicStoreStatus.compare("yes",Qt::CaseInsensitive));
#ifdef HS_WIDGET_ENABLED            
        iPlayController->reloadNowplayingPreset(preset,false,EIRQAdhocExternal);
#endif
        iStationLogo->setIcon(HbIcon(KDefaultStationLogo));
        iLogoNeedUpdate = true;            
#ifdef ADV_ENABLED
        iAdvUrl = KDefaultAdvLink; 
        iAdvImage->setIcon(logoHbIcon);
        iAdvImageNeedUpdate = true;  
#endif
        delete preset;
    }
    else
    {
        IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();
        if( lastPlayedStationInfo )
        {
            IRQPreset *lastPreset = lastPlayedStationInfo->getLastPlayedStation();
            if (lastPreset)
            {
                iStationName->setPlainText(lastPreset->nickName); 
                iFindinNmsAllowed = (0 == lastPreset->musicStoreStatus.compare("yes",Qt::CaseInsensitive));
            }
            else
            {
                iStationName->setPlainText(QString("")); 
                iFindinNmsAllowed = false;
            }             
        }
        loadStationLogo();
    }
}

/********************************************************************************************************
 * SLOTs for Logo Download
 *******************************************************************************************************
 */
void IRNowPlayingView::handleLogoDownloaded(IRQPreset* aPreset)
{
    LOG_METHOD;
    if( EIdle == iLogoDownloadState )
    {        
        return;
    } 
            
    if( NULL == aPreset )
    {        
        if( EDownloadLogo == iLogoDownloadState )
        {         
            iLogoNeedUpdate = false;  
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif            
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            iAdvImageNeedUpdate = false;            
        }
#endif
        iLogoDownloadState = EIdle;      
        return;
    }
    
 
    
    QPixmap logoPixmap;
    if( logoPixmap.loadFromData((const unsigned char*)aPreset->logoData.constData(), aPreset->logoData.size()) )
    {
        if( EDownloadLogo == iLogoDownloadState )
        {		
            saveStationLogo(logoPixmap);
            QPixmap newLogoPixmap = 
                 logoPixmap.scaled(QSize(KNowPlayingLogoSize,KNowPlayingLogoSize),Qt::KeepAspectRatio);
            QIcon logoQIcon(newLogoPixmap);
            HbIcon logoHbIcon(logoQIcon);
            iStationLogo->setIcon(logoHbIcon);
            iPlayController->emitStationLogoUpdated(true);
            iLogoNeedUpdate = false;          
            getViewManager()->saveScreenShot();
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            QIcon logoQIcon(logoPixmap);
            HbIcon logoHbIcon(logoQIcon);            
            iAdvImage->setIcon(logoHbIcon); 
            iAdvUrl = iPlayController->getNowPlayingPreset()->advertisementUrl;  
            iAdvImageNeedUpdate = false;            
        }
#endif
    }
    else
    {
        if( EDownloadLogo == iLogoDownloadState )
        {         
            iLogoNeedUpdate = false;  
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif            
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            iAdvImageNeedUpdate = false;            
        }
#endif        
    }

    iLogoDownloadState = EIdle;      
    
    delete aPreset;
    aPreset = NULL;
}



/********************************************************************************************************
 * SLOT for Network Event
 *******************************************************************************************************
 */
void IRNowPlayingView::handleNetworkEvent(IRQNetworkEvent aEvent)
{
    LOG_METHOD;
	LOG_FORMAT("aEvent = %d", (int)aEvent );
    if( this != getViewManager()->currentView() )
    {
        return;
    }
    
    switch(aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        {
            if( EIR_UseNetwork_StartingView == getUseNetworkReason() )
            {
                IRPlayList *playList = iApplication->getPlayList();
                if (playList && 1 == playList->getNumberOfEntries())
                {
                    IRQPreset *preset = playList->getPresetForEntry(0);
                    iPlayController->connectToChannel(preset, EIRQAdhocExternal);
                    delete preset;
                }
                else
                {
                    IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();                    
                    if( lastPlayedStationInfo )
                    {
                        IRQPreset *lastPreset = NULL;
                        lastPreset = lastPlayedStationInfo->getLastPlayedStation();
						if (lastPreset)
                        {
                            iPlayController->connectToChannel( lastPreset, lastPlayedStationInfo->connectedFrom() );
                        }
                    }                    
                }
            }
            else if( EIR_UseNetwork_PlayStation == getUseNetworkReason() )
            {
                iPlayController->resume();
            }
            break;
        }
    default:
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

/********************************************************************************************************
 * SLOT for Orientation Changed
 *******************************************************************************************************
 */

void IRNowPlayingView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    LOG_METHOD;
    
    if (aOrientation == Qt::Vertical)
    {   
        LOG("load portrait layout");
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, PORTRAIT_SEC);
    }
    else
    {
        LOG("load landscape layout");
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, LANDSCAPE_SEC);      
    }
    // re-load only update the size of the widget, the rect of the widget will get updated only after re-layout.
    updateGeometry();
}

void IRNowPlayingView::handleGeometryChanged()
{
    LOG_METHOD;
    updateSongName(iSongNameLabel->plainText());
}

/********************************************************************************************************
 * SLOT for Play Controller
 *******************************************************************************************************
 */
void IRNowPlayingView::handlePlayStarted()
{
    LOG_METHOD;
    iPlayStopAction->setIcon(HbIcon(KStopButtonIcon));
}

void IRNowPlayingView::handlePlayStopped()
{
    LOG_METHOD;
    iPlayStopAction->setIcon(HbIcon(KPlayButtonIcon));
    updateSongName(QString(""));
    iArtistName->setPlainText("");
    iSongNameAvailable = false;
}

void IRNowPlayingView::updateMetaData(IRQMetaData* aMetaData)
{
    LOG_METHOD;
    iSongNameAvailable = false;
    if(aMetaData)
    {
        updateSongName(aMetaData->getSongName());
        iArtistName->setPlainText(aMetaData->getArtistName());
        if(!aMetaData->getSongName().isEmpty() ||
           !aMetaData->getArtistName().isEmpty())
        {
            iSongNameAvailable = true;
        }
    }
    else
    {
        updateSongName(QString(""));
        iArtistName->setPlainText("");  
    }
}

/********************************************************************************************************
 * SLOT for Media Key
 *******************************************************************************************************
 */
void IRNowPlayingView::handlePlayPauseMediaKey()
{
    LOG_METHOD;
    iPlayController->resume();
}

void IRNowPlayingView::handleStopMediaKey()
{
    LOG_METHOD;
    iPlayController->stop(EIRQUserTerminated);
}

/********************************************************************************************************
 * SLOT for Toolbar Action
 *******************************************************************************************************
 */
void IRNowPlayingView::handleMusicStoreAction()
{
    LOG_METHOD;

    if (!IRQUtility::findAppByUid(KIRMusicStoreUid))  // if no music store, coming soon is shown.
    {
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_music_store_not_available"), HbMessageBox::MessageTypeInformation);
#else
        popupNote(hbTrId("Service Coming Soon"), HbMessageBox::MessageTypeInformation);    
#endif        
        return;
    }
    
    bool launchResult = false;
    if (iFindinNmsAllowed && iSongNameAvailable)
    {       
        // TODO : launch music store with search result page
        launchResult = IRQUtility::launchAppByUid(KIRMusicStoreUid);
        if (launchResult)        
        {
            if (iPlayController->getNowPlayingPreset()->type)
            {
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsFind,iPlayController->getNowPlayingPreset()->presetId);
            }
            else
            {            
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsFind,0);
            }
        }
    }
    else
    {
        // TODO : lunch music store with homepage  
        launchResult = IRQUtility::launchAppByUid(KIRMusicStoreUid);
        if (launchResult)        
        {
            if (iPlayController->getNowPlayingPreset()->type)
            {
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsLaunch,iPlayController->getNowPlayingPreset()->presetId);
            }
            else
            {            
                iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRNmsLaunch,0);
            }
        }        
    }
    
    if (!launchResult)
    {        
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_music_store_not_available"), HbMessageBox::MessageTypeInformation);
#else
        popupNote(hbTrId("Music store not ready"), HbMessageBox::MessageTypeInformation);    
#endif
    }
}

void IRNowPlayingView::handleIdentifySongAction()
{
    LOG_METHOD;
#ifdef STATISTIC_REPORT_TEST_ENABLED    
    if(IRQUtility::launchAppByUid(KDummySongRecognitionUid))
#else
    if(IRQUtility::launchAppByUid(iSettings->getSongRecognitionAppUid()))
#endif         
    {       
        iStatisticsReporter->logSongRecogEvent();
    }
}


void IRNowPlayingView::handlePlayStopAction()
{
    LOG_METHOD;
    switch (iPlayController->state())
    {
        case IRPlayController::EPlaying:
            iPlayController->stop(EIRQUserTerminated);
            return;
                    
        case IRPlayController::EStopped:
            setUseNetworkReason(EIR_UseNetwork_PlayStation);
            break;
            
        default:
            return;
    }
    
 
#ifdef HS_WIDGET_ENABLED		
    iPlayController->setConnectingStationName(iStationName->plainText());         
#endif
		
    if (iApplication->verifyNetworkConnectivity())
    {
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        iPlayController->resume();
    }
}

void IRNowPlayingView::handleAddToFavAction()
{
    LOG_METHOD;
    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    int retValue = iFavorites->addPreset(*nowPlayingPreset);
    HbNotificationDialog *add2FavNote = new HbNotificationDialog();
    add2FavNote->setModal(true);
    add2FavNote->setTimeout(HbPopup::ConfirmationNoteTimeout);
    add2FavNote->setAttribute(Qt::WA_DeleteOnClose);
    switch (retValue)
    {
    case EIRQErrorNone:
#ifdef SUBTITLE_STR_BY_LOCID       
        add2FavNote->setTitle(hbTrId("txt_irad_info_added_to_favorites"));
#else
        add2FavNote->setTitle(hbTrId("Added to Favorites"));         
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();
        break;

    case EIRQErrorOutOfMemory:
#ifdef SUBTITLE_STR_BY_LOCID 
        add2FavNote->setTitle(hbTrId("txt_irad_info_can_not_add_more"));
#else
        add2FavNote->setTitle(hbTrId("Can't add more"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();        
        break;

    case EIRQErrorAlreadyExist:
#ifdef SUBTITLE_STR_BY_LOCID 
        add2FavNote->setTitle(hbTrId("txt_irad_info_favorite_updated"));
#else
        add2FavNote->setTitle(hbTrId("Favorite updated"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();           
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void IRNowPlayingView::handleDetailInfoAction()
{
    LOG_METHOD;
    getViewManager()->activateView(EIRView_StationDetailsView);
    IRStationDetailsView *channelHistoryView = static_cast<IRStationDetailsView*>(getViewManager()->getView(EIRView_StationDetailsView));
    channelHistoryView->setDetails();

}

/********************************************************************************************************
 * SLOT for Menu Action
 *******************************************************************************************************
 */
void IRNowPlayingView::handleGoToStationAction()
{
    LOG_METHOD;
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

void IRNowPlayingView::handleShareStationAction()
{
    LOG_METHOD;
    if (NULL == iStationShare)
    {
        iStationShare = new IRStationShare();
    }
    iStationShare->shareStations(*iPlayController->getNowPlayingPreset());
}

void IRNowPlayingView::handleSettingAction()
{
    LOG_METHOD_ENTER;
    getViewManager()->activateView(EIRView_SettingsView);
}

#ifdef ADV_ENABLED
void IRNowPlayingView::mousePressEvent(QGraphicsSceneMouseEvent *aEvent)
{
    LOG_METHOD_ENTER;
    QRectF advRect = iAdvImage->geometry();
    QPointF pos = aEvent->pos();
    
    if (advRect.contains(pos))
    {
        openAdvLink();
    }
}

void IRNowPlayingView::openAdvLink()
{
    LOG_METHOD_ENTER;
    IRQUtility::openAdvLink(iAdvUrl);
}
#endif
   
void IRNowPlayingView::loadStationLogo()
{
    LOG_METHOD;
    if (iPlayController->isStationLogoAvailable())    
    { 
        QSettings settings(KIrSettingOrganization, KIrSettingApplication);
        if (settings.value(KIrSettingStationLogo).canConvert<QPixmap>())
        {
            QPixmap logoPixmap = settings.value(KIrSettingStationLogo).value<QPixmap>();
            QPixmap newLogoPixmap = 
                 logoPixmap.scaled(QSize(KNowPlayingLogoSize,KNowPlayingLogoSize),Qt::KeepAspectRatio);
            QIcon logoQIcon(newLogoPixmap);
            HbIcon logoHbIcon(logoQIcon);
            iStationLogo->setIcon(logoHbIcon);
            iLogoNeedUpdate = false;            
#ifdef ADV_ENABLED
            iAdvUrl = iPlayController->getNowPlayingPreset()->advertisementUrl; 
            iAdvImage->setIcon(logoHbIcon);
            iAdvImageNeedUpdate = false;  
#endif            	
            return;
        }  
    }

    iStationLogo->setIcon(HbIcon(KDefaultStationLogo));
    iLogoNeedUpdate = true;            
#ifdef ADV_ENABLED
    iAdvUrl = KDefaultAdvLink; 
    iAdvImage->setIcon(logoHbIcon);
    iAdvImageNeedUpdate = true;  
#endif
}

void saveStationLogo(const QPixmap &aStationLogo)
{
    LOG_METHOD_ENTER;
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    QVariant logoData(QVariant::Pixmap);
    logoData.setValue(aStationLogo);
    settings.setValue(KIrSettingStationLogo,logoData);    
}
//Implemented to receive theme change event
//@param QEvent, The change events to be received  
//
void IRNowPlayingView::changeEvent(QEvent *event)
{
    if (HbEvent::ThemeChanged == event->type())
    {
        // get the text color from theme and 
        // set it to corresponding labels
        iArtistName->setTextColor(HbColorScheme::color(KArtistColor));
        iSongNameLabel->setTextColor(HbColorScheme::color(KSongColor));
        iSongNameMarquee->setTextColor(HbColorScheme::color(KSongColor));
        iStationName->setTextColor(HbColorScheme::color(KStationColor));
          
    }
    
    HbWidget::changeEvent(event);
}
