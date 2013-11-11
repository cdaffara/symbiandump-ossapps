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
* Description: FM Radio widget radio service client
*
*/

#ifndef RADIOHSWIDGETRADIOSERVICECLIENT_H_
#define RADIOHSWIDGETRADIOSERVICECLIENT_H_

// System includes
#include <QObject>
#include <xqappmgr.h>

// User includes
#include "radiohswidget.h"
#include "radioservicedef.h"

class RadioHsWidgetRadioServiceClient : public QObject
{
Q_OBJECT

public:
    explicit RadioHsWidgetRadioServiceClient(RadioHsWidget *parent = 0);
    virtual ~RadioHsWidgetRadioServiceClient();
    
public:
    void commandFmRadio(const RadioServiceCommand::CommandId command);
    void startMonitoring(
        const FmRadio::VisibiltyAfterRequest visibility);
    void stopMonitoring();
    QVariant createRadioNotificationData(int type, const QVariant& data);

private slots:
    void handleError(const int errorCode, const QString &errorMessage);
    void handleFmRadioChange(const QVariant &value);

private:
    void handleRequestError(const int error);

    void createControlServiceRequest();
    void createMonitorServiceRequest();

    void sendMonitorRequest(
        const FmRadio::VisibiltyAfterRequest visibility);
    void sendControlRequest(const QVariant &argument,
        const FmRadio::VisibiltyAfterRequest visibility);

    void prepareRequestInfo(XQAiwRequest *request,
        const FmRadio::VisibiltyAfterRequest visibility);
    
private: // data
    /*!
        Stores the reference to parent. This is used to call parents
        functions when notifications are received.
     */
    RadioHsWidget &mParent;
    /*! Is the request send. */
    bool mRequestPending;
    /*! For service and interface listings and request creation. */
    XQApplicationManager mApplicationManager;
    /*! Asynchronous monitoring request. */
    XQAiwRequest* mRadioMonitorRequest;
    /*! Synchronous control request. */
    XQAiwRequest* mRadioControlRequest;
    
    /*! Is the first monitoring request done. */
    bool mDataInitialized;
};

#endif /* RADIOHSWIDGETRADIOSERVICECLIENT_H_ */
