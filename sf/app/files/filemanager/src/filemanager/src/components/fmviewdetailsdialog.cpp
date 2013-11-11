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
 *     The source file of view details dialog of file manager
 */

#include "fmviewdetailsdialog.h"
#include "fmutils.h"
#include "fmviewdetailsitem.h"
#include "fmdrivedetailstype.h"
#include "fmviewmanager.h"

#include <QFileInfo>
#include <QDateTime>
#include <QStandardItemModel>
#include <QStringList>

#include <hblabel.h>
#include <hblistview.h>
#include <hbaction.h>

FmViewDetailsDialog::FmViewDetailsDialog( QGraphicsItem *parent ) 
    : FmDialog( parent ),
      mNumofSubFolers( 0 ),
      mNumofFiles( 0 ),
      mSizeofFolder( 0 ),
      mSizeofImage( 0 ),
      mSizeofSounds( 0 ),
      mSizeofVideo( 0 ),
      mSizeofJava( 0 ),
      mSizeofNativeFiles( 0 ),
      mSizeofDocument( 0 ),
      mSizeofCalendar( 0 ),
      mSizeofContacts( 0 ),
      mSizeofMessages( 0 )
{
    init();
    
    setTimeout( NoTimeout );
}

FmViewDetailsDialog::~FmViewDetailsDialog()
{
    if( mDataListModel ){
        mListView->setModel( 0 );
        delete mListView;
    }
}

void FmViewDetailsDialog::init()
{
    setObjectName( "viewDetailsDialog" );
    mHeaderLabel = new HbLabel( this );
    setHeadingWidget( mHeaderLabel );
    mHeaderLabel->setObjectName( "viewDetailsHeaderLabel" );

    mListView = new HbListView( this );
    mListView->setObjectName( "viewDetailsListView" );
    setContentWidget( mListView );
    
    // init QStardardItemModel as 0 row and 1 column
    mDataListModel = new QStandardItemModel( 0, 1 );
    mListView->setModel( mDataListModel ); 
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);

    mOkAction = new HbAction( this );
    mOkAction->setText( hbTrId("txt_common_button_ok") );
    setPrimaryAction( mOkAction );
}

