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
 *     The source file of the back up view of file manager
 */

#include "fmbackupview.h"
#include "fmbackupwidget.h"
#include "fmviewmanager.h"
#include "fmoperationbase.h"
#include "fmdlgutils.h"

#include <QApplication>

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbmainwindow.h>

FmBackupView::FmBackupView() : FmViewBase( EBackupView )
{
    setTitle( hbTrId( "Backup" ) );

    initToolBar();
    initMainWidget();
    initMenu();
    mOperationService = FmViewManager::viewManager()->operationService();

}

FmBackupView::~FmBackupView()
{
    removeToolBarAction();
}

void FmBackupView::initMenu()
{
    HbAction *action = 0;
    
#ifdef FM_CHANGE_ORIENT_ENABLE
    action = new HbAction( this );
    action->setObjectName( "rotateAction" );
    action->setText( hbTrId( "Change orientation" ) );
    menu()->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT( on_rotateAction_triggered() ), Qt::QueuedConnection );
#endif

    action = new HbAction( this );
    action->setObjectName( "backupAction" );
    action->setText( hbTrId( "Start backup" ) );
    menu()->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT( on_backupAction_triggered() ), Qt::QueuedConnection );

    action = new HbAction( this );
    action->setObjectName( "deleteBackupAction" );
    action->setText( hbTrId( "Delete backup" ) );
    menu()->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT( on_deleteBackupAction_triggered() ), Qt::QueuedConnection );
}

void FmBackupView::initMainWidget()
{
    mMainWidget = new FmBackupWidget( this );
    setWidget( mMainWidget );

}

void FmBackupView::initToolBar()
{
    mToolBarAction = new HbAction( this );
    mToolBarAction->setObjectName( "leftAction" );
    mToolBarAction->setText( hbTrId("Start backup") );
    toolBar()->addAction( mToolBarAction );
    connect( mToolBarAction, SIGNAL(triggered()), this, SLOT( on_leftAction_triggered() ), Qt::QueuedConnection );
    
    toolBar()->setOrientation( Qt::Horizontal );
}

void FmBackupView::on_leftAction_triggered()
{
    int ret = mOperationService->asyncBackup();
    FM_LOG( "FmBackupView_asyncBackup: ret= " + QString::number(ret) );
    switch( ret )
    {
    case FmErrNone:
        break;
    case FmErrWrongParam:
        FmDlgUtils::warning( QString( hbTrId("Operation canceled with wrong param!") ) );
        break;
    case FmErrAlreadyStarted:
        FmDlgUtils::warning( QString( hbTrId("Operation canceled because already started!") ) );
        break;
    case FmErrPathNotFound:
        FmDlgUtils::warning( QString( hbTrId("Operation canceled because can not find target path or drive is not available!") ) );
        break;
    case FmErrAlreadyExists:
        FmDlgUtils::warning( QString( hbTrId("backup canceled") ) );
        break;
    default:
        FmDlgUtils::warning( QString( hbTrId("backup failed") ) );
        break;
    }
}
#ifdef FM_CHANGE_ORIENT_ENABLE
void FmBackupView::on_rotateAction_triggered()
{
	if ( mainWindow()->orientation() == Qt::Vertical ) {
		mainWindow()->setOrientation( Qt::Horizontal );
	}
	else {
		mainWindow()->setOrientation( Qt::Vertical );
	}
}
#endif //FM_CHANGE_ORIENT_ENABLE

void FmBackupView::on_backupAction_triggered()
{
    on_leftAction_triggered();
}

void FmBackupView::on_deleteBackupAction_triggered()
{
    FmViewManager::viewManager()->createDeleteBackupView();
}

void FmBackupView::removeToolBarAction()
{
    toolBar()->removeAction( mToolBarAction );
}

void FmBackupView::refreshBackupDate()
{
    mMainWidget->updateBackupDate();
}

void FmBackupView::refreshModel()
{
    mMainWidget->refreshModel();
}
