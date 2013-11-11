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
#include "playmusicservice.h"
#include "mptrace.h"

/*!
    \class MusicServices
    \brief Music services available.

    MusicServices implements fetching of music URIs via service provider API and embedded playback.
*/

/*!
    \fn void playReady( const QString& uri )

    This signal is emitted when the play service has been loaded and playback of \a uri should start.

 */

/*!
    \fn void playReady( const XQSharableFile& file )

    This signal is emitted when the play service has been loaded and playback of file handle should start

 */

/*!
    \fn void serviceActive( quint32 clientSecureId )

    This signal is emitted when the requested service has been detected and view initialization should continue.

 */

/*!
 Constructs music services
 */
MusicServices::MusicServices() : 
        mCurrentService( MusicServices::ENoService )
{
    TX_ENTRY
    mFetchService = new GetMusicService( this );
    mPlayService = new PlayMusicService( this );
    TX_EXIT
}

/*!
 Destruct music services
 */
MusicServices::~MusicServices()
{
    TX_ENTRY
    delete mFetchService;
    delete mPlayService;
    TX_EXIT
}

/*!
 Gets current active service
 */
MusicServices::MusicService MusicServices::currentService()
{
    return mCurrentService;
}

/*!
 Sets current active service
 */
void MusicServices::setCurrentService( MusicServices::MusicService service )
{
    mCurrentService = service;
}

/*!
 Returns service context title
 */
QString MusicServices::contextTitle() const
{
    if( mCurrentService == MusicServices::EUriFetcher ) {
        return mFetchService->contextTitle();
    }
    else if ( mCurrentService == MusicServices::EPlayback ) {
        return mPlayService->contextTitle();
    }
    else {
        Q_ASSERT_X(false, "MusicServices::contexTitle", "undefined service");
        return QString();
    }
}

/*!
 Slot to be called when service is to be completed with selected item \songName
 */
void MusicServices::itemSelected( QString songName )
{
    TX_ENTRY_ARGS("songName: " << songName);
    QStringList list;
    list.append( songName );
    if ( mCurrentService == MusicServices::EUriFetcher ) {
        mFetchService->complete( list );
    }
    else if ( mCurrentService == MusicServices::EPlayback ) {
        mPlayService->complete( list );
    }
}

