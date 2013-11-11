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

#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <QMetaObject>
#include <QMetaMethod>
#include <QDir>
#include <QUrl>
#include <QIcon>
#include <QWebSettings>
#include <QCryptographicHash>
#include <QTime>

#include "bedrockprovisioning.h"
#include "hsbookmarkpublishclient.h"

namespace WRT {

/*!
    \ingroup group_bookmark_publish_client
    \class HsBookmarkPublishClient
    \brief Implementation for homescreen bookmark publish client.

    HsBookmarkClient is derived from QObject and implements 
    needed functions for the homescreen bookmark publish client. 
*/

/*!
    Constructor.
*/
HsBookmarkPublishClient::HsBookmarkPublishClient(QObject *parent)
    : QObject(parent),
      mWidgetUri("hsbookmarkwidgetplugin")
{
	  setObjectName("hsBookmarkPublishClient");
}

/*!
    Destructor
*/
HsBookmarkPublishClient::~HsBookmarkPublishClient()
{
}

/*!
    Adds widget \a widgetUri utilizing service interface and invokeMethod call
*/
int HsBookmarkPublishClient::addWidget(QString Title, QString Url)
{   
    // find interface IHomeScreenClient from service hshomescreenclientplugin 
    QServiceManager manager;
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);
    QVariantHash params;
    QString iconFileName;
    params["bookmarkTitle"] = Title;
    params["bookmarkUrl"] = Url;
    params["faviconPath"] = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("DataBaseDirectory");

    if(interfaces.isEmpty()) {
        QServiceManager::Error error = manager.error();
        return FAILURE;
    }
    
    // Retrieve the favicon and check its exsistance
    QIcon icon = QWebSettings::iconForUrl(Url);
    if (!icon.isNull())
        iconFileName = getIconFileName(Url);
        
    params["faviconFileName"] = iconFileName;
    
    saveFavicon(Url, iconFileName);
    
    QObject* service = manager.loadInterface(interfaces.first());

    // access service's addWidget function
    bool retVal = false;
    bool ret = QMetaObject::invokeMethod( 
        service, 
        "addWidget",
        Qt::DirectConnection,
        Q_RETURN_ARG(bool, retVal),
        Q_ARG(QString,mWidgetUri),
        Q_ARG(QVariantHash,params));   
        
    if(!ret){
        // invokeMethod returned error
        return FAILURE;
    }
    if(!retVal){
        // addWidget returned error
        return FAILURE;
    }
    
    return SUCCESS;
}

bool HsBookmarkPublishClient::saveFavicon(const QString Url, const QString iconFileName)
{
    QIcon icon;
    QDir iconDir(BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("DataBaseDirectory"));
    QUrl url(Url);
    
    // Retrieve the favicon and check its exsistance
    icon = QWebSettings::iconForUrl(url);
    if (icon.isNull())
        return false;
	
    QString iconPath = iconDir.filePath(iconFileName);
    QFile iconFile(iconPath);
    iconFile.open(QIODevice::WriteOnly);
    
    QPixmap pixmap = icon.pixmap(30, 30);
    pixmap.save(&iconFile, "PNG");
    
    iconFile.close();
    
    return true;
}

/*!
    \internal
*/
QString HsBookmarkPublishClient::getIconFileName(QString url) const
{
    QByteArray hash = QCryptographicHash::hash(url.toAscii(), QCryptographicHash::Md5);
    QString iconFile = hash.left(16).toHex();    
    return iconFile + ".png";
}

}
