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
*     The source file of filemanager view manager
*
*/

#include "fmviewmanager.h"
#include "fmmainwindow.h"
#include "fmdriverview.h"
#include "fmfileview.h"
#include "fmfindview.h"
#include "fmbackupview.h"
#include "fmrestoreview.h"
#include "fmdeletebackupview.h"
#include "fmsplitview.h"
#include "fmoperationservice.h"
#include "fmoperationbase.h"
#include "fmdrivewatcher.h"
#include "fmdialog.h"
#include "fmdlgutils.h"
#include "fmserviceutils.h"

#include <hbview.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QPixmap>

FmViewManager *FmViewManager::mViewManager = 0;
const QString FM_MAIN_VIEW = "FMMemoryStoragesList";

FmDlgCloseUnit::FmDlgCloseUnit( FmDialog *dialog ) : mDialog( dialog )
{
}
FmDlgCloseUnit::~FmDlgCloseUnit()
{
}

void FmDlgCloseUnit::addAssociatedDrives( QString drives )
{
	for( int i = 0; i< drives.length(); ++i ) {
		if( !mAssociatedDrives.contains( drives[i], Qt::CaseInsensitive ) ) {
			mAssociatedDrives += drives[i];
		}
	}
}
void FmDlgCloseUnit::removeAssociatedDrives( QString drives )
{
	for( int i = 0; i < drives.length(); ++i ){
		mAssociatedDrives.remove( drives[i], Qt::CaseInsensitive );
	}
}
QString FmDlgCloseUnit::associatedDrives()
{
	return mAssociatedDrives;
}

FmDialog *FmDlgCloseUnit::dialog()
{
	return mDialog;
}

FmViewManager::FmViewManager( FmMainWindow* mainWindow ) 
    : mOperationService( 0 ), mServiceUtils( 0 )
{
    mMainWindow = mainWindow;
    mDriveWatcher = new FmDriveWatcher( this );
    mDriveWatcher->setObjectName( "driveWatcher" );
    mDriveWatcher->startWatch();
    
    QMetaObject::connectSlotsByName( this );
}

FmViewManager::~FmViewManager(void)
{
    FmViewBase *view = static_cast<FmViewBase *>( mMainWindow->currentView() );
    while( view ) {
        mMainWindow->removeView( view );
        delete view;
        view = static_cast<FmViewBase *>( mMainWindow->currentView() );
    }

    delete mOperationService;
    mOperationService = 0;

    mDriveWatcher->cancelWatch();
    delete mDriveWatcher;
    mDriveWatcher = 0;
    
    delete mServiceUtils;
    mServiceUtils = 0;
}

FmViewManager *FmViewManager::CreateViewManager( FmMainWindow* mainWindow )
{
    if( !mViewManager ){
        mViewManager = new FmViewManager( mainWindow );
    }

    return mViewManager;
}
void FmViewManager::RemoveViewManager()
{
    delete mViewManager;
    mViewManager = 0;
}

FmViewManager *FmViewManager::viewManager()
{
    return mViewManager;
}

FmOperationService *FmViewManager::operationService()
{
    if ( !mOperationService ) {
        mOperationService = new FmOperationService( this );
        mOperationService->setObjectName( "operationService" );
        QMetaObject::connectSlotsByName( this );
    }    
    return mOperationService;
}

/*
   return \a FmServiceUtils. 
 */
FmServiceUtils *FmViewManager::serviceUtils()
{
    if( !mServiceUtils ) {
        mServiceUtils = new FmServiceUtils;
    }
    return mServiceUtils;
}

void FmViewManager::popViewAndShow()
{
    FmViewBase *view = static_cast<FmViewBase *>( mMainWindow->currentView() );
    view->setNavigationAction( 0 );
    // grap the screen when only the drive view is in view stack.
    if( viewCount() == 1 ) {
        mScreenShot = QPixmap::grabWidget(mMainWindow, mMainWindow->rect());
    }
    mMainWindow->removeView( view );  
	// If call QEventLoop in destructor of HbView, it will panic
	// So add aboutToClose function to do some prepare when view will be closed
	// for example, close find view while find is inprogress.
    // stop find thread in destructor of FmFindView(called by delete container->model...)
    // and use QEventLoop to wait till thread exit.
    view->aboutToClose();
    delete view;

    if( viewCount() < 1 )
    {
        mMainWindow->close();
    }
}

void FmViewManager::on_operationService_driveSpaceChanged( FmOperationBase *operationBase )
{
    Q_UNUSED( operationBase );
    // request drive view to refresh
    emit driveSpaceChanged();
}

void FmViewManager::on_operationService_notifyFinish( FmOperationBase *operationBase )
{
    if( !operationBase ){
        Q_ASSERT_X( false, "FmViewManager",
                "NULL is not accepted in on_operationService_notifyFinish()" );
        return;
    }
    
    if( operationBase->operationType() == FmOperationService::EOperationTypeBackup )
        {
        // after finish backup, we need refresh backup date in backup view.
        emit refreshBackupDate();
        }
}


int FmViewManager::viewCount()
{
    return mMainWindow->views().count();
}


void FmViewManager::createDriverView()
{
    FmDriverView *driverView= new FmDriverView();

    mMainWindow->addView( driverView );
    mMainWindow->setCurrentView( driverView );
    bool ok = serviceUtils()->removeActivity(FM_MAIN_VIEW);
    Q_UNUSED( ok );

    connect( this, SIGNAL( driveSpaceChanged() ), 
        driverView, SLOT( refreshDrive() ) );
    // Refresh drive view when gaining foreground
    // ( switch back to filemanager from home screen or any other applications )
    connect( mMainWindow, SIGNAL( revealed() ), 
        driverView, SLOT( refreshDrive() ) );
    
    // use Qt::QueuedConnection because synchronous refresh is not work well while
    // connected with another OTG device which have more than one external drive in it.
    // File server only give driveChanged event once, but two drive is inserted.
    // Synchronous refresh will miss another drive.
    // So use Qt::QueuedConnection to performance asynchronous refresh.
    connect( this, SIGNAL( driveChanged() ), 
        driverView, SLOT( refreshDrive() ), Qt::QueuedConnection );
}

