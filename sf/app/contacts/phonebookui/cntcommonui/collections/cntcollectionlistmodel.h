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

#ifndef CNTCOLLECTIONLISTMODEL_H
#define CNTCOLLECTIONLISTMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <qmobilityglobal.h>
#include <cntviewparams.h>
#include <hbicon.h>
#include <xqsettingsmanager.h>

class CntCollectionListModelWorker;
class CntCollectionListModelData;
class CntCollectionItem;

class CntExtensionManager;
class CntExtensionGroupCallback;
class CntAbstractEngine;

QTM_USE_NAMESPACE

class CntCollectionListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    CntCollectionListModel(CntAbstractEngine* aEngine, QObject *parent = 0);
    ~CntCollectionListModel();
    
public:
    // from QAbstractItemModel
    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    
public:
    // removing and adding groups
    bool removeGroup(int localId);
    void addGroup(int localId);
    
    // extension (plugin) group handling
    bool isExtensionGroup(const QModelIndex &index);
    CntViewParameters extensionGroupActivated(int row);
    void extensionGroupLongPressed(int row, const QPointF& coords, CntExtensionGroupCallback* interface);
    
    QModelIndex indexOfGroup(int localId);
    
private:
    void doConstruct();
    void initializeStaticGroups();
    void initializeExtensions();
    void initializeUserGroups();
    
    QVariant displayRoleData(const CntCollectionItem& item) const;
    QVariant decorationRoleData(const CntCollectionItem& item) const;
    
    bool validateRowIndex(const int index) const;
    
private slots:
    void informationUpdated(int id, const QString& secondRowText, const QList<QContactLocalId>& members);
    void onIconReady(const QPixmap& pixmap, void *data, int id, int error);
    void extensionGroupsReady();
    
    void handleAdded(const QList<QContactLocalId>& contactIds);
    void handleRemoved(const QList<QContactLocalId>& contactIds);
    void handleChanged(const QList<QContactLocalId>& contactIds);
    void handleAddedRelationship(const QList<QContactLocalId>& contactIds);
    void handleRemovedRelationship(const QList<QContactLocalId>& contactIds);
    
signals:
    void groupCountChanged();
    
private:
    QSharedDataPointer<CntCollectionListModelData>  d;
    CntCollectionListModelWorker                   *mThread;
    XQSettingsManager                               mSettings;
    
    friend class TestCntCollectionListModel;
};

#endif // CNTCOLLECTIONLISTMODEL_H
