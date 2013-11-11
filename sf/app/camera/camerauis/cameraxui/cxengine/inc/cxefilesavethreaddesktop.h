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
* Description:  Still image saving thread
*
*/

#ifndef CXEFILESAVETHREADDESKTOP_H
#define CXEFILESAVETHREADDESKTOP_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "cxefilesavethread.h"

class CxeImageDataItem;


class CxeFileSaveThreadDesktop : public CxeFileSaveThread
{
    Q_OBJECT
public:

    CxeFileSaveThreadDesktop(QObject *parent = 0);
    ~CxeFileSaveThreadDesktop();

    void save(CxeImageDataItem *data);

public slots:
    void handleVideoSaved(CxeError::Id status, const QString &filename);
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot, const QString &filename);
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot, int id);

private:
    /**
     * This method includes saving in own thread.
     * Functions called from this method are running in "saving thread"
     */
    void run();
    void read();

private: // data shared between the threads
    
    bool mExitThread;
    QList<CxeImageDataItem*> mDataList; //! @todo Consider QQueue 

    QMutex mMutex;
    QWaitCondition mDataToSave;

private: // thread private data    
    int mCount;
    bool mExit;
};

#endif // CXEFILESAVETHREADDESKTOP_H
