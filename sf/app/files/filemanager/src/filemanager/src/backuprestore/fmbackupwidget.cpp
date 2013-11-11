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
 *     The header file of the back up widget of file manager
 */

#include "fmbackupwidget.h"
#include "fmbackupsettings.h"
#include "fmdlgutils.h"
#include "fmviewmanager.h"
#include "fmoperationservice.h"
#include "fmbkupengine.h"
#include "fmbackuprestorehandler.h"
#include "fmutils.h"


#include <QGraphicsLinearLayout>
#include <QDir>

#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>


FmBackupWidget::FmBackupWidget( QGraphicsItem *parent )
: HbWidget( parent )
{
    init();
}

FmBackupWidget::~FmBackupWidget()
{
    mDataForm->setModel( 0 );
    delete mModel;
}


void FmBackupWidget::on_list_activated( const QModelIndex &index )
{    
    // change item data when receive activated signal
	HbDataFormModelItem *item = mModel->itemFromIndex(index);
	if( item == mContentsItem ){
        emit changeContents();
    } else if( item == mTargetItem ){
        emit changeTargetDrive();
    } else if( item == mSchedulingItem ){
        emit changeScheduling();
    } else if( item == mWeekdayItem ){
        emit changeWeekday();
    } else if( item == mTimeItem ){
        emit changeTime();
    }
}

void FmBackupWidget::init()
{
    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout( this );
    vLayout->setOrientation( Qt::Vertical );


    mModel = new HbDataFormModel();
    initModel();

    mDataForm = new HbDataForm( this );
    mDataForm->setModel( mModel );
    vLayout->addItem( mDataForm );
        
    connect( mDataForm, SIGNAL( activated( const QModelIndex & ) ),
             this, SLOT( on_list_activated( const QModelIndex & ) ) );

    connect( this, SIGNAL( doModelRefresh() ),
             this, SLOT( refreshModel() ), Qt::QueuedConnection );
    
    connect( this, SIGNAL(changeContents()), this, SLOT(on_changeContents()), Qt::QueuedConnection);
    connect( this, SIGNAL(changeScheduling()), this, SLOT(on_changeScheduling()), Qt::QueuedConnection);
    connect( this, SIGNAL(changeWeekday()), this, SLOT(on_changeWeekday()), Qt::QueuedConnection);
    connect( this, SIGNAL(changeTime()), this, SLOT(on_changeTime()), Qt::QueuedConnection);
    connect( this, SIGNAL(changeTargetDrive()), this, SLOT(on_changeTargetDrive()), Qt::QueuedConnection);
    
    //mBackupSettings = new FmBackupSettings();
    mBackupSettings = FmViewManager::viewManager()->operationService()->backupRestoreHandler()->bkupEngine()->BackupSettingsL();
    mBackupSettings->load();

    emit doModelRefresh();

}

void FmBackupWidget::expandAllGroup()
{
    FM_LOG( QString( "function expandAllGroup start" ) );
    
    mDataForm->setModel( 0 );
    mDataForm->setModel( mModel );

    HbDataFormViewItem *itemScheduling = 
        (HbDataFormViewItem *)(mDataForm->itemByIndex(mModel->indexFromItem(mSchedulingGroup)));
    if( itemScheduling ){
        FM_LOG( QString( "expand itemScheduling start" ) );
        itemScheduling->setExpanded( true );
        FM_LOG( QString( "expand itemScheduling end" ) );
    }

    HbDataFormViewItem *itemGroup = 
        (HbDataFormViewItem *)(mDataForm->itemByIndex(mModel->indexFromItem(mContentsGroup)));
    if( itemGroup ){
        FM_LOG( QString( "expand itemGroup start" ) );
        itemGroup->setExpanded( true );
        FM_LOG( QString( "expand itemGroup end" ) );
    }
    FM_LOG( QString( "function expandAllGroup end" ) );
}

