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
#ifndef CXETHUMBNAILMANAGERSYMBIAN_H_
#define CXETHUMBNAILMANAGERSYMBIAN_H_

#include <QHash>
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QImage>

// forward declaration
class ThumbnailManager;


/*!
* Class handles thumbnail creation for both still and video. It uses Thumbnail manager
* component for creating the thumb nails for the given snapshot and the file name.
*/
class CxeThumbnailManagerSymbian  : public QObject
{
    Q_OBJECT

public:

    CxeThumbnailManagerSymbian();
    ~CxeThumbnailManagerSymbian();

public slots:

    /**
     * Creates from given thumbnail.
     */
    virtual void createThumbnail(const QString &filename, const QImage &thumbnail);

    /**
     * Cancels thumbnail creation.
     */
    virtual void cancelThumbnail(const QString& filename);

private slots:

    /**
     * slot that gets called when thumbnail manager has created a thumbnail
     */
    void thumbnailReady(QPixmap thumbnail, void * data, int id, int error);

private:
    ThumbnailManager *mThumbnailManager;
    // file name, thumbnail id hash table
    QHash<QString, int> mThumbnailRequests;
};

#endif /* CXETHUMBNAILMANAGERSYMBIAN_H_ */
