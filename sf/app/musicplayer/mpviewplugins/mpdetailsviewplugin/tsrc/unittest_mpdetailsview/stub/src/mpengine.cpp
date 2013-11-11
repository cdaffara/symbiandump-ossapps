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
* Description: Wrapper for mpx framework utilities.
*
*/

#include "mpengine.h"

#include "mptrace.h"


MpEngine::MpEngine()
{
    TX_LOG_ARGS("Stub")
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::refreshLibrary()
{
    TX_ENTRY_ARGS("Stub")
}

bool MpEngine::verifyUsbBlocking( bool showMessage )
{
    Q_UNUSED(showMessage)
    TX_ENTRY_ARGS("Stub")
    return false;
}

void MpEngine::checkForSystemEvents()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleScanStarted() 
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleScanEnded( int count, int error ) 
{
    Q_UNUSED(count)
    Q_UNUSED(error)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleDiskEvent( MpxDiskEvents event )
{
    Q_UNUSED(event)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleUsbEvent( MpxUsbEvents event )
{
    Q_UNUSED(event)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleBlockingNoteClosing()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openCollection( TCollectionContext context )
{
    Q_UNUSED(context)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openCollectionItem( int index )
{
    Q_UNUSED(index)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::back()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::findPlaylists( QStringList &playlists )
{
    Q_UNUSED(playlists)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    Q_UNUSED(playlistName)
    Q_UNUSED(selection)
    Q_UNUSED(collectionData)

    TX_ENTRY_ARGS("Stub")
}

void MpEngine::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    Q_UNUSED(playlistIndex)
    Q_UNUSED(selection)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::renamePlaylist( QString &newName, int index )
{
    Q_UNUSED(newName)
    Q_UNUSED(index)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    Q_UNUSED(selection)
    Q_UNUSED(collectionData)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::renamePlaylist( QString &newName )
{
    Q_UNUSED(newName)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::deleteSongs( QList<int> &selection )
{
    Q_UNUSED(selection)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::previewItem( int index )
{
    Q_UNUSED(index)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openIsolatedCollection( TCollectionContext context )
{
    Q_UNUSED(context)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::releaseIsolatedCollection()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::findAlbumSongs( int index )
{
    Q_UNUSED(index)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playAlbumSongs( int albumIndex, int songIndex )
{
    Q_UNUSED(albumIndex)
    Q_UNUSED(songIndex)
    TX_ENTRY_ARGS("Stub")
}

MpMpxCollectionData *MpEngine::collectionData()
{
    TX_ENTRY_ARGS("Stub")
    return false;
}

void MpEngine::reopenCollection()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    Q_UNUSED(playlistId)
    Q_UNUSED(songId)
    Q_UNUSED(originalOrdinal)
    Q_UNUSED(newOrdinal)
    TX_ENTRY_ARGS("Stub")
}

MpPlaybackData *MpEngine::playbackData()
{
    TX_ENTRY_ARGS("Stub")
    return false;
}


void MpEngine::playEmbedded( QString aFilename )
{
    Q_UNUSED(aFilename)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playEmbedded(const XQSharableFile& file )
{
    Q_UNUSED(file)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playPause()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::stop()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::skipForward()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::skipBackward()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setPosition( int position )
{
    Q_UNUSED(position)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setShuffle( bool mode )
{
    Q_UNUSED(mode)
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setRepeat( bool mode )
{
    Q_UNUSED(mode)
    TX_ENTRY_ARGS("Stub")
}

MpSongData *MpEngine::songData()
{
    TX_ENTRY_ARGS("Stub")
    return 0;
}

void MpEngine::retrieveSong()
{
    TX_ENTRY_ARGS("Stub")
}
