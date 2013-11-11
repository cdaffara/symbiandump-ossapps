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
 *     The header file of view details dialog of file manager
 */

#ifndef FMVIEWDETAILSDIALOG_H
#define FMVIEWDETAILSDIALOG_H

#include "fmcommon.h"
#include "fmdialog.h"

class QStandardItemModel;
class HbListView;
class HbLabel;
class HbAction;
class FmDriveDetailsSize;

class FmViewDetailsDialog : public FmDialog
{
    Q_OBJECT
  
public:
    enum driveDetailEntry{
        EMemoryName = 0,
        EMemorySize,
        EMemoryFree,
        EMemoryImages,
        EMemorySounds,
        EMemoryVideos,
        EMemoryMidpJava,
        EMemoryNativeApps,
        EMemoryDocuments,
        EMemoryCalendar,
        EMemoryContacts,
        EMemoryMessages,
        EMemoryOthers,
        EDriveDetailEntryEnd
    };
    
    enum folderDetailEntry{
        EFolderName = 0,
        EModifiedDate,
        EModifiedTime,
        EFolderSize,
        ENumofSubFolders,
        ENumofFiles,
        EFolderDetailEntryEnd
    };
    
    enum fileDetailEntry{
        EFileName = 0,
        EFileType,
        EFileModifiedDate,
        EFileModifiedTime,
        EFileSize,
        EFileDetailEntryEnd
    };
    
public:
    virtual ~FmViewDetailsDialog();
    
    static void showDriveViewDetailsDialog( const QString &driverName, 
                                            QList<FmDriveDetailsSize*> detailsSizeList,
											const QString& associatedDrives = QString() );
    static void showFolderViewDetailsDialog( const QString &folderPath,
                                             int numofSubFolders, int numofFiles, 
                                             quint64 sizeofFolder, const QString& associatedDrives = QString() );
    static void showFileViewDetailsDialog( const QString &filePath, const QString& associatedDrives = QString() );
    
private:
    FmViewDetailsDialog( QGraphicsItem *parent = 0 );
    
    void init();
    void setDriveDetails( const QString &driverName );
    void setFolderDetails( const QString &folderPath  );
    void setFileDetails( const QString &filePath );
    void setNumofSubfolders( int numofSubFolders, int numofFiles, quint64 sizeofFolder );
    void setSizeofContent( QList<FmDriveDetailsSize*> detailsSizeList );
    
	static HbAction *executeDialog( FmDialog *dialog, const QString &associatedDrives );
private:
    HbListView *mListView;
    HbLabel *mHeaderLabel;  
    HbAction *mOkAction;
    
    QStandardItemModel *mDataListModel;
    QString     mDrive;
    
    int mNumofSubFolers;
    int mNumofFiles;
    quint64 mSizeofFolder;
    quint64 mSizeofImage;
    quint64 mSizeofSounds;
    quint64 mSizeofVideo;
    quint64 mSizeofJava;
    quint64 mSizeofNativeFiles;
    quint64 mSizeofDocument;
    quint64 mSizeofCalendar;
    quint64 mSizeofContacts;
    quint64 mSizeofMessages;
};

#endif /* FMVIEWDETAILSDIALOG_H_ */
