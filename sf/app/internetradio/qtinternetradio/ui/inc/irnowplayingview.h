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
#ifndef IRNOWPLAYINGVIEW_H
#define IRNOWPLAYINGVIEW_H

#include "irbaseview.h"
#include "irqevent.h"

class HbAction;
class IRQPreset;
class IRQStatisticsReporter;
class IRQMetaData;
class IRStationShare;
class HbLabel;
class HbMarqueeItem;
class QEvent;

/**
 * This class shows the station information when it's played.
 */

class IRNowPlayingView : public IRBaseView
      
{
    Q_OBJECT

public:
    ~IRNowPlayingView();
        
protected:
    IRNowPlayingView(IRApplication* aApplication, TIRViewId aViewId);

    //from base class IRBaseView
    void launchAction();   
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);

    // from HbWidget
    void changeEvent(QEvent *event);

private slots:
    // slots for logo download
    void handleLogoDownloaded(IRQPreset* aPreset);
    
    // slots for network event
    void handleNetworkEvent(IRQNetworkEvent aEvent);
    
    // slots for orientation change
    void handleOrientationChanged(Qt::Orientation);    
    void handleGeometryChanged();
    
    // slots for play controller
    void handlePlayStarted();
    void handlePlayStopped();
    void updateMetaData(IRQMetaData* aMetaData);

    // slots for media key
    void handlePlayPauseMediaKey();
    void handleStopMediaKey();
    
    // slots for toolbar action
    void handleMusicStoreAction();
    void handleIdentifySongAction();    
    void handlePlayStopAction();    
    void handleAddToFavAction();   
    void handleDetailInfoAction();
    
    // slots for menu action
    void handleGoToStationAction();    
    void handleShareStationAction();
    void handleSettingAction();

#ifdef ADV_ENABLED
    // since this maybe called during handleLogoDownloaded(), 
    // so make it as slot connected to a single timer.
    void updateAdvImage();
#endif
   
private:
    void initialize();
    void initMenu();
    void initToolBar();
    void initWidget();

    void updateWidgets();
    void updateMusicStoreStatus();
    void updateForLauchAction();
    void updateSongName(const QString &aSongName);
    
    void updateStationLogo();	
    void loadStationLogo();

    
#ifdef ADV_ENABLED
    void mousePressEvent(QGraphicsSceneMouseEvent *aEvent);
    void openAdvLink();
#endif
    
private:
    IRQStatisticsReporter *iStatisticsReporter;
    IRStationShare *iStationShare;

    // tool bar actions
    HbAction *iPlayStopAction;
    
    bool iLaunchActionNeeded;
    
    enum LogoDownloadState
    {
        EIdle              = 0,
#ifdef ADV_ENABLED        
        EDownLoadAdvImage  = 1,
#endif                
        EDownloadLogo      = 2        
    };
    LogoDownloadState iLogoDownloadState;
    
    HbLabel *iSongNameLabel;
    HbMarqueeItem *iSongNameMarquee;
    HbLabel *iArtistName;
    HbLabel *iStationName;
    HbLabel *iStationLogo;
    bool iLogoNeedUpdate;
        
    bool iFindinNmsAllowed;
    bool iSongNameAvailable;
    
#ifdef ADV_ENABLED
    HbLabel *iAdvImage;
    bool iAdvImageNeedUpdate;
    QString iAdvUrl;
#endif    
    friend class IRViewManager;
};

#endif   //IRNOWPLAYINGVIEW_H
