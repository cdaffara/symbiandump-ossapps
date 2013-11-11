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
*     Zhiqiang Yang <zhiqiang.yang@nokia.com>
*     Steven Yao <steven.yao@nokia.com>
* 
* Description:
*     The source file of the file browse view of file manager
*
*/

#include "fmfileview.h"
#include "fmutils.h"
#include "fmfiledialog.h"
#include "fmfilebrowsewidget.h"
#include "fmviewmanager.h"
#include "fmoperationservice.h"
#include "fmdlgutils.h"

#include <QApplication>
#include <QGraphicsLinearLayout>

#include <hbinstance.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbtoolbarextension.h>
#include <hblineedit.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>

FmFileView::FmFileView() : FmViewBase( EFileView ), mWidget( 0 ),
    mStyleAction( 0 ), mSelectableAction( 0 ),
    mFindAction( 0 ), mSortExtension( 0 ), 
    mOperationService( 0 ), mMenu( 0 ), mIsFindDisabled( false )
{
    mOperationService = FmViewManager::viewManager()->operationService();
	initMenu();
	initMainWidget();
    initToolBar();

	QMetaObject::connectSlotsByName( this );
}

FmFileView::~FmFileView()
{
    removeToolBarAction();
}

void FmFileView::setRootPath( const QString &pathName )
{
    if( pathName.isEmpty() ) {
        return;
    }
    mWidget->setRootPath( pathName );
    QString driveName = FmUtils::getDriveNameFromPath( pathName );
    /*
	FmDriverInfo driverInfo = FmUtils::queryDriverInfo( driveName );
    QString volumeName = driverInfo.volumeName();
	if( volumeName.isEmpty() ){
        FmDriverInfo::DriveState driveState = FmUtils::queryDriverInfo( driveName ).driveState();
        if( !( driveState & FmDriverInfo::EDriveNotPresent ) ){
            if( driveState & FmDriverInfo::EDriveRemovable ) {
                if( driveState & FmDriverInfo::EDriveMassStorage ) {
                    volumeName.append( hbTrId( "Mass Storage" ) );  
                }
                else{
                    volumeName.append( hbTrId( "Memory Card" ) );
                }
            }
            else{
                volumeName.append( hbTrId( "Phone Memory" ) );
            }
        }
	    }
	*/
	//QString titleText = FmUtils::removePathSplash( driverInfo.name() ) + ' ' + volumeName;
    QString titleText( FmUtils::fillDriveVolume( driveName, true ) );
	setTitle( titleText );
}

void FmFileView::setRootLevelPath( const QString &pathName )
{
    mWidget->setRootLevelPath( FmUtils::fillPathWithSplash( pathName ) );
}

void FmFileView::setFindDisabled( bool disable )
{
    mIsFindDisabled = disable;
    if( mFindAction ) {
        mFindAction->setDisabled( mIsFindDisabled );
    }
}

void FmFileView::initMenu()
{
    HbAction *action = 0;
#ifdef FM_CHANGE_ORIENT_ENABLE
	action = new HbAction( this );
	action->setObjectName( "rotateAction" );
	action->setText( hbTrId( "Change orientation" ) );
	menu()->addAction( action );
#endif

//	mStyleAction = new HbAction();
//	mStyleAction->setObjectName( "switchStyle" );
//	menu()->addAction( mStyleAction );

	mSelectableAction = new HbAction();
	mSelectableAction->setObjectName( "setSelectable" );
	menu()->addAction( mSelectableAction );
    connect( mSelectableAction, SIGNAL( triggered() ),
        this, SLOT( on_setSelectable_triggered() ), Qt::QueuedConnection );


	action = new HbAction();
	action->setObjectName( "delete" );
	action->setText( hbTrId( "txt_common_opt_delete" ) );
	menu()->addAction( action );
    connect( action, SIGNAL( triggered() ),
        this, SLOT( on_delete_triggered() ), Qt::QueuedConnection );

    action = new HbAction();
    action->setObjectName( "copy" );
    action->setText( hbTrId( "txt_common_opt_copy_to_folder" ) );
    menu()->addAction( action );
    connect( action, SIGNAL( triggered() ),
        this, SLOT( on_copy_triggered() ), Qt::QueuedConnection );

    action = new HbAction();
    action->setObjectName( "move" );
    action->setText( hbTrId( "txt_common_opt_move_to_folder" ) );
    menu()->addAction( action );
    connect( action, SIGNAL( triggered() ),
        this, SLOT( on_move_triggered() ), Qt::QueuedConnection );
    
    action = new HbAction();
    action->setObjectName( "newFolder" );
    action->setText( hbTrId( "txt_fmgr_opt_new_folder" ) );
    menu()->addAction( action );
    connect( action, SIGNAL( triggered() ),
        this, SLOT( on_newFolder_triggered() ), Qt::QueuedConnection );
    
    HbMenu *subMenu = new HbMenu( hbTrId( "Sort" ) );
    HbAction *sortNameAction = new HbAction( subMenu );
    sortNameAction->setObjectName( "sortNameAction" );
    sortNameAction->setText( hbTrId( "Sort by name" ) );
    subMenu->addAction( sortNameAction );
    
    HbAction *sortTimeAction = new HbAction( subMenu );
    sortTimeAction->setObjectName( "sortTimeAction" );
    sortTimeAction->setText( hbTrId( "Sort by time" ) );
    subMenu->addAction( sortTimeAction );
    
    HbAction *sortSizeAction = new HbAction( subMenu );
    sortSizeAction->setObjectName( "sortSizeAction" );
    sortSizeAction->setText( hbTrId( "Sort by size" ) );
    subMenu->addAction( sortSizeAction );
    
    HbAction* sortTypeAction = new HbAction( subMenu );
    sortTypeAction->setObjectName( "sortTypeAction" );
    sortTypeAction->setText( hbTrId( "Sort by type" ) );
    subMenu->addAction( sortTypeAction );
    
    menu()->addMenu( subMenu );
	
    connect( sortNameAction, SIGNAL( triggered() ),
             this, SLOT( on_sortNameAction_triggered() ), Qt::QueuedConnection );
    connect( sortTimeAction, SIGNAL( triggered() ),
             this, SLOT( on_sortTimeAction_triggered() ), Qt::QueuedConnection );
    connect( sortSizeAction, SIGNAL( triggered() ),
             this, SLOT( on_sortSizeAction_triggered() ), Qt::QueuedConnection );
    connect( sortTypeAction, SIGNAL( triggered() ),
             this, SLOT( on_sortTypeAction_triggered() ), Qt::QueuedConnection );
    
    mMenu = takeMenu();
}

