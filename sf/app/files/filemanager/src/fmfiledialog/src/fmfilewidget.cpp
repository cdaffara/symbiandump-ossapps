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
*     The source file of the file widget
*
*/
#include "fmfilewidget.h"
#include "fmutils.h"
#include "fmcommon.h"
#include "fmdrivemodel.h"
#include "fmdrivewatcher.h"
#include "fmfileiconprovider.h"
#include "fmfilesystemproxymodel.h"

#include "hbstyle.h"
#include "hbabstractitemview.h"
#include <hblistview.h>

#include <QModelIndex>
#include <QGraphicsLinearLayout>
#include <QFileSystemModel>
#include <QTime>
#include <QFileInfo>




FmFileWidget::FmFileWidget( HbWidget *parent ) :
    HbWidget( parent ), mCurrentModel( 0 )
{
    init();
}
FmFileWidget::~FmFileWidget()
{
    setModel( 0 );
    delete mFileSystemSourceModel;
    delete mFileModel;
    delete mFileIconProvider;
    delete mDriveModel;
    
    mDriveWatcher->cancelWatch();
    delete mDriveWatcher;
    mDriveWatcher = 0;
}

QFileInfo FmFileWidget::currentPath() const
{
    QModelIndex index( mListView->rootIndex() );
    if( !index.isValid() ) {
        return QFileInfo();
    }
    if( mCurrentModel == mFileModel ) {
        return mFileModel->fileInfo( index );
    } else {
        return QFileInfo();
    }
}

void FmFileWidget::setRootPath( const QString &pathName )
{
    FM_LOG( "FmFileWidget::setRootPath start" );
	// If path is empty or can not access, set model as DriveModel
	// Otherwise set model as FileSystemModel, means it will return to drive view if error occur.
    if( ( pathName.isEmpty() ) || ( FmErrNone != FmUtils::isPathAccessabel( pathName ) ) ) {
        FM_LOG( "FmFileWidget::setRootPath set drive model" );
        setModel( mDriveModel );
        FM_LOG( "FmFileWidget::setRootPath set drive model end" );
        emit pathChanged( QString() );
    } else {
        FM_LOG( "FmFileWidget::setRootPath set dir model end" );
        setModel( mFileModel );
        FM_LOG( "FmFileWidget::setRootPath set dir model end" );
		mListView->setRootIndex( mFileModel->setRootPath( pathName ) );
        FM_LOG( "FmFileWidget::setRootPath set rootIndex" );
        emit pathChanged( pathName );
    }    
    FM_LOG( "FmFileWidget::setRootPath end" );
}
void FmFileWidget::on_list_activated( const QModelIndex &index )
    {
    mActivatedModelIndex = index;
    emit listActivated();
    }

void FmFileWidget::on_listActivated()
{
    FM_LOG("FmFileWidget::on_list_activated start" );
    if( mCurrentModel == mDriveModel ) {
    //If current model is DriveModel, open drive and set path
        QString driveName = mDriveModel->driveName( mActivatedModelIndex );
        QString checkedPath = FmUtils::checkDriveToFolderFilter( driveName );
        if( checkedPath.isEmpty() ) {
            FM_LOG("FmFileWidget::on_list_activated end becaise checkedpath empty" );
            return;
        }

        FM_LOG("FmFileWidget::on_list_activated setModel dir start" );
        setModel( mFileModel );
        FM_LOG("FmFileWidget::on_list_activated setModel dir end" );
        setRootPath( checkedPath );
        FM_LOG("FmFileWidget::on_list_activated setRootIndex" );
        emit pathChanged( checkedPath );
        FM_LOG("FmFileWidget::on_list_activated finish emit pathChanged" );
    }
    else if( mCurrentModel == mFileModel ) {
    //If current model is FileSystemModel, open path or emit file activate signal.
        if ( mFileModel->isDir( mActivatedModelIndex ) ) {
            FM_LOG("FmFileWidget::on_list_activated start changeRootIndex" );
            changeRootIndex( mActivatedModelIndex );
            FM_LOG("FmFileWidget::on_list_activated finish changeRootIndex" );
        } else {
            QFileInfo fileInfo( mFileModel->filePath( mActivatedModelIndex ) );
            if( fileInfo.isFile() ) {
                emit fileActivated( fileInfo.fileName() );
                FM_LOG("FmFileWidget::on_list_activated finish emit fileActivated" );
            }
        }
    } else {
        Q_ASSERT( false );
    }
    FM_LOG("FmFileWidget::on_list_activated end" );
}

void FmFileWidget::setModelFilter( QDir::Filters filters )
{
    mFileModel->setFilter( filters );
}

