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
* Description:
*
*/

#include "cxeimagedataitemdesktop.h"

CxeImageDataItemDesktop::CxeImageDataItemDesktop(int index, QByteArray data, QString filename, int id) :
    mIndex(index),
    mData(data),
    mFilename(filename),
    mPixmapData(QPixmap()),
    mState(CxeImageDataItem::SavePending),
    mId(id)

{

}

CxeImageDataItemDesktop::CxeImageDataItemDesktop(int index, QPixmap pixmapData, QString filename, int id) :
    mIndex(index),
    mData(QByteArray()),
    mFilename(filename),
    mPixmapData(pixmapData),
    mState(CxeImageDataItem::SavePending),
    mId(id)
{

}


/**
 * Helper method to save file to Fs
 * @return CameraX error code
 */
CxeError::Id CxeImageDataItemDesktop::save()
{
    CxeError::Id errorId = CxeError::None;

    mPixmapData.save(mFilename);

    mState = CxeImageDataItem::Saved;
    emit stateChanged(mState, errorId);

    return errorId;
}

/**
 * Get the current state for this item.
 *
 * @return State
 */
CxeImageDataItem::State CxeImageDataItemDesktop::state() const
{
    return mState;
}

/**
 * Get the id of this item.
 */
int CxeImageDataItemDesktop::id() const
{
    return mId;
}

/**
 * Get the id of this item.
 */
QString CxeImageDataItemDesktop::path() const
{
    return mFilename;
}