void FmFileView::initMainWidget()
{
	QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout( this );
	vLayout->setOrientation( Qt::Vertical );

	mWidget = new FmFileBrowseWidget( this );
	mWidget->setObjectName( "mainWidget" );
	setStyle( FmFileBrowseWidget::ListStyle );
	setSelectable( false );

	vLayout->addItem( mWidget );
	//set stretch factor to file browser widget, so that it could fully cover the client area.
	vLayout->setStretchFactor( mWidget, 1 );

	setLayout( vLayout );
	
    connect( mWidget, SIGNAL( startSearch( QString ) ),
             this, SLOT( startSearch( QString ) ) );
    connect( mWidget, SIGNAL( setEmptyMenu( bool ) ),
             this, SLOT( on_mainWidget_setEmptyMenu( bool ) ) );
    connect( mWidget, SIGNAL( setTitle( QString ) ),
             this, SLOT( on_mainWidget_setTitle( QString ) ) );
}

void FmFileView::initToolBar()
{
    toolBar()->clearActions();
    mFindAction = new HbAction( this );
    mFindAction->setObjectName( "leftAction" );
    mFindAction->setText( hbTrId("txt_fmgr_opt_find") );
    mFindAction->setDisabled( mIsFindDisabled );
    toolBar()->addAction( mFindAction );
    toolBar()->setOrientation( Qt::Horizontal );
    
    mSortNameAction = new HbAction( this );    
    mSortNameAction->setText( hbTrId( "txt_fmgr_setlabel_sort_by_name" ) );
    
    mSortTimeAction = new HbAction( this );   
    mSortTimeAction->setText( hbTrId( "txt_fmgr_setlabel_sort_by_time" ) );
    
    mSortSizeAction = new HbAction( this );   
    mSortSizeAction->setText( hbTrId( "txt_fmgr_setlabel_sort_by_size" ) );
    
    mSortTypeAction = new HbAction( this );   
    mSortTypeAction->setText( hbTrId( "txt_fmgr_setlabel_sort_by_type" ) );
    
    mSortExtension = new HbToolBarExtension();    
    mSortExtension->addAction( mSortNameAction );
    mSortExtension->addAction( mSortTimeAction );
    mSortExtension->addAction( mSortSizeAction );
    mSortExtension->addAction( mSortTypeAction );
    
    HbAction* extensionAction = toolBar()->addExtension( mSortExtension );
    extensionAction->setText( hbTrId("txt_fmgr_opt_sort"));
   
    connect( mFindAction, SIGNAL( triggered() ),
            this, SLOT( on_leftAction_triggered() ) );
    connect( mSortNameAction, SIGNAL( triggered() ),
            this, SLOT( on_sortNameAction_triggered() ), Qt::QueuedConnection );
    connect( mSortTimeAction, SIGNAL( triggered() ),
            this, SLOT( on_sortTimeAction_triggered() ), Qt::QueuedConnection );
    connect( mSortSizeAction, SIGNAL( triggered() ),
            this, SLOT( on_sortSizeAction_triggered() ), Qt::QueuedConnection );
    connect( mSortTypeAction, SIGNAL( triggered() ),
            this, SLOT( on_sortTypeAction_triggered() ), Qt::QueuedConnection );
}

