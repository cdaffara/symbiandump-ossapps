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
 *      The source file of the drive details type
 */

#include "fmdrivedetailstype.h"
#include "fmfiletyperecognizer.h"
#include "fmutils.h"

#include <QDir>
#include <QFileInfo>
#include <QStringList>

int FmDriveDetailsContent::querySizeofContent( const QString &driveName,
        QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped )
{      
    int err = FmErrNone;
    detailsSizeList.clear();
    err = getDataSizeByTraversePath( driveName, detailsSizeList, isStopped );
    if( err != FmErrNone ) {
        return err;
    }
    
    QStringList dataPathList;
    dataPathList.append( QString( FmViewDetail_Contacts ) );    
    FmDriveDetailsDataGroup driveDetailsDataGroupForContacts( FmDriveDetailsSize::ETypeContacts, dataPathList );
    
    err = getDataSizeByAbsolutePath(driveName, driveDetailsDataGroupForContacts, detailsSizeList, isStopped);
    if( err != FmErrNone ) {
        return err;
    }
    
    dataPathList.clear();
    dataPathList.append( QString( FmViewDetail_Messages ) );    
    FmDriveDetailsDataGroup driveDetailsDataGroupForMessage( FmDriveDetailsSize::ETypeMessages, dataPathList );
    
    err = getDataSizeByAbsolutePath(driveName, driveDetailsDataGroupForMessage, detailsSizeList, isStopped);
    if( err != FmErrNone ) {
        return err;
    }
    return FmErrNone;
}
int FmDriveDetailsContent::getDataSizeByTraversePath( const QString &driveName,
            QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped )
{
    qint64 imageSize( 0 );
    qint64 soundSize( 0 );
    qint64 midpJavaSize( 0 );
    qint64 nativeAppsSize( 0 );
    qint64 videoSize( 0 );
    qint64 documentsSize( 0 );
   
    FmFileTypeRecognizer fileTypeRecognizer;
    
    QList<QDir> dirs;
    dirs.append( QDir( driveName ) );
    
    // traverse the whole drive
    while (!dirs.isEmpty()) {
        QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
        // do not summarize system and hidden files, these size will go into others category
        // if( isSysHiddenIncluded ) {
        // filter = filter | QDir::Hidden | QDir::System;
        // }

        QFileInfoList infoList = dirs.first().entryInfoList( filter );
        for ( QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it ) {
            if ( *isStopped ){
                return FmErrCancel;
            }
            
            if ( it->isFile() ) {
            FmFileTypeRecognizer::FileType fileType = 
                    fileTypeRecognizer.getType( it->absoluteFilePath() );
            switch ( fileType )
                {
                case FmFileTypeRecognizer::FileTypeImage:
                    imageSize += it->size();
                    break;
                case FmFileTypeRecognizer::FileTypeTone:
                    soundSize += it->size();
                    break;
                case FmFileTypeRecognizer::FileTypeJava:
                    midpJavaSize += it->size();
                    break;
                case FmFileTypeRecognizer::FileTypeSisx:
                    nativeAppsSize += it->size();
                    break;
                case FmFileTypeRecognizer::FileTypeVideo:
                    videoSize += it->size();
                    break;
                case FmFileTypeRecognizer::FileTypeText:
                    documentsSize += it->size();
                    break;
                default:
                    // do not need handle other type 
                    break;
                }
            }
            else if ( it->isDir() ) {
                dirs.append( QDir( it->absoluteFilePath() ) );
            } 
        }
        dirs.removeFirst();
    }
       
    // store result to detailsSizeList.
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeImages, imageSize ) ) ;
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeSoundFiles, soundSize ) );
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeMidpJava, midpJavaSize ) );
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeNativeApps, nativeAppsSize ) );
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeVideos, videoSize ) );
    detailsSizeList.append( new FmDriveDetailsSize( FmDriveDetailsSize::ETypeDocuments, documentsSize ) );
    return FmErrNone;
}

