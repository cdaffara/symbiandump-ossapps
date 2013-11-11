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

#ifndef IRCHANNELDATAPROVIDER_H
#define IRCHANNELDATAPROVIDER_H

#include <hgwidgets/hgdataprovidermodel.h>

class IRQChannelItem;
class IRQPreset;
class IRQIsdsClient;

const int KCacheSize = 50;
const int KCacheThreshold = 20;

class IRChannelDataProvider : public HgDataProviderModel
{
Q_OBJECT
    
public:
    IRChannelDataProvider(QObject *aParent);
    ~IRChannelDataProvider();
    
    void activate();
    void deactivate();
    IRQChannelItem* getChannelItemByIndex(int aIndex);
    
signals:
    void dataAvailable();

protected:
    //from HgDataProviderModel
    void doReleaseData(QList<int> aList, bool aSilent);
    void doRequestData(QList<int> aList, bool aSilent);  
    QVariant defaultIcon() const;  
    void doResetModel();
    
private slots:
    void updateData(QList<IRQChannelItem*> *aPushItemsList);
    void presetLogoDownload(IRQPreset* aPreset);
    void presetLogoDownloadError();
    void downloadNextLogo();
    
private:
    void startDownloading(int aIndex);
    void clearAndDestroyItems();
    void clearAndDestroyLogos();
    
private:
    QList<IRQChannelItem*> *iChannelList;
    QVariant iDefaultLogo;
    QMap<int, HbIcon*>     iLogos;  //save the downloaded logos
    QList<int> iRequestedItems; //save the indexes of requested items
    IRQPreset *iLogoPreset;
    QTimer *iLogoDownloadTimer;
    IRQIsdsClient *iIsdsClient;
    
    bool iActivated;
};

#endif
