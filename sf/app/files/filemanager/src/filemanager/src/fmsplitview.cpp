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
*     The source file of the split view of file manager
*
*/
#include "fmsplitview.h"
#include "fmfilebrowsewidget.h"
#include "fmdriverlistwidget.h"
#include <QGraphicsLinearLayout>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>

FmSplitView::FmSplitView() : FmViewBase( ESplitView )
{
    initMenu();
    initMainWidget();

    QMetaObject::connectSlotsByName( this );
}

FmSplitView::~FmSplitView()
{
}

void FmSplitView::on_rotateAction_triggered()
{
	if ( mainWindow()->orientation() == Qt::Vertical ) {
		mainWindow()->setOrientation( Qt::Horizontal );
	}
	else {
		mainWindow()->setOrientation( Qt::Vertical );
	}
}

void FmSplitView::on_normalViewAction_triggered()
{
	mainWindow()->setCurrentView( mainWindow()->views().first() );
}

void FmSplitView::initMenu()
{
    HbAction *action = 0;
#ifdef FM_CHANGE_ORIENT_ENABLE
	action = new HbAction( this );
	action->setObjectName( "rotateAction" );
	action->setText( hbTrId( "Change orientation" ) );
	menu()->addAction( action );
#endif

	action = new HbAction( this );
	action->setObjectName( "normalViewAction" );
	action->setText( hbTrId( "Switch to normal view" ) );
	menu()->addAction( action );
}

void FmSplitView::initMainWidget()
{
	QGraphicsLinearLayout *hLayout = new QGraphicsLinearLayout( this );
	hLayout->setOrientation( Qt::Horizontal );

    mDriverWidget = new FmDriverListWidget( this );
    mDriverWidget->setMaximumWidth( 200.0 );
    hLayout->addItem( mDriverWidget );
     

    mFileWidget = new FmFileBrowseWidget( this, FmFileBrowseWidget::TreeStyle );
    hLayout->addItem( mFileWidget );

    connect( mDriverWidget, SIGNAL( activated( const QString & ) ),
             mFileWidget, SLOT( setRootPath( const QString & ) ) );
}
