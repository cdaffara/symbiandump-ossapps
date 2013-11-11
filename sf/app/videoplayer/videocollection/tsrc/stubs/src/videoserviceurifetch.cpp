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
 * Description:  Implementation of VideoServiceUriFetch
 *
 */

#include "xqserviceproviderstub.h"
#include "videoserviceurifetch.h"
#include "videoservices.h"

VideoServiceUriFetch::VideoServiceUriFetch(VideoServices* parent, QLatin1String service):
    XQServiceProvider( service, parent ),
    mRequestIndex(0),
    mServiceApp(parent)
{
    // nop    
}

VideoServiceUriFetch::~VideoServiceUriFetch()
{
    // nop
}

void VideoServiceUriFetch::fetchFailed(int errorCode)
{
    Q_UNUSED(errorCode);
    // not stubbed
}

void VideoServiceUriFetch::complete(QStringList filesList)
{
    Q_UNUSED(filesList);
    // not stubbed
}

void VideoServiceUriFetch::doComplete(QStringList filesList)
{
    Q_UNUSED(filesList);
    // not stubbed
}

bool VideoServiceUriFetch::isActive()
{
    // not stubbed
    return false;
}

QString VideoServiceUriFetch::contextTitle() const
{
    // not stubbed
    return QString();
}

void VideoServiceUriFetch::fetch()
{
    mServiceApp->setCurrentService(VideoServices::EUriFetcher);
}

void VideoServiceUriFetch::fetch(const QString& title)
{
    Q_UNUSED(title);
    
    mServiceApp->setCurrentService(VideoServices::EUriFetcher);
}

// end of file
