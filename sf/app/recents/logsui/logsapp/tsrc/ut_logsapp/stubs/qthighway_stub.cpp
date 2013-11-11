/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qglobal.h>
#include <xqserviceprovider.h>
#include <xqservicerequest.h>
#include <xqserviceutil.h>
#include "qthighway_stub_helper.h"

QString qtHighwayStubService;
QString qtHighwayStubMessage;
bool qtHighwayStubToBackground = false;
bool qtHighwayStubIsService = false;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

void QtHighwayStubHelper::reset()
{
    qtHighwayStubService.clear();
    qtHighwayStubMessage.clear();
    qtHighwayStubToBackground = false;
    qtHighwayStubIsService = false;
}

QString QtHighwayStubHelper::service()
{
    return qtHighwayStubService;
}

QString QtHighwayStubHelper::message()
{
    return qtHighwayStubMessage;
}

bool QtHighwayStubHelper::utilToBackground()
{
    return qtHighwayStubToBackground;
}

void QtHighwayStubHelper::setIsService(bool isService)
{
    qtHighwayStubIsService = isService;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQServiceProvider::XQServiceProvider( const QString& service, QObject *parent )
{
    Q_UNUSED(service);
    Q_UNUSED(parent);
}

XQServiceProvider::~XQServiceProvider()
{
    
}

void XQServiceProvider::SetPlugin(QObject* impl_plugin)
{
    Q_UNUSED(impl_plugin);
}

void XQServiceProvider::publishAll()
{
    
}

int XQServiceProvider::setCurrentRequestAsync()
{
    return 0;
}
    
bool XQServiceProvider::completeRequest(int index, const QVariant& retValue)
{
    Q_UNUSED(index);
    Q_UNUSED(retValue);
    return true;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQServiceRequest::XQServiceRequest(
    const QString& service, const QString& message, const bool& synchronous)
{
    qtHighwayStubService = service;
    qtHighwayStubMessage = message;
}

XQServiceRequest::~XQServiceRequest()
{

}


void XQServiceRequest::setArguments(const QList<QVariant> &arguments)
{
    Q_UNUSED(arguments)
}

QString XQServiceRequest::service() const
{
    return qtHighwayStubService;
}

QString XQServiceRequest::message() const
{
    return qtHighwayStubMessage;
}

bool XQServiceRequest::send(QVariant& retValue) 
{
    return true;
}

void XQServiceRequest::addArg(const QVariant& v)
{

}

void XQServiceRequest::setInfo(const XQRequestInfo &info)
{
    Q_UNUSED(info);
}

void XQRequestInfo::setForeground(bool on)
{
    Q_UNUSED(on)
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void XQServiceUtil::toBackground(bool value)
{
    qtHighwayStubToBackground = value;
}

bool XQServiceUtil::isService()
{
    return qtHighwayStubIsService;
}
