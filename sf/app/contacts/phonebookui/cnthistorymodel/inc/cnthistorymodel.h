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
#ifndef CNTHISTORYMODEL_H
#define CNTHISTORYMODEL_H

#include <QAbstractListModel>
#include <QSharedData>
#include <qmobilityglobal.h>
#include <qcontactid.h>
#include "cntglobal.h"

#include "cnthistorymodelconsts.h"
#include "cnthistorymodelglobal.h"

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntHistoryModelPrivate;

/*!
 * CntHistoryModel is a model used to fetch conversation history
 * (SMS & call logs) from databases. It will 
 * cache content from the databases to be displayed on the screen
 * by the conversation view.
 * 
 * Note that that this is a prototype implementation and does
 * not yet support more advanced features.
 */
class CNTHISTORYMODEL_EXPORT CntHistoryModel : public QAbstractListModel
{    
    Q_OBJECT
    
public:
    CntHistoryModel(QContactLocalId contactId,
                    QContactManager* manager,
                    QObject *parent = 0);
    ~CntHistoryModel();
    
public: // from QAbstractTableModel/QAbstractItemModel
    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

public:
    void clearHistory();
    void markAllAsSeen();
    void sortAndRefresh(Qt::SortOrder order = Qt::AscendingOrder);

private:
    void doBeginInsertRows(const QModelIndex &parent, int first, int last);
    void doEndInsertRows();
    void doBeginRemoveRows(const QModelIndex &parent, int first, int last);
    void doEndRemoveRows();
    void doBeginResetModel();
    void doEndResetModel();
    void doDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    
private:
	CntHistoryModelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntHistoryModel)
    Q_DISABLE_COPY(CntHistoryModel)
    
    // Testing related friend definitions
    friend class TestCntHistoryModel;
};

#endif
