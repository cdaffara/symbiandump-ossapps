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

#ifndef CNTADDRESSVIEWITEM_H_
#define CNTADDRESSVIEWITEM_H_

#include <hbdataformviewitem.h>
#include <hbpushbutton.h>

class HbWidget;
class HbAbstractViewItem;
class XQApplicationManager;
class XQAiwRequest;

class CntAddressViewItem : public /*CntDetailViewItem*/ HbDataFormViewItem
    {
    Q_OBJECT
    
public:
    CntAddressViewItem( QGraphicsItem* aParent = 0 );
    ~CntAddressViewItem();

private slots:
    void launchLocationPicker();
    void handleLocationChange(const QVariant& aValue );
    
public:
    HbAbstractViewItem* createItem();
    HbWidget* createCustomWidget();
    
    bool canSetModelIndex( const QModelIndex &index ) const;
    
private:
    XQApplicationManager *mAppManager;
    XQAiwRequest* mRequest;
    bool mRequestPending;
    friend class T_AddressEditorTest;
};

#endif /* CNTADDRESSVIEWITEM_H_ */
