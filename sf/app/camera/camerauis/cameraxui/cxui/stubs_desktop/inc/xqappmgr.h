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

#ifndef XQAPPMGRDESKTOP_H
#define XQAPPMGRDESKTOP_H

#include <QVariant>

class XQAiwRequest
{
public:
    inline void setArguments(QVariantList &fileList) {}
    inline bool send(QVariant &result) {return false;}
    inline int lastError() {return 1;}
};

class XQApplicationManager
{
public:
    XQApplicationManager() : videoRequest(NULL) { }
    inline XQAiwRequest *create(const QString &service, const QString &operation, bool embedded = true)
    {
        videoRequest = new XQAiwRequest();
        return videoRequest;
    }

    inline ~XQApplicationManager() {if (videoRequest) delete videoRequest;}

private:
    // Owned
    XQAiwRequest *videoRequest;
};

#endif // XQAPPMGRDESKTOP_H
