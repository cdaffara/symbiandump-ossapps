/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: HgWidgetDataModel implementation
*
*/

#include <QBrush>
#include <HbIcon>
#include <hgwidgets/hgwidgets.h>
#include <QPainter>

#include "locationpickertypes.h"
#include "hgwidgetdatamodel.h"
#include "locationpickerproxymodel.h"

const int MAPSTROKE(3);
// ----------------------------------------------------------------------------
// HgWidgetDataModel::HgWidgetDataModel()
// ----------------------------------------------------------------------------
HgWidgetDataModel::HgWidgetDataModel( LocationPickerProxyModel *aProxyModel, QObject *aParent )
    : QAbstractListModel(aParent),
      mImageType(ETypeHbIcon),
      mDefaultImage(KDummyImage),
      mUseLowResImages(false),
      mProxyModel(NULL)
{
    mProxyModel  = aProxyModel;   
    
}

// ----------------------------------------------------------------------------
// HgWidgetDataModel::resetModel()
// ----------------------------------------------------------------------------
void HgWidgetDataModel::resetModel( LocationPickerProxyModel *aProxyModel )
{   
    //reset and update
    mProxyModel = aProxyModel;
    reset();
    emit beginResetModel();
    emit endResetModel();
}


// ----------------------------------------------------------------------------
// HgWidgetDataModel::~HgWidgetDataModel()
// ----------------------------------------------------------------------------
HgWidgetDataModel::~HgWidgetDataModel()
{

}

// ----------------------------------------------------------------------------
// HgWidgetDataModel::rowCount()
// ----------------------------------------------------------------------------
int HgWidgetDataModel::rowCount( const QModelIndex &aParent ) const
{
    Q_UNUSED(aParent);

    return mProxyModel->rowCount(QModelIndex());
   
}

// ----------------------------------------------------------------------------
// HgWidgetDataModel::data()
// ----------------------------------------------------------------------------
QVariant HgWidgetDataModel::data(const QModelIndex &aIndex, int aRole) const
{   
    int row= aIndex.row();
    int col = aIndex.column();
    //get proxy model index
    QModelIndex proxyModelIndex = mProxyModel->index(row,col);
    QVariant returnValue = QVariant();
    if ( !aIndex.isValid() )
    {
        return returnValue;
    }

    if( row >= mProxyModel->rowCount(QModelIndex()) )
    {
        return returnValue;
    }

    switch ( aRole )
    {
        case HgWidget::HgVisibilityRole:
        {
            returnValue = true;
        }
        break;
        case Qt::DisplayRole:
        {
            QStringList displayText;
            QStringList adressDetail = mProxyModel->data(proxyModelIndex,Qt::DisplayRole).
            	toStringList();
            QString displayString;
            if(!adressDetail[0].isEmpty())
            {
                displayString = adressDetail[0]+KSpace+KTitleSeparator+KSpace+
                adressDetail[1]+KSpace+KSeparator+adressDetail[2];
            }
            else if (!adressDetail[1].isEmpty())
            {
                displayString = adressDetail[1]+KSpace+KSeparator+adressDetail[2];
            }
            else
            {
                displayString = adressDetail[2];
            }
            QString text("");
            displayText <<displayString<<text;
            returnValue = displayText;
            break;
        }
        case Qt::DecorationRole:
        {
            //get icon name from data model
            QString iconName =  mProxyModel->data(proxyModelIndex,Qt::UserRole+1).toString();
            if (iconName.isEmpty()) 
            {
                returnValue = mDefaultImage;
            }
            else 
            {   
                QString adressType =  mProxyModel->data(proxyModelIndex,Qt::UserRole+2).toString();
                QPixmap mapPixmap(iconName);
                int mapWidth = mapPixmap.width();
                int mapHeight = mapPixmap.height();
                QBrush brush(Qt::black,Qt::SolidPattern);
                QPainter painter;
                painter.begin(&mapPixmap);
                HbIcon adressTypeIcon(adressType);
                //draw the adressType Icon over mapTile Icon
                adressTypeIcon.paint(&painter,QRectF((mapPixmap.width()-adressTypeIcon.width()),
                	0,adressTypeIcon.width(),adressTypeIcon.height()));
                painter.fillRect(QRect(0,0,mapWidth,MAPSTROKE),brush);
                painter.fillRect(QRect(0,mapHeight-MAPSTROKE,mapWidth,
                	(mapHeight-MAPSTROKE)),brush);
                painter.fillRect(QRect(0,0,MAPSTROKE,mapPixmap.height()),brush);
                painter.fillRect(QRect((mapWidth-MAPSTROKE),0,mapWidth,mapHeight),brush);
                painter.end();
                QIcon landscape( mapPixmap );
                HbIcon landscapeIcon(landscape);
                returnValue = landscapeIcon;
            }
            break;
        }
        default:
            break;
        }

    return returnValue;
}


// ----------------------------------------------------------------------------
// HgWidgetDataModel::setImageDataType()
// ----------------------------------------------------------------------------
void HgWidgetDataModel::setImageDataType(TImageType type)
{
    mImageType = type;
}



