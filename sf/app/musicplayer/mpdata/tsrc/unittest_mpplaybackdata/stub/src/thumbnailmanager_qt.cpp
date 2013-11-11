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
* Description: ThumbnailManager stub for testing MpPlaybackData
*
*/

#include <QSize>
#include "stub/inc/thumbnailmanager_qt.h"

int gInitCounter = 0;

/*!
 Returns gInitCounter.
 gInitCounter counts the number of constructor/destructor calls.
 */
int ThumbnailManager::getInitCounter()
{
    return gInitCounter;
}

/*!
 Resets gInitCounter to zero.
 */
void ThumbnailManager::resetInitCounter()
{
    gInitCounter = 0;
}

/*!
 Stub function.
 */
ThumbnailManager::ThumbnailManager( QObject* parentPtr ) :
QObject( parentPtr ),
mGetThumbFails(false),
mThumbnailReqCounter(0),
mCancelCounter(0)
{
    gInitCounter++;
}

/*!
 Stub function.
 */
ThumbnailManager::~ThumbnailManager()
{
    gInitCounter--;
}

/*!
 Stub function.
 */
bool ThumbnailManager::setMode( ThumbnailMode mode )
{
    Q_UNUSED(mode);
    return true;
}

/*!
 Stub function.
 */
bool ThumbnailManager::setQualityPreference( QualityPreference
    qualityPreference )
{
    Q_UNUSED(qualityPreference);
    return true;
}

/*!
 Stub function.
 */
bool ThumbnailManager::setThumbnailSize( ThumbnailSize thumbnailSize )
{
    Q_UNUSED(thumbnailSize);
    return true;
}

/*!
 Stub function.
 */
int ThumbnailManager::getThumbnail( const QString& fileName, void *clientData,
        int priority )
{
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

/*!
 Stub function.
 */
bool ThumbnailManager::cancelRequest( int id )
{
    Q_UNUSED(id);
    mCancelCounter++;
    return true;
}

