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
 * 
 * Description:
 *      The header file of the drive details type
 */

#ifndef FMDRIVEDETAILSTYPE_H
#define FMDRIVEDETAILSTYPE_H

#include <QString>
#include <QList>
#include <QStringList>

/*
    \class FmDriveDetailsSize
    \brief The class FmDriveDetailsSize used to store view details result.
 */
class FmDriveDetailsSize
{
public:
    enum TDataType{
        ETypeImages = 0,
        ETypeSoundFiles,
        ETypeMidpJava,
        ETypeNativeApps,
        ETypeVideos,
        ETypeDocuments,
        ETypeCalendar,
        ETypeContacts,
        ETypeMessages,
        ETypeOthers
        };
    
public:
    FmDriveDetailsSize( TDataType dataType, quint64 size ) 
        : mDataType( dataType ), mSize( size ) {}
    FmDriveDetailsSize( const FmDriveDetailsSize &other )
    {
        *this = other;
    }
    
    FmDriveDetailsSize &operator= ( const FmDriveDetailsSize &rhs )
    {
        this->mDataType = rhs.mDataType;
        this->mSize = rhs.mSize;
        return *this;
    }
    
    /*!
     request the stored data type
     */
    TDataType dataType() const { return mDataType; }
    
    /*!
     request the stored size that related to data type
     */
    quint64 size() const { return mSize; }
    
private:
    /*!
     store the data type
     */
    TDataType mDataType;
    
    /*!
     store the size that related to mDataType
     */
    quint64 mSize;
};

/*
    \class FmDriveDetailsDataGroup
    \brief The class FmDriveDetailsDataGroup used to store command of get size of absolute file path.
 */
class FmDriveDetailsDataGroup
{
public:
    FmDriveDetailsDataGroup( FmDriveDetailsSize::TDataType dataType,
                QStringList filePathList ) 
               : mDataType( dataType ), mFilePath( filePathList )
    {
    }
    FmDriveDetailsDataGroup( const FmDriveDetailsDataGroup &other )
    {
        *this = other;
    }
    
    FmDriveDetailsDataGroup &operator= ( const FmDriveDetailsDataGroup &rhs )
    {
        this->mDataType = rhs.mDataType;
        this->mFilePath = rhs.mFilePath;
        return *this;
    }
    
    FmDriveDetailsSize::TDataType dataType() const { return mDataType; }
    QStringList pathList() const { return mFilePath; }
private:
    /*!
     Store which data type does mFilePath belong to.
     */
    FmDriveDetailsSize::TDataType mDataType;
    
    /*!
     Store absolute file path that will used to calcuate whole data size related to mDataType
     */
    QStringList mFilePath;
};

/*
    \class FmDriveDetailsContent
    \brief The class FmDriveDetailsContent is the interface of view details feature
 */
class FmDriveDetailsContent
{
public:
    /*!
     Gets data size for drive
     \a driveName which drive is searching
     \a detailsSizeList if got result, new FmDriveDetailsSize will be appended to detailsSizeList
     \a isStopped isStopped will be set as true if user cancel this operation
     return Filemanage wide error. Please refer to fmdefine.h
     */
    static int querySizeofContent(
            const QString &driveName, QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped );
    
private:
    /*!
     Gets data size(related to file type,e.g. image, sound) by traverse designated drive name.
     provide size except the path related to absolute path(getDataSizeByAbsolutePath)
     \sa driveName which drive is searching
     \a detailsSizeList if got result, new FmDriveDetailsSize will be appended to detailsSizeList
     \a isStopped isStopped will be set as true if user cancel this operation
     return Filemanage wide error. Please refer to fmdefine.h
     */
    static int getDataSizeByTraversePath( const QString &driveName,
            QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped );
    
    /*!
     Gets data size for single FmDriveDetailsDataGroup
     The method will find file or traverse all folders of absolute path
     \a driveName which drive is searching
     \a dataGroup which dataGroup is searching, for example, EGroupContacts...
     \a detailsSizeList if got result, new FmDriveDetailsSize will be appended to detailsSizeList
     \a isStopped isStopped will be set as true if user cancel this operation
     return Filemanage wide error. Please refer to fmdefine.h
     */
    static int getDataSizeByAbsolutePath( const QString &driveName, const FmDriveDetailsDataGroup &dataGroup, 
               QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped );
};

class FmFolderDetails
{
public:

    /*!
     Gets details for a list of folders
     \a folderPathList folder path list
     \a numofFolders output how many folders and subfolders in the list
     \a numofFiles output how many files in the list
     \a totalSize output the total size
     \a isStopped isStopped will be set as true if user cancel this operation
     \a isSysHiddenIncluded will add QDir::Hidden | QDir::System into filter if true
     return Filemanage wide error. Please refer to fmdefine.h
     */
    static int queryDetailOfContentList( const QStringList folderPathList,int &numofFolders, 
                                    int &numofFiles, quint64 &totalSize, volatile bool *isStopped, bool isSysHiddenIncluded = false );
    /*!
     Gets details for a folder
     \a folderPath path of the folder
     \a numofSubFolders output how many subfolders in the list, not include itself
     \a numofFiles output how many files in the folder
     \a sizeofFolder output the size of folder
     \a isStopped isStopped will be set as true if user cancel this operation
     \a isSysHiddenIncluded will add QDir::Hidden | QDir::System into filter if true
     return Filemanage wide error. Please refer to fmdefine.h
     */
    static int getNumofSubfolders( const QString &folderPath, int &numofSubFolders, 
                                    int &numofFiles, quint64 &sizeofFolder,
                                    volatile bool *isStopped, bool  isSysHiddenIncluded = false );
};

#endif /* FMDRIVEDETAILSTYPE_H */

