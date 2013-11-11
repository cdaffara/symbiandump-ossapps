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
#include <QMetaType>
#include <QImage>

#include "cxesettings.h"
#include "CxeFakeFilenameGenerator.h"
#include "cxutils.h"
#include "cxesettingsmappersymbian.h"

using namespace Cxe;

namespace
{
    static const char *VIDEO_FILENAME = "C:\\Images\\Camera\\dummy.mp4";
    static const char *IMAGE_FILENAME = "C:\\Images\\Camera\\dummy.jpg";
}

CxeFakeFilenameGenerator::CxeFakeFilenameGenerator()
{
    CX_DEBUG_IN_FUNCTION();
}


/*
* CxeFakeFilenameGenerator::~CxeFakeFilenameGenerator
*/
CxeFakeFilenameGenerator::~CxeFakeFilenameGenerator()
{
    CX_DEBUG_IN_FUNCTION();
}

/**
 * Initialize the file name generator for the given mode.
 * @param   mode: current camera mode (image/video)
 * @return  error id, Id::None if no error
 */
CxeError::Id CxeFakeFilenameGenerator::init(Cxe::CameraMode mode)
{
    mCurrentMode = mode;
    return CxeError::None;
}

/**
 * This must be called for every burst capture.
 */
void CxeFakeFilenameGenerator::startNewImageFilenameSequence()
{
    CX_DEBUG_IN_FUNCTION();
}

/**
 * Generates the next file name in the sequence.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, Id::None if no error
 */
CxeError::Id CxeFakeFilenameGenerator::nextImageFilenameInSequence(QString& /*filename*/, const QString &/*fileExt*/)
{
    CX_DEBUG_IN_FUNCTION();
    return CxeError::None;
}

/**
 * Generates image/video file name depending on the current active mode.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, CxeError::None if no error.
 */
CxeError::Id CxeFakeFilenameGenerator::generateFilename(QString &qfName, const QString &/*fileExt*/)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mCurrentMode == VideoMode) {
        qfName = VIDEO_FILENAME;
    } else {
        qfName = IMAGE_FILENAME;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}

/**
 * Raises file number counter value by one
 */
void CxeFakeFilenameGenerator::raiseCounterValue()
{
    CX_DEBUG_IN_FUNCTION();
}

// end of file
