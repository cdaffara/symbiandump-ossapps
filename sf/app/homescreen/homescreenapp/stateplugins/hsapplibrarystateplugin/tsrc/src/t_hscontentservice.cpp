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
#include "t_hscontentservice.h"

HsContentService::HsContentService(QObject *parent): QObject(parent) {}

HsContentService::~HsContentService() {}

bool HsContentService::widgets(const QString &uri, const QVariantHash &preferences, int &count)
{
    Q_UNUSED(uri)
    Q_UNUSED(preferences)
    count = 0;
    return true;
}

HsContentService *HsContentService::instance() {
    static HsContentService service;
    return &service;
}
