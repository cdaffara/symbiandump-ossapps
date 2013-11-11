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
*     The source file of the file find widget of file manager
*
*/

#include "fmfindwidget.h"
#include "fmfindresultmodel.h"
#include "fmviewmanager.h"
#include "fmoperationservice.h"

#include <QGraphicsLinearLayout>
#include <QFileInfo>

#include <hblistview.h>
#include <hbsearchpanel.h>
#include <hblabel.h>
#include <hbwidget.h>
#include <hbstackedwidget.h>

FmFindWidget::FmFindWidget( QGraphicsItem *parent )
    : HbWidget( parent )
{
    FM_LOG("FmFindWidget::FmFindWidget()");
    init();
}

FmFindWidget::~FmFindWidget()
{
    FM_LOG("FmFindWidget::~FmFindWidget() START");
    delete mModel;
    FM_LOG("FmFindWidget::~FmFindWidget() END");
}

void FmFindWidget::itemActivated(const QModelIndex &index)
{   
    FmOperationService *operationService = FmViewManager::viewManager()->operationService();
    if( !operationService ) {
        return;
    }

    QFileInfo fileInfo = mModel->fileInfo( index );
    if ( fileInfo.isFile() ) {
        operationService->syncLaunchFileOpen( fileInfo.filePath() );
    }else if( fileInfo.isDir() ) {
        emit activated( fileInfo.filePath() );
    }
}

void FmFindWidget::find( const QString &keyword, const QStringList &pathList )
{
    mPathList = pathList;
    // find will auto-start after receive criteria change event.
    mSearchPanel->setCriteria( keyword );
}

void FmFindWidget::stopFind()
{
    mModel->stopFind();
}

void FmFindWidget::on_resultModel_modelCountChanged( int count )
{
    if( count > 0 ) {
        mContentWidget->setCurrentWidget( mListView );
    } else {
        mContentWidget->setCurrentWidget( mEmptyTipWidget );
    }
}

void FmFindWidget::on_resultModel_findStarted()
{
    emit setEmptyMenu( true );
}

void FmFindWidget::on_resultModel_findFinished()
{
    if ( mModel->rowCount() > 0 ) {
        emit setEmptyMenu( false );
    } else {
        emit setEmptyMenu( true );
    }
}

void FmFindWidget::init()
{
    mLayout = new QGraphicsLinearLayout( this );
    mLayout->setOrientation( Qt::Vertical );
 
    mContentWidget = new HbStackedWidget( this );
    mLayout->addItem( mContentWidget );
    initSearchPanel();
    mLayout->addItem( mSearchPanel );
    setLayout( mLayout );

    mModel = new FmFindResultModel( this );
    mModel->setObjectName( "resultModel" );

    connect( mModel, SIGNAL( modelCountChanged( int )),
        this, SLOT( on_resultModel_modelCountChanged( int )), Qt::QueuedConnection );

    connect( mModel, SIGNAL( findStarteded()),
        this, SLOT( on_resultModel_findStarted()) );

    connect( mModel, SIGNAL( findFinished()),
        this, SLOT( on_resultModel_findFinished()) );
    
    mListView = new HbListView( this );
    mListView->setModel( mModel );

    mEmptyTipWidget = new HbWidget( this );
    QGraphicsLinearLayout *emptyTipLayout = new QGraphicsLinearLayout( mEmptyTipWidget );
    HbLabel *emptyTipLable = new HbLabel( hbTrId( "No found files or folders" ), mEmptyTipWidget );
    emptyTipLayout->addItem( emptyTipLable );
    emptyTipLayout->setAlignment( emptyTipLable, Qt::AlignCenter );
    emptyTipLable->setAlignment( Qt::AlignCenter );
 
    mContentWidget->addWidget( mListView );
    mContentWidget->addWidget( mEmptyTipWidget );
    
    mContentWidget->setCurrentWidget( mEmptyTipWidget );
    connect( mListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
}

void FmFindWidget::initSearchPanel()
{
    mSearchPanel = new HbSearchPanel( this );
    mSearchPanel->setObjectName( "searchPanel" );
    mSearchPanel->setSearchOptionsEnabled( false );
    mSearchPanel->setProgressive( true );
    
    connect( mSearchPanel, SIGNAL( criteriaChanged( const QString & ) ),
        this, SLOT( on_searchPanel_criteriaChanged( const QString & ) ), Qt::QueuedConnection  );
    
    connect( mSearchPanel, SIGNAL( exitClicked() ),
        this, SLOT( on_searchPanel_exitClicked() ), Qt::QueuedConnection );
}

void FmFindWidget::sortFiles( FmFindResultModel::SortFlag sortFlag )
{
    mModel->sort( sortFlag );
}

void FmFindWidget::on_searchPanel_criteriaChanged( const QString &criteria )
{
    QRegExp regExp;
    if( !criteria.isEmpty() ) {
        regExp.setPattern( '*' + criteria + '*' );
    }
    // if criteria is empty, then keep regExp empty. find thread will return empty result.
    
    regExp.setPatternSyntax( QRegExp::Wildcard );
    regExp.setCaseSensitivity( Qt::CaseInsensitive );

    mModel->find( regExp, mPathList );
}

void FmFindWidget::on_searchPanel_exitClicked()
{
    FmViewManager::viewManager()->popViewAndShow();
}



