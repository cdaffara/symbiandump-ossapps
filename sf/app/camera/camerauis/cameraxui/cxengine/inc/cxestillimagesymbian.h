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
#ifndef CXESTILLIMAGESYMBIAN_H
#define CXESTILLIMAGESYMBIAN_H

//  Include Files
#include <QObject>
#include "cxeerror.h"
#include "cxestillimage.h"

class QString;
class QPixmap;
class CxeImageDataItem;

class CxeStillImageSymbian : public QObject,
                             public CxeStillImage
{
    Q_OBJECT
public:
    CxeStillImageSymbian();
    virtual ~CxeStillImageSymbian();

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

private slots:
    void imageDataSaved(CxeError::Id status);

private:
    QPixmap mSnapshot;
    CxeImageDataItem *mDataItem; // not own
    QString mFilename;
    bool mSaved;
    const int mId;
    static int mNextId;
};

#endif  // CXESTILLIMAGESYMBIAN_H
