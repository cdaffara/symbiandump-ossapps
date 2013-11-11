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

#ifndef QDESKTOPSERVICES_H
#define QDESKTOPSERVICES_H

#include <QtCore/qstring.h>

class QUrl;

class QDesktopServices
{
private:
    explicit QDesktopServices();

public:
    virtual ~QDesktopServices();
    static QDesktopServices *instance();
    static bool openUrl( const QUrl &url);

    int mOpenUrlCount;

};

#endif    // QDESKTOPSERVICES_H
