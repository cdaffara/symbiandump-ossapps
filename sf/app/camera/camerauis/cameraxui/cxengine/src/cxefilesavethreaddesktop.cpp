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


#include "cxefilesavethreaddesktop.h"
#include "cxeimagedataitem.h"  // item
#include "cxutils.h" // debug

/*!
    \class CxeFileSaveThreadDesktop
    \brief Still image saving thread    
*/


// ======== MEMBER FUNCTIONS ========

CxeFileSaveThreadDesktop::CxeFileSaveThreadDesktop(QObject *parent)
    : CxeFileSaveThread(parent), mExitThread(false), mExit(false)
{
    start(IdlePriority);
}

CxeFileSaveThreadDesktop::~CxeFileSaveThreadDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mMutex.lock();
    mExitThread = true;
    mDataToSave.wakeOne();
    mMutex.unlock();

    wait(); // until the thread has finished execution.
    qDeleteAll(mDataList);  // Ensure destruction
    mDataList.clear();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFileSaveThreadDesktop::save(CxeImageDataItem *data)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ensure safe data adding. 
    // Saving thread will wait if needed, in read method, until mutex is unlocked
    mMutex.lock();
    mDataList.append(data);
    // Wake up saving thread if it's sleeping
    mDataToSave.wakeOne(); 
    mMutex.unlock();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFileSaveThreadDesktop::handleVideoSaved(CxeError::Id status, const QString &filename)
{
    Q_UNUSED(status);
    Q_UNUSED(filename);
}

void CxeFileSaveThreadDesktop::handleSnapshotReady(CxeError::Id status, const QImage &snapshot, const QString &filename)
{
    Q_UNUSED(status);
    Q_UNUSED(snapshot);
    Q_UNUSED(filename);
}

void CxeFileSaveThreadDesktop::handleSnapshotReady(CxeError::Id status, const QImage &snapshot, int id)
{
    Q_UNUSED(status);
    Q_UNUSED(snapshot);
    Q_UNUSED(id);
}

void CxeFileSaveThreadDesktop::read()
{
    mMutex.lock();
    mCount = mDataList.count();
    mExit = mExitThread;
    mMutex.unlock();
}

void CxeFileSaveThreadDesktop::run()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    CxeImageDataItem *item = NULL;
    int i = 0;
    // Check if there is data to save.
    // There should not be any, because the thread is just contructed
    read();

    while (!mExit || i < mCount) { // Complete save before exit
        CX_DEBUG(("CxeFileSaveThreadDesktop: index %d", i));
        // Wait data
        if (!mExit && i >= mCount) {
            // If there isn't any data to save, put the thread sleeping
            CX_DEBUG(("CxeFileSaveThreadDesktop: set thread sleeping"));
            mMutex.lock();
            //! @todo: read datalist count here before clearing, because there is some machine cycles after previous read 
            // Clear the datalist, since all items are saved
            mDataList.clear();
            i = 0;
            mDataToSave.wait(&mMutex); // waiting "wakeOne"
            mMutex.unlock();
            CX_DEBUG(("CxeFileSaveThreadDesktop: woken up"));
        }

        // There should be data now, because the thread is woken up
        read();

        if (i < mCount) { 
            item = mDataList[i];
        } else {
            CX_DEBUG(("CxeFileSaveThreadDesktop: ERROR - woke up without data"));
            continue;
        }

        // Check that item is not saved already
        if (item && item->state() == CxeImageDataItem::SavePending) {
            item->save();// Error ignored since it is emitted from CxeImageDataItemSymbian
            // Delete item, since we own it
            delete item; 
            item = NULL;
        }

        // Saving takes some seconds, there might be new data available.
        read();
        ++i;
    }
  
    CX_DEBUG_EXIT_FUNCTION();
}
