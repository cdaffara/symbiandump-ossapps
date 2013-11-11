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

#ifndef IRMONITORSERVICE_H
#define IRMONITORSERVICE_H

// System includes
#include <QObject>
#include <QList>
#include <xqserviceprovider.h>

// User includes
#include "irservicedef.h"
#include "irservicedata.h"

// Forward declarations
class IRApplication;
class IRPlayController;
class IRQMetaData;

class IrMonitorService : public XQServiceProvider
{
    Q_OBJECT

public:
    explicit IrMonitorService(IRApplication *aIrApp, QObject *aParent = 0);
    virtual ~IrMonitorService();

public slots:
    void registerNotifications();  // service interface, called via Qt Highway
    void refreshAllData();     // service interface, called via Qt Highway

private slots:
    // slots for station logo update
    void handleStationLogoUpdated(bool aLogoAvailable);
    
    // slots for play controller
    void handleLoadingStarted(const QString &aStationName);
    void handleLoadingCancelled(const QString &aStationName);    
    void handlePlayStarted();
    void handlePlayStopped();
    void updateMetaData(IRQMetaData* aMetaData);

public:
     enum ReadyItem
     {
         StationName = 0x01,
         StationLogo = 0x02,
         MetaData    = 0x04,
         IrState     = 0x08
     };
     Q_DECLARE_FLAGS(ReadyItems, ReadyItem)    

private:
    void setupConnection();
    
    void notifyAll();
    bool notifyList(const IrServiceDataList &aDataList);

private:
    ReadyItems       mReadyItems;    
    QString          mStationName;
    bool             mStationLogoAvailable; // true if station has its own logo
    QString          mMetaData;
    IrAppState::Type mIrState;
        
    IRApplication    *mIrApp;
    IRPlayController *mPlayController;    
    
    QList<int>       mRequestList; // used to maintain the async request

    Q_DISABLE_COPY(IrMonitorService)

};

Q_DECLARE_OPERATORS_FOR_FLAGS(IrMonitorService::ReadyItems)

#endif // IRMONITORSERVICE_H
