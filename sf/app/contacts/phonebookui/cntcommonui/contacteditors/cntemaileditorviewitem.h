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

#ifndef CNTEMAILEDITORVIEWITEM_H_
#define CNTEMAILEDITORVIEWITEM_H_

#include "cntdetailviewitem.h"

class HbWidget;
class HbAbstractViewItem;
class CntCommonDetailViewItem;

class CntEmailEditorViewItem : public CntDetailViewItem
    {
    Q_OBJECT
    
public:
    CntEmailEditorViewItem( QGraphicsItem* aParent = 0 );
    ~CntEmailEditorViewItem();
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
public slots:
    void indexChanged( int aIndex );    // HbComboBox index changed
    void textChanged( QString aText );  // HbLineEdit text changed
    
private:
    void constructSubTypeModel( QStringList aContext );
    
private:
    CntCommonDetailViewItem*  mItem;
    
    friend class T_EmailEditorTest;
    };
#endif /* CNTEMAILEDITORVIEWITEM_H_ */
