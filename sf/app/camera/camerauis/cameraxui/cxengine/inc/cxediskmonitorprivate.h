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

#ifndef CXEDISKMONITORPRIVATE_H
#define CXEDISKMONITORPRIVATE_H

#include <QObject>
#include <QTimer>
#ifdef Q_OS_SYMBIAN
#include <f32file.h>
#endif // Q_OS_SYMBIAN

class CxeSettings;

class CxeDiskMonitorPrivate : public QObject
{
    Q_OBJECT

private:
    explicit CxeDiskMonitorPrivate(CxeSettings &settings);
    ~CxeDiskMonitorPrivate();

signals:
    void diskSpaceLow();
    void diskSpaceChanged();

private slots:
    void checkSpace();

private:
    void setLowWarningLevel(qint64 bytes);
    void start();
    void stop();
    bool isMonitoring() const;
    qint64 free(bool cached = true) const;

private:
    CxeSettings &mSettings;
    QTimer mTimer;
#ifdef Q_OS_SYMBIAN
    RFs& mFs;
    int mCameraDrive;
#endif // Q_OS_SYMBIAN
    qint64 mTriggerLevelBytes;
    qint64 mLatestFreeBytes;

    friend class CxeDiskMonitor;
};


#endif // CXEDISKMONITORPRIVATE_H
