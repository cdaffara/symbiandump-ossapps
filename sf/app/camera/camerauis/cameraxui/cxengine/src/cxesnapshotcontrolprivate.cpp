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

#include <QSize>
#include <QPixmap>
// Note: Keep atleast one Qt include before preprocessor flags
#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#include <fbs.h>
#include <ecam.h>
#include <ecam/camerasnapshot.h>
#endif // Q_OS_SYMBIAN

#include "cxutils.h"
#include "cxeerror.h"
#include "cxeexception.h"
#include "cxeerrormappingsymbian.h"
#include "cxecameradevice.h"
#include "cxestate.h"
#include "cxesnapshotcontrol.h"
#include "cxesnapshotcontrolprivate.h"


namespace
{
    const int MAINTAIN_ASPECT = false;

    const QSize ASPECT_RATIO_SIZE_4BY3  = QSize(4,3);
    const QSize ASPECT_RATIO_SIZE_16BY9 = QSize(16, 9);
    const QSize ASPECT_RATIO_SIZE_11BY9 = QSize(11, 9);

#ifdef Q_OS_SYMBIAN
    /*!
    * Helper class for cleaning up MCameraBuffer instances.
    */
    class CxeCameraBufferCleanup
    {
    public:
        explicit CxeCameraBufferCleanup(MCameraBuffer *buffer)
            : mBuffer(buffer)
        {
        }

        ~CxeCameraBufferCleanup()
        {
            if (mBuffer) {
                CX_DEBUG(("CxeCameraBufferCleanup - releasing MCameraBuffer.."));
                mBuffer->Release();
                mBuffer = NULL;
            }
        }
    private:
        Q_DISABLE_COPY(CxeCameraBufferCleanup)
        MCameraBuffer *mBuffer;
    };
#endif // Q_OS_SYMBIAN
}


/*!
* Constructor.
* @param parent Public interface of Snapshot control.
* @param device Camera device interface, used for getting hold of adaptation snapshot API.
*/
CxeSnapshotControlPrivate::CxeSnapshotControlPrivate(CxeSnapshotControl *parent,
                                                     CxeCameraDevice& device)
    : CxeStateMachine("CxeSnapshotControlPrivate"),
      q(parent),
      mDevice(device)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(q);
    initializeStates();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor.
*/
CxeSnapshotControlPrivate::~CxeSnapshotControlPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();
    stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Called when state changes.
* @param newStateId Id of the new state.
* @param error Error code or CxeError::None in successful case.
*/
void CxeSnapshotControlPrivate::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit q->stateChanged(static_cast<CxeSnapshotControl::State>(newStateId), error);
}


/*!
* Get the state of Snapshot Control.
* @return The current state.
*/
CxeSnapshotControl::State CxeSnapshotControlPrivate::state() const
{
    return static_cast<CxeSnapshotControl::State> (stateId());
}

/*!
* Initialize Snapshot Control states
*/
void CxeSnapshotControlPrivate::initializeStates()
{
    CX_DEBUG_ENTER_FUNCTION();
    // addState( id, name, allowed next states )
    addState(new CxeState(CxeSnapshotControl::Idle, "Idle", CxeSnapshotControl::Active));
    addState(new CxeState(CxeSnapshotControl::Active, "Active", CxeSnapshotControl::Idle));

    setInitialState(CxeSnapshotControl::Idle);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Calculate snapshot size based on diplay size and image / video output resolution.
* @param displaySize Display size in pixels.
* @param Cxe::AspectRatio Aspect ratio of image/video resolution.
* @return Proposed best snapshot size.
*/

QSize CxeSnapshotControlPrivate::calculateSnapshotSize(const QSize& displaySize, Cxe::AspectRatio aspectRatio) const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeSnapshotControlPrivate - display size      (%d,%d)", displaySize.width(), displaySize.height()));

    // Take resolution as reference for aspect ratio.
    // Scale keeping aspect ratio to just fit display.
    QSize size;
    
    if (aspectRatio == Cxe::AspectRatio4to3) {
        size = ASPECT_RATIO_SIZE_4BY3;
    } else if (aspectRatio == Cxe::AspectRatio16to9) {
        size = ASPECT_RATIO_SIZE_16BY9;
    } else if (aspectRatio == Cxe::AspectRatio11to9) {
        size = ASPECT_RATIO_SIZE_11BY9;
    }
    size.scale(displaySize, Qt::KeepAspectRatio);

    CX_DEBUG(("CxeSnapshotControlPrivate - adjusted final size, (%d,%d)", size.width(), size.height()));

    CX_DEBUG_EXIT_FUNCTION();

    return size;
}