void FmFileView::setStyle( FmFileBrowseWidget::Style style )
{
    /*
	if ( style == FmFileBrowseWidget::ListStyle ) {
		mStyleAction->setText( hbTrId("Tree") );
	} else if ( style == FmFileBrowseWidget::TreeStyle ) {
		mStyleAction->setText( hbTrId("List") );
	}
	*/
	mWidget->setStyle( style );
}

void FmFileView::setSelectable( bool enable )
{
	if (enable) {
		mSelectableAction->setText( hbTrId("No Select") );
	} else {
		mSelectableAction->setText( hbTrId("Select") );
	}

	mWidget->setSelectable( enable );
}

void FmFileView::infoNoFileSelected()
{
	FmDlgUtils::warning( hbTrId("No File/Folder selected" ) );
}

#ifdef FM_CHANGE_ORIENT_ENABLE
void FmFileView::on_rotateAction_triggered()
{
	if ( mainWindow()->orientation() == Qt::Vertical ) {
		mainWindow()->setOrientation( Qt::Horizontal );
	}
	else {
		mainWindow()->setOrientation( Qt::Vertical );
	}
}
#endif

void FmFileView::on_switchStyle_triggered()
{
	if (mWidget->style() == FmFileBrowseWidget::ListStyle) {
		setStyle( FmFileBrowseWidget::TreeStyle );
	} else if (mWidget->style() == FmFileBrowseWidget::TreeStyle) {
		setStyle( FmFileBrowseWidget::ListStyle );
	}
}

void FmFileView::on_setSelectable_triggered()
{
	setSelectable( !mWidget->selectable() );
}

void FmFileView::on_delete_triggered()
{
	QList<QFileInfo> files = mWidget->checkedItems();
    if (files.size() == 0) {
		infoNoFileSelected();
    } else {
        if (FmDlgUtils::question( hbTrId("Confirm Deletion?" ) )) {
            QStringList fileList;
            for (int i = 0; i < files.size(); ++i) {
                fileList.push_back( files[i].absoluteFilePath() );
                }
            int ret = mOperationService->asyncRemove( fileList );
            switch( ret ) {
                case FmErrNone:
                    // no error, do not show note to user
                    break;
                case FmErrAlreadyStarted:
                    // last operation have not finished
                    FmDlgUtils::warning( hbTrId( "Operatin already started!" ) );
                    break;
                case FmErrWrongParam:
                    FmDlgUtils::warning( hbTrId( "Wrong parameters!" ) );
                    break;
                default:
                    FmDlgUtils::warning( hbTrId( "Operation fail to start!" ) );
            }
            setSelectable( false );
        }
    }
}

void FmFileView::on_copy_triggered()
{
    QList<QFileInfo> files = mWidget->checkedItems();
    QStringList srcFileList;

    if (files.size() == 0) {
        infoNoFileSelected();
    } else {
        QString targetPathName = FmFileDialog::getExistingDirectory( 0, hbTrId( "copy to" ),
            QString(""), QStringList() );
        if( !targetPathName.isEmpty() && files.size() > 0 ) {
            targetPathName = FmUtils::fillPathWithSplash( targetPathName );

            foreach( QFileInfo fileInfo, files ){
                srcFileList.push_back( fileInfo.absoluteFilePath() );
            }

            int ret = mOperationService->asyncCopy(
                srcFileList, targetPathName );
            switch( ret ) {
                case FmErrNone:
                    // no error, do not show note to user
                    break;
                case FmErrAlreadyStarted:
                    // last operation have not finished
                    FmDlgUtils::warning( hbTrId( "Operatin already started!" ) );
                    break;
                case FmErrWrongParam:
                    FmDlgUtils::warning( hbTrId( "Wrong parameters!" ) );
                    break;
                default:
                    FmDlgUtils::warning( hbTrId( "Operation fail to start!" ) );
            }
            setSelectable( false );
        }
    }

}
 
void FmFileView::on_move_triggered()
{
    QList<QFileInfo> files = mWidget->checkedItems();

    if (files.size() == 0) {
        infoNoFileSelected();
    } else {
        QString targetPathName = FmFileDialog::getExistingDirectory( 0, hbTrId( "move to" ),
            QString(""), QStringList() );

        if( !targetPathName.isEmpty() && files.size() > 0 ) {
            targetPathName = FmUtils::fillPathWithSplash( targetPathName );

            QStringList fileList;
            for (int i = 0; i < files.size(); ++i) {
                fileList.push_back( files[i].absoluteFilePath() );
            }
            int ret = mOperationService->asyncMove( fileList, targetPathName );
            switch( ret ) {
                case FmErrNone:
                    // no error, do not show note to user
                    break;
                case FmErrAlreadyStarted:
                    // last operation have not finished
                    FmDlgUtils::warning( hbTrId( "Operatin already started!" ) );
                    break;
                case FmErrWrongParam:
                    FmDlgUtils::warning( hbTrId( "Wrong parameters!" ) );
                    break;
                default:
                    FmDlgUtils::warning( hbTrId( "Operation fail to start!" ) );
            }
            setSelectable( false );
        }
    }
}

