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
#ifndef CXESTILLIMAGE_H
#define CXESTILLIMAGE_H

#include <QString>
#include <QPixmap>
#include <QObject>

class CxeImageDataItem;

/**
 * The CxeStillImage class represents a single image (part of a burst capture). This class can
 * be used to access the file name and snapshot bitmap for the image.
 *
 * The dataItem method can be used to access the CxeImageDataItem object, which represents
 * the image in the file saving queue.
 */
class CxeStillImage
{
public:
    static const int INVALID_ID = -1;

    virtual CxeImageDataItem* dataItem() = 0; // NULL if already saved or if image data has not yet been received

    virtual QString filename() const = 0;

    /**
     * Get the snapshot image.
     *
     * @return Snapshot pixmap. May be null pixmap if no snapshot is available.
     */
    virtual QPixmap snapshot() const = 0;

    virtual bool saved() const = 0;

    virtual int id() const = 0;

protected:
    CxeStillImage() {}

private:
    Q_DISABLE_COPY( CxeStillImage )
};


#endif // CXESTILLIMAGE_H