void FmBackupWidget::initModel()
{
    mContentsGroup = mModel->appendDataFormGroup(
        QString( hbTrId( "Contents" ) ), mModel->invisibleRootItem());

    mContentsItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( "" ), mContentsGroup );
    mContentsItem->setContentWidgetData( QString("readOnly"), QString("true") );
    mContentsItem->setContentWidgetData( QString("objectName"), QString("backupContentItem") );

    mTargetItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( "" ), mContentsGroup );
    mTargetItem->setContentWidgetData( QString("readOnly"), QString("true") );
    mTargetItem->setContentWidgetData( QString("objectName"), QString("backupTargetItem") );

    mSchedulingGroup = mModel->appendDataFormGroup(
        QString( hbTrId( "Scheduling" ) ), mModel->invisibleRootItem());

    mSchedulingItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( "" ), mSchedulingGroup );
    mSchedulingItem->setContentWidgetData( QString("readOnly"), QString("true") );
    mSchedulingItem->setContentWidgetData( QString("objectName"), QString("backupSchedulingItem") );

    mWeekdayItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( "" ), mSchedulingGroup );
    mWeekdayItem->setContentWidgetData( QString("readOnly"), QString("true") );
    mWeekdayItem->setContentWidgetData( QString("objectName"), QString("backupWeekdayItem") );

    mTimeItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( "" ), mSchedulingGroup );
    mTimeItem->setContentWidgetData( QString("readOnly"), QString("true") );
    mTimeItem->setContentWidgetData( QString("objectName"), QString("backupTimeItem") );
    
    mLastBackupDateItem = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, QString( ( "" )), mModel->invisibleRootItem() );
    mLastBackupDateItem->setContentWidgetData( QString("readOnly"), QString("true") );   
    mLastBackupDateItem->setContentWidgetData( QString("objectName"), QString("backupLastBackupDateItem") );
}



