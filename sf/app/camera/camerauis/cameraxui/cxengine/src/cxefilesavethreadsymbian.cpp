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

#include <QPixmap>
#include <QByteArray>
#include <QCoreApplication>
#include <QMutexLocker>

#include "cxestillimage.h"
#include "cxeimagedataitem.h"
#include "cxeimagedataitemsymbian.h"
#include "cxeharvestercontrolsymbian.h"
#include "cxethumbnailmanagersymbian.h"
#include "cxefilesavethreadsymbian.h"
#include "cxutils.h" // debug

namespace
{
    static const int SLEEP_MS_FOR_SIGNALS = 100;
    // Id for "Captured" album
    static const quint32 MDS_CAPTURED_ALBUM_ID = 2;
}


/**
* Implement factory method for CxeFileSaveThreadFactory.
*/
CxeFileSaveThread *CxeFileSaveThreadFactory::createFileSaveThread(QObject *parent)
{
    return new CxeFileSaveThreadSymbian(parent);
}


/*!
    \class CxeFileSaveThreadSymbian
    \brief Still image saving thread
*/


// ======== MEMBER FUNCTIONS ========

CxeFileSaveThreadSymbian::CxeFileSaveThreadSymbian( QObject *parent )
    : CxeFileSaveThread(parent),
      mExitThread(false),
      mSnapshots(),
      mExit(false),
      mThumbnailManager(NULL),
      mHarvesterControl(NULL)
{
    start(IdlePriority);
}

CxeFileSaveThreadSymbian::~CxeFileSaveThreadSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    mMutex.lock();
    mExitThread = true;
    mDataToSave.wakeOne();
    mMutex.unlock();

    wait(); // until the thread has finished execution.
    qDeleteAll(mQueue);  // Ensure destruction
    mQueue.clear();

    mSnapshots.clear();

    delete mThumbnailManager;
    delete mHarvesterControl;

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFileSaveThreadSymbian::save(CxeImageDataItem *data)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ensure safe data adding.
    // Saving thread will wait if needed, in read method, until mutex is unlocked
    mMutex.lock();
    mQueue.enqueue(data);
    // Wake up saving thread if it's sleeping
    mDataToSave.wakeOne();
    mMutex.unlock();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFileSaveThreadSymbian::read()
{
    mMutex.lock();
    mCount = mQueue.count();
    mExit = mExitThread;
    mMutex.unlock();
}

void CxeFileSaveThreadSymbian::run()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Init Thumbnail Manager and Harvester Control.
    init();
    mActiveHarvests = 0;

    // Check if there is data to save.
    // There should not be any, because the thread is just contructed
    read();

    while (!mExit || mCount > 0) { // Complete save before exit
        CX_DEBUG(("CxeFileSaveThreadSymbian: mCount %d", mCount));
        // Wait data
        if (!mExit && mCount == 0) {
            // If there isn't any data to save, put the thread sleeping
            mMutex.lock();
            if(mActiveHarvests > 0) {
                // If we have active harvest requests, continue after a while to check if
                // there are signals waiting.
                CX_DEBUG(("CxeFileSaveThreadSymbian: %d harvesting requests active..", mActiveHarvests));
                mDataToSave.wait(&mMutex, SLEEP_MS_FOR_SIGNALS); // waiting "wakeOne"
                QCoreApplication::processEvents();
            } else {
                // If no active requests, and no data,
                // halt this thread until something to
                // save is available.
                CX_DEBUG(("CxeFileSaveThreadSymbian: set thread sleeping"));
                mDataToSave.wait(&mMutex); // waiting "wakeOne"
            }
            mMutex.unlock();
            CX_DEBUG(("CxeFileSaveThreadSymbian: woken up"));

        }

        // There should be data now, because the thread is woken up
        read();

        if (mCount > 0) {
            // Save one item now.
            saveNow();
        }

        // If we did start harvesting, check if there's signal waiting
        // for harvesting completed already.
        if(mActiveHarvests > 0) {
            msleep(SLEEP_MS_FOR_SIGNALS);
            QCoreApplication::processEvents();
        }

        // Saving takes some seconds, there might be new data available.
        read();
    }

    // Cleanup in the same thread as init() was done.
    deinit();

    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Slot for saved video signal.
