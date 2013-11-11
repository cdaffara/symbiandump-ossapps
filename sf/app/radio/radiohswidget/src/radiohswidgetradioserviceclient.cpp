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

// System includes
#include <xqserviceglobal.h>

// User includes
#include "radiohswidgetradioserviceclient.h"
#include "radiohswidget.h"
#include "radionotificationdata.h"
#include "radioservicedef.h"
#include "radiologger.h"

/*!
    \class RadioHsWidgetRadioServiceClient
    \brief Implementation of QtHighway based communicator with radio application.

    RadioHsWidgetRadioServiceClient implements monitoring of radio
    information and controlling of radio. This is done by service request
    through QtHighway.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a profile reader which is a child of \a parent.
 */
RadioHsWidgetRadioServiceClient::RadioHsWidgetRadioServiceClient(
    RadioHsWidget *parent) :
    QObject(parent),
    mParent(*parent),
    mRequestPending(false),
    mRadioMonitorRequest(NULL),
    mRadioControlRequest(NULL),
    mDataInitialized(false)
{
    LOG_METHOD;
}

/*!
    Destructor 
 */
RadioHsWidgetRadioServiceClient::~RadioHsWidgetRadioServiceClient()
{
    LOG_METHOD;
    stopMonitoring();
}

/*!
    Command FM Radio station.
 
    \param command Command to request radio to perform.
 */
void RadioHsWidgetRadioServiceClient::commandFmRadio(
    const RadioServiceCommand::CommandId command)
{
    LOG_METHOD_ENTER;
    QVariant commandArgument;
    commandArgument.setValue(static_cast<int>(command));
    FmRadio::VisibiltyAfterRequest visibility;
    if (command == RadioServiceCommand::Foreground) {
        visibility = FmRadio::VisibiltyToForeground;
    } else if (command == RadioServiceCommand::Background) {
        visibility = FmRadio::VisibiltyToBackground;
    }  else {
        visibility = FmRadio::VisibiltyDoNotChange;
    }
    sendControlRequest(commandArgument, visibility);
}

/*!
    Start radio monitoring.
    
    \param visibility Desired visibility for the radio after the request.
 */
void RadioHsWidgetRadioServiceClient::startMonitoring(
    FmRadio::VisibiltyAfterRequest visibility)
{
    LOG_METHOD_ENTER;
    createMonitorServiceRequest();
    sendMonitorRequest(visibility);
}

/*!
    Stops radio monitoring.
 */
void RadioHsWidgetRadioServiceClient::stopMonitoring()
{
    LOG_METHOD_ENTER;
    // Delete the mRadioMonitorRequest. 
    if (mRadioMonitorRequest) {
        delete mRadioMonitorRequest;
        mRadioMonitorRequest = NULL;
        mRequestPending = false;
        mDataInitialized = false;
    }
}

/*!
    Creates and returns RadioNotificationData.
    This is needed because radionotificationdata.h cannot be
    included on test code.
 */
QVariant RadioHsWidgetRadioServiceClient::createRadioNotificationData(int type, const QVariant& data)
{
    LOG_METHOD_ENTER;
    QVariant ret;
    ret.setValue(RadioNotificationData(type, data));
    return ret;
}


/*!
    Handles request errors.
 
    \param errorCode Code of the error type.
    \param errorMessage Error message.
 */
void RadioHsWidgetRadioServiceClient::handleError(const int errorCode,
    const QString &errorMessage)
{
    LOG_METHOD_ENTER;
    Q_UNUSED(errorMessage)
    LEVEL2(LOG_SLOT_CALLER);
    handleRequestError(errorCode);
}

/*!
    Handle changes in FM Radio.
 
    \param value List of information published by radio.
 */
void RadioHsWidgetRadioServiceClient::handleFmRadioChange(
    const QVariant &value)
{
    LOG_METHOD_ENTER;
    LEVEL2(LOG_SLOT_CALLER);
    // Request is not pending anymore.
    mRequestPending = false;
    // If first request was refresh operation.
    if (!mDataInitialized) {
        LOG("Set operation to KRadioServiceMonitorOperation");
        // Change the operation to the monitoring.
        mRadioMonitorRequest->setOperation(RADIO_MONITOR_SERVICE_OPERATION);
        // Data is now initialized.
        mDataInitialized = true;
    }
    // Request notifications again.
    startMonitoring(FmRadio::VisibiltyDoNotChange);
    
    // If valid and right kind of data was received.
    if (value.canConvert(QVariant::List) ) {
        QVariantList notificationList = value.toList();
        // Iterate through the list.
        foreach (const QVariant& variant, notificationList) {
            // Extract notification data.
            RadioNotificationData notification = variant.value<RadioNotificationData>();
            // And it's type.
            const int notificationId = notification.mType;
            // And the data
            const QVariant notificationData = notification.mData;
            // If the data is valid.
            if (notificationData.isValid()) {
                // Notify the observer about the new information.
                mParent.handleRadioInformationChange(notificationId, notificationData);
            }
        }
    }
}

/*!
    Handles request error.
 
    \param error Identifies the error. 
 */
