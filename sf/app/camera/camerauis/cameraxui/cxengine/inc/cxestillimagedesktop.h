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
#ifndef CXESTILLIMAGEDESKTOP_H
#define CXESTILLIMAGEDESKTOP_H

//  Include Files
#include <QString>
#include <QPixmap>
#include "cxestillimage.h"

class CxeImageDataItem;

class CxeStillImageDesktop : public CxeStillImage
{
public:
    CxeStillImageDesktop();
    virtual ~CxeStillImageDesktop();

public: // from CxeStillImage
    CxeImageDataItem *dataItem(); // NULL if already saved
    QString filename() const;
    QPixmap snapshot() const;
    bool saved() const;
    int id() const;

public:
    void setSnapshot(QPixmap pixmap);
    void setFilename(const QString &filename);
    void setSaved(bool saved);
    void setDataItem(CxeImageDataItem *dataItem);

private:
    QPixmap mSnapshot;
    CxeImageDataItem *mDataItem; // not own
    QString mFilename;
    bool mSaved;
    const int mId;
    static int mNextId;
};

#endif  // CXESTILLIMAGEDESKTOP_H