void FmFileWidget::setNameFilters( const QStringList &nameFilters )
{
    mFileModel->setNameFilters( nameFilters );
}

void FmFileWidget::changeRootIndex( const QModelIndex &index )
{
    FM_LOG("FmFileWidget::changeRootIndex start" );
    if( mCurrentModel != mFileModel ) {
        FM_LOG("FmFileWidget::changeRootIndex end because model not equal mFileModel" );
        return;
    }

    mFileModel->fetchMore(index);
	QString filePath = mFileModel->fileInfo( index ).absoluteFilePath();
    // pathChanged signal will be emitted in setRootPath
	setRootPath( filePath );
    FM_LOG("FmFileWidget::changeRootIndex end" );
}

void FmFileWidget::init()
{
    mLayout = new QGraphicsLinearLayout( this );
    mLayout->setOrientation( Qt::Vertical );
    setLayout( mLayout );

    mListView = new HbListView( this );
    mListView->setObjectName( "list" );
    mLayout->addItem( mListView );

    mDriveModel = new FmDriveModel( this, 
        FmDriveModel::FillWithVolume | FmDriveModel::FillWithDefaultVolume | FmDriveModel::HideUnAvailableDrive );
    
    qDebug( QTime::currentTime().toString().toUtf8().data() );
    mFileSystemSourceModel = new QFileSystemModel( this );
    mFileIconProvider = new FmFileIconProvider();
    mFileSystemSourceModel->setIconProvider( mFileIconProvider );
    qDebug("constructed mFileSystemSourceModel");

    
    mFileModel = new FmFileSystemProxyModel( this );
    mFileModel->setSourceModel( mFileSystemSourceModel );

    
    qDebug( QTime::currentTime().toString().toUtf8().data() );
    setModel( mDriveModel );
    qDebug("setmodel");
    qDebug( QTime::currentTime().toString().toUtf8().data() );

    mDriveWatcher = new FmDriveWatcher( this );
    mDriveWatcher->setObjectName( "driveWatcher" );
    mDriveWatcher->startWatch();
    
//    QMetaObject::connectSlotsByName( this );
    connect( mListView, SIGNAL( activated( QModelIndex ) ),
        this, SLOT( on_list_activated( QModelIndex ) ) );
    connect( this, SIGNAL( listActivated() ),
        this, SLOT( on_listActivated() ), Qt::QueuedConnection );
        
    connect( mDriveWatcher, SIGNAL( driveAddedOrChanged() ),
            this, SLOT( on_driveWatcher_driveAddedOrChanged() ) );
    

}

void FmFileWidget::setModel( QAbstractItemModel *model )
{
    mListView->setModel( model );
    mCurrentModel = model;
}

FmFileWidget::ViewType FmFileWidget::currentViewType()
{
    ViewType viewType = DriveView;
    if( mCurrentModel == mDriveModel ) {
        viewType = DriveView;
    } else if( mCurrentModel == mFileModel ) {
        viewType = DirView;
    } else {
        Q_ASSERT( false );
    }
    return viewType;
}


bool FmFileWidget::cdUp()
{
    QString path( FmUtils::checkFolderToDriveFilter( currentPath().absoluteFilePath() ) );
    QFileInfo fileInfo( path );
    QString cdUpPath;
    // path length>3 means current path is a sub folder, then get up level path and navigate to it
    // Otherwise means current path is a top level drive path, Should navigate to drive view. So setRootPath with empty path string.
    if( path.length() > 3 ) {
        cdUpPath = fileInfo.dir().absolutePath();
    }
    setRootPath( FmUtils::fillPathWithSplash( cdUpPath ) );
    return true;
}

void FmFileWidget::on_driveWatcher_driveAddedOrChanged()
{
    FM_LOG( QString( "FmFileDialog_FmFileWidget::on_driveWatcher_driveAddedOrChanged start" ) );
    mDriveModel->refresh();
    if( currentViewType() == DriveView ) {
        setModel( 0 );
        setModel( mDriveModel );
        emit pathChanged( QString() );
    } else if( currentViewType() == DirView ) {
        if( FmErrNone != FmUtils::isPathAccessabel( currentPath().absoluteFilePath() ) ) {
            // path not available, set model to drive
            FM_LOG( QString( "FmFileDialog_FmFileWidget::on_driveWatcher_driveAddedOrChanged path not availeable, set drivemodel:"
                    + currentPath().absoluteFilePath() ) );
            setModel( mDriveModel );
            emit pathChanged( QString() );
        }
    }
    FM_LOG( QString( "FmFileDialog_FmFileWidget::on_driveWatcher_driveAddedOrChanged end" ) );
}
