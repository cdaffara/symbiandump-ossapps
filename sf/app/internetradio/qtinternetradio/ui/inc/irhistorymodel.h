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
#ifndef IRHISTORYMODEL_H_
#define IRHISTORYMODEL_H_

#include <QAbstractItemModel>

class HbIcon;
class IRQPreset;
class channelHistoryWrapper;
class urlInfoWrapper;
class IRLogoProvider;
class IRQIsdsClient;
class QTimer;

class IRHistoryModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit IRHistoryModel(QObject *aParent = NULL);
    ~IRHistoryModel();
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    IRQPreset* getHistoryInfo(int aIndex);
    void clearAllHistory();
    bool checkHistoryUpdate();
    bool deleteHistory(int aIndex);
    
    void startDownloadingLogo();
    void stopDownloadingLogo();    
    
signals:
    void modelChanged();   

private slots:
    void downloadNextLogo();
    void logoData(const QByteArray &aLogoData);
    
private:
    void refreshModel();
    void clearModel();
    
    void clearAndDestroyLogos();
    QString getImageUrl(int aRow) const;
    void setLogo(HbIcon *aIcon, int aIndex);
		
    bool isLogoReady(int aIndex) const;
    void updateIconIndexArray();
    
private:
    QList<IRQPreset *>      iHistoryList;
    channelHistoryWrapper  *iHistoryEngine;
    urlInfoWrapper         *iUrlInfoWrapper;
    QList<HbIcon*>          iLogos;
    HbIcon                 *iStationLogo;
    
    QList<int>              iIconIndexArray;
    IRQIsdsClient          *iIsdsClient;
    IRLogoProvider         *iLogoProvider;
    QTimer                 *iTimer;    
};

#endif /* IRHISTORYMODEL_H_ */
