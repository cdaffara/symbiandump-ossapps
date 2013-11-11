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
* Description: ThumbnailManager stub for testing MpSongData
*
*/
#include <QSize>
#include "stub/inc/thumbnailmanager_qt.h"
#include "mptrace.h"

int gInitCounter = 0;

/*!
 Returns gInitCounter.
 gInitCounter counts the number of constructor/destructor calls.
 */
int ThumbnailManager::getInitCounter()
{
    TX_LOG_ARGS("ThumbnailManager::getInitCounter() STUB");
    return gInitCounter;
}

/*!
 Resets gInitCounter to zero.
 */
void ThumbnailManager::resetInitCounter()
{
    TX_LOG_ARGS("ThumbnailManager::resetInitCounter() STUB");
    gInitCounter = 0;
}

ThumbnailManager::ThumbnailManager( QObject* parentPtr ) :
QObject( parentPtr ),
mGetThumbFails(false),
mThumbnailReqCounter(0),
mCancelCounter(0)
{
    TX_ENTRY_ARGS("Stub")    
    gInitCounter++;
}

ThumbnailManager::~ThumbnailManager()
{
    TX_ENTRY_ARGS("Stub")    
    gInitCounter--;
}

bool ThumbnailManager::setMode( ThumbnailMode mode )
{
    TX_ENTRY_ARGS("Stub")    
    Q_UNUSED(mode);
    return true;
}

bool ThumbnailManager::setQualityPreference( QualityPreference
    qualityPreference )
{
    TX_ENTRY_ARGS("Stub")    
    Q_UNUSED(qualityPreference);
    return true;
}

bool ThumbnailManager::setThumbnailSize( ThumbnailSize thumbnailSize )
{
    TX_ENTRY_ARGS("Stub")    
    Q_UNUSED(thumbnailSize);
    return true;
}

int ThumbnailManager::getThumbnail( const QString& fileName, void *clientData,
        int priority )
{
    TX_ENTRY_ARGS("Stub")    
    Q_UNUSED(fileName);
    Q_UNUSED(priority);

    if (clientData) {
        // Delete this to prevent memory leak
        int *data = (int *)clientData;
        delete data;

        data = 0;
    }

    if ( mGetThumbFails ) {
        return -1;
    }

    mThumbnailReqCounter++;
    return mThumbnailReqCounter;
}

bool ThumbnailManager::cancelRequest( int id )
{
    TX_ENTRY_ARGS("Stub")    
    Q_UNUSED(id);
    mCancelCounter++;
    return true;
}
