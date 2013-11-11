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

#ifndef IRFAVORITESMODEL_H
#define IRFAVORITESMODEL_H

#include <QAbstractItemModel>

class IRQPreset;
class IRQFavoritesDB;
class HbIcon;
class IRLogoProvider;
class IRQIsdsClient;
class QTimer;

class IRFavoritesModel : public QAbstractListModel
{    
    
Q_OBJECT

public:
    explicit IRFavoritesModel(IRQFavoritesDB *aFavoritesDb, QObject *aParent = NULL);
    ~IRFavoritesModel();
    
    IRQPreset* getPreset(int aIndex) const;
    QString    getImgUrl(int aIndex) const;
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    void setLogo(HbIcon *aIcon, int aIndex);
    bool isLogoReady(int aIndex) const;
    bool checkFavoritesUpdate();
    
    void clearAndDestroyLogos();
    bool deleteOneFavorite(int aIndex);  
    bool deleteMultiFavorites(const QModelIndexList &aIndexList);
    
    void updateFavoriteName(int aIndex, const QString &aNewName);
    
    void startDownloadingLogo();
    void stopDownloadingLogo();
    
signals:
    void modelChanged();
    
private slots:
    void downloadNextLogo();
    void logoData(const QByteArray &aLogoData);
    
private:
    void clearPresetList();
    void updateIconIndexArray();
    
private:
    IRQFavoritesDB    *iFavoritesDb;
    QList<IRQPreset*> *iPresetsList;
    HbIcon            *iStationLogo;
    QList<HbIcon*>    iLogos;
    QList<int>         iIconIndexArray;
    IRQIsdsClient *iIsdsClient;
    IRLogoProvider *iLogoProvider;
    QTimer *iTimer;
};

#endif
