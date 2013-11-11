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

#ifndef CNTURLEDITORVIEWITEM_H_
#define CNTURLEDITORVIEWITEM_H_

#include "cntdetailviewitem.h"
#include <hbabstractviewitem.h>
#include <hbwidget.h>
#include <hbcombobox.h>
#include <hblineedit.h>

class CntCommonDetailViewItem;

class CntUrlEditorViewItem : public CntDetailViewItem
    {
    Q_OBJECT
    
public:
    CntUrlEditorViewItem( QGraphicsItem* aParent = 0 );
    ~CntUrlEditorViewItem();
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
public slots:
    void indexChanged( int aIndex );    // HbComboBox index changed
    void textChanged( QString aText );  // HbLineEdit text changed
    
private:
    void constructSubTypeModel( QStringList aContext );
    
private:
    CntCommonDetailViewItem*    mItem;
    
    friend class T_UrlEditorTest;
    };

#endif /* CNTURLEDITORVIEWITEM_H_ */
