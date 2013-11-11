/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QString>
#include <QPixmap>

#include "cxeimagedataitem.h"
#include "cxestillimagesymbian.h"
#include "cxutils.h"

// Static initialization
int CxeStillImageSymbian::mNextId = CxeStillImage::INVALID_ID + 1;

/*!
* Constructor.
*/
CxeStillImageSymbian::CxeStillImageSymbian()
  : mSnapshot(),
    mDataItem(NULL),
    mSaved(false),
    mId(++mNextId)
{
}

/*!
* Destructor.
*/
CxeStillImageSymbian::~CxeStillImageSymbian()
{
    // Not owned.
    mDataItem = NULL;
}

/*!
* Get the contained data item.
* @return Image data item.
*/
CxeImageDataItem *CxeStillImageSymbian::dataItem()
{
    return mDataItem;
}

/*!
* Get the filename of this image.
* @return The filename.
*/
QString CxeStillImageSymbian::filename() const
{
    return mFilename;
}

/*!
* Get snapshot for this image.
* @return Snapshot for this image if set. Null pixmap otherwise.
*/
QPixmap CxeStillImageSymbian::snapshot() const
{
    return mSnapshot;
}

/*!
* Image saved status.
* @return Is this image saved.
*/
bool CxeStillImageSymbian::saved() const
{
    return mSaved;
}

/*!
* Set the unique id of this image.
* Usable in one application run context.
*/
int CxeStillImageSymbian::id() const
{
    return mId;
}

/*!
* Set snapshot for this image.
* @param pixmap The snapshot.
*/
void CxeStillImageSymbian::setSnapshot(QPixmap pixmap)
{
    mSnapshot = pixmap;
}

/*!
* Set the filename of this image.
* @param filename The filename.
*/
void CxeStillImageSymbian::setFilename(const QString &filename)
{
    mFilename = filename;
}

/*!
* Set this image as saved.
* @param saved Is this item saved or not.
*/
void CxeStillImageSymbian::setSaved(bool saved)
{
    mSaved = saved;
}

/*!
* Set the contained image data item.
* @param dataItem Image data item to store. Ownership not taken.
*/
void CxeStillImageSymbian::setDataItem(CxeImageDataItem *dataItem)
{
    // Disconnect from old data item if it exists.
    if (mDataItem) {
        disconnect(mDataItem, 0, this, 0);
    }

    // Set the new item.
    mDataItem = dataItem;

    // Connnect to the new item's saved signal.
    if (mDataItem) {
        bool ok = connect(mDataItem, SIGNAL(imageSaved(CxeError::Id, const QString&, int)),
                          this, SLOT(imageDataSaved(CxeError::Id)), Qt::UniqueConnection);
        CX_ASSERT_ALWAYS(ok);
    }
}

/*!
* Slot to handle saving the data item has finished.
*/
void CxeStillImageSymbian::imageDataSaved(CxeError::Id status)
{
    setSaved(status == CxeError::None);
}

// end of file
