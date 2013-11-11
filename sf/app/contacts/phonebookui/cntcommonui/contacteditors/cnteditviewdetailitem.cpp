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
* Description:
*
*/

#include "cnteditviewdetailitem.h"
#include "cntabstractviewmanager.h"
#include "cnteditviewlistmodel.h"

#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbeffect.h>
#include <hbinstantfeedback.h>

#include <QGraphicsSceneMouseEvent>

CntEditViewDetailItem::CntEditViewDetailItem( QContactDetail aDetail, QString aField, int aEditorViewId ) :
QObject(),
mDetail( aDetail ),
mEditorViewId( aEditorViewId )
{
    mField << aField;
}

CntEditViewDetailItem::CntEditViewDetailItem( QContactDetail aDetail, QStringList aField, int aEditorViewId ) :
QObject(),
mDetail( aDetail ),
mField( aField ),
mEditorViewId( aEditorViewId )
{   
}

CntEditViewDetailItem::~CntEditViewDetailItem()
{
}

void CntEditViewDetailItem::addText( QString aText )
{
    mTextList << aText;
}

void CntEditViewDetailItem::addIcon( HbIcon aIcon )
{
    mIconList << aIcon;
}

QVariant CntEditViewDetailItem::data( int aRole ) const
{
    switch ( aRole )
    {
    case Qt::DisplayRole: // text
        return mTextList;
        
    case Qt::DecorationRole: // icon
        return mIconList;
        
    case ERoleItemType:
        return QVariant( ETypeDetailItem );
        
    case ERoleEditorViewId: // detail editor view id 
        return QVariant( mEditorViewId );
        
    case ERoleContactDetailFields:
        return QVariant( mField );
     
    case ERoleContactDetailDef:
        return QVariant( mDetail.definitionName() );
        
    case ERoleContactDetail:
    {
        QVariant detail;
        detail.setValue<QContactDetail>(mDetail);
        return detail;
    }
        
    default:
        return QVariant();
    }
}

void CntEditViewDetailItem::activated(CntEditViewItemCallback* interface)
{
    Q_UNUSED( interface );
}

void CntEditViewDetailItem::longPressed(const QPointF &coords, CntEditViewItemCallback* interface)
{
    Q_UNUSED( interface );
    Q_UNUSED( coords );
}

