/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XQApplicationManager stub for FM Radio home screen widget
*               unit testing.
*
*/

// User includes
#include "xqapplicationmanager.h"
#include "xqaiwrequest.h"
#include "radioservicedef.h"
#include "radiologger.h"

XQAiwRequest *application_manager_mock_refresh_request = new XQAiwRequest("interface", "operation", false);
XQAiwRequest *application_manager_mock_monitor_request = new XQAiwRequest("interface", "operation2", false);

/*!
    \class XQApplicationManager
    \brief Stub implementation of XQApplicationManager for unit testing
*/

// ======== MEMBER FUNCTIONS ========

XQApplicationManager::XQApplicationManager()
{
    LOG_METHOD_ENTER;
}

XQApplicationManager::~XQApplicationManager()
{
    LOG_METHOD_ENTER;
}

XQAiwRequest* XQApplicationManager::create(const QString& interface, const QString& operation, bool embedded)
{
    LOG_METHOD_ENTER;
    if (operation.compare(RADIO_MONITOR_SERVICE_REFRESH_OPERATION) == 0) {
        application_manager_mock_refresh_request = new XQAiwRequest(interface, operation, embedded);
        return application_manager_mock_refresh_request;
    } else if (operation.compare(RADIO_MONITOR_SERVICE_OPERATION) == 0) {
        application_manager_mock_monitor_request = new XQAiwRequest(interface, operation, embedded);
        return application_manager_mock_monitor_request;
    }
    return NULL; 
}
