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

#ifndef CNTPHONENUMBERVIEWITEM_H_
#define CNTPHONENUMBERVIEWITEM_H_

#include "cntdetailviewitem.h"
#include <hbinputeditorinterface.h>

class HbWidget;
class HbAbstractViewItem;
class CntCommonDetailViewItem;
class CntDetailModelItem;

class CntPhoneNumberViewItem : public CntDetailViewItem
    {
    Q_OBJECT
    
public:
    CntPhoneNumberViewItem( QGraphicsItem* aParent = 0 );
    ~CntPhoneNumberViewItem();
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
public slots:
    void indexChanged( int aIndex );    // HbComboBox index changed
    void textChanged( QString aText );  // HbLineEdit text changed
   
private:
    void constructSubtypeModel( QString aSubType, QStringList aContext );
    void constructPhoneNumber(CntDetailModelItem* aItem, QString aSubType, QStringList aContext );
    void constructOnlineAccount( CntDetailModelItem* aItem, QString aSubType, QStringList aContext );
  
    bool isOnlineAccount( QString aSubtype );
    
private:
    CntCommonDetailViewItem*   mItem; // Ownership transfered
    
    friend class T_NumberEditorTest;
    };
#endif /* CNTPHONENUMBERVIEWITEM_H_ */
