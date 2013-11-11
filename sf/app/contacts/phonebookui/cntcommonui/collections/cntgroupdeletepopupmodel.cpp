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

#include "cntgroupdeletepopupmodel.h"
#include "cntthumbnailmanager.h"
#include "cntfavourite.h"
#include <cntabstractengine.h>
#include "cntglobal.h"

#include <qtcontacts.h>
#include <hbglobal.h>
#include <hbicon.h>

/*!
Constructor
*/
CntGroupDeletePopupModel::CntGroupDeletePopupModel(CntAbstractEngine *aEngine, QObject *parent)
    : QAbstractListModel(parent),
    mFavoriteGroupId(-1)
{
    d = new CntGroupPopupListData();
    d->mContactManager = &aEngine->contactManager( SYMBIAN_BACKEND );
    
    mFavoriteGroupId = CntFavourite::createFavouriteGroup( d->mContactManager );
    
    d->mThumbnailManager = &aEngine->thumbnailManager();
    connect(d->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(onIconReady(QPixmap, void *, int, int)));
}

/*!
Destructor
*/
CntGroupDeletePopupModel::~CntGroupDeletePopupModel()
{
    foreach (int id, d->mIconRequests.keys())
    {
        d->mThumbnailManager->cancelRequest(id);
    }

    d->mIconRequests.clear();
}

/*!
Initialize groups
*/
void CntGroupDeletePopupModel::initializeGroupsList()
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    
    QContactSortOrder sortOrderGroupName;
    sortOrderGroupName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldCustomLabel);
    sortOrderGroupName.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> groupsOrder;
    groupsOrder.append(sortOrderGroupName);

    QList<QContactLocalId> groupContactIds = d->mContactManager->contactIds(groupFilter, groupsOrder);

    if (!groupContactIds.isEmpty())
    {
        for(int i = 0;i < groupContactIds.count();i++)
        {
            QVariantList dataList;
            
            // group name
            QStringList displayList;
            
            QContact contact = d->mContactManager->contact(groupContactIds.at(i));
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            if(groupContactIds.at(i) != mFavoriteGroupId )
            {
                if (groupName.isNull())
                    {
                    QString unnamed(hbTrId("txt_phob_dblist_unnamed"));
                    displayList.append(unnamed);
                    }
                else
                    {
                    displayList.append(groupName);
                    }    
                
                dataList.append(displayList);
                
                // contact Id for identification
                dataList.append(groupContactIds.at(i));
                
                // Default if no image for group
                dataList.append(HbIcon("qtg_large_custom"));
                
                QList<QContactAvatar> details = contact.details<QContactAvatar>();
                for (int k = 0;k < details.count();k++)
                {
                   if (details.at(k).imageUrl().isValid())
                   {
                       int id = d->mThumbnailManager->getThumbnail(ThumbnailManager::ThumbnailSmall, details.at(k).imageUrl().toString());
                       d->mIconRequests.insert(id, rowCount());
                       break;
                   }
                }

                d->mDataList.append(dataList);
            }
        }
    }
}

int CntGroupDeletePopupModel::favoriteGroupId()
{
    return mFavoriteGroupId;
}

/*!
Returns the number of rows in the model
*/
int CntGroupDeletePopupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->mDataList.count();
}
/*!
Returns data for given index with a given role
*/
QVariant CntGroupDeletePopupModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    
    if (row < 0)
    {
        return QVariant();
    }
    
    QVariantList values = d->mDataList.at(row);
    
    if (role == Qt::DisplayRole)
    {
        QStringList list = values[0].toStringList();        
        return QVariant(list);
    }
    
    else if (role == Qt::DecorationRole)
    {
        return values[2];
    }
    
    else if (role == Qt::UserRole)
       {
           int contactId = values[1].toInt();
           return QVariant(contactId);
       }
    return QVariant();
}


QContact CntGroupDeletePopupModel::contact(QModelIndex &index) const
{
    int row = index.row();
    QVariantList values = d->mDataList.at(row);
    int groupId = values[1].toInt();
    return d->mContactManager->contact( groupId );
}

void CntGroupDeletePopupModel::onIconReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    
    if (error == 0)
    {
        int row = d->mIconRequests.take(id);
        
        QVariantList values = d->mDataList.at(row);
        QVariantList newValues;
        
        newValues.append(values[0]);
        newValues.append(values[1]);
        newValues.append(HbIcon(pixmap));
        
        d->mDataList.removeAt(row);
        
        d->mDataList.insert(row, newValues);
        
        emit dataChanged(index(row, 0), index(row, 0));
    }
}
