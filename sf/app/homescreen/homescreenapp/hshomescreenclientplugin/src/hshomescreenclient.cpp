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
* Description:  Homescreen client api
*
*/
#include "hshomescreenclient.h"
#include <xqservicerequest.h>
#include <xqrequestinfo.h>
#include <QEventLoop>

const char INTERFACE_NAME[] = "com.nokia.symbian.IHomeScreenClient";


/*!
    \class HsHomescreenClient
    \ingroup group_hshomescreenclient
    \brief Implements home screen client api, see @page_homescreenclientapi for 
    usage details.
    
*/


/*!
  Constructor
*/
HsHomescreenClient::HsHomescreenClient(QObject *parent)
    :QObject(parent),
    mAsyncRequest(0),
    mRequestResult(false)
{   
}

/*!
  Destructor
*/
HsHomescreenClient::~HsHomescreenClient()
{
    delete mAsyncRequest;
}

/*!
    Adds new widget, with the given \a uri and \a preferences, 
    to the active home screen page.
    Note that \a preferences is dependent on the 
    widget implementation and it can be empty if widget does not have any preferences see @ref sssection_setpreferences. 
 */
bool HsHomescreenClient::addWidget(
    const QString &uri, 
    const QVariantHash &preferences)
{
    QEventLoop eventLoop;
    connect(this, SIGNAL(requestFinished()), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(this, "doAddWidget", Qt::QueuedConnection,
            Q_ARG(QString,uri),Q_ARG(QVariantHash,preferences));
    eventLoop.exec();
   
    return mRequestResult;
}

/*!
   Sets wallpaper \a fileName. Filename should include full path to the image file.
 */
bool HsHomescreenClient::setWallpaper(const QString &fileName)
{
    QEventLoop eventLoop;
    connect(this, SIGNAL(requestFinished()), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(
            this, 
            "doSetWallpaper", 
            Qt::QueuedConnection,
            Q_ARG(QString,fileName));
    eventLoop.exec();
   
    return mRequestResult;
}

/*!

*/
bool HsHomescreenClient::setWallpaper(const QString &portraitFileName, const QString &landscapeFileName)
{
    QEventLoop eventLoop;
    connect(this, SIGNAL(requestFinished()), &eventLoop, SLOT(quit()));
    QMetaObject::invokeMethod(
            this, 
            "doSetWallpaper2", 
            Qt::QueuedConnection,
            Q_ARG(QString,portraitFileName),
            Q_ARG(QString,landscapeFileName));
    eventLoop.exec();
   
    return mRequestResult;
}
    
/*!
   Called when request is completed.
 */
void HsHomescreenClient::onRequestCompleted(const QVariant &result)
{
    mRequestResult = result.toBool();
    emit requestFinished();
}
    
/*!
   Called when request has some errors.
 */
void HsHomescreenClient::onRequestError(int error)
{
    Q_UNUSED(error)
    emit requestFinished();
}
    
/*!
 * Uses Qt Highway to send 'addWidget' request to home screen application.
 * \a uri and \a preferences as in widget model.
 */
void HsHomescreenClient::doAddWidget(
    const QString &uri, 
    const QVariantHash &preferences)
{
    delete mAsyncRequest;
    mAsyncRequest = 0;
    mAsyncRequest = new XQServiceRequest(INTERFACE_NAME,
                       "addWidget(QString,QVariantHash)", false);
    
    XQRequestInfo requestInfo = mAsyncRequest->info();
    requestInfo.setBackground(true);
    mAsyncRequest->setInfo(requestInfo);
    
    *mAsyncRequest << uri;
    *mAsyncRequest << preferences;
    
    connect(mAsyncRequest, SIGNAL(requestCompleted(QVariant)), 
            SLOT(onRequestCompleted(QVariant)));
    connect(mAsyncRequest, SIGNAL(requestError(int)), 
            SLOT(onRequestError(int)));
       
    mRequestResult = false;
    if (!mAsyncRequest->send()) {
       emit requestFinished();
    }
}

/*!
 * Uses Qt Highway to send 'setWallpaper' request to home screen application.
 * \a fileName full path to the image file.
 */
void HsHomescreenClient::doSetWallpaper(const QString &fileName)
{
    delete mAsyncRequest;
    mAsyncRequest = 0;
    mAsyncRequest = new XQServiceRequest(INTERFACE_NAME,
                        "setWallpaper(QString)", false);
    XQRequestInfo requestInfo = mAsyncRequest->info();
    requestInfo.setBackground(true);
    mAsyncRequest->setInfo(requestInfo);
    
    *mAsyncRequest << fileName;
    
    connect(mAsyncRequest, SIGNAL(requestCompleted(QVariant)), 
            SLOT(onRequestCompleted(QVariant)));
    connect(mAsyncRequest, SIGNAL(requestError(int)),
            SLOT(onRequestError(int)));
       
    mRequestResult = false;
    if (!mAsyncRequest->send()) {
       emit requestFinished();
    }    
}

void HsHomescreenClient::doSetWallpaper2(const QString &portraitFileName, const QString &landscapeFileName)
{
    delete mAsyncRequest;
    mAsyncRequest = 0;
    mAsyncRequest = new XQServiceRequest(INTERFACE_NAME,
                        "setWallpaper(QString,QString)", false);
    XQRequestInfo requestInfo = mAsyncRequest->info();
    requestInfo.setBackground(true);
    mAsyncRequest->setInfo(requestInfo);
    
    *mAsyncRequest << portraitFileName;
    *mAsyncRequest << landscapeFileName;
    
    connect(mAsyncRequest, SIGNAL(requestCompleted(QVariant)), 
            SLOT(onRequestCompleted(QVariant)));
    connect(mAsyncRequest, SIGNAL(requestError(int)),
            SLOT(onRequestError(int)));
       
    mRequestResult = false;
    if (!mAsyncRequest->send()) {
       emit requestFinished();
    }
}
