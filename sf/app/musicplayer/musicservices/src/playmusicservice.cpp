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
#include "playmusicservice.h"
#include "mptrace.h"




/*!
 Constructs play music service
 */
PlayMusicService::PlayMusicService( MusicServices* parent )
: XQServiceProvider(QLatin1String("musicplayer.com.nokia.symbian.IFileView"), parent),
  mRequestIndex( 0 ),
  mServiceApp( parent )
{
    TX_ENTRY
    publishAll();
    TX_EXIT
}

/*!
 Destructs play music service
 */
PlayMusicService::~PlayMusicService()
{
    TX_LOG
}


/*!
 Returns whether current service is still active
 */
bool PlayMusicService::isActive()
{
    return mRequestIndex > 0;
}

/*!
 Completes current request 
 */
void PlayMusicService::complete(QStringList filesList)
{   TX_ENTRY_ARGS("EmbeddeddoComplete: ");
    if ( isActive() ) {
        TX_ENTRY_ARGS("EmbeddeddoComplete2: ");
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        completeRequest(mRequestIndex, filesList);
        mRequestIndex=0;
    }
}

/*!
 Returns title of calling application
 */
QString PlayMusicService::contextTitle() const
{
    return mTitle;
}

/*!
    Embedded Playback service interface defined in service registration xml.

    Example usage:
    
    QFile file("c:\\audio.mp3");

    mReq = mAppMgr.create(file);
    if (mReq == NULL)
    {
           // No handlers for the URI
           return;
     }
    // By default operation is "view(QString)"

    // Set function parameters
   QList<QVariant> args;
   args << file.fileName();
   mReq->setArguments(args);
   QString title("WindowTitle");
   QVariant title2(QString("<app_name>View"));
   XQRequestInfo info;
   info.setInfo(title, title2);
   mReq->setInfo(info);
   // Send the request
   bool res = mReq->send();
   if  (!res) 
   {
       // Request failed. 
      int error = mReq->lastError();

      // Handle error
   }
 
   // If making multiple requests to same service, you can save the request as member variable
   // In this example all done.
   delete mReq;
 */

bool PlayMusicService::view(const QString& file)
{
    TX_ENTRY
	QString uri(file);
    uri.replace(QString("/"),QString("\\"));
    mTitle = requestInfo().info("WindowTitle").toString();
    mServiceApp->setCurrentService( MusicServices::EPlayback );
    emit mServiceApp->serviceActive( requestInfo().clientSecureId() );
    emit mServiceApp->playReady( uri );
    connect(this, SIGNAL( clientDisconnected() ), qApp, SLOT( quit() ) );
    mRequestIndex = setCurrentRequestAsync();
    
    return true;
    
}

/*!
    Embedded Playback service interface defined in service registration xml.

    Example usage:
    
   XQSharableFile sf;
    if (!sf.open("c:\\audio.mp3"))
    {
        // Failed to open sharable file
        return;
    }

    // Create request for the sharable file
    mReq = mAppMgr.create(sf);
    if (!mReq)
    {
         // No viewer app found for the file
         // As we opened the handle, we need to close it !
         sf.close(); 
         return;  
    }
    // By default operation is "view(XQSharableFile)"

    // Set function parameters
    // Not only one sharable handle supported,  otherwise upon send EArgumentError error occurs
    QList<QVariant> args;
    args << qVariantFromValue(sf);  
    mReq->setArguments(args);
    QString title("WindowTitle");
   QVariant title2(QString("<app_name>"));
   XQRequestInfo info;
   info.setInfo(title, title2);
   mReq->setInfo(info);
   // Send the request
   bool res = mReq->send();
   if  (!res) 
   {
       // Request failed. 
      int error = mReq->lastError();
      // Handle error
   }

    // As we opened the handle, we need to close it !
    sf.close(); 

   // If making multiple requests to same service, you can save the mReq as member variable
   // In this example all done.
   delete mReq;
 */
bool PlayMusicService::view(const XQSharableFile& file)
{
    TX_ENTRY
    mTitle = requestInfo().info("WindowTitle").toString();
    mServiceApp->setCurrentService( MusicServices::EPlayback );
    emit mServiceApp->serviceActive( requestInfo().clientSecureId() );
    emit mServiceApp->playReady( file );
    connect(this, SIGNAL( clientDisconnected() ), qApp, SLOT( quit() ) );
    mRequestIndex = setCurrentRequestAsync();
    return true;
}
