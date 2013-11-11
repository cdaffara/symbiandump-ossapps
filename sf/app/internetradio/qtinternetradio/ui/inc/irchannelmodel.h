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

#ifndef IRCHANNELMODEL_H
#define IRCHANNELMODEL_H

#include <QAbstractItemModel>

class HbIcon;
class IRQChannelItem;
class IRSearchResultDB;
class IRLogoProvider;
class IRQIsdsClient;
class QTimer;

class IrChannelModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IrChannelModel(QObject *aParent = NULL);
    ~IrChannelModel();

    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    void initWithCache();    
    void save2Cache();
    //not take ownership
    IRQChannelItem * getChannelItemByIndex(int aIndex); 
    void cleanupDatabase();
    void startDownloadingLogo();
    void stopDownloadingLogo();

public slots:
    void updateData(QList<IRQChannelItem*> *aPushItemsList, bool bInit=false);

signals:
    void dataAvailable();

private slots:
    void downloadNextLogo();
    void logoData(const QByteArray &aLogoData);

private:
    void clearAndDestroyLogos();
    void clearAndDestroyItems();
    void updateIconIndexArray();
    void setLogo(HbIcon *aIcon, int aIndex);
    
private:
    QList<IRQChannelItem*> *iChannelList;
    QMap<int, HbIcon*>      iLogos;
    HbIcon                 *iStationLogo;
    IRSearchResultDB       *iDB;
    QList<int>              iIconIndexArray;
    IRQIsdsClient          *iIsdsClient;
    IRLogoProvider         *iLogoProvider;
    QTimer                 *iTimer;
};

#endif