void FmViewManager::createFileView( const QString &path,
                                    bool enableBackPathCheck, bool disableFind )
{
    QFileInfo fileInfo( path );
    QString absolutePath = fileInfo.absoluteFilePath();

    QString checkedPath = FmUtils::checkDriveToFolderFilter( absolutePath );
    if( FmUtils::isDriveAvailable( path ) ) {
        if( FmErrNone != FmUtils::isPathAccessabel( checkedPath )  ) {
            checkedPath.clear();
        }
    }
   
    if( checkedPath.isEmpty() ) {
        FmDlgUtils::warning( QString( hbTrId("Path: %1 is unavailable!").arg( path )) );
        return;
    }

    FmFileView *fileView= new FmFileView();
    fileView->setRootPath( checkedPath );
    if( enableBackPathCheck ) {
        fileView->setRootLevelPath( checkedPath );
    }

    fileView->setFindDisabled( disableFind );

    mMainWindow->addView( fileView );
    mMainWindow->setCurrentView( fileView );
    
    connect( this, SIGNAL( driveChanged() ), //emit when drive changed.
        fileView, SLOT( on_driveChanged() ) );

    connect( fileView, SIGNAL( popViewAndShow() ),                  //emit when fileView need delete itself and pop&show view from stack.
        this, SLOT( popViewAndShow() ), Qt::QueuedConnection );
}

/*
    Create fild view and search \a keyword in \a pathList
*/
void FmViewManager::createFindView( const QString &keyword, const QStringList &pathList )
{
    FmFindView *findView= new FmFindView();
    
    mMainWindow->addView( findView );
    mMainWindow->setCurrentView( findView );

    findView->find( keyword, pathList );
 }

void FmViewManager::createSplitView()
{
    FmSplitView *splitView = new FmSplitView();
    mMainWindow->addView( splitView );
    mMainWindow->setCurrentView( splitView );

}

void FmViewManager::createBackupView()
{
    FmBackupView *backupView= new FmBackupView();

    mMainWindow->addView( backupView );
    mMainWindow->setCurrentView( backupView );
    connect( this, SIGNAL( driveChanged() ), //emit when need refresh model
            backupView, SLOT( refreshModel() ) );
    connect( this, SIGNAL( refreshBackupDate() ),  //emit when need refresh backup date
            backupView, SLOT( refreshBackupDate() ) );
}

void FmViewManager::createRestoreView()
{
    FmRestoreView *restoreView= new FmRestoreView();

    mMainWindow->addView( restoreView );
    mMainWindow->setCurrentView( restoreView );
    connect( this, SIGNAL( driveChanged() ), restoreView, SLOT( refreshRestoreView() ) );

}

void FmViewManager::createDeleteBackupView()
{
   FmDeleteBackupView *deleteBackupView= new FmDeleteBackupView();

    mMainWindow->addView( deleteBackupView );
    mMainWindow->setCurrentView( deleteBackupView );
    connect( this, SIGNAL( driveChanged() ), deleteBackupView, SLOT( refreshDeleteBackupView() ) );

}

Qt::Orientation FmViewManager::orientation(){
    return mMainWindow->orientation();
}

void FmViewManager::on_driveWatcher_driveAddedOrChanged()
{
    FM_LOG( QString( "FmViewManager::on_driveWatcher_driveAddedOrChanged start" ) );
    emit driveChanged();
	checkDlgCloseUnit();
    FM_LOG( QString( "FmViewManager::on_driveWatcher_driveAddedOrChanged end" ) );

}


void FmViewManager::checkDlgCloseUnit()
{
	foreach( FmDlgCloseUnit* unit, mDlgCloseUnitList ) {
		QString drives( unit->associatedDrives() );
		for( int i = 0; i < drives.length(); i++ ) {
			QString drive( drives[i] + QString( ":/" ) );
			if( !FmUtils::isDriveAvailable( drive ) ) {
				FM_LOG( " close Dialog start " );
				unit->dialog()->close();
				FM_LOG( " close Dialog end " );
			}
		}
	}
}

void FmViewManager::saveActivity()
{    
    //grap the screen when current view is drive view.
    FmDriverView *driveView = qobject_cast<FmDriverView *>(mMainWindow->currentView());
    if ( ( driveView != 0 ) && ( driveView->viewType() == FmViewBase::EDriverView ) ) {
        mScreenShot = QPixmap::grabWidget(mMainWindow, mMainWindow->rect());
    }
    QVariantHash metadata;
    metadata.insert("screenshot", mScreenShot);         
    // save the activity
    bool ok = serviceUtils()->saveActivity(FM_MAIN_VIEW, QVariant(), metadata);
    // do not need check return value
    Q_UNUSED( ok );
}

void FmViewManager::onAboutToChangeView(HbView * oldView, HbView *newView)
{
    Q_UNUSED(newView);
    //grap the screen when oldView is drive view.
    FmDriverView *driveView = qobject_cast<FmDriverView *>(oldView);
    if ( ( driveView != 0 ) && ( driveView->viewType() == FmViewBase::EDriverView ) ) {        
        mScreenShot = QPixmap::grabWidget(mMainWindow, mMainWindow->rect());
    }
}