int FmDriveDetailsContent::getDataSizeByAbsolutePath( const QString &driveName,
        const FmDriveDetailsDataGroup &dataGroup,
            QList<FmDriveDetailsSize*> &detailsSizeList, volatile bool *isStopped )
{
    quint64 totalSize = 0;
    QStringList typeFilter = dataGroup.pathList();
    
    for( QStringList::const_iterator it = typeFilter.begin(); 
           it!= typeFilter.end(); ++it ) {
        if ( *isStopped ){
            return FmErrCancel;
        }        
        QString driver(FmUtils::removePathSplash(FmUtils::getDriveNameFromPath(driveName)));
        QFileInfo fileInfo(QString(driver + (*it)));
        if (fileInfo.exists()) {
            if (fileInfo.isFile()) {
                totalSize += fileInfo.size();    
            } else if (fileInfo.isDir()) {
                QList<QDir> dirs;
                dirs.append(QDir(fileInfo.absolutePath()));
                // traverse the whole path
                while (!dirs.isEmpty()) {
                    QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
                    QFileInfoList infoList = dirs.first().entryInfoList( filter );
                    for ( QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it ) {
                        if ( *isStopped ){
                            return FmErrCancel;
                        }                        
                        if ( it->isFile() ) {
                            totalSize += it->size();  
                        }
                        else if ( it->isDir() ) {
                            dirs.append( QDir( it->absoluteFilePath() ) );
                        } 
                    }
                    dirs.removeFirst();
                }
            }
            
        }
    }
 
    detailsSizeList.append( new FmDriveDetailsSize( dataGroup.dataType(), totalSize ) );
    return FmErrNone;
}

int FmFolderDetails::queryDetailOfContentList( const QStringList folderPathList,int &numofFolders, 
                                    int &numofFiles, quint64 &totalSize,
                                    volatile bool *isStopped, bool isSysHiddenIncluded )
{
    numofFolders = 0;
    numofFiles = 0;
    totalSize = 0;

    int tempNumofFolders = 0;
    int tempNumofFiles = 0;
    quint64 tempSize = 0;

    int ret = FmErrNone;

    foreach( const QString& folderPath, folderPathList ) {
        QFileInfo fileInfo( folderPath );
        if( fileInfo.isDir() ){
            int tempNumofSubFolders = 0;
            ret = getNumofSubfolders( folderPath, tempNumofSubFolders,
                tempNumofFiles, tempSize, isStopped, isSysHiddenIncluded );
            tempNumofFolders = tempNumofSubFolders + 1; // add itself to folder count;
        } else if( fileInfo.isFile() ) {
            tempNumofFiles = 1;
            tempSize = fileInfo.size();
            ret = FmErrNone;
        } else {
            ret = FmErrIsNotFileOrFolder;
        }
        if( ret != FmErrNone ) {
            return ret;
        }
        numofFolders+= tempNumofFolders;
        numofFiles  += tempNumofFiles;
        totalSize   += tempSize;

        tempNumofFolders = 0;
        tempNumofFiles      = 0;
        tempSize    = 0;
    }

    return ret;
}

int FmFolderDetails::getNumofSubfolders( const QString &folderPath, int &numofSubFolders, 
                                          int &numofFiles, quint64 &sizeofFolder,
                                          volatile bool *isStopped, bool isSysHiddenIncluded )
{
    numofSubFolders = 0;
    numofFiles = 0;
    sizeofFolder = 0;

    QList<QDir> dirs;
    dirs.append( QDir( folderPath ) );
    
    while (!dirs.isEmpty()) {
        QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
        if( isSysHiddenIncluded ) {
            filter = filter | QDir::Hidden | QDir::System;
        }

        QFileInfoList infoList = dirs.first().entryInfoList( filter );
        for ( QFileInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it ) {
            if ( *isStopped ){
                return FmErrCancel;
            }
            
            if ( it->isFile() ) {
                ++numofFiles;
                sizeofFolder += it->size();
            }
            else if ( it->isDir() ) {
                ++numofSubFolders;
                dirs.append( QDir( it->absoluteFilePath() ) );
            } 
        }
        
        dirs.removeFirst();
    }
    
    return FmErrNone;
}
