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
#include <QSettings>

// User includes
#include "irmonitorservice.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irlastplayedstationinfo.h"
#include "irqisdsdatastructure.h"
#include "irqmetadata.h"
#include "irplaylist.h"
#include "irviewmanager.h"
#include "irqlogger.h"

// Contants
static const QString IR_MONITOR_SERVICE = "internet_radio_10_1.com.nokia.symbian.IInternetRadioMonitor";

#define IS_READY(itemFlag)   mReadyItems.testFlag(itemFlag)
#define SET_FLAG(itemFlag)   mReadyItems |= itemFlag
#define CLEAR_FLAG(itemFlag) mReadyItems &= ~itemFlag
#define CLEAR_ALL_FLAGS()    mReadyItems &= !mReadyItems
#define ANY_READY() \
        mReadyItems.testFlag(StationName) || \
        mReadyItems.testFlag(StationLogo) || \
        mReadyItems.testFlag(MetaData) || \
        mReadyItems.testFlag(IrState)

// Constructor
IrMonitorService::IrMonitorService(IRApplication *aIrApp, QObject *aParent) :
    XQServiceProvider(IR_MONITOR_SERVICE, aParent),
    mStationLogoAvailable(false),
    mIrState(IrAppState::Unknown),
    mIrApp(aIrApp),
    mPlayController(NULL)
{   
    LOG_METHOD;
    publishAll();
    mPlayController = mIrApp->getPlayController();
    setupConnection();
}

// Destructor
IrMonitorService::~IrMonitorService()
{
    LOG_METHOD;
}

void IrMonitorService::setupConnection()
{
    LOG_METHOD;
    // meta data update
    connect(mPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), 
        this, SLOT(updateMetaData(IRQMetaData*)));
    // station logo update
    connect(mPlayController, SIGNAL(stationLogoUpdated(bool)), 
        this, SLOT(handleStationLogoUpdated(bool)));
    // connecting started
    connect(mPlayController, SIGNAL(connectingStarted(QString)), 
        this, SLOT(handleLoadingStarted(QString))); 
    // buffering started   
    connect(mPlayController, SIGNAL(bufferingStarted(QString)), 
        this, SLOT(handleLoadingStarted(QString))); 
    // playing started  
    connect(mPlayController, SIGNAL(playingStarted()), 
        this, SLOT(handlePlayStarted()));
     // playing stopped  
    connect(mPlayController, SIGNAL(playingStopped()), 
        this, SLOT(handlePlayStopped()));  
    // connecting cancelled
    connect(mPlayController, SIGNAL(connectingCancelled(QString)), 
        this, SLOT(handleLoadingCancelled(QString)));  
    // buffering cancelled
    connect(mPlayController, SIGNAL(bufferingCancelled(QString)), 
        this, SLOT(handleLoadingCancelled(QString)));          
}

// service interface, called via Qt Highway
void IrMonitorService::registerNotifications()
{
    LOG_METHOD;
    mRequestList.append(setCurrentRequestAsync());
    
    if (ANY_READY())
    {
        notifyAll();
    }
}

// service interface, called via Qt Highway
void IrMonitorService::refreshAllData()
{
    LOG_METHOD;
    IrServiceDataList notificationList;
    
    IRQPreset * currentPreset = mPlayController->getNowPlayingPreset();    
    
    LOG_FORMAT("mPlayController->state() = %d", mPlayController->state());
    
    switch (mPlayController->state())
    {
        case IRPlayController::EIdle:
            notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)IrAppState::RunningInit));
            break;
                    
        case IRPlayController::EStopped:
            notificationList.append(IrServiceData((int)IrServiceNotification::StationName, currentPreset->name));
#ifdef HS_WIDGET_ENABLED                
            notificationList.append(IrServiceData((int)IrServiceNotification::StationLogo, mPlayController->isStationLogoAvailable()));
#endif                
            notificationList.append(IrServiceData((int)IrServiceNotification::MetaData, QString("")));            
            notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)IrAppState::RunningStopped));                           
            break;

#ifdef HS_WIDGET_ENABLED
        case IRPlayController::EConnecting:   
            notificationList.append(IrServiceData((int)IrServiceNotification::StationName, mPlayController->getConnectingStationName()));
            notificationList.append(IrServiceData((int)IrServiceNotification::StationLogo, mPlayController->isStationLogoAvailable()));   
            notificationList.append(IrServiceData((int)IrServiceNotification::MetaData, QString("")));                       
            notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)IrAppState::Loading));
            break;
