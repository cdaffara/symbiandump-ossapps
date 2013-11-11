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
#include <QtAlgorithms>
#include <HbIcon>
#include <QTimer>

#include "irqfavoritesdb.h"
#include "irqisdsdatastructure.h"
#include "irfavoritesmodel.h"
#include "irqisdsclient.h"
#include "irlogoprovider.h"

IRFavoritesModel::IRFavoritesModel(IRQFavoritesDB *aFavoritesDb, QObject *aParent) 
                                  : QAbstractListModel(aParent), iFavoritesDb(aFavoritesDb),
                                    iPresetsList(NULL)
{
    if (aFavoritesDb)
    {
        iPresetsList = aFavoritesDb->getPresets();
    }
    
    iIsdsClient = IRQIsdsClient::openInstance();
    iLogoProvider = new IRLogoProvider(iIsdsClient);
    
    iTimer = new QTimer;
    iTimer->setInterval(10);
    connect(iTimer, SIGNAL(timeout()), this, SLOT(downloadNextLogo()));
    
    iStationLogo = new HbIcon("qtg_large_internet_radio");
}

IRFavoritesModel::~IRFavoritesModel()
{
    delete iStationLogo;
    clearPresetList();
    clearAndDestroyLogos();
    
    delete iLogoProvider;
    if (iIsdsClient)
    {
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    delete iTimer;
}

IRQPreset* IRFavoritesModel::getPreset(int aIndex) const
{
    if (iPresetsList && aIndex >= 0 && aIndex < iPresetsList->count())
    {
        return iPresetsList->at(aIndex);
    }
    
    return NULL;
}

QString  IRFavoritesModel::getImgUrl(int aIndex) const
{
    if ( aIndex<0 || aIndex >=iPresetsList->count() )
    {
        return "";
    }
    return iPresetsList->at(aIndex)->imgUrl;     
}

int IRFavoritesModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    if (iPresetsList)
    {
        int count = iPresetsList->count();
        return count;
    }
    
    return 0;
}

void IRFavoritesModel::setLogo(HbIcon *aIcon, int aIndex)
{
    int elementCountNeedToAdd = aIndex + 1 - iLogos.size();
    while (elementCountNeedToAdd > 0)
    {
        iLogos.append(NULL);
        elementCountNeedToAdd--;
    }
    iLogos[aIndex] = aIcon;
    emit dataChanged(index(aIndex), index(aIndex));
}

bool IRFavoritesModel::isLogoReady(int aIndex) const
{
    int logoListCount = iLogos.count();
    if (aIndex >= 0 
        && aIndex < logoListCount)
    {
        return iLogos[aIndex] != NULL;
    }
    else
    {
        return false;
    }
}

QVariant IRFavoritesModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aIndex.row() >= iPresetsList->count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;
        int row = aIndex.row();

        QString primaryText = iPresetsList->at(row)->nickName;
        list.append(primaryText);
        QString secondaryText = iPresetsList->at(row)->shortDesc;
        
        // fix bug #9888,if left descriptions as blank,
        // only one line displays in favorites view
        if (0 == secondaryText.length())
        {
            secondaryText = " ";
        }
        
        list.append(secondaryText);
        return list;
    }
	else if ( aRole == Qt::DecorationRole)
	{
	    QVariantList list;
		int row = aIndex.row();
		if(row < iLogos.size())
		{
            const HbIcon *icon = iLogos[row];
            if (icon)
            {
                list.append(*icon);
            }
            else
            {
                list.append(*iStationLogo);
            }            
		}
		else
		{
            list.append(*iStationLogo);
		}		
        return list;
    }
    return QVariant();
}

bool IRFavoritesModel::checkFavoritesUpdate()
{
    clearPresetList();
    clearAndDestroyLogos();
    iPresetsList = iFavoritesDb->getPresets();
    updateIconIndexArray();
    emit modelChanged();
    return true;
}

void IRFavoritesModel::clearAndDestroyLogos()
{
    int size = iLogos.size(); 
    for (int i = 0; i < size; i ++)
    {
        delete iLogos[i];
        iLogos[i] = NULL;
    }
    iLogos.clear();
}

void IRFavoritesModel::clearPresetList()
{
    if (NULL == iPresetsList)
    {
        return;
    }
    
    while (!iPresetsList->isEmpty())
    {
        delete iPresetsList->takeLast();
    }
    delete iPresetsList;
    iPresetsList = NULL;
}