void RadioHsWidgetRadioServiceClient::handleRequestError(const int error)
{
    LOG_METHOD;
    QString errorStr;
    switch (error) {
    case XQService::ENoError:
        errorStr = "No error";
        break;
    case XQService::EConnectionError:
        errorStr = "Error in IPC Connection";
        break;
    case XQService::EConnectionClosed:
        errorStr = "IPC Connection is closed";
        stopMonitoring();
        mParent.handleRadioStateChange(FmRadio::StateNotRunning);
        break;
    case XQService::EServerNotFound:
        errorStr = "Can not find server";
        break;
    case XQService::EIPCError:
        errorStr = "Known IPC error defined by SDK";
        break;
    case XQService::EUnknownError:
        errorStr = "Unknown IPC error";
        break;
    case XQService::ERequestPending:
        errorStr = "Already pending request";
        break;
    default:
        errorStr = "default case at error";
        break;
    }
    LOG(GETSTRING(errorStr));
}

/*!
    Creates control service request object.
 */
void RadioHsWidgetRadioServiceClient::createControlServiceRequest()
{
    LOG_METHOD_ENTER;
    if (!mRadioControlRequest) {
        LOG("Create request");
        mRadioControlRequest = mApplicationManager.create(
            RADIO_CONTROL_SERVICE, RADIO_CONTROL_SERVICE_OPERATION, false);
        
        if (mRadioControlRequest) {
            LOG("Request created");
            // Connect request to handle it's error.
            Radio::connect(mRadioControlRequest,
                SIGNAL(requestError(int,const QString&)), this,
                SLOT(handleError(int,const QString&)));
            
            // Request is synchronous.
            mRadioControlRequest->setSynchronous(true);
            // Request is not embedded.
            mRadioControlRequest->setEmbedded(false);
        }
    }
}

/*!
    Creates monitor service request object.
 */
void RadioHsWidgetRadioServiceClient::createMonitorServiceRequest()
{
    LOG_METHOD_ENTER;
    if (!mRadioMonitorRequest) {
        // If data is not initialized, set operation to refresh,
        // otherwise to monitor operation.
        QString operation = mDataInitialized ? RADIO_MONITOR_SERVICE_OPERATION
            : RADIO_MONITOR_SERVICE_REFRESH_OPERATION;

        LOG("Create request");
        mRadioMonitorRequest = mApplicationManager.create(
            RADIO_MONITOR_SERVICE, operation, false);
        
        if (mRadioMonitorRequest) {
            LOG("Request created");
            // Connect request to handle it's completion.
            Radio::connect(mRadioMonitorRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleFmRadioChange(const QVariant&)));
            // Connect request to handle it's error.
            Radio::connect(mRadioMonitorRequest,
                SIGNAL(requestError(int,const QString&)), this,
                SLOT(handleError(int,const QString&)));
            
            // Request is asynchronous.
            mRadioMonitorRequest->setSynchronous(false);
            // Request is not embedded.
            mRadioMonitorRequest->setEmbedded(false);
        }
    }
}

/*!
    Sends the control request containing the command.
    
    \param argument Contains the command.
    \param visibility Desired visibility for the radio after the request.
 */
void RadioHsWidgetRadioServiceClient::sendControlRequest(
    const QVariant &argument,
    const FmRadio::VisibiltyAfterRequest visibility)
{
    LOG_METHOD_ENTER;
    if (!argument.isValid()) {
        return;
    }
    
    // If there is not mRadioControlRequest already.
    if (!mRadioControlRequest) {
        // Create it.
        createControlServiceRequest();
    }
    
    // Set argument list for request.
    QVariantList arguments;
    arguments.append(argument);
    mRadioControlRequest->setArguments(arguments);
    
    prepareRequestInfo(mRadioControlRequest, visibility);
    
    LOG("Send request");
    bool res = mRadioControlRequest->send();
    
    /*
    if (!res) {
        LOG("Send failed");
        int error = mRadioControlRequest->lastError();
        handleRequestError(error);
    }
    */
}

/*!
    Sends the monitor request.
    
    \param visibility Desired visibility for the radio after the request.
 */
void RadioHsWidgetRadioServiceClient::sendMonitorRequest(
    const FmRadio::VisibiltyAfterRequest visibility)
{
    LOG_METHOD_ENTER;
    prepareRequestInfo(mRadioMonitorRequest, visibility);
    if (!mRequestPending) {
        LOG("Send request");
        mRequestPending = mRadioMonitorRequest->send();
    }
}

/*!
    Prepares the visibility of the request.
    
    \param request Request to prepare.
    \param visibility Desired visibility for the radio after the request.
 */
void RadioHsWidgetRadioServiceClient::prepareRequestInfo(
    XQAiwRequest *request,
    const FmRadio::VisibiltyAfterRequest visibility)
{
    LOG_METHOD_ENTER;
    XQRequestInfo info;
    if (visibility == FmRadio::VisibiltyToForeground) {
        info.setForeground(true);
    } else if (visibility == FmRadio::VisibiltyToBackground) {
        info.setBackground(true);
    }
    if (request) {
        request->setInfo(info);
    }
}
