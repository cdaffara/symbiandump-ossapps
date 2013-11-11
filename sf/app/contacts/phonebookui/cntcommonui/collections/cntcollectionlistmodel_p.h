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

#ifndef CNTCOLLECTIONLISTMODELPRIVATE_H
#define CNTCOLLECTIONLISTMODELPRIVATE_H

#include <QSharedData>
#include <QSharedPointer>
#include <QMap>

#include <qmobilityglobal.h>

#include <hbicon.h>

class CntExtensionManager;
class CntUiGroupSupplier;
class CntThumbnailManager;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    Helper class for CntCollectionListModel
*/
class CntCollectionItem
{   
public:   
    CntCollectionItem() : memberCount(0), isPlugin(false), fetched(false) {}
    ~CntCollectionItem() {}
    
    inline const CntCollectionItem& operator=(const CntCollectionItem& other)
    {
        groupName = other.groupName;
        secondLineText = other.secondLineText;
        icon = other.icon;
        secondaryIcon = other.secondaryIcon;
        memberCount = other.memberCount;
        members = other.members;
        isPlugin = other.isPlugin;
        id = other.id;
        return *this;
    }
    
    inline bool operator==(const CntCollectionItem& other) const
    {
         return id == other.id;
    }
    
public:
    QString     groupName;
    QString     secondLineText;
    HbIcon      icon;
    HbIcon      secondaryIcon;
    int         memberCount;
    QList<QContactLocalId>  members;
    bool        isPlugin;
    bool        fetched;
    int         id;
};

typedef QSharedPointer<CntCollectionItem> CollectionItemPointer;

/*!
    Helper class for CntCollectionListModel
*/
class CntCollectionListModelData : public QSharedData
{
public:
    CntCollectionListModelData(CntExtensionManager &extensionManager)
        : QSharedData(),
          mContactManager(NULL),
          mExtensionManager(extensionManager),
          mFavoriteGroupId(-1),
          mThumbnailManager(NULL)
          {}
    ~CntCollectionListModelData() { }
    
public:
    QContactManager                    *mContactManager;
    CntExtensionManager                &mExtensionManager;
    int                                 mFavoriteGroupId;
    
    QList<CollectionItemPointer>        mList;
    QMap<int, CntUiGroupSupplier*>      mExtensions;
    
    QMap<int,int>                       mIconRequests;
    
    CntThumbnailManager                 *mThumbnailManager;
};

#endif // CNTCOLLECTIONLISTMODELPRIVATE_H