void FmBackupWidget::refreshModel()
{
    mDataForm->setModel( 0 );

    mBackupSettings->load();
    
    int index = 0;

    index = mSchedulingGroup->indexOf(  mWeekdayItem );
    if( index >=0 ){
        mSchedulingGroup->removeChild( index );
        mWeekdayItem = 0;
    }

    index = mSchedulingGroup->indexOf(  mTimeItem );
    if( index >=0 ){
        mSchedulingGroup->removeChild( index );
        mTimeItem = 0;
    }
    
    index = mContentsGroup->indexOf(  mTargetItem );
    if( index >=0 ){
        mContentsGroup->removeChild( index );
        mTargetItem = 0;
    }
   
    QList< FmBackupEntry* > backupEntryList = mBackupSettings->backupEntryList();

    QList< FmBackupEntry* >::iterator it;

    for (  it = backupEntryList.begin(); it != backupEntryList.end(); ++it ){
    
        int type = ( *it )->type();

        QString title = ( *it )->title();
        QString tips  = ( *it )->tips();
      
        switch ( type )
        {
        case FmBackupEntry::EContents:
            {
            mContentsItem->setLabel(title);
            mContentsItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        case FmBackupEntry::ETarget:
            {
            mTargetItem = mModel->appendDataFormItem(
                HbDataFormModelItem::TextItem, QString( "" ), mContentsGroup );
            mTargetItem->setContentWidgetData( QString("readOnly"), QString("true") );        
            mTargetItem->setLabel(title);
            mTargetItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        case FmBackupEntry::EScheduling:
            {
            mSchedulingItem->setLabel(title);
            mSchedulingItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        case FmBackupEntry::EWeekday:
            {
            mWeekdayItem = mModel->appendDataFormItem(
                HbDataFormModelItem::TextItem, QString( "" ), mSchedulingGroup );
            mWeekdayItem->setContentWidgetData( QString("readOnly"), QString("true") );
            mWeekdayItem->setLabel(title);
            mWeekdayItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        case FmBackupEntry::ETime:
            {
            mTimeItem = mModel->appendDataFormItem(
                HbDataFormModelItem::TextItem, QString( "" ), mSchedulingGroup );
            mTimeItem->setContentWidgetData( QString("readOnly"), QString("true") );
            mTimeItem->setLabel(title);
            mTimeItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        case FmBackupEntry::EBackupdate:
            {            
            mLastBackupDateItem->setContentWidgetData( QString("text"), tips );
            break;
            }
        }
    }

    mDataForm->setModel( mModel );

    expandAllGroup();
}

void FmBackupWidget::on_changeContents()
{
    
    QString title = mContentsItem->label();  
    QStringList queryStringList;
    quint32 contentMask = FmBackupSettings::EFileManagerBackupContentAll;
    while ( contentMask <= FmBackupSettings::EFileManagerBackupContentLast)
    {
        queryStringList.push_back( mBackupSettings->contentToString( contentMask ) );
        contentMask = contentMask << 1;
    }
    quint32 select = mBackupSettings->content();
    if( FmDlgUtils::showMultiSettingQuery( title, queryStringList, select, 0 ) )
    {
        mBackupSettings->setContent( select );
        emit doModelRefresh();
    }
    
} 


void FmBackupWidget::on_changeScheduling()
{
    QString title = mSchedulingItem->label();  
    QStringList queryStringList;

    FmBackupSettings::TFileManagerBackupSchedule schedule = FmBackupSettings::EFileManagerBackupScheduleNever;
    while ( schedule <= FmBackupSettings::EFileManagerBackupScheduleWeekly )
    {
        queryStringList.push_back( mBackupSettings->schedulingToString( schedule ) );
        schedule = FmBackupSettings::TFileManagerBackupSchedule(((int)schedule)+1);
    }
    
    // adjust index offset against monday.
    int selectIndex = mBackupSettings->scheduling() - FmBackupSettings::EFileManagerBackupScheduleNever;
    if( FmDlgUtils::showSingleSettingQuery( title, queryStringList, selectIndex ) )
    {
        selectIndex += FmBackupSettings::EFileManagerBackupScheduleNever;
        mBackupSettings->setScheduling( ( FmBackupSettings::TFileManagerBackupSchedule )selectIndex );
        emit doModelRefresh();
    }
}

void FmBackupWidget::on_changeWeekday()
{
    QString title = mWeekdayItem->label();
    QStringList queryStringList;

    FmBackupSettings::TFileManagerBackupWeekday weekday = FmBackupSettings::EFileManagerBackupWeekdayMonday;
    while ( weekday <= FmBackupSettings::EFileManagerBackupWeekdaySunday )
    {
        queryStringList.push_back( mBackupSettings->weekdayToString( weekday ) );
        weekday = FmBackupSettings::TFileManagerBackupWeekday(((int)weekday)+1);
    }
    
    // adjust index offset against monday.
    int selectIndex = mBackupSettings->weekday() - FmBackupSettings::EFileManagerBackupWeekdayMonday;
    if( FmDlgUtils::showSingleSettingQuery( title, queryStringList, selectIndex ) )
    {
        selectIndex += FmBackupSettings::EFileManagerBackupWeekdayMonday;
        mBackupSettings->setWeekday( (FmBackupSettings::TFileManagerBackupWeekday)selectIndex );
        emit doModelRefresh();
    }
}

void FmBackupWidget::on_changeTime()
{
    QString title = mTimeItem->label();  
    QTime queryTime = mBackupSettings->time();

    if( FmDlgUtils::showTimeSettingQuery( title, queryTime ) )
    {
        mBackupSettings->setTime( queryTime );
        emit doModelRefresh();
    }
}


void FmBackupWidget::on_changeTargetDrive()
{
    QString title = mTargetItem->label();  
    QString drive = FmDlgUtils::showBackupDriveQuery( title );
    if( ( !drive.isEmpty() ) && 
            ( mBackupSettings->targetDrive().compare( drive, Qt::CaseInsensitive ) != 0 ) ) {
        mBackupSettings->setTargetDrive( drive );
        emit doModelRefresh();
    }
}

void FmBackupWidget::updateBackupDate()
{
    mBackupSettings->updateBackupDate();
    emit doModelRefresh();
}
