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
#ifndef CXESTILLCAPTURECONTROL_H
#define CXESTILLCAPTURECONTROL_H

#include <QObject>
#include <QMetaType>
#include <QPixmap>
#include "cxeerror.h"
#include "cxequalitydetails.h"

class CxeStillImage;
class CxeImageDataQueue;

/**
 * Still capture control implements basic still image capture functionality. It
 * automatically plays back the capture sound when capturing images (or a
 * sequence of images) and it also provides snapshots of captured images.
 * Still capture control also automatically makes sure that the images are
 * saved in the file system in the background.
 *
 * The CxeStillImage is used to access information (snapshot image, filename)
 * about captured images. The still capture control owns an array of CxeStillImage
 * objects representing the images that were captured. The CxeStillCaptureControl::reset()
 * function should be used to clear any data about previously captured images and to initialize
 * the control for a new capture.
 *
 * The images will be saved to the file system by CxeImageDataQueue and CxeImageDataItem classes.
 * @sa CxeStillImage
 * @sa CxeImageDataQueue
 * @sa CxeImageDataItem
 */
class CxeStillCaptureControl : public QObject
{
    Q_OBJECT
public:
    /**
     * Image capture mode.
     */
    enum CaptureMode
        {
        //! Single image mode
        SingleImageCapture,

        //! Burst capture mode. One capture operation captures images until
        //! stopped or the maximum amount of images is reached.
        BurstCapture
        };

    //! Still capture control states
    enum State {
        //! Not prepared
        Uninitialized = 0x01,

        //! Prepared and ready to capture images
        Ready         = 0x02,

        //! Capture operation is in progress
        Capturing     = 0x04
        };

    /**
     * Start capturing images.
     */
    virtual void capture() = 0;

    /**
     * Current state.
     */
    virtual State state() const = 0;

    /**
     * Get the number of images captured. The actual images can be accessed
     * using the [] operator.
     *
     * @return Number of CxeStillImage object available
     */
    virtual int imageCount() const = 0;

    /**
     * Clear the image array, destroy all CxeStillImage objects, including
     * snapshot images. Items in the file save queue remain and are saved in
     * the background.
     */
    virtual void reset() = 0;

    /**
     * Clear the image array, destroy all CxeStillImage objects, including
     * snapshot images. File save queue is cleared as well and all unsaved
     * images will be lost.
     */
    virtual void cancelAll() = 0;

    /**
     * Set current image capture mode.
     */
    virtual void setMode(CaptureMode mode) = 0;

    /**
     * Get current image capture mode.
     */
    virtual CaptureMode mode() const = 0;

    /**
     * Access images that were captured during this capture sequence.
     *
     * @param index Image index. Must be 0 <= index < imageCount.
     * @return Reference to corresponding CxeStillImage
     */
    virtual CxeStillImage &operator[]( int index ) = 0;

    /**
     * File save queue
     */
    virtual CxeImageDataQueue& imageDataQueue() = 0;

    /*
    * supported list of image qualities, sorted in descending order based on image resolution
    * i.e. the list has the highest resolution first and so on
    * Internally, we check if we are using Primary/Secondary camera and return the list based on it
    */
    virtual QList<CxeImageDetails> supportedImageQualities() = 0;

    /*
    * Returns the number of images left for the current image quality setting
    */
    virtual int imagesLeft() = 0;

public slots:
    virtual void init() = 0;
    virtual void deinit() = 0;

signals:
    void stateChanged(CxeStillCaptureControl::State newState, CxeError::Id error);

    /**
     * A signal indicating a snapshot image is available.
     *
     * @param error Status code for the snapshot.
     *              CxeError::None if snapshot successfully received.
     * @param snapshot The snapshot image.
     * @param id       Id of the image this snapshot is for.
     */
    void snapshotReady(CxeError::Id error, const QImage &snapshot, int id);
    void imageCaptured(CxeError::Id error, int id);

    /**
    * Amount of images that can be captured has changed.
    */
    void availableImagesChanged();

protected:
    CxeStillCaptureControl() {} // protected empty contructor so that derived class construction works

private:
    Q_DISABLE_COPY( CxeStillCaptureControl )
};

Q_DECLARE_METATYPE(CxeStillCaptureControl::State)


#endif // CXESTILLCAPTURECONTROL_H
