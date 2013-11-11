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
#ifndef  CXEFILESAVETHREAD_H_
#define  CXEFILESAVETHREAD_H_

#include <QThread>
#include "cxeerror.h"

class CxeImageDataItem;

/*!
* Base class for file save thread.
*/
class CxeFileSaveThread : public QThread
{
    Q_OBJECT
public:
    virtual ~CxeFileSaveThread();

    /**
     * Save is called by Camera UI thread.
     * Method takes ownership of data.
     */
    virtual void save(CxeImageDataItem *data) = 0;

public slots:

    virtual void handleVideoSaved(CxeError::Id status, const QString& filename) = 0;
    virtual void handleSnapshotReady(CxeError::Id status, const QImage& snapshot, const QString& filename) = 0;
    virtual void handleSnapshotReady(CxeError::Id status, const QImage& snapshot, int id) = 0;

protected:

    CxeFileSaveThread(QObject *parent = 0);
};

/*!
* Factory for creating file save thread.
*/
class CxeFileSaveThreadFactory
{
public:

    static CxeFileSaveThread *createFileSaveThread(QObject *parent = 0);
};

#endif  // CXEFILESAVETHREAD_H_