void FmFileView::on_newFolder_triggered()
{
    int maxFileNameLength = FmUtils::getMaxFileNameLength();
    QString associatedDrive = FmUtils::getDriveLetterFromPath( mWidget->currentPath().absoluteFilePath() );
    QString path = FmUtils::fillPathWithSplash( mWidget->currentPath().absoluteFilePath() );
    QString dirName = createDefaultFolderName( path );
    QStringList regExpList = (QStringList() << Regex_ValidFileFolderName << Regex_ValidNotEndWithDot );

    QDir dir( path );  
    if( dir.exists() ) {
        while( FmDlgUtils::showTextQuery( hbTrId( "txt_fmgr_title_new_folder" ), dirName,
            regExpList, maxFileNameLength, associatedDrive , false ) ){
            // remove whitespace from the start and the end.
            dirName = dirName.trimmed();
            QString newTargetPath = FmUtils::fillPathWithSplash(
                dir.absolutePath() ) + dirName;
            QString errString;
            // check if name/path is available for use
            if( !FmUtils::checkNewFolderOrFile( dirName, newTargetPath, errString ) ) {
                FmDlgUtils::warning( errString, HbMessageBox::Ok, true );
                continue;
            }
            if( !dir.mkdir( dirName ) ) {
                FmDlgUtils::warning( hbTrId("Operation failed!") );
            }
            break;
        }
        
    }
}

void FmFileView::on_leftAction_triggered()
{
    mWidget->activeSearchPanel();
}

void FmFileView::on_driveChanged()
{
    mWidget->on_driveChanged();  
}

void FmFileView::on_sortNameAction_triggered()
{
    mWidget->sortFiles( FmFileBrowseWidget::ESortByName );
}

void FmFileView::on_sortTimeAction_triggered()
{
    mWidget->sortFiles( FmFileBrowseWidget::ESortByTime );
}

void FmFileView::on_sortSizeAction_triggered()
{
    mWidget->sortFiles( FmFileBrowseWidget::ESortBySize );
}

void FmFileView::on_sortTypeAction_triggered()
{
    mWidget->sortFiles( FmFileBrowseWidget::ESortByType );
}

void FmFileView::startSearch( const QString &criteria )
{    
    if ( !criteria.isEmpty() ) {
        QString targetPath( mWidget->currentPath().filePath() );
        FmViewManager::viewManager()->createFindView( criteria, QStringList()<<targetPath );
    }
}

void FmFileView::removeToolBarAction()
{
    toolBar()->clearActions();
    if ( mSortExtension ) {
        delete mSortExtension;
        mSortExtension = 0;
    }
}

void FmFileView::on_mainWidget_setEmptyMenu( bool isMenuEmpty )
{
    if( isMenuEmpty ){
        FM_LOG( "setEmptyMenu true" );
        if( !mMenu ) {
            mMenu = takeMenu();
        }
        toolBar()->clearActions();
    }
    else {
        FM_LOG( "setEmptyMenu false" );
        if( mMenu ) {
            setMenu( mMenu );
            mMenu = 0;
        }
        initToolBar();
    }
}

void FmFileView::on_mainWidget_setTitle( const QString &title )
{
    this->setTitle( title );
}

QString FmFileView::createDefaultFolderName( const QString &path )
{
    // create new folder name, for example, New folder(01), New folder(02)
    QString checkedPath( FmUtils::fillPathWithSplash( path ) );
    QString dirName( hbTrId( "txt_fmgr_dialog_entry_new_folder" ) );
    QString dirAbsolutePath( checkedPath + dirName );
    QFileInfo fileInfo( dirAbsolutePath );
    int i = 0;    
    while ( fileInfo.exists() ) {
        ++i;
		QString numName;
        if ( i < 10 ) {
            numName.append( QString::number(0) );                        
        }
        numName.append( QString::number(i) );
        // txt_fmgr_dialog_entry_new_folder_l1 is not available now. use actual text instead of it.
        dirName = hbTrId( "New folder (%L1)" ).arg( numName );
        dirAbsolutePath = checkedPath + dirName;
        fileInfo.setFile( dirAbsolutePath );
    }
    return dirName;
}

FmEventResponse FmFileView::offerBackEvent()
{
    return mWidget->offerBackEvent();
}

