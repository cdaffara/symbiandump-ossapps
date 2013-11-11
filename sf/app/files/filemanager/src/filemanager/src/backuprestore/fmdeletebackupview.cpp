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
 *     The source file of the delete backup view of file manager
 */

#include "fmdeletebackupview.h"
#include "fmdeletebackupwidget.h"
#include "fmviewmanager.h"
#include "fmoperationbase.h"
#include "fmdlgutils.h"

#include <QApplication>

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbmainwindow.h>

FmDeleteBackupView::FmDeleteBackupView() : FmViewBase( EDeleteBackupView )
{
	setTitle( hbTrId( "Delete Backup" ) );

	initMainWidget();
	initToolBar();
	initMenu();    
	mOperationService = FmViewManager::viewManager()->operationService();
}

FmDeleteBackupView::~FmDeleteBackupView()
{
    removeToolBarAction();
}

void FmDeleteBackupView::initMenu()
{
#ifdef FM_CHANGE_ORIENT_ENABLE
    HbAction *action = 0;
	action = new HbAction( this );
    action->setObjectName( "rotateAction" );
    action->setText( hbTrId( "Change orientation" ) );
    menu()->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT( on_rotateAction_triggered() ), Qt::QueuedConnection );
#endif

	mDeleteAction = new HbAction( this );
    mDeleteAction->setObjectName( "deleteAction" );
    mDeleteAction->setText( hbTrId( "Delete backups" ) );
    menu()->addAction( mDeleteAction );
    mDeleteAction->setEnabled(false);
    connect( mDeleteAction, SIGNAL(triggered()), this, SLOT( on_deleteAction_triggered() ), Qt::QueuedConnection );
}

void FmDeleteBackupView::initMainWidget()
{
	mDeleteBackupWidget = new FmDeleteBackupWidget( this );
    mDeleteBackupWidget->setObjectName( "deleteBackupWidget" );
    setWidget( mDeleteBackupWidget );
    connect(mDeleteBackupWidget, SIGNAL(stateChanged(int)), this, SLOT(on_deleteBackupWidget_stateChanged(int)));

}

void FmDeleteBackupView::on_deleteBackupWidget_stateChanged(int state)
{
    Q_UNUSED(state);
    QList<int> items = mDeleteBackupWidget->selectionIndexes();
    if (items.count() > 0) {
        mLeftAction->setEnabled(true);
        mDeleteAction->setEnabled(true);
    } else {
        mLeftAction->setEnabled(false);
        mDeleteAction->setEnabled(false);
    }
}

void FmDeleteBackupView::initToolBar()
{
	mLeftAction = new HbAction( this );
    mLeftAction->setObjectName( "leftAction" );
    mLeftAction->setText( hbTrId( "delete" ) );
    toolBar()->addAction( mLeftAction );
    mLeftAction->setEnabled(false);
    toolBar()->setOrientation( Qt::Horizontal );

    connect( mLeftAction, SIGNAL(triggered()), this, SLOT( on_leftAction_triggered() ), Qt::QueuedConnection );
}

void FmDeleteBackupView::on_leftAction_triggered()
{
    on_deleteAction_triggered();
}

#ifdef FM_CHANGE_ORIENT_ENABLE
void FmDeleteBackupView::on_rotateAction_triggered()
{
	if ( mainWindow()->orientation() == Qt::Vertical ) {
		mainWindow()->setOrientation( Qt::Horizontal );
	}
	else {
		mainWindow()->setOrientation( Qt::Vertical );
	}
}
#endif

void FmDeleteBackupView::on_deleteAction_triggered()
{
    QList<int > items = mDeleteBackupWidget->selectionIndexes();
    if( items.count() <= 0 || !FmDlgUtils::question( hbTrId("Confirm Deletion?" ) ) ){
        return;
    }

    quint64 selection( 0 );
    for ( int i( 0 ); i < items.count(); ++i )
       {
       selection |= ( ( quint64 ) 1 ) << ( items[ i ] );
       }
                   
    mOperationService->syncDeleteBackup( selection );
    mDeleteBackupWidget->refresh();
    on_deleteBackupWidget_stateChanged(0);
}

void FmDeleteBackupView::removeToolBarAction()
{
    toolBar()->removeAction( mLeftAction );
}

void FmDeleteBackupView::refreshDeleteBackupView()
{
    mDeleteBackupWidget->refresh();
}
