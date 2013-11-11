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

#ifndef __SECUREUICONTROLLER_H__
#define __SECUREUICONTROLLER_H__

#include <QWidget>
#include <QSslError>
#include <QNetworkReply>
#include "BWFGlobal.h"
#include <QWebFrame>
#include <QMessageBox>
#include "messageboxproxy.h"

#define NOLOAD 0x0
#define TOPLEVELLOADED 0x01
#define TOPLEVELSECURE 0x02
#define TOPLEVELUNTRUSTED 0x04
#define SUBLOADUNSECURE 0x08

namespace WRT {

    class SecureUIControllerPrivate;

    class BWF_EXPORT SecureUIController : public QObject,
                                          public MessageBoxProxy
    {
        Q_OBJECT
    public:
        enum SecureState{
            unsecureLoadFinished = 0,
            secureLoadFinished,
            untrustedLoadFinished,
            mixedLoadFinished,
            untrustedMixedLoadFinished,
            secureInvalid
        };
       /*!
        * SecureUIController Constructor
        * @param parent: parent QObject 
        */  
        SecureUIController(QObject* parent = 0);
        /*!
         * SecureUIController Destructor
         */ 

        ~SecureUIController();

        QList<QString> * sslTrustedHostList();
        int secureState();
        void setSecureState(int state);
        QNetworkReply* getReply();
        void setReply(QNetworkReply* reply);

        void onMessageBoxResponse(int retValue);

    public slots:

        void setTopLevelScheme(const QUrl & url);
        void secureCheck(QWebFrame* frame, QNetworkRequest* request);
        void endSecureCheck(bool loadFinished);
        void onSslErrors(QNetworkReply*,const QList<QSslError> &);
                
    signals:
        void pageSecureState(int);
        void loadStopped();
        void showMessageBox(WRT::MessageBoxProxy* data);
        
    private:
        void determineSecureState();
        SecureUIControllerPrivate * const d;
    };
}
#endif // __SECUREUICONTROLLER_H__
