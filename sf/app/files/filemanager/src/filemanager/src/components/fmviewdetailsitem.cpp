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
 *     The source file of the view details list item of file manager
 */

#include "fmviewdetailsitem.h"
#include "fmviewmanager.h"

#include <QGraphicsLinearLayout>

#include <hblabel.h>

FmViewDetailsItem::FmViewDetailsItem( QGraphicsItem *parent )
    : HbListViewItem( parent ),
      mDetailsContentLabel( 0 ),
      mDetailsLabel( 0 ),
      mLayout( 0 )

{
}

FmViewDetailsItem::~FmViewDetailsItem()
{
}

HbAbstractViewItem *FmViewDetailsItem::createItem()
{
    return new FmViewDetailsItem( *this );
}

bool FmViewDetailsItem::canSetModelIndex( const QModelIndex &index ) const
{
    Q_UNUSED( index );
    return true;
}

void FmViewDetailsItem::polish(HbStyleParameters& params)
{
    Q_UNUSED(params);
}

void FmViewDetailsItem::updateChildItems()
{
    if( !mLayout ) {
        init();
    }
    QString string = modelIndex().data( Qt::DisplayRole ).toString();   
    
    mDetailsContentLabel->setPlainText( string );
    
    string = modelIndex().data( Qt::UserRole ).toString();  
    
    mDetailsLabel->setPlainText( string );

}

void FmViewDetailsItem::init()
{
    mLayout = new QGraphicsLinearLayout();
    
    if( FmViewManager::viewManager()->orientation() == Qt::Vertical ){
        mLayout->setOrientation( Qt::Vertical );
    }
    else{
        mLayout->setOrientation( Qt::Horizontal );   
    }


    mDetailsContentLabel = new HbLabel("");
    mDetailsContentLabel->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mDetailsContentLabel->setObjectName( "detaisContentLabel" );
    mLayout->addItem( mDetailsContentLabel );
    mLayout->setAlignment( mDetailsContentLabel, Qt::AlignLeft );

    mDetailsLabel = new HbLabel("");
    mDetailsLabel->setFontSpec( HbFontSpec( HbFontSpec::Secondary ) );
    mDetailsLabel->setObjectName( "detailsLabel" );
    mLayout->addItem( mDetailsLabel );
    mLayout->setAlignment( mDetailsLabel, Qt::AlignLeft );

    setLayout( mLayout );
}
