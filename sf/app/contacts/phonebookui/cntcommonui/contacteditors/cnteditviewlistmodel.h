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

#ifndef CNTEDITVIEWLISTMODEL_H_
#define CNTEDITVIEWLISTMODEL_H_

#include <QAbstractListModel>
#include <qtcontacts.h>
#include <hbnamespace.h>

#include <cnteditviewitem.h>
#include "cntstringmapper.h"
#include "cntdetailpopup.h"

class CntExtensionManager;
class CntEditViewItemSupplier;
class CntEditViewDelegateItem;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntEditViewItemBuilder;

enum KLookupKey {
    EPhonenumber,
    EEmailAddress,
    EAddressTemplate,
    EPluginItem,
    EUrl,
    ESeparator,
    EAddressDetail,
    ECompany,
    EDate,
    ERingingTone,
    ENote,
    EFamily,
    ESynchronization
};

class CntEditViewSeparator : public QObject, public CntEditViewItem
{
    Q_OBJECT
public:
    CntEditViewSeparator();
    ~CntEditViewSeparator();
    
    QVariant data(int role) const;
    void activated(CntEditViewItemCallback* interface);
    void longPressed(const QPointF &coords, CntEditViewItemCallback* interface);  
};

class CntEditViewListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CntEditViewListModel( QContact& aContact, CntExtensionManager& aExtensions ); // ownership not transferred
    ~CntEditViewListModel();
    
public: // From QAbstractListModel
    int rowCount( const QModelIndex& aParent = QModelIndex() ) const;
    QVariant data( const QModelIndex& aIndex, int aRole = Qt::DisplayRole ) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    
public:
    CntEditViewItem* itemAt( const QModelIndex& aIndex ) const;
    void removeItem( CntEditViewItem* aItem, const QModelIndex& aIndex );
    bool isEmptyItem( CntEditViewItem* aItem );
    void refreshExtensionItems();
    void allInUseFields( CntViewIdList& aList );
    QModelIndex itemIndex( QContactDetail aDetail ) const;
    void updateRingtone();
        
private:
    void refresh();
    void loadPluginItems( CntEditViewItemSupplier* aSupplier );
    
    void insertItem( KLookupKey aKey, QList<CntEditViewItem*> aList );
    void insertDetailItem( KLookupKey aKey, QList<CntEditViewItem*> aList );
    void removeItem( KLookupKey aKey );
    void insertSeparator();
    
private:
    QList<CntEditViewItem*> mItemList;
    CntExtensionManager& mManager;
    CntEditViewItemBuilder* mBuilder;
    CntEditViewItem* mSeparator;
    QContact& mContact;
    
    QMap<KLookupKey, int> mLookupTable;
    QMap<QString, KLookupKey> mLookupMap;
    
    friend class TestCntEditView;
    };
#endif /* CNTEDITVIEWLISTMODEL_H_ */
