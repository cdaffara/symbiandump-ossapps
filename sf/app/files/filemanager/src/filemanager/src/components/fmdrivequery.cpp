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
 *     The source file of backup drive query of file manager
 */

#include "fmdrivequery.h"
#include "fmviewmanager.h"
#include "fmbackuprestorehandler.h"

#include <QGraphicsLinearLayout>
#include "hblistview.h"

FmDriveQuery::FmDriveQuery( QGraphicsItem *parent ) : FmDialog( parent )
{
    setObjectName( "driveQueryDialog" );
	mContentWidget = new QGraphicsWidget();
	mContentWidget->setObjectName( "driveQueryContentWidget" );
	setContentWidget(mContentWidget);

	QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mListView = new HbListView();
    mListView->setObjectName( "driveQueryListView" );
	vLayout->addItem( mListView );

	mListView->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );

	this->setTimeout( NoTimeout );
	mContentWidget->setLayout( vLayout );

	connect( mListView, SIGNAL( activated  ( const QModelIndex& ) ), this, SLOT( activated( const QModelIndex& ) ) );
    this->setDismissPolicy( NoDismiss );

    
    mDriveModel = new FmDriveModel( this, 
            FmDriveModel::FillWithVolume | FmDriveModel::FillWithDefaultVolume, this );
    mListView->setModel( mDriveModel );
    
    // connect refreshModel signal of viewmanager  for drive insert/remove event.
    connect( FmViewManager::viewManager(), SIGNAL( refreshModel( QString ) ), 
            this, SLOT( refreshModel( QString ) ) );
}

FmDriveQuery::~FmDriveQuery(void)
{
    delete mDriveModel;
    mDriveModel = 0;
}

void FmDriveQuery::getDriveList( QStringList &driveList )
{
    // provide drive list which is got from backup engine wrapper.
    FmViewManager::viewManager()->operationService()->backupRestoreHandler()->getBackupDriveList( driveList );
}

void FmDriveQuery::activated( const QModelIndex &index )
{
    QString driveName( mDriveModel->driveName( index ) );
    
    // if drive is not available, ignore activate signal.
    FmDriverInfo driveInfo = FmUtils::queryDriverInfo( driveName );
    if( !( driveInfo.driveState() & FmDriverInfo::EDriveAvailable ) ) {
        return;
    }
    
    mSelectedDrive = driveName;
    close();
}

QString FmDriveQuery::selectedDrive() const
{
    return mSelectedDrive;
}

void FmDriveQuery::refreshModel( QString path )
{
    if( !path.isEmpty() ) {
        // ignore non-empty refresh signal as it means change of folder/file, not drive.
        return;
    }
    mDriveModel->refresh();
}
