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
 * 
 * Description:
 *     The source file of File Manager driver view
 */

#include "fmdriverview.h"
#include "fmdriverlistwidget.h"
#include "fmviewmanager.h"
#include "fmcommon.h"
#include "fmutils.h"

#include <QDir>
#include <QApplication>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmainwindow.h>
#include <hbsearchpanel.h>

FmDriverView::FmDriverView() : FmViewBase( EDriverView )
{
    FM_LOG( "FmDriverView::FmDriverView" );
    initMenu(); 
    initToolBar();
    initDiskListWidget();

    QMetaObject::connectSlotsByName( this );
}

FmDriverView::~FmDriverView()
{
    removeToolBarAction();
}

void FmDriverView::initMenu()
{
    HbAction *action = 0;

#ifdef FM_CHANGE_ORIENT_ENABLE
    action = new HbAction( this );
    action->setObjectName( "rotateAction" );
    action->setText( hbTrId( "Change orientation" ) );
    menu()->addAction( action );
#endif

    action = new HbAction( this );
    action->setObjectName( "findAction" );
    action->setText( hbTrId( "txt_fmgr_opt_find" ) );
    menu()->addAction( action );

    action = new HbAction( this );
    action->setObjectName( "backupAction" );
    action->setText( hbTrId( "txt_fmgr_opt_backup" ) );
    menu()->addAction( action );

    action = new HbAction( this );
    action->setObjectName( "restoreAction" );
    action->setText( hbTrId( "txt_fmgr_opt_restore" )  );
    menu()->addAction( action );
}

void FmDriverView::initDiskListWidget()
{
    mDriverList = new FmDriverListWidget( this );
    connect( mDriverList, SIGNAL( activated( QString ) ),
             this, SLOT( activated( QString ) ), Qt::QueuedConnection );
    
    connect( mDriverList, SIGNAL( startSearch( QString ) ),
        this, SLOT( startSearch( QString ) ) );
    
    setWidget( mDriverList );
}

void FmDriverView::initToolBar()
{ 
    mToolBarLeftAction = new HbAction( this );
    mToolBarLeftAction->setObjectName( "leftAction" );
    mToolBarLeftAction->setText( hbTrId( "txt_fmgr_opt_backup" ) );
    toolBar()->addAction( mToolBarLeftAction );

    mToolBarRightAction = new HbAction( this );
    mToolBarRightAction->setObjectName( "rightAction" );
    mToolBarRightAction->setText( hbTrId( "txt_fmgr_opt_restore" ) );
    toolBar()->addAction( mToolBarRightAction );
	
    toolBar()->setOrientation( Qt::Horizontal );
}

void FmDriverView::activated( const QString& pathName )
{
    FmViewManager::viewManager()->createFileView( pathName );
}

void FmDriverView::refreshDrive()
{
    FM_LOG( QString( "FmDriverView::refreshDrive start" ) );
    mDriverList->refreshDrive();
    FM_LOG( QString( "FmDriverView::refreshDrive end" ) );
}

void FmDriverView::on_leftAction_triggered()
{
    FmViewManager::viewManager()->createBackupView();
}

void FmDriverView::on_rightAction_triggered()
{
	FmViewManager::viewManager()->createRestoreView();
}

#ifdef FM_CHANGE_ORIENT_ENABLE
void FmDriverView::on_rotateAction_triggered()
{
	if ( mainWindow()->orientation() == Qt::Vertical ) {
		mainWindow()->setOrientation( Qt::Horizontal );
	}
	else {
		mainWindow()->setOrientation( Qt::Vertical );
	}
}
#endif

void FmDriverView::on_backupAction_triggered()
{
	FmViewManager::viewManager()->createBackupView();
}

void FmDriverView::on_restoreAction_triggered()
{
	FmViewManager::viewManager()->createRestoreView();
}

void FmDriverView::on_findAction_triggered()
{
    mDriverList->activeSearchPanel();  
}

void FmDriverView::startSearch( const QString &criteria )
{    
    // search all drives when start find in drive view
    QStringList findTargetPathList;           
    QStringList driveList;
    FmUtils::getDriveList( driveList, true );
    foreach( const QString &drive, driveList ) {
        QString targetPath =  FmUtils::fillPathWithSplash( drive );
        if( FmUtils::isDriveC( targetPath ) ) {
            targetPath =  QString( Folder_C_Data );
        }
        findTargetPathList.append( targetPath );
    }

    if ( !criteria.isEmpty() ) {
        FmViewManager::viewManager()->createFindView( criteria, findTargetPathList );
    }
}

void FmDriverView::removeToolBarAction()
{
    toolBar()->removeAction( mToolBarLeftAction );
    toolBar()->removeAction( mToolBarRightAction );
}


