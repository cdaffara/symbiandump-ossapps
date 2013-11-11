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

#ifndef BUBBLEIMAGEMANAGER_H_
#define BUBBLEIMAGEMANAGER_H_

#include <QObject>
#include <QPixmap>

class BubbleImageManagerPrivate;

class BubbleImageManager : public QObject
    {
    Q_OBJECT
    
public:
    explicit BubbleImageManager(QObject* parent = 0);
    virtual ~BubbleImageManager();

    const QPixmap* pixmap(const QString& imageFileName);

    void loadImage(const QString& imageFileName);

    void unloadImage(const QString& imageFileName);

    void releasePixmaps();

signals:
    void pixmapReady(const QString& imageFileName);
    
protected:
    BubbleImageManagerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(BubbleImageManager)
    };

#endif /* BUBBLEIMAGEMANAGER_H_ */
