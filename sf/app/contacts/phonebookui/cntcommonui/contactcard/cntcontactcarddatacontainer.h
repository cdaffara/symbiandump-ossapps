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

#ifndef CNTCOMMLAUNCHERLISTMODEL_H_
#define CNTCOMMLAUNCHERLISTMODEL_H_

#include <QSharedData>
#include <qtcontacts.h>

#include "cntstringmapper.h"

class CntContactCardDataItem;
class MapTileService;
class CntExtensionManager;

class CntContactCardDataContainer: public QObject
{
    Q_OBJECT    

public:
    CntContactCardDataContainer(
        MapTileService* maptile,
        CntExtensionManager& aExtensionManager,
        Qt::Orientations orientation = Qt::Vertical,
        QObject *parent = 0);
    virtual ~CntContactCardDataContainer();

public:
    CntContactCardDataItem* dataItem(int index) const;
    int itemCount() const;
    void setContactData(QContact* contact, bool aMyCard = false);
    void clearContactData();
    
#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif  
    void initializeActionsData(bool myCard);
    void initialiseExtensionActions(bool aMyCard);
    void initializeGroupData();
    void initializeDetailsData();
    QList<QContactDetail> actionDetails(const QString &actionName, const QContact &contact);
    bool supportsDetail(const QString &actionName, const QContactDetail &contactDetail);
    void addSeparator(int index);
    void sortDataItems();
    int getPosition(const QString& aId, const QString& aContext, bool dynamicAction = false);
    
#ifdef PBK_UNIT_TEST
public:
#else
private:    
#endif       
    QContact*                       mContact;
    QContactAction*                 mCallAction;
    QContactAction*                 mMessageAction;
    QList<CntContactCardDataItem*>  mDataItemList;
    int                             mSeparatorIndex;
    CntStringMapper                 mStringMapper;
    bool                            mLocationFeatureEnabled;
    MapTileService*                 mMaptileInterface;
    Qt::Orientations                mOrientation;
    CntExtensionManager*            mExtensionManager;
};

#endif /* CNTCOMMLAUNCHERLISTMODEL_H_ */
