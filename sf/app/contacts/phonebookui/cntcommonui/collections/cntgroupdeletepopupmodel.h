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

#ifndef CNTGROUPDELETEPOPUPMODEL_H
#define CNTGROUPDELETEPOPUPMODEL_H

#include <QSharedData>
#include <QAbstractListModel>
#include <qmobilityglobal.h>

class CntThumbnailManager;
class CntAbstractEngine;
QTM_BEGIN_NAMESPACE
class QContactManager;
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntGroupPopupListData : public QSharedData
{
public:
    CntGroupPopupListData() { }
    ~CntGroupPopupListData() { }

public:
    QContactManager    *mContactManager;
    CntThumbnailManager   *mThumbnailManager;
    QList<QVariantList> mDataList;
    QMap<int,int>       mIconRequests;
};

class CntGroupDeletePopupModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    CntGroupDeletePopupModel(CntAbstractEngine *aEngine, QObject *parent = 0);
    ~CntGroupDeletePopupModel();
    
public:
    void initializeGroupsList();
    int favoriteGroupId();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QContact contact(QModelIndex &index) const ;
    
private slots:
    void onIconReady(const QPixmap& pixmap, void *data, int id, int error);
    
private:
    QSharedDataPointer<CntGroupPopupListData>  d;
    int                                        mFavoriteGroupId;
};

#endif // CNTGROUPDELETEPOPUPMODEL_H

// EOF
