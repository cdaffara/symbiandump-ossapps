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
#include "cntimageutility.h"

#include <pathinfo.h>
#include <driveinfo.h>

#include <QFile>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QDateTime>

const char* CNT_IMAGES_FOLDER   = "Data\\20022EF9\\";
const char* IMAGEFILE_EXT   = ".jpg";
const char* IMAGE_TYPE   = "JPEG";

const TInt64 KMinRequiredSpaceImage = 2000000;

_LIT( KImagesFolder, "Data\\20022EF9\\");

// Local function
inline TPtrC16 convertToTPtrC16(const QString& string)
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()));
}


CntImageUtility::CntImageUtility(QObject *parent)
    : QObject(parent)
{
    m_Fs.Connect();
}

CntImageUtility::~CntImageUtility()
{
    m_Fs.Close();
}

/*
 * Create thumbnail image.
 *
 * \param filePath The file path of source image.
 * \param imagePath The file path of new/created image.
 * \return Success status.
 */
bool CntImageUtility::createImage(const QString& filePath, QString& imagePath)
{
    int drive;
    bool success(false);
    m_error = CntImageUtility::NoError;
    int err= selectDrive(drive);
    if (KErrNone==err
        && initPath(imagePath, drive)) {
        // Filename format
        // image_<yyhh_mm_ss_zzz>.jpg
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("yyhh_mm_ss_zzz");
        QFileInfo fi(filePath);
        QString filename = "image_" + dateTimeString + "." + fi.suffix();
        imagePath += filename;
        
        // Copy image
        QFile destFile;
        success= destFile.copy(filePath,imagePath);
    }
    else {
        m_error = CntImageUtility::UnspecifiedError;
    }
    return success;
}

/*
 * Create thumbnail image.
 *
 * \param pixmap The pixmap of source image.
 * \param imagePath The file path of new/created image.
 * \return Success status.
 */
bool CntImageUtility::createImage(const QPixmap& pixmap, QString& imagePath)
{
    int drive;
    bool success(false);
    m_error = CntImageUtility::NoError;
    QString fileExt(IMAGEFILE_EXT);
    int err= selectDrive(drive);
    if (KErrNone==err
        && initPath(imagePath, drive)) {
        // Filename format
        // image_<yyhh_mm_ss_zzz>.jpg
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString("yyhh_mm_ss_zzz");
        QString filename = "image_" + dateTimeString + fileExt;
        imagePath += filename;
        
        if(!pixmap.isNull()) {
            QPixmap pix(pixmap);
            QImage image(pix.toImage());
            success= image.save(imagePath, IMAGE_TYPE);
        }
    }
    else {
        m_error = CntImageUtility::UnspecifiedError;
    }
    return success;
}

/*
 * Remove thumbnail image.
 *
 * \param imagePath The file path of thumbnail image.
 * \return Success status.
 */
bool CntImageUtility::removeImage(const QString& imagePath)
{
    QDir dir;
    bool success(false);
    m_error = CntImageUtility::NoError;
    
    QFile file( imagePath );
    if( !file.exists()
      || dir.remove(imagePath)) {
        success= true;
    }
    return success;
}

int CntImageUtility::selectDrive(int &driveIndex)
{
    int err = KErrNone;
    TInt64 minSpaceInBytes = KMinRequiredSpaceImage;
    
    // Get the drive/volume details
    TVolumeInfo vInfo;
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, driveIndex);
    if (KErrNone==err) {
        err = m_Fs.Volume(vInfo, driveIndex);
    }

    // Check the availability and disk space
    if ( !err && vInfo.iFree < minSpaceInBytes) {
        // All drives are full or inaccessible
        err = KErrDiskFull;
    }
    return err;
}

bool CntImageUtility::initPath(QString &path, int drive)
{
    int err = KErrNone;
    bool success(false);
        
    // Get contacts images path
    TFileName tPath;
    PathInfo::GetRootPath(tPath, drive);
    tPath.Append(KImagesFolder());
    path = QString::fromUtf16(tPath.Ptr(), tPath.Length());
    
    /* Contact images should go to the
     * hidden folder \data\20022EF9
     */
    QDir dir(path);
    if (!dir.cd(CNT_IMAGES_FOLDER)) {
        TPtrC16 pathDesc(convertToTPtrC16(path));
        err=m_Fs.MkDirAll(tPath);
        err=m_Fs.SetAtt(tPath, KEntryAttHidden, 0);
     }
    
    success = (err == KErrNone);
    return success;
}

/*
 * \return whether mass storage is supported in phone.
 */
bool CntImageUtility::isMassStorageAvailable()
{
    int err = KErrNone;
    bool success(false);
    m_error = CntImageUtility::NoError;
        
    int drive;
    err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive);
    success = (err == KErrNone);
    return success;
}

/*
 * \return whether thumbnail image is removable.
 */
bool CntImageUtility::isImageRemovable(const QString& filePath)
{
    int drive;
    bool success(false);
    m_error = CntImageUtility::NoError;
    
    // Image removable if saved in contacts images folder
    // Folder <Drive>:/Data/20022EF9
    int err= selectDrive(drive);
    if(KErrNone==err) {
        // Get the root path
        TFileName tPath;
        PathInfo::GetRootPath(tPath, drive);
        QString basePath = QString::fromUtf16(tPath.Ptr(), tPath.Length());
        
        QDir dir(basePath);
        if (dir.cd(CNT_IMAGES_FOLDER)) {
            basePath=QDir::toNativeSeparators(dir.path());
            if(filePath.startsWith(basePath))
                success=true;
        }
    }
    return success;
}

/*
 * \return Error status.
 */
CntImageUtility::Error CntImageUtility::error()
{
    return m_error;
}
