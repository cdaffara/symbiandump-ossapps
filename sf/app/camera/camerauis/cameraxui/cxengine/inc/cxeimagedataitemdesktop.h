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
#ifndef CXEIMAGEDATAITEMDESKTOP_H
#define CXEIMAGEDATAITEMDESKTOP_H

#include <QPixmap>
#include "cxeimagedataitem.h"


/**
 * CxeImageDataItem represents a single object that is in a queue to be saved to
 * the file system.
 */
class CxeImageDataItemDesktop : public CxeImageDataItem
{
    Q_OBJECT

public:
    CxeImageDataItemDesktop(int index, QByteArray data, QString filename, int id);
    CxeImageDataItemDesktop(int index, QPixmap pixmapData, QString filename, int id);
public: // from CxeImageDataItem
    virtual CxeError::Id save();
    virtual CxeImageDataItem::State state() const;
    virtual int id() const;
    virtual QString path() const;

protected:
    int mIndex;
    int mId;
    QByteArray mData;
    QString mFilename;
    QPixmap mPixmapData;
    CxeImageDataItem::State mState;
};

#endif // CXEIMAGEDATAITEMDESKTOP_H