bool IRFavoritesModel::deleteOneFavorite(int aIndex)
{
    if ( aIndex < 0 || aIndex >= iPresetsList->size())
    {
        return false;
    }
    
    if (!iIconIndexArray.empty())
    {
        iIsdsClient->isdsLogoDownCancelTransaction();
        iTimer->stop();
    }  
    
    IRQPreset *preset = iPresetsList->at(aIndex);        
    int ret = iFavoritesDb->deletePreset(*preset);
    if ( 0 != ret )
    {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    delete preset;
    iPresetsList->removeAt(aIndex);
    
    if (aIndex<iLogos.size())
    {
        delete iLogos[aIndex];
        iLogos[aIndex] = NULL;
    }
    iLogos.removeAt(aIndex);
    endRemoveRows();
    
    updateIconIndexArray();
    
    if (!iIconIndexArray.empty())
    {
        iTimer->start();
    }   
    
    return true;    
}

bool IRFavoritesModel::deleteMultiFavorites(const QModelIndexList &aIndexList)
{
    if (aIndexList.empty())
    {
        return true;
    }

    if (!iIconIndexArray.empty())
    {
        iIsdsClient->isdsLogoDownCancelTransaction();
        iTimer->stop();
    }
    
    int index = 0;
    bool retVal = true;
    QList<int> indexToBeDelete;
    
    // delete from DB
    for (int i = 0; i < aIndexList.count(); i++)
    {
        index = aIndexList.at(i).row();
        
        if (index < 0 || index >= iPresetsList->size())
        {
            continue;
        }
        
        IRQPreset *preset = iPresetsList->at(index);
        if (0 != iFavoritesDb->deletePreset(*preset) )
        {
            retVal = false;
            continue;
        }
        indexToBeDelete.append(index);
    }
    
    qSort(indexToBeDelete);
    
    
    // delete from model
    for (int i = indexToBeDelete.count() - 1; i >= 0; i--)
    { 
        index = indexToBeDelete.at(i);
        
        beginRemoveRows(QModelIndex(), index, index);
        IRQPreset *preset = iPresetsList->at(index);
        delete preset;
        iPresetsList->removeAt(index);
        if (index<iLogos.size())
        {
            delete iLogos[index];
            iLogos[index] = NULL;
        }
        iLogos.removeAt(index);
        endRemoveRows();         
    }

    updateIconIndexArray();
    
    if (!iIconIndexArray.empty())
    {
        iTimer->start();
    }
    
    return retVal;    
}

void IRFavoritesModel::updateFavoriteName(int aIndex, const QString &aNewName)
{
    if (aIndex >= 0 && aIndex < iPresetsList->count())
    {
        iPresetsList->at(aIndex)->nickName = aNewName;
        emit dataChanged(index(aIndex), index(aIndex));
    }
}

void IRFavoritesModel::startDownloadingLogo()
{
    iLogoProvider->activate(this, SLOT(logoData(const QByteArray&)));
    iTimer->start();
}

void IRFavoritesModel::stopDownloadingLogo()
{
    iIsdsClient->isdsLogoDownCancelTransaction();
    iTimer->stop();
    iIconIndexArray.clear();
    iLogoProvider->deactivate();
}

void IRFavoritesModel::downloadNextLogo()
{
    iTimer->stop();
    int leftCount = iIconIndexArray.count();

    if (0 != leftCount)
    {
         iLogoProvider->getLogo(iPresetsList->at(iIconIndexArray[0]));
    }
}

void IRFavoritesModel::logoData(const QByteArray &aLogoData)
{
    if (aLogoData.size() > 0)
    {
        QPixmap tempMap;
        bool ret = tempMap.loadFromData((const unsigned char*)aLogoData.constData(), aLogoData.size());
        if( ret )
        {
            QIcon convertIcon(tempMap);
            HbIcon *hbIcon = new HbIcon(convertIcon);
            int index = iIconIndexArray[0];
            setLogo(hbIcon, index);  
        }
    }
    
    iIconIndexArray.removeAt(0);
    int leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iTimer->start();  
    }
}

void IRFavoritesModel::updateIconIndexArray()
{
    iIconIndexArray.clear();
    
    for (int i = 0; i < rowCount(); ++i)
    {
        if (getImgUrl(i) != "" && !isLogoReady(i))
        {
            iIconIndexArray.append(i);
        }
    } 
}