void FmViewDetailsDialog::setDriveDetails( const QString &driverName )
{
    const QString dataDir = "C:\\data";
	QStringList dataList;
    mHeaderLabel->setPlainText( hbTrId( "txt_fmgr_title_memory_details" ) );
    
    quint64 sizeOfOthers = 0;
    
    FmDriverInfo driverInfo = FmUtils::queryDriverInfo( driverName );
    QString detailString( "" );
    
    mDataListModel->removeRows( 0, mDataListModel->rowCount() );
    mDataListModel->insertRows( 0, EDriveDetailEntryEnd );
    
	dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_memory_name" );
	dataList << FmUtils::fillDriveVolume( driverInfo.name(), true ) ; 
    mDataListModel->setData(  mDataListModel->index( EMemoryName, 0 ), dataList, Qt::DisplayRole );  
    
    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_size" ); 
    dataList << FmUtils::formatStorageSize( driverInfo.size() ) ;
    mDataListModel->setData(  mDataListModel->index( EMemorySize, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_free_memory" ); 
    dataList << FmUtils::formatStorageSize( driverInfo.freeSize() );
    mDataListModel->setData(  mDataListModel->index( EMemoryFree, 0 ), dataList, Qt::DisplayRole );    
    
    sizeOfOthers = driverInfo.size() - driverInfo.freeSize();
    
//    getContentDetails( dataDir );
    
    dataList.clear();
    dataList << hbTrId ( "Images:" ) ;
    dataList << FmUtils::formatStorageSize( mSizeofImage );
    mDataListModel->setData(  mDataListModel->index( EMemoryImages, 0 ), dataList, Qt::DisplayRole );  
    
    sizeOfOthers -= mSizeofImage;
    
    dataList.clear();
    dataList << hbTrId ( "Sounds:" );
	dataList << FmUtils::formatStorageSize( mSizeofSounds );
    mDataListModel->setData(  mDataListModel->index( EMemorySounds, 0 ), dataList, Qt::DisplayRole );   
    
    sizeOfOthers -= mSizeofSounds;
    
    dataList.clear();
    dataList << hbTrId ( "Video:" );
    dataList << FmUtils::formatStorageSize( mSizeofVideo );
    mDataListModel->setData(  mDataListModel->index( EMemoryVideos, 0 ), dataList, Qt::DisplayRole );   
    
    sizeOfOthers -= mSizeofVideo;
    
    dataList.clear();
    dataList << hbTrId ( "Java Files:" );
    dataList << FmUtils::formatStorageSize( mSizeofJava );
    mDataListModel->setData(  mDataListModel->index( EMemoryMidpJava, 0 ), dataList, Qt::DisplayRole );  
    
    sizeOfOthers -= mSizeofJava;
    
    dataList.clear();
    dataList << hbTrId ( "Sis files:" );
    dataList << FmUtils::formatStorageSize( mSizeofNativeFiles );
    mDataListModel->setData(  mDataListModel->index( EMemoryNativeApps, 0 ), dataList, Qt::DisplayRole );   
    
    sizeOfOthers -= mSizeofNativeFiles;
    
    dataList.clear();
    dataList << hbTrId ( "Documents:" );
    dataList << FmUtils::formatStorageSize( mSizeofDocument );
    mDataListModel->setData(  mDataListModel->index( EMemoryDocuments, 0 ), dataList, Qt::DisplayRole );   
    
    sizeOfOthers -= mSizeofDocument;
    
    dataList.clear();
    dataList << hbTrId ( "Calendars:" );
    dataList << FmUtils::formatStorageSize( mSizeofCalendar );
    mDataListModel->setData(  mDataListModel->index( EMemoryCalendar, 0 ), dataList, Qt::DisplayRole );  
    
    sizeOfOthers -= mSizeofCalendar;
    
    dataList.clear();
    dataList << hbTrId ( "Contacts:" );
    dataList << FmUtils::formatStorageSize( mSizeofContacts );
    mDataListModel->setData(  mDataListModel->index( EMemoryContacts, 0 ), dataList, Qt::DisplayRole );  
    
    sizeOfOthers -= mSizeofContacts;
    
    dataList.clear();
    dataList << hbTrId ( "Messages:" );
    dataList << FmUtils::formatStorageSize( mSizeofMessages );
    mDataListModel->setData(  mDataListModel->index( EMemoryMessages, 0 ), dataList, Qt::DisplayRole );  
    
    sizeOfOthers -= mSizeofMessages;
    
    dataList.clear();
    dataList << hbTrId ( "Others:" );
    dataList << FmUtils::formatStorageSize( sizeOfOthers );
    mDataListModel->setData(  mDataListModel->index( EMemoryOthers, 0 ), dataList, Qt::DisplayRole );     
}

void FmViewDetailsDialog::setFolderDetails( const QString &folderPath )
{
    mHeaderLabel->setPlainText( hbTrId( "txt_fmgr_title_folder_details" ) );
    
    QString detailString( "" );
	QStringList dataList;
	QFileInfo fileInfo( folderPath );
	
    mDataListModel->removeRows( 0, mDataListModel->rowCount() );
    mDataListModel->insertRows( 0, EFolderDetailEntryEnd );
    
    dataList << hbTrId ( "txt_fmgr_dblist_name" );
    dataList << fileInfo.fileName();
    mDataListModel->setData(  mDataListModel->index( EFolderName, 0 ), dataList, Qt::DisplayRole );  
    
    QDateTime modifiedDateTime = fileInfo.lastModified ();
    QDate modifiedDate = modifiedDateTime.date();
    QTime modifiedTime = modifiedDateTime.time();
    
    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_date" );
    dataList << modifiedDate.toString();
    mDataListModel->setData(  mDataListModel->index( EModifiedDate, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_time" );
    dataList << modifiedTime.toString();
    mDataListModel->setData(  mDataListModel->index( EModifiedTime, 0 ), dataList, Qt::DisplayRole );  
    
    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_size" );
    dataList << FmUtils::formatStorageSize( mSizeofFolder );
    mDataListModel->setData(  mDataListModel->index( EFolderSize, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_subfolders" );
    dataList << QString::number( mNumofSubFolers );
    mDataListModel->setData(  mDataListModel->index( ENumofSubFolders, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_files" );
    dataList << QString::number( mNumofFiles );
    mDataListModel->setData(  mDataListModel->index( ENumofFiles, 0 ), dataList, Qt::DisplayRole );  
}

void FmViewDetailsDialog::setFileDetails( const QString &filePath )
{
    mHeaderLabel->setPlainText( hbTrId( "txt_fmgr_title_file_details") );
    
    QString detailString( "" );
    QStringList dataList;

    QFileInfo fileInfo( filePath );
    
    mDataListModel->removeRows( 0, mDataListModel->rowCount() );
    mDataListModel->insertRows( 0, EFileDetailEntryEnd );
    
    dataList << hbTrId ( "txt_fmgr_dblist_name" );
    dataList << fileInfo.baseName();
    mDataListModel->setData(  mDataListModel->index( EFileName, 0 ), dataList, Qt::DisplayRole );  
    
    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_type_file" );
    
    
    
    QString fileType( fileInfo.completeSuffix() );
    if( fileType.isEmpty() ){
        fileType.append( FmUtils::getFileType( filePath ) );
    } else {
        //file type need to include the dot(.)
        fileType.insert( 0, Pre_Suffix_Dot );
    }
    dataList << fileType;
    mDataListModel->setData(  mDataListModel->index( EFileType, 0 ), dataList, Qt::DisplayRole );  

    QDateTime modifiedDateTime = fileInfo.lastModified ();
    QDate modifiedDate = modifiedDateTime.date();
    QTime modifiedTime = modifiedDateTime.time();
    
    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_date" );
    dataList << modifiedDate.toString();
    mDataListModel->setData(  mDataListModel->index( EFileModifiedDate, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_time" );
    dataList << modifiedTime.toString();
    mDataListModel->setData(  mDataListModel->index( EFileModifiedTime, 0 ), dataList, Qt::DisplayRole );  

    dataList.clear();
    dataList << hbTrId ( "txt_fmgr_dblist_size" );
    dataList << FmUtils::formatStorageSize( fileInfo.size() );
    mDataListModel->setData(  mDataListModel->index( EFileSize, 0 ), dataList, Qt::DisplayRole );  
}

void FmViewDetailsDialog::setNumofSubfolders(  int numofSubFolders, int numofFiles, quint64 sizeofFolder )
{
    mNumofSubFolers = 0;
    mNumofFiles = 0;
    mSizeofFolder = 0;
    
    mNumofSubFolers = numofSubFolders;
    mNumofFiles = numofFiles;
    mSizeofFolder = sizeofFolder;    
}

void FmViewDetailsDialog::setSizeofContent( QList<FmDriveDetailsSize*> detailsSizeList )
{
    mSizeofImage = 0;
    mSizeofSounds = 0;
    mSizeofVideo = 0;
    mSizeofJava = 0;
    mSizeofNativeFiles = 0;
    mSizeofDocument = 0;
    mSizeofCalendar = 0;
    mSizeofContacts = 0;
    mSizeofMessages = 0;
    
    for( QList< FmDriveDetailsSize* >::const_iterator it = detailsSizeList.begin(); 
            it!= detailsSizeList.end(); ++it ){
    
        switch( ( *it )->dataType() ){
            case FmDriveDetailsSize::ETypeImages:
            {
                mSizeofImage = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeSoundFiles:
            {
                mSizeofSounds = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeMidpJava:
            {
                mSizeofJava = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeNativeApps:
            {
                mSizeofNativeFiles = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeVideos:
            {
                mSizeofVideo = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeDocuments:
            {
                mSizeofDocument = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeCalendar:
            {
                mSizeofCalendar = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeContacts:
            {
                mSizeofContacts = ( *it )->size();
                break;
            }
            case FmDriveDetailsSize::ETypeMessages:
            {
                mSizeofMessages = ( *it )->size();
                break;
            } 
            default:
                break;
        }
    }
}

HbAction *FmViewDetailsDialog::executeDialog( FmDialog *dialog, const QString &associatedDrives )
{
    for( int i = 0; i < associatedDrives.length(); i++ ) {
        QString drive( associatedDrives[i] + QString( ":/" ) );
        if( !FmUtils::isDriveAvailable( drive ) ) {
            FM_LOG( "executeDialog return 0_ " + associatedDrives );
            return 0;
        }
    }
    
	FmDlgCloseUnit dlgCloseUnit( dialog );
	dlgCloseUnit.addAssociatedDrives( associatedDrives );

	FmViewManager::viewManager()->addDlgCloseUnit( &dlgCloseUnit );
	FM_LOG( " Exec Dialog start " );
    HbAction* action = dialog->exec();
	FM_LOG( " Exec Dialog end " );
	FmViewManager::viewManager()->removeDlgCloseUnit( &dlgCloseUnit );
	return action;
}

void FmViewDetailsDialog::showDriveViewDetailsDialog( const QString &driverName, 
            QList<FmDriveDetailsSize*> detailsSizeList, const QString& associatedDrives )
{
	FM_LOG( "showDriveViewDetailsDialog_" + associatedDrives );
    FmViewDetailsDialog viewDetailsDialog;
    
    viewDetailsDialog.setSizeofContent( detailsSizeList );
    viewDetailsDialog.setDriveDetails( driverName );
       
    //viewDetailsDialog.exec();
	executeDialog( &viewDetailsDialog , associatedDrives );
}

void FmViewDetailsDialog::showFolderViewDetailsDialog( const QString &folderPath,
           int numofSubFolders, int numofFiles, quint64 sizeofFolder, const QString& associatedDrives )
{
    FmViewDetailsDialog viewDetailsDialog;
    
    viewDetailsDialog.setNumofSubfolders( numofSubFolders, numofFiles, sizeofFolder );
    viewDetailsDialog.setFolderDetails( folderPath );
    
    //viewDetailsDialog.exec();
	executeDialog( &viewDetailsDialog , associatedDrives );
}

void FmViewDetailsDialog::showFileViewDetailsDialog( const QString &filePath, const QString& associatedDrives )
{
    FmViewDetailsDialog viewDetailsDialog;
    viewDetailsDialog.setFileDetails( filePath );
    
    //viewDetailsDialog.exec();
	executeDialog( &viewDetailsDialog , associatedDrives );
}
