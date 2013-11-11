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
#ifndef CXEFILENAMEGENERATORSYMBIAN_H
#define CXEFILENAMEGENERATORSYMBIAN_H

#include <QString>
#include "cxefilenamegenerator.h"
#include "cxenamespace.h"
#include "cxeerror.h"

class RFs;
class CxeSettings;
static const int KCamMonthFolderNameLength = 8;

/**
 * CxeFilenameGeneratorSymbian provides file names for saving images and videos.
 */
class CxeFilenameGeneratorSymbian : public CxeFilenameGenerator
{
public:

    CxeFilenameGeneratorSymbian(CxeSettings &settings, Cxe::CameraMode mode);
    virtual ~CxeFilenameGeneratorSymbian();

public:

    CxeError::Id init(Cxe::CameraMode mode);
    void startNewImageFilenameSequence();
    CxeError::Id nextImageFilenameInSequence(QString &filename, const QString &fileExt);
    CxeError::Id generateFilename(QString &filename, const QString &fileExt);
    void raiseCounterValue();

private:

    // Private methods

    int checkExistence(const QString& path);
    int ensureExists(const QString& path, bool& created);
    bool spaceAvailable(int &driveIndex, TInt64 minSpaceInBytes);
    int selectDrive(int &drive);
    void initBasePath(QString &path, int drive);
    int initMonthFolders();
    int computeMonthCounter(QString &path, QString& monthFolder, int &monthCounter);
    void computeFolderSuffix(int folderNumber, QString &suffix);
    int selectFolder(QString &path);
    void resetCounters(QString &monthFolder);
    int generateUniqueFileName(QString &filePath, QString &fileName, const QString &ext);

private:

    // FileSystem object
    RFs &mFs;

    // Reference of SettingsImp (not owned)
    CxeSettings &mSettings;
    // Current mode: image/video
    Cxe::CameraMode mCurrentMode;

    /**
     * Base paths for image and video files: (E:\images\camera\).
     */
    QString mImagesPath;
    QString mVideosPath;

    // Month folder name: YYMMDD - common for image and video
    QString mCurrentMonth;

    // Month counters for image and video modes.
    // Used for computing two digit postfix for month folders
    int mMonthCounterImage;
    int mMonthCounterVideo;

    // Last used image and video numbers
    int mImageCounter;
    int mVideoCounter;

};

#endif // CXEFILENAMEGENERATORSYMBIAN_H
