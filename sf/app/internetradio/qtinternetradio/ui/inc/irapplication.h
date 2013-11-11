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
#ifndef IRAPPLICATION_H
#define IRAPPLICATION_H

#include <QObject>
#include <QEvent>
#include <hbglobal.h>
#include <qpoint.h>

#include "irqevent.h"
#include "irviewdefinitions.h"

 

class IRViewManager;
class IRQIsdsClient;
class IRPlayController;     
class IRQSettings;
class IRMediaKeyObserver;
class IRLastPlayedStationInfo;
class IRQFavoritesDB;
class IRQNetworkController;
class IRQDiskSpaceWatcher; 
class IRQAdvClient;
class HbProgressDialog;
class IRQSystemEventHandler;
class IRPlayList;
#ifdef HS_WIDGET_ENABLED
class IrMonitorService;
class IrControlService;
#endif 

class IRFileViewService;
class HbMessageBox;

 

class IRApplication : public QObject
{
    Q_OBJECT
    
public:
    IRApplication(IRViewManager *aViewManager, IRQSystemEventHandler* aSystemEventHandler);    

    ~IRApplication();
#ifdef SUBTITLE_STR_BY_LOCID
    bool verifyNetworkConnectivity(const QString &aConnectingText = hbTrId("txt_common_info_loading"));
#else
    bool verifyNetworkConnectivity(const QString &aConnectingText = hbTrId("Loading"));
#endif
    
    void startLoadingAnimation(const QObject *aReceiver, const char *aFunc);    
    void stopLoadingAnimation();
	
#ifdef HS_WIDGET_ENABLED    
    bool startPlaying();
    void cancelPlayerLoading();
#endif
    
    IRViewManager* getViewManager() const;
    IRQNetworkController* getNetworkController();
    IRQIsdsClient* getIsdsClient();
    IRPlayController* getPlayController();
    IRLastPlayedStationInfo* getLastPlayedStationInfo();
    IRQFavoritesDB* getFavoritesDB();
    IRQSettings *   getSettings();
    IRMediaKeyObserver* getMediaKeyObserver();
    IRQAdvClient* getAdvClient(); 
    IRPlayList* getPlayList() const;
	
    void setLaunchView();    
    void launchStartingView(TIRViewId aViewId);
    
    bool isAppFullyStarted() const;
    bool isEmbeddedInstance() const;
	
    //from QObject
    bool event(QEvent* e);

#ifdef _DEBUG
public:
    bool iTestPreferredBitrate;
#endif
    
signals:
    void quit();
    
private slots:
    //connect to signal 'networkEventNotified' from IRQNetworkController
    void networkEventNotified(IRQNetworkEvent aEvent);
    void cancelConnect();
    void loadGenre();     
    void handleDiskSpaceLow(qint64 aCriticalLevel);
  
    void handleCallActivated();
    void handleCallDeactivated();
    void handleHeadsetConnected();
    void handleHeadsetDisconnected();
    
private:
    void createComponents();
    void destroyComponents();
    void setupConnection();
    void startSystemEventMonitor();
    void initApp();
    void setExitingView(); 
    TIRHandleResult handleConnectionEstablished();
    
    
#ifdef _DEBUG
    void readConfiguration();
#endif

private:
    IRViewManager* iViewManager;
    
    TIRViewId   iStartingViewId;
    
    IRQNetworkController* iNetworkController;
    
    IRQIsdsClient* iIsdsClient;
    
    IRPlayController* iPlayController;
    
    IRQFavoritesDB* iFavPresets;
    
    IRQSettings      *iSettings;

    IRMediaKeyObserver *iMediaKeyObserver;

    IRLastPlayedStationInfo *iLastPlayedStationInfo;
    
    IRQAdvClient *iAdvertisementClient;   

    bool iEnableGlobalAdv;

    bool iDisconnected;

    bool iConnectingCanceled;

    QEvent::Type iInitEvent;

    QString iConnectingText;     
    
    HbProgressDialog *iLoadingNote;    
    IRQSystemEventHandler *iSystemEventHandler;    	
    TIRUseNetworkReason iUseNetworkReason;	
    
    bool iAppFullyStarted;
    
#ifdef HS_WIDGET_ENABLED    
    IrControlService    *iControlService;
    IrMonitorService    *iMonitorService;
#endif	
      
    IRFileViewService   *iFileViewService;
    HbMessageBox *iMessageBox;
	bool          iIsEmbedded;
};

#endif
