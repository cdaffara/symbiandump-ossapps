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

#ifndef CNTDETAILVIEWITEM_H_
#define CNTDETAILVIEWITEM_H_

#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>

class CntDetailViewItem : public HbDataFormViewItem
{
    Q_OBJECT
public:
    inline CntDetailViewItem( QGraphicsItem* aParent );
    inline virtual ~CntDetailViewItem();
    
public:
    inline virtual bool canSetModelIndex( const QModelIndex& aIndex ) const; 
};

inline CntDetailViewItem::CntDetailViewItem( QGraphicsItem* aParent ) : HbDataFormViewItem( aParent )
{   
}

inline CntDetailViewItem::~CntDetailViewItem()
{
}

    
inline bool CntDetailViewItem::canSetModelIndex( const QModelIndex& aIndex ) const
{
    int itemType = aIndex.data(HbDataFormModelItem::ItemTypeRole).toInt();
    return itemType == HbDataFormModelItem::CustomItemBase;
}
#endif /* CNTDETAILVIEWITEM_H_ */
