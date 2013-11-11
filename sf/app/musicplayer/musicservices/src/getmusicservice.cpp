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
* Description:
*
*/

#include <hbapplication.h>
#include <QStringList>

#include "musicservices.h"
#include "getmusicservice.h"
#include "mptrace.h"


/*!
 Constructs music fetcher service
 */
GetMusicService::GetMusicService( MusicServices* parent )
    : XQServiceProvider( QLatin1String( "musicplayer.com.nokia.symbian.IMusicFetch" ), parent ),
    mRequestIndex( 0 ),
    mServiceApp( parent )
{
    TX_ENTRY
    publishAll();
    TX_EXIT
}

/*!
 Destructs music fetcher service
 */
GetMusicService::~GetMusicService()
{
    TX_LOG
}

/*!
 Returns whether current service is still active
 */
bool GetMusicService::isActive()
{
    return mRequestIndex > 0;
}

/*!
 Completes current request 
 */
void GetMusicService::complete(QStringList filesList)
{
    if ( isActive() ) {
        connect( this, SIGNAL( returnValueDelivered() ), qApp, SLOT( quit() ) );
        completeRequest( mRequestIndex, filesList );
        mRequestIndex = 0;
    }
}

/*!
 Returns title of calling application
 */
QString GetMusicService::contextTitle() const
{
    return mTitle;
}


/*!
    Music Fetcher service interface defined in service registration xml.
    Service client needs to create a request using interface name
    "com.nokia.services.media.Music" and set string to show in
    opened views title as a request argument.

    Example usage:
    \code
    XQApplicationManager appMgr;
    XQAiwRequest* req = appMgr.create("com.nokia.services.media.IMusicFetch",
        "fetch(QString)", true);

    if (req)
    {
        connect(req, SIGNAL(requestOk(const QVariant&)),
            SLOT(handleRequestOk(const QVariant&)));
        connect(req, SIGNAL(requestError(int,const QString&)),
            SLOT(handleRequestError(int,const QString&)));

        // Set argument for request (title for opened views)
        QList<QVariant> args;
        args << QVariant(QString("<title to show>"));
        req->setArguments(args);

        // Make the request
        if (!req->send())
        {
            qDebug() <<  "Failed to send REQ";
        }
        delete req;
    }
    \endcode
 */
void GetMusicService::fetch()
{
    TX_ENTRY
   //TODO: change to string constant when available
    mTitle = requestInfo().info("WindowTitle").toString();
    mServiceApp->setCurrentService( MusicServices::EUriFetcher );
    emit mServiceApp->serviceActive( requestInfo().clientSecureId() );
    connect(this, SIGNAL( clientDisconnected() ), qApp, SLOT( quit() ) );
    mRequestIndex = setCurrentRequestAsync();
 
    TX_EXIT
}
