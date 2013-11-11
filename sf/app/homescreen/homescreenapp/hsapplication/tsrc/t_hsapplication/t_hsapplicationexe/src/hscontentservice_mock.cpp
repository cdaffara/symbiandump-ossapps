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
* Description:
*
*/
#include "hscontentservice.h"

HsContentService::HsContentService(QObject *parent): QObject(parent) {}

HsContentService::~HsContentService() {}

HsContentService *HsContentService::instance() {
    static HsContentService service;
    return &service;
}
bool HsContentService::createWidget(const QVariantHash &params)
{
    Q_UNUSED(params);
    return true;
}

HsWidgetHost *HsContentService::createWidgetForPreview(const QVariantHash &params)
{
    Q_UNUSED(params);
    return 0;
}

bool HsContentService::addWidget(const QString &uri, const QVariantHash &preferences,
                                 const QVariant &homescreenData)
{
    Q_UNUSED(preferences);
    Q_UNUSED(homescreenData);
    if (!uri.isEmpty()) {
        return true;
    } else {
        return false;
    }
}

void HsContentService::emitWidgetRemoved(const QString &uri, const QVariantHash &preferences)
{
    Q_UNUSED(uri);
    Q_UNUSED(preferences);
}

void HsContentService::widgetStartFaulted()
{
}
