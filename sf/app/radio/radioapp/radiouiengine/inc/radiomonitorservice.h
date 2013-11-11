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

#ifndef RADIOMONITORSERVICE_H
#define RADIOMONITORSERVICE_H

// System includes
#include <xqserviceprovider.h>

// User includes
#include "radioservicedef.h"
#include "radioenginewrapperobserver.h"

// Forward declarations
class RadioUiEnginePrivate;
class RadioStation;
class QTimer;

class RadioMonitorService : public XQServiceProvider
                          , public RadioEngineWrapperObserver
{
    Q_OBJECT

public:

    RadioMonitorService( RadioUiEnginePrivate& engine );
    virtual ~RadioMonitorService();

    void init();

public slots:

    void requestNotifications();    // Slot called by Qt Highway
    void requestAllData();          // Slot called by Qt Highway

private slots:

    void notifyRadioStatus();
    void notifyFavoriteCount();
    void notifyLocalCount();
    void notifyStationChange( const RadioStation& station );
    void sendNotifications();

private:

// from base class RadioEngineWrapperObserver

    void tunedToFrequency( uint frequency, int reason );

// New functions

    RadioStatus::Status determineRadioStatus() const;

    void checkIfCurrentStationIsFavorite();

    QString trimHtmlTags( const QString& html );

    void notify( const QVariant& notification );
    void notifyList( const QVariantList& list );

private: // data

    RadioUiEnginePrivate&   mUiEngine;

    QList<int>              mRequestIndexes;

    RadioStatus::Status     mRadioStatus;

    QVariantList            mNotificationList;

    QTimer*                 mNotificationTimer;

};

#endif // RADIOMONITORSERVICE_H
