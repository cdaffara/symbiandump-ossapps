/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef __DOWNLOAD_PROXY_H__
#define __DOWNLOAD_PROXY_H__

#include <QString>
#include <QSharedPointer>

#include "BWFGlobal.h"

// Each DownloadProxy object encapsulates an instance of the Download class
// used by the Download Manager.  DownloadProxy is used to shield client
// code from direct knowledge of the Download class.
//
// Knowledge of the Download class is confined to the DownloadProxyData class.
// DownloadProxyData objects are created by the DownloadController as necessary
// to construct DownloadProxy objects that may be exposed to client code via
// signals or other mechanisms.

class DownloadProxyData;

class BWF_EXPORT DownloadProxy
{
public:
    DownloadProxy(DownloadProxyData * data);

    ~DownloadProxy();

    void debug();

    void remove();

    QString fileName();

private:
    QSharedPointer<DownloadProxyData> m_data;
};

#endif // __DOWNLOAD_PROXY_H__
