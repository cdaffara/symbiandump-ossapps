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
* Description:  Still image saving thread
*
*/



#ifndef CXEFILESAVETHREADSYMBIAN_H
#define CXEFILESAVETHREADSYMBIAN_H

#include <QMutex>
#include <QWaitCondition>
#include <QHash>
#include <QQueue>
#include "cxeerror.h"
#include "cxefilesavethread.h"

class CxeImageDataItem;
class CxeThumbnailManagerSymbian;
class CxeHarvesterControlSymbian;


class CxeFileSaveThreadSymbian : public CxeFileSaveThread
    {
    Q_OBJECT
public:

    CxeFileSaveThreadSymbian(QObject *parent = 0);
    ~CxeFileSaveThreadSymbian();

    /**
     * Save is called by Camera UI thread.
     * Method takes ownership of data.
     */
    void save(CxeImageDataItem* data);

public slots:
    void handleVideoSaved(CxeError::Id status, const QString& filename);
    void handleSnapshotReady(CxeError::Id status, const QImage& snapshot, const QString& filename);
    void handleSnapshotReady(CxeError::Id status, const QImage& snapshot, int id);

protected slots:
    void handleFileHarvested(CxeError::Id status, const QString& filename);

protected:
    virtual void init();
    virtual void deinit();

signals:
    void fileSaved(CxeError::Id status, const QString &filename);

private:
    /**
     * This method includes saving in own thread.
     * Functions called from this method are running in "saving thread"
     */
    void run();
    void read();
    void saveNow();
    void harvestFile(const QString& filename, bool addLocation);

private: // data shared between the threads

    bool mExitThread;
    QQueue<CxeImageDataItem*> mQueue;

    mutable QMutex mMutex;
    QWaitCondition mDataToSave;

    mutable QMutex mSnapshotsMutex;
    QHash<QString, QImage> mSnapshots;

protected: // thread only used data
    int mCount;
    int mActiveHarvests;
    bool mExit;
    CxeThumbnailManagerSymbian *mThumbnailManager; // own
    CxeHarvesterControlSymbian *mHarvesterControl; // own
    };

#endif // CXEFILESAVETHREADSYMBIAN_H
