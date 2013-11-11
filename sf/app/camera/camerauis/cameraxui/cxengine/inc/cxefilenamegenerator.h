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
#ifndef CXEFILENAMEGENERATOR_H
#define CXEFILENAMEGENERATOR_H

#include <QString>
#include "cxenamespace.h"
#include "cxeerror.h"


/**
 * CxeFilenameGeneratorSymbian provides file names for saving images and videos.
 */
class CxeFilenameGenerator
{
public:
    virtual ~CxeFilenameGenerator() {}

    /**
     * Initialize the file name generator for the given mode.
     * @param   mode: current camera mode (image/video)
     * @return  error id, Id::None if no error
     */
    virtual CxeError::Id init(Cxe::CameraMode mode) = 0;

    /**
     * This must be called for every burst capture.
     */
    virtual void startNewImageFilenameSequence() = 0;

    /**
     * Generates the next file name in the sequence.
     * @param fileExt, used a file extension for image/video
     * @param   filename: A QString reference to hold the filename
     * @return  error id, Id::None if no error
     */
    virtual CxeError::Id nextImageFilenameInSequence(QString &filename, const QString &fileExt) = 0;

    /**
     * Generates image/video file name depending on the current active mode.
     * @param fileExt, used a file extension for image/video
     * @param   filename: A QString reference to hold the filename
     * @return  error id, CxeError::None if no error.
     */
    virtual CxeError::Id generateFilename(QString &filename, const QString &fileExt) = 0;

    /**
     * Raises file number counter value by one
     */
    virtual void raiseCounterValue() = 0;

protected:
    CxeFilenameGenerator() {} // protected empty contructor so that derived class construction works
};

#endif // CXEFILENAMEGENERATOR_H
