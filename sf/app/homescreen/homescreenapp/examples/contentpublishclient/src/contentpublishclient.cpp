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
* Description: Example of home screen content publishing client
*
*/

#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>

#include <QMetaObject>
#include <QMetaMethod>

#include "contentpublishclient.h"

/*!
    \ingroup group_content_publish_client
    \class ContentPublishClient
    \brief Example implementation for home screen client api usage.

    ContentPublishClient is derived from QObject and implements 
    needed functions for the home screen content publish client. 
*/

/*!
    Constructor.
*/
ContentPublishClient::ContentPublishClient(QServiceManager &manager, QObject *parent)
    : QObject(parent),
      mManager(&manager)
{
}

/*!
    Destructor
*/
ContentPublishClient::~ContentPublishClient()
{
}

/*!
    Loads service interface
*/
bool ContentPublishClient::load()
{    
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = mManager->findInterfaces(filter);

    if(interfaces.isEmpty()) {
        QServiceManager::Error error = mManager->error();
        return false;
    }
    qDebug() << interfaces.first().interfaceName()
             << interfaces.first().serviceName()
             << interfaces.first().isValid();

    mService = mManager->loadInterface(interfaces.first());
        
    return (mService);
}

/*!
    Adds hello world widget 
*/
void ContentPublishClient::addHelloworldWidget()
{
    if ( addWidget("helloworldwidgetplugin")) {
        qDebug() << "HelloWorld added"; 
    } else {
        qDebug() << "adding HelloWorld failed"; 
    }
}

/*!
    Adds clock world widget 
*/
void ContentPublishClient::addClockWidget()
{
    if ( addWidget("hsclockwidgetplugin")) {
        qDebug() << "Clock widget added"; 
    } else {
        qDebug() << "adding Clock widget failed"; 
    }
}

/*!
    Adds widget \a widgetUri utilizing service interface and invokeMethod call
*/
// Start of snippet 1
bool ContentPublishClient::addWidget(QString widgetUri)
{   
    // find interface IHomeScreenClient from service hshomescreenclientplugin 
    QServiceManager manager;
    QServiceFilter filter("com.nokia.symbian.IHomeScreenClient");
    filter.setServiceName("hshomescreenclientplugin");
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(filter);

    if(interfaces.isEmpty()) {
        QServiceManager::Error error = manager.error();
        return false;
    }
    
    QObject* service = manager.loadInterface(interfaces.first());

    // access service's addWidget function
    bool retVal = false;
    bool ret = QMetaObject::invokeMethod( 
        service, 
        "addWidget",
        Qt::DirectConnection,
        Q_RETURN_ARG(bool, retVal),
        Q_ARG(QString,widgetUri),
        Q_ARG(QVariantHash,QVariantHash()));
        
    if(!ret){
        // invokeMethod returned error
        return false;
    }
    if(!retVal){
        // addWidget returned error
        return false;
    }
    return true;
}
// End of snippet 1

/*!
    Sets home screen wallpaper as testwallpaper.png 
*/
void ContentPublishClient::setWallpaper1()
{
    QString wallpaper = "c:/data/images/testwallpaper.jpg";
    setWallpaper(wallpaper);
}

/*!
    Sets home screen wallpaper as testwallpaper2.png 
*/
void ContentPublishClient::setWallpaper2()
{
    QString wallpaper = "c:/data/images/testwallpaper2.jpg";
    setWallpaper(wallpaper);
}

/*!

*/
void ContentPublishClient::setWallpaper3()
{
    QString portraitWallpaper = "c:/data/images/testwallpaper_p.png";
    QString landscapeWallpaper = "c:/data/images/testwallpaper_l.png";
    setWallpaper(portraitWallpaper, landscapeWallpaper);
}

/*!
    Changes home screen wallpaper to \a wallpaper image.
    Note. load function needs to be called before this, it creates mService object.
    
*/
// Start of snippet 2
bool ContentPublishClient::setWallpaper(QString wallpaper)
{   
    QByteArray signature = QMetaObject::normalizedSignature("setWallpaper(QString)");
    int methodIndex = mService->metaObject()->indexOfMethod(signature);   
    if (methodIndex<0) {
        return false;
    }
    QMetaMethod method = mService->metaObject()->method(methodIndex);
    bool retVal(false);

    bool ret = method.invoke( mService,
                              Qt::DirectConnection,
                              Q_RETURN_ARG(bool, retVal),
                              Q_ARG(QString,wallpaper));
                    
    if(!ret){
        // invokeMethod returned error
        return false;
    }
    if(!retVal){
        // setWallpaper returned error
        return false;
    }
    return true;
}

// End of snippet 2

bool ContentPublishClient::setWallpaper(const QString &portraitFileName, const QString &landscapeFileName)
{   
    QByteArray signature = QMetaObject::normalizedSignature("setWallpaper(QString,QString)");
    int methodIndex = mService->metaObject()->indexOfMethod(signature);   
    if (methodIndex<0) {
        return false;
    }
    QMetaMethod method = mService->metaObject()->method(methodIndex);
    bool retVal(false);

    bool ret = method.invoke( mService,
                              Qt::DirectConnection,
                              Q_RETURN_ARG(bool, retVal),
                              Q_ARG(QString,portraitFileName),
                              Q_ARG(QString,landscapeFileName));
                    
    if(!ret){
        // invokeMethod returned error
        return false;
    }
    if(!retVal){
        // setWallpaper returned error
        return false;
    }
    return true;
}
