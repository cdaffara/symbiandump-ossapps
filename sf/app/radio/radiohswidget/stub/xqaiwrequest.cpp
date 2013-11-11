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
* Description:  XQAiwRequest stub for FM Radio home screen widget
*               unit testing.
*
*/

// System includes
#include <QVariant>

// User includes
#include "xqaiwrequest.h"
#include "xqrequestinfo.h"
#include "radioservicedef.h"
#include "radiologger.h"

extern QVariantList request_mocks;
extern int send_count;

/*!
    \class XQAiwRequest
    \brief Stub implementation of XQAiwRequest for unit testing
*/

// ======== MEMBER FUNCTIONS ========

XQAiwRequest::XQAiwRequest(const QString &interface,
    const QString &operation, bool isEmbedded) :
    mOperation(operation),
    mRequestCount(0)
{
    LOG_METHOD_ENTER;
}

XQAiwRequest::~XQAiwRequest()
{
    LOG_METHOD_ENTER;
}

static void replyRequestOk(QVariant reply)
{
    LOG_METHOD_ENTER;
}

void XQAiwRequest::setOperation(const QString &operation)
{
    LOG_METHOD_ENTER;
    mOperation = operation;
}

void XQAiwRequest::setSynchronous(bool synchronous)
{
    LOG_METHOD_ENTER;
}

void XQAiwRequest::setEmbedded(bool embedded)
{
    LOG_METHOD_ENTER;
}

bool XQAiwRequest::send()
{
    LOG_METHOD_ENTER;
    pendingRequests.append(mOperation);
    replyAfterDelay();
    return true;
}

void XQAiwRequest::setInfo(const XQRequestInfo &info)
{
    LOG_METHOD_ENTER;
}

bool XQAiwRequest::replyAfterDelay()
{
    LOG_METHOD;
    QString operationStr;
    bool ret = true;
    if (mRequestCount < send_count) {
        replyRequestOk((request_mocks.at(mRequestCount)).toList());
        ret = false;
    }
    return ret;
}

void XQAiwRequest::replyRequestOk(const QVariantList &reply)
{
    LOG_METHOD_ENTER;
    ++mRequestCount;
    emit requestOk(reply);
}
