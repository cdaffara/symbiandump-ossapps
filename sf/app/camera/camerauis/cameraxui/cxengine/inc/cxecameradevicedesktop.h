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

#ifndef CXECAMERADEVICEDESKTOP_H
#define CXECAMERADEVICEDESKTOP_H

#include <QGraphicsWidget>
#include <QList>

class QTimer;
class QPixmap;

class CxeCameraDeviceDesktop : public QObject
{
    Q_OBJECT
public:
    CxeCameraDeviceDesktop();
    virtual ~CxeCameraDeviceDesktop();
    void start();
    void stop();
    const QPixmap &currentSnaphot();
    
signals:
    void imageChanged(const QPixmap &newImage);

protected slots:
    void handleTimeout();

private:
    void loadPictures();
    void setupTimer();

    QTimer *mSwitchPictureTimer;
    QList<QPixmap> mPictureList;
    QList<QPixmap>::size_type mPictureIndex;
};

#endif // CXECAMERADEVICEDESKTOP_H
