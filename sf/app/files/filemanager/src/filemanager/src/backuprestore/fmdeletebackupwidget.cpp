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
 *     The header file of the delete backup widget of file manager
 */
#include "fmdeletebackupwidget.h"
#include "fmrestoresettings.h"
#include "fmrestoreviewitem.h"
#include "fmoperationservice.h"
#include "fmviewmanager.h"
#include "fmbackupconfigloader.h"
#include "fmbkupengine.h"
#include "fmbackuprestorehandler.h"

#include <QGraphicsLinearLayout>
#include <QStringListModel>
#include <QDateTime>
#include <hblistview.h>

FmDeleteBackupWidget::FmDeleteBackupWidget(  HbWidget *parent )
: HbWidget( parent ), mModel( 0 )
{
    init();
}

FmDeleteBackupWidget::~FmDeleteBackupWidget()
{
    mListView->setModel( 0 );
    delete mModel;
}

void FmDeleteBackupWidget::refresh()
{
    mListView->setModel( 0 );
    loadData();
    mListView->setModel( mModel );    
    for (int i = 0; i < mModel->rowCount(); ++i) {
        QModelIndex index = mModel->index(i);
        FmRestoreViewItem* restoreViewItem = static_cast< FmRestoreViewItem* >
                                                 (mListView->itemByIndex(index));
        connect(restoreViewItem, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));     
    }
    emit stateChanged(0);
}

QList<int> FmDeleteBackupWidget::selectionIndexes()
{
    QList<int> selectionList;

    for ( int i=0; i<mModel->rowCount(); i++ ) {
        FmRestoreViewItem *item = static_cast<FmRestoreViewItem*>( 
            mListView->itemByIndex( mModel->index( i ) ) );
        if( item && item->getCheckBoxState() ) {
            selectionList.push_back( i );
        }
    }

    return selectionList;
}

void FmDeleteBackupWidget::loadData()
{
    if( !mModel ) {
        mModel = new QStringListModel();
    }
    mModel->removeRows( 0, mModel->rowCount() );

    mRestoreSettings = FmViewManager::viewManager()->operationService()->backupRestoreHandler()->bkupEngine()->RestoreSettingsL();
    mBackupConfigLoader = FmViewManager::viewManager()->operationService()->backupRestoreHandler()->backupConfigLoader();
    mRestoreSettings->load( mBackupConfigLoader->driversAndOperationList() );

    int index = 0;

    QList< FmRestoreEntry* > retoreEntryList = mRestoreSettings->restoreEntryList();
    mModel->insertRows( 0, retoreEntryList.count() );

    for ( QList< FmRestoreEntry* >::iterator it = retoreEntryList.begin(); 
         it != retoreEntryList.end(); ++it ){
             QString string = ( *it )->text();
             QDateTime datetime = ( *it )->restoreInfo().dateTime();
             QString drive = ( *it )->restoreInfo().drive();
             string.append( '\t' );
             string.append( datetime.toString( "hh:mm ap dd/MM/yyyy") );
             string.append( '\t' );
             string.append( drive );
             QVariant variant( string );             
             mModel->setData( mModel->index( index ), variant, Qt::DisplayRole );
             ++index;
    }

}

void FmDeleteBackupWidget::init()
{    
    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout( this );
    vLayout->setOrientation( Qt::Vertical );

    mListView = new HbListView( this );
    connect( mListView, SIGNAL( activated( const QModelIndex & ) ),
             this, SLOT( on_list_activated( const QModelIndex & ) ) );

    mListView->setSelectionMode( HbAbstractItemView::MultiSelection );

    vLayout->addItem( mListView );

    loadData();
    mListView->setModel( mModel );

    mListView->setItemPrototype( new FmRestoreViewItem( this ) );
   

}

void FmDeleteBackupWidget::on_list_activated( const QModelIndex &index )
{
    FmRestoreViewItem *restoreViewItem = static_cast< FmRestoreViewItem* >
                                         ( mListView->itemByIndex( index ) );

    restoreViewItem->setCheckBoxState();
}

int FmDeleteBackupWidget::backupDataCount()
{
    return mModel->rowCount();
}