#endif            
                        
        case IRPlayController::EBuffering:         
            notificationList.append(IrServiceData((int)IrServiceNotification::StationName, currentPreset->name));
            notificationList.append(IrServiceData((int)IrServiceNotification::StationLogo, mPlayController->isStationLogoAvailable()));   
            notificationList.append(IrServiceData((int)IrServiceNotification::MetaData, QString("")));                       
            notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)IrAppState::Loading));
            break;

        case IRPlayController::EPlaying:
            notificationList.append(IrServiceData((int)IrServiceNotification::StationName, currentPreset->name));
#ifdef HS_WIDGET_ENABLED                
            notificationList.append(IrServiceData((int)IrServiceNotification::StationLogo, mPlayController->isStationLogoAvailable()));  
#endif                 
            notificationList.append(IrServiceData((int)IrServiceNotification::MetaData, mMetaData));             
            notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)IrAppState::Playing));      
            break;

        default:
            break;
    }
    
    completeRequest(setCurrentRequestAsync(), qVariantFromValue(notificationList));
}

/********************************************************************************************************
 * SLOT for Play Controller
 *******************************************************************************************************
 */

void IrMonitorService::updateMetaData(IRQMetaData* aMetaData)
{    
    LOG_METHOD;
    if (aMetaData)
    {
        SET_FLAG(MetaData);
        mMetaData = aMetaData->getArtistSongName();
        
        notifyAll();
    }
}
 
void IrMonitorService::handleStationLogoUpdated(bool aLogoAvailable)
{
    LOG_METHOD;
    mStationLogoAvailable = aLogoAvailable;
    SET_FLAG(StationLogo);
    
    notifyAll();
}
 
void IrMonitorService::handleLoadingStarted(const QString &aStationName)
{
    LOG_METHOD;
    mMetaData.clear();
    SET_FLAG(MetaData);
    
    mStationName = aStationName;        
    SET_FLAG(StationName);
    
    mIrState = IrAppState::Loading;
    SET_FLAG(IrState);
                 
    notifyAll();
}

void IrMonitorService::handleLoadingCancelled(const QString &aStationName)
{
    LOG_METHOD;
    mMetaData.clear();
    SET_FLAG(MetaData);
    
    mStationName = aStationName;
    SET_FLAG(StationName);
    
    if (mPlayController->isStopped())
    {
        mIrState = IrAppState::RunningStopped;
    }
    else
    {
        mIrState = IrAppState::RunningInit;
    }   
    SET_FLAG(IrState); 

    notifyAll();
}

void IrMonitorService::handlePlayStarted()
{
    LOG_METHOD;
    mMetaData.clear();
    SET_FLAG(MetaData);
    
    IRQPreset * currentPreset = mPlayController->getNowPlayingPreset();
    mStationName = currentPreset->name;
    SET_FLAG(StationName);
    
    mIrState = IrAppState::Playing;
    SET_FLAG(IrState);
    
    notifyAll();
}

void IrMonitorService::handlePlayStopped()
{
    LOG_METHOD;
    mMetaData.clear();
    SET_FLAG(MetaData);

    mIrState = IrAppState::RunningStopped;
    SET_FLAG(IrState);
        
    notifyAll();
}


/********************************************************************************************************
 * Others
 *******************************************************************************************************
 */
void IrMonitorService::notifyAll()
{
    LOG_METHOD;
    IrServiceDataList notificationList;
    
    if(IS_READY(StationName))
    {
        notificationList.append(IrServiceData((int)IrServiceNotification::StationName, mStationName)); 
    }
    
    if(IS_READY(StationLogo))
    {
        notificationList.append(IrServiceData((int)IrServiceNotification::StationLogo, mStationLogoAvailable)); 
    }
    
    if(IS_READY(MetaData))
    {
        notificationList.append(IrServiceData((int)IrServiceNotification::MetaData, mMetaData)); 
    }
    
    if(IS_READY(IrState))
    {
        notificationList.append(IrServiceData((int)IrServiceNotification::IrState, (int)mIrState)); 
    }
    
    if(notifyList(notificationList))
    {
        CLEAR_ALL_FLAGS();        
    }
}

bool IrMonitorService::notifyList(const IrServiceDataList &aDataList)
{
    LOG_METHOD;
    bool retVal = true;
    
    if (mRequestList.count() > 0
        && aDataList.count() > 0)
    {
        foreach (int requestId, mRequestList) 
        {
            retVal &= completeRequest(requestId, qVariantFromValue(aDataList));
        }
        mRequestList.clear();
    }
    else
    {
        return false;
    }
    
    return retVal;
}

Q_IMPLEMENT_USER_METATYPE(IrServiceData)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(IrServiceDataList)

