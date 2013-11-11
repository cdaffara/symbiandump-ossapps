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
* Description: QDesktopServices stub for testing MpMtpInfoLink.
*
*/

#include <QUrl>

#include "stub/inc/qdesktopservices.h"


/*!
 Constructs the QDesktopServices.
 */
QDesktopServices::QDesktopServices()
{
    mOpenUrlCount = 0;
}

/*!
 Destructs the QDesktopServices.
 */
QDesktopServices::~QDesktopServices()
{
}

/*!
 Returns the singleton instance to the QDesktopServices.
 */
QDesktopServices * QDesktopServices::instance()
{
    static QDesktopServices instance;
    return &instance;
}

bool QDesktopServices::openUrl( const QUrl &url)
{
    Q_UNUSED( url );
    instance()->mOpenUrlCount++;
    return true;
}

