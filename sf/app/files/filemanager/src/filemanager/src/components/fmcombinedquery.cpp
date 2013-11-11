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
 *     The source file of combined query of file manager
 */

#include "fmcombinedquery.h"

#include <QGraphicsLinearLayout>

#include <hblistwidget.h>
#include <hbaction.h>
#include <hblistwidgetitem.h>
#include <hbabstractviewitem.h>

FmCombinedQuery::FmCombinedQuery( QGraphicsItem *parent ) : FmDialog( parent ), 
    mSelectMode ( MultiSelectMode ), mDominantIndex ( -1 )
{
    setObjectName( "combinedQueryDialog" );
	mContentWidget = new QGraphicsWidget();
	mContentWidget->setObjectName( "CombinedQueryContentWidget" );
	setContentWidget(mContentWidget);

	QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mListWidget = new HbListWidget();
	vLayout->addItem( mListWidget );

    mListWidget->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mListWidget->setObjectName( "CombinedQueryListWidget" );
	this->setTimeout( NoTimeout );
	mContentWidget->setLayout( vLayout );

    setSelectMode( mSelectMode );
	connect( mListWidget, SIGNAL( activated  ( const QModelIndex& ) ), this, SLOT( activated( const QModelIndex& ) ) );
    connect( this, SIGNAL( doCheckForPrimaryAction  () ), this, SLOT( checkForPrimaryAction() ), Qt::QueuedConnection );
    this->setDismissPolicy( NoDismiss );

    doCheckForPrimaryAction();
}

void FmCombinedQuery::setSelectMode( SelectMode mode )
{ 
    mSelectMode = mode;
    switch ( mSelectMode )
    {
    case SingleSelectMode:
        mListWidget->setSelectionMode( HbAbstractItemView::SingleSelection );
        break;

    case MultiSelectMode:
        mListWidget->setSelectionMode( HbAbstractItemView::MultiSelection );
        break;

    case NoSelectMode:
    default:
        mListWidget->setSelectionMode( HbAbstractItemView::NoSelection );
        break;
    }
}

FmCombinedQuery::~FmCombinedQuery(void)
{
}

void FmCombinedQuery::setMarkedListItem( int row, bool mark )
{
    QModelIndex index = mListWidget->model()->index( row, 0 );
    if ( mark ){
		mListWidget->selectionModel()->select( index, QItemSelectionModel::Select );
    }
    else{
		mListWidget->selectionModel()->select( index, QItemSelectionModel::Deselect );
    }
}

void FmCombinedQuery::setDominantIndex( int index )
{
	mDominantIndex = index;
}

void FmCombinedQuery::setListItems( const QMap< int, QString > &list )
{
    mListWidget->clear();
 
    for( QMap< int, QString >::const_iterator it = list.begin(); 
        it != list.end(); ++it ){
        HbListWidgetItem* modelItem = new HbListWidgetItem();
        if( !modelItem ){
            return;
        }

        modelItem->setText( it.value() );
        modelItem->setData( it.key() );

        mListWidget->addItem( modelItem );
    }
}

void FmCombinedQuery::activated( const QModelIndex &index )
{
	int selectedRow = index.row();
	if( selectedRow == mDominantIndex ){
		QModelIndexList modelIndexList = mListWidget->selectionModel()->selectedIndexes();
		for( QModelIndexList::const_iterator it = modelIndexList.begin(); it != modelIndexList.end(); ++it ){
			if( ( *it ) == index  ){
				mListWidget->clearSelection();
				setMarkedListItem( selectedRow );
			}
		}
	}
	else if( mDominantIndex >= 0 ){
		setMarkedListItem( mDominantIndex, false );
	}

    emit doCheckForPrimaryAction();
}

QStringList FmCombinedQuery::selectedValues() const
{
    QStringList selList;
    QItemSelectionModel *selectionModel = 0;
    QList<int> selIndexes; 

    if( !mListWidget ){
        return selList;
    }

    selectionModel = mListWidget->selectionModel();

    if( selectionModel ){
        QModelIndexList indexes = selectionModel->selectedIndexes();
        int count = indexes.count();
        QModelIndex index;
        for( int i = 0; i < count; ++i ){
            index = indexes[i];
            HbListWidgetItem *item = mListWidget->item( index.row() );
            if (item) {
                QString text = item->text();
                selList.append( text );
            }
        }
    }

    return selList;
}


QList<int> FmCombinedQuery::selectedKeys() const
{
    QList<int> selList;
    QItemSelectionModel *selectionModel = 0;
    QList<int> selIndexes; 

    if( !mListWidget ){
        return selList;
    }

    selectionModel = mListWidget->selectionModel();

    if( selectionModel ){
        QModelIndexList indexes = selectionModel->selectedIndexes();
        int count = indexes.count();
        QModelIndex index;
        for( int i = 0; i < count; ++i ){
            index = indexes[i];
            HbListWidgetItem *item = mListWidget->item( index.row() );
            if ( item ) {
                int key = item->data().toInt();
                selList.append( key );
            }
        }
    }

    return selList;
}

void FmCombinedQuery::checkForPrimaryAction()
{
    QItemSelectionModel *selectionModel = mListWidget->selectionModel();

    if( selectionModel->selectedIndexes().count() <= 0 ) {
        primaryAction()->setDisabled( true );
    } else {
        primaryAction()->setDisabled( false );
    }
}
