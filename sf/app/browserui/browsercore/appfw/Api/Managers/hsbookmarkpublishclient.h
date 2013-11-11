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
* Description: Homescreen bookmark publishing client
*
*/
#ifndef HSBOOKMARKPUBLISHCLIENT_H
#define HSBOOKMARKPUBLISHCLIENT_H
#include "brtglobal.h"
#include <QObject>

#include <qservicemanager.h>

#include "singleton.h"

QTM_USE_NAMESPACE

namespace WRT {

class WRT_BROWSER_EXPORT HsBookmarkPublishClient: public QObject
{
    Q_OBJECT
    
public:

    enum status {
            SUCCESS = 0,
            FAILURE = -1
    };
    
    HsBookmarkPublishClient(QObject *parent=0 );
    ~HsBookmarkPublishClient();
    
public slots:
    int addWidget(QString Title, QString Url);
    
private:
    bool saveFavicon(const QString Url, const QString iconFileName);
    QString getIconFileName(QString url) const;
	  
    QString mWidgetUri;   
};

// Define the Meyer's singleton for HsBookmarkPublishClient 
typedef Singleton<HsBookmarkPublishClient> HsBookmarkPublishClientSingleton;

}

#endif HSBOOKMARKPUBLISHCLIENT_H