*/
void CxeFileSaveThreadSymbian::handleVideoSaved(CxeError::Id status, const QString& filename) {
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[INFO] current thread 0x%x", QThread::currentThreadId()));
    CX_DEBUG(("status = %d filename = %s", status, filename.toAscii().constData()));

    if (status == CxeError::None) {
        // Use a dummy "image data item" with empty data and saved state for videos.
        // We just need to harvest the file and provide snapshot to Thumbnail Manager.
        QByteArray empty;
        CxeImageDataItem* item = new CxeImageDataItemSymbian(empty,
                                                             filename,
                                                             CxeStillImage::INVALID_ID,
                                                             false,
                                                             CxeImageDataItem::Saved);
        if (item) {
            save(item);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Handles snapshot event from CxeStillCaptureControl and CxeVideoCaptureControl.
 *
 * @param status Status of snapshot creation. CxeError::None if no error, otherwise contains the error code
 * @param snapshot Snapshot as QImage
 * @param filename Name of the file that the snapshot is from
 */
void CxeFileSaveThreadSymbian::handleSnapshotReady(CxeError::Id status, const QImage& snapshot, const QString& filename)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[INFO] current thread 0x%x", QThread::currentThreadId()));
    CX_DEBUG(("status = %d filename = %s", status, filename.toAscii().constData()));

    if (status == CxeError::None) {
        // Store snapshot.
        if (!snapshot.isNull()) {
            // QMutexLocker handles locking and unlocking automaticly.
            QMutexLocker lock(&mSnapshotsMutex);
            mSnapshots.insert(filename, snapshot);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Handles snapshot event from CxeStillCaptureControl and CxeVideoCaptureControl.
 *
 * @param status Status of snapshot creation. CxeError::None if no error, otherwise contains the error code
 * @param snapshot Snapshot as QImage
 * @param id Id of the file that the snapshot is from
 */
void CxeFileSaveThreadSymbian::handleSnapshotReady(CxeError::Id status, const QImage& snapshot, int id)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[INFO] current thread 0x%x", QThread::currentThreadId()));
    CX_DEBUG(("status = %d id = %d", status, id));
    // Using id number as "filename" for images as filename is not available
    // at the time of snapshot for still images (and preparing the filename
    // would be slowing down showing the snapshot).
    handleSnapshotReady(status, snapshot, QString::number(id));
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Slot to handle harvested file.
* @param status Status of the harvesting.
* @param filename Path of the file just harvested.
*/
void CxeFileSaveThreadSymbian::handleFileHarvested(CxeError::Id status, const QString& filename)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[INFO] current thread 0x%x", QThread::currentThreadId()));
    CX_DEBUG(("status = %d filename = %s", status, filename.toAscii().constData()));

    // Synchronize snapshots container access.
    QMutexLocker lock(&mSnapshotsMutex);

    // Decrease count to enable sleeping in main loop
    // if nothing to save is available.
    --mActiveHarvests;

    if (status == KErrNone && mThumbnailManager) {
        if (mSnapshots.contains(filename)) {
            // File has been successfully harvested,
            // let's provide the snapshot to Thumbnail Manager.
            mThumbnailManager->createThumbnail(filename, mSnapshots[filename]);
        }
    }

    // Releasing the snapshot if it exists.
    mSnapshots.remove(filename);

    emit fileSaved(status, filename);

    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Init the utility classes.
*/
void CxeFileSaveThreadSymbian::init()
{
    // Create the thumbnail manager and harvester control objects in the new thread.
    if (!mThumbnailManager) {
        mThumbnailManager = new CxeThumbnailManagerSymbian();
    }
    if (!mHarvesterControl) {
        mHarvesterControl = new CxeHarvesterControlSymbian();
        connect(mHarvesterControl, SIGNAL(fileHarvested(CxeError::Id, const QString&)),
                this, SLOT(handleFileHarvested(CxeError::Id, const QString&)),
                Qt::DirectConnection);
    }
}

/**
* Clean up the utility classes
*/
void CxeFileSaveThreadSymbian::deinit()
{
    // Delete in the same thread where created.
    CX_DEBUG(("CxeFileSaveThreadSymbian: delete Thumbnail Manager"));
    delete mThumbnailManager;
    mThumbnailManager = NULL;
    CX_DEBUG(("CxeFileSaveThreadSymbian: delete Harvester Control"));
    delete mHarvesterControl;
    mHarvesterControl = NULL;
}



/**
* Save the item now.
*/
void CxeFileSaveThreadSymbian::saveNow()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[INFO] current thread 0x%x", QThread::currentThreadId()));

    CxeImageDataItem* item(mQueue.dequeue());
    if (item ) {
        // If item needs to be saved, do it now.
        if( item->state() == CxeImageDataItem::SavePending) {
            // Save the item.
            // Error ignored since we'll check the state.
            item->save();
        }

        CX_DEBUG(("Item state after saving: %d", item->state()));
        // If item is saved ok, ask to harvest it now.
        if (item->state() == CxeImageDataItem::Saved) {

            QString path(item->path());

            if (item->id() != CxeStillImage::INVALID_ID) {
                // Synchronize snapshots container access.
                QMutexLocker lock(&mSnapshotsMutex);

                // If snapshot was stored using id as a "filename", replace key with real filename now,
                // so we can find the snapshot when harvesting is ready.
                QString idString(QString::number(item->id()));
                if (mSnapshots.contains(idString)) {
                    QImage snapshot(mSnapshots[idString]);
                    mSnapshots.remove(idString);
                    mSnapshots.insert(path, snapshot);
                }
            }
            CxeImageDataItemSymbian* item_s = qobject_cast<CxeImageDataItemSymbian*>(item);
            if (item_s) {
                harvestFile(path, item_s->isLocationEnabled());
            }
        }

        // Delete item, since we own it
        delete item;
        item = NULL;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Harvest one file.
* @param filename Path of the file to be harvested.
*/
void CxeFileSaveThreadSymbian::harvestFile(const QString& filename, bool addLocation)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mHarvesterControl) {
        // Synchronize snapshots container access.
        QMutexLocker lock(&mSnapshotsMutex);

        // harvest file ( filename, add to album, album id )
        CX_DEBUG(("Requesting harvesting for file: %s addLocation = %d", filename.toAscii().constData(), addLocation));
        CxeError::Id status = mHarvesterControl->harvestFile(filename, addLocation, MDS_CAPTURED_ALBUM_ID);
        CX_DEBUG(("Status for starting harvesting: %d", status));

        // If there were errors, release any snapshot stored for this file.
        // Otherwise waiting for the harvesting to complete to
        // provide the snapshot to Thumbnail Manager.
        if(status != CxeError::None) {
            mSnapshots.remove(filename);
        } else {
            // Update count to process events in main loop.
            ++mActiveHarvests;
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}
