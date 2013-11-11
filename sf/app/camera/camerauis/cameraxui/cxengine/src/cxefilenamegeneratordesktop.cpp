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

#include "cxefilenamegeneratordesktop.h"

#include <QDate>
#include <qdebug.h>

#ifdef Q_OS_WIN32
const char* IMAGE_BASE_FOLDER = "C:\\Users\\My Pictures\\CameraX";
const char* VIDEO_BASE_FOLDER = "C:\\Users\\My Videos\\CameraX";
const char* IMAGE_EXTENSION = "jpg";
const char* VIDEO_EXTENSION = "mp4";
const char* FOLDER_SEPARATOR = "\\";
const char* FILE_SUFFIX = ".";
#else
const char* IMAGE_BASE_FOLDER = "";
const char* VIDEO_BASE_FOLDER = "";
const char* IMAGE_EXTENSION = "";
const char* VIDEO_EXTENSION = "";
const char* FOLDER_SEPARATOR = "/";
const char* FILE_SUFFIX = ".";
#endif

CxeFilenameGeneratorDesktop::CxeFilenameGeneratorDesktop() : mMode(Cxe::ImageMode),
                                                             mImageBaseFolder(IMAGE_BASE_FOLDER),
                                                             mVideoBaseFolder(VIDEO_BASE_FOLDER),
                                                             mImageExtension(IMAGE_EXTENSION),
                                                             mVideoExtension(VIDEO_EXTENSION),
                                                             mImageSequenceNumber(0),
                                                             mVideoSequenceNumber(0)
{

}

CxeFilenameGeneratorDesktop::~CxeFilenameGeneratorDesktop()
{

}

/**
 * Initialize the file name generator for the given mode.
 * @param   mode: current camera mode (image/video)
 * @return  error id, Id::None if no error
 */
CxeError::Id CxeFilenameGeneratorDesktop::init(Cxe::CameraMode mode)
{
    mMode = mode;
    return CxeError::None;
}

/**
 * This must be called for every burst capture.
 */
void CxeFilenameGeneratorDesktop::startNewImageFilenameSequence()
{
    mImageSequenceNumber = 0;
}

/**
 * Generates the next file name in the sequence.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, Id::None if no error
 */
CxeError::Id CxeFilenameGeneratorDesktop::nextImageFilenameInSequence(QString &filename, const QString &fileExt)
{
    mImageExtension = fileExt;
    getNextImageFilename(filename);
    raiseCounterValue();
    return CxeError::None;
}

/**
 * Generates image/video file name depending on the current active mode.
 * @param   filename: A QString reference to hold the filename
 * @return  error id, CxeError::None if no error.
 */
CxeError::Id CxeFilenameGeneratorDesktop::generateFilename(QString &filename, const QString &fileExt)
{
    mImageExtension = fileExt;
    if (Cxe::ImageMode == mMode) {
        nextImageFilenameInSequence(filename, fileExt);
    } else {
        getNextVideoFilename(filename);
    }

    return CxeError::None;
}

void CxeFilenameGeneratorDesktop::getNextImageFilename(QString& filename)
{
    filename = assembleFilename(mImageBaseFolder, mImageSequenceNumber, mImageExtension);
    return;
}

void CxeFilenameGeneratorDesktop::getNextVideoFilename(QString &filename)
{
    filename = assembleFilename(mVideoBaseFolder, mVideoSequenceNumber, mVideoExtension);
    return;
}

QString CxeFilenameGeneratorDesktop::assembleFilename(const QString& baseFolder, int sequenceNumber, const QString &extension)
{
    QString fileName;
    QDate current = QDate::currentDate();

    fileName = QString("%1%2").arg(current.toString("yyyyMM")).arg(sequenceNumber,3,16,QLatin1Char('0'));

    QString assembledName = baseFolder + FOLDER_SEPARATOR + fileName + FILE_SUFFIX + extension;

    return assembledName;
}

/**
 * Raises file number counter value by one
 */
void CxeFilenameGeneratorDesktop::raiseCounterValue()
{
    if (Cxe::ImageMode == mMode) {
        mImageSequenceNumber++;
    } else {
        mVideoSequenceNumber++;
    }
}
