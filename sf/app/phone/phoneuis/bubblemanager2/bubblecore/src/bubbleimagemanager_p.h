/*!
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

#ifndef BUBBLEIMAGEMANAGER_P_H_
#define BUBBLEIMAGEMANAGER_P_H_

#include <QObject>
#include <QMap>
#include <QPixmap>
#include <QQueue>
#include <QPair>

class ThumbnailManager;
class BubbleImageManager;

class BubbleImageManagerPrivate : public QObject
    {
    Q_OBJECT

    Q_DECLARE_PUBLIC( BubbleImageManager )

public:
    explicit BubbleImageManagerPrivate(BubbleImageManager* parent = 0);
    virtual ~BubbleImageManagerPrivate();

    const QPixmap* pixmap(const QString& imageFileName);

    void loadImage(const QString& imageFileName);

    void unloadImage(const QString& imageFileName);

    void releasePixmaps();

protected:
    void initialize();
    bool isLoading(const QString& imageFileName) const;

protected slots:
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void thumbnailReady(); // for stub

signals:
    void pixmapReady(const QString& imageFileName);
    
private:
    BubbleImageManager* q_ptr;
    ThumbnailManager* mThumbnailManager;
    QMap<QString,QPixmap*> mImageCache;
    QQueue< QPair<QString, int> > mRequestQueue;
    bool mInitialized;
    };

#endif /* BUBBLEIMAGEMANAGER_P_H_ */