/*!
* Start getting snapshots from camera.
* Throws CxeException with CxeError::Id if error encountered.
* @param size Size of the snapshot in pixels.
*/
void CxeSnapshotControlPrivate::start(const QSize &size)
{
    CX_DEBUG_ENTER_FUNCTION();
#ifdef Q_OS_SYMBIAN
    CCamera::CCameraSnapshot *ss = mDevice.cameraSnapshot();
    CX_ASSERT_ALWAYS(ss);

    if (ss->IsSnapshotActive()) {
        CX_DEBUG(("Stop currently active snapshot.."));
        ss->StopSnapshot();
    }

    // Prepare snapshot
    CCamera::TFormat snapFormat = CCamera::EFormatFbsBitmapColor16MU;
    TSize snapSize = TSize(size.width(), size.height());

    CX_DEBUG(("Prepare snapshot, size (%d x %d)..", size.width(), size.height()));
    TRAPD(status, ss->PrepareSnapshotL(snapFormat, snapSize, MAINTAIN_ASPECT));
    CxeException::throwIfError(CxeErrorHandlingSymbian::map(status));
    CX_DEBUG(("After prepare ECAM modified size to (%d x %d)..", size.width(), size.height()));

    CX_DEBUG(("Start snapshot.."));
    ss->StartSnapshot();
#else
    Q_UNUSED(size);
#endif // Q_OS_SYMBIAN
    setState(CxeSnapshotControl::Active);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stop getting snapshots from camera.
*/
void CxeSnapshotControlPrivate::stop()
{
    CX_DEBUG_ENTER_FUNCTION();
#ifdef Q_OS_SYMBIAN
    if (mDevice.cameraSnapshot()) {
        mDevice.cameraSnapshot()->StopSnapshot();
    }
#endif // Q_OS_SYMBIAN
    setState(CxeSnapshotControl::Idle);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method for getting the snapshot.
* Throws exception if fetching the snapshot fails.
* @return QImage containing the snapshot.
*/
QImage CxeSnapshotControlPrivate::snapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    QImage image;

    #ifdef Q_OS_SYMBIAN

        CFbsBitmap *snapshot = NULL;
        TRAPD(status, {
            RArray<TInt> frameIndex;
            CleanupClosePushL(frameIndex);

            MCameraBuffer &buffer(mDevice.cameraSnapshot()->SnapshotDataL(frameIndex));
            // Make sure buffer is released on leave / exception.
            // Buffer is released once the cleanup item goes out of scope.
            CxeCameraBufferCleanup cleaner(&buffer);

            TInt firstImageIndex(frameIndex.Find(0));
            snapshot = &buffer.BitmapL(firstImageIndex);

            CleanupStack::PopAndDestroy(); // frameIndex

            TSize size = snapshot->SizeInPixels();
            TInt sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, EColor16MU) / sizeof(TUint32);
            CX_DEBUG(("size %d x %d, sizeInWords = %d", size.iWidth, size.iHeight, sizeInWords ));

            CX_DEBUG(("Creating QImage"));
            image = QImage(size.iWidth, size.iHeight, QImage::Format_RGB32);

            // Convert to QImage
            snapshot->LockHeap();
            const uchar *dataPtr = (const uchar*) snapshot->DataAddress();
            uchar *dst = image.bits();
            memcpy(dst, dataPtr, image.numBytes());
            snapshot->UnlockHeap();

        });
        // We throw error with the Symbian error code if there was problems.
        CxeException::throwIfError(status);


    #endif // Q_OS_SYMBIAN

        CX_DEBUG_EXIT_FUNCTION();
        return image;
}

/*!
* Handle camera snapshot events.
* @param id Event uid.
* @param error Status code of the event.
*/
void CxeSnapshotControlPrivate::handleCameraEvent(int id, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ignoring all events if not active.
    if (state() == CxeSnapshotControl::Active) {
#ifdef Q_OS_SYMBIAN
        if (id == KUidECamEventSnapshotUidValue) {
            QImage ss;

            if (!error) {
                try {
                    ss = snapshot();
                } catch (const CxeException& e) {
                    // Note: Normally CxeException carries CxeError::Id,
                    // but we intentionally use Symbian code in getSnapshot
                    // as it's easier to handle here.
                    error = e.error();
                } catch (...) {
                    error = KErrGeneral;
                }
            }

            // Emit snapshot ready signal through the public interface.
            emit q->snapshotReady(CxeErrorHandlingSymbian::map(error), ss);
        }
#else
        Q_UNUSED(id)
        Q_UNUSED(error)
#endif // Q_OS_SYMBIAN
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
