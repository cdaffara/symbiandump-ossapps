/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SECUREUICONTROLLER_P_H__
#define __SECUREUICONTROLLER_P_H__

#include <QWidget>
#include "BWFGlobal.h"
#include <QNetworkReply>

namespace WRT {

    class SecureUIController;
    class SecureUIControllerPrivate
    {
     public:
        SecureUIControllerPrivate(SecureUIController* qq);
        ~SecureUIControllerPrivate();

        SecureUIController * const q;

        QWidget * m_widgetParent; // owned
        int m_internalState;
        int m_secureState;
        QList<QString> m_sslTrustedHostList;
        QNetworkReply* m_reply;
    };
}
#endif // __SECUREUICONTROLLER_P_H__
