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
 * Description: caitemmodel.h
 *
 */

#ifndef CASTANDARDITEMMODEL_H
#define CASTANDARDITEMMODEL_H

// System includes
#include <QAbstractItemModel>
#include <QAbstractItemView>

#include "caquery.h"
#include "caentry.h"
// Forward declarations
class CaEntry;

// mock Class declaration
class CaItemModel: public QAbstractItemModel
{

Q_OBJECT

public:
    mutable CaQuery mCaItemModelQuery;
    mutable QVariant mDataResult;
    mutable QModelIndex mDataIndex;
    mutable QModelIndex mIndexResult;
    mutable QModelIndex mRootResult;
    mutable QModelIndex mParentResult;
    mutable int mDataRole;
    mutable int mRowCountResult;
    mutable int mColumnCountResult;
    mutable SortAttribute mSetSortAttribute;
    mutable Qt::SortOrder mSetSortOrder;
    mutable bool mSecondLineVisibilityResult;
    mutable bool mIsAutoUpdateResult;
    mutable QSizeF mGetIconSizeResult;
    mutable QSharedPointer<CaEntry> mEntryResult;
    // Data types
    enum CaItemRole
    {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        TypeRole,
        FlagsRole,
        TextRole,//contains title represented as QString,
        FullTextRole //'title description' convinient for operations such as search
    };

    // Function declarations
    CaItemModel(const CaQuery &query, QObject *parent = 0): 
        QAbstractItemModel(parent), 
        mCaItemModelQuery(query),
        mDataRole(0),
        mSecondLineVisibilityResult(false),
        mIsAutoUpdateResult(false),
        mEntryResult(new CaEntry)
    {
    }
    
    ~CaItemModel(){}

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return mColumnCountResult;
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return mRowCountResult;
    }
    virtual QModelIndex index(int row, int column = 0,
        const QModelIndex &parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }
    
    virtual QModelIndex parent(const QModelIndex &index) const
    {
        Q_UNUSED(index);
        return mParentResult;
    }
    
    QModelIndex root() const
    {
        return mRootResult;
    }
    
    virtual QVariant data(const QModelIndex &index, int role =
        Qt::DisplayRole) const
    {
        mDataIndex = index;
        mDataRole = role;
        return mDataResult;
    }

    void setAutoUpdate(bool autoUpdate)
    {
        mIsAutoUpdateResult = autoUpdate;
    }

    void setSecondLineVisibility(bool secondLineVisible)
    {
        mSecondLineVisibilityResult = secondLineVisible;
    }

    bool secondLineVisibility() const
    {
        return mSecondLineVisibilityResult;
    }
    
    bool isAutoUpdate() const
    {
        return mIsAutoUpdateResult;
    }
    
    void setSort(SortAttribute sortAttribute, 
        Qt::SortOrder sortOrder = Qt::AscendingOrder)
    {
        mSetSortAttribute = sortAttribute;
        mSetSortOrder = sortOrder;
    }
    
    void setIconSize(const QSizeF &size) 
    {
        mGetIconSizeResult = size;
    }
    
    QSizeF getIconSize() const
    {
        return mGetIconSizeResult;
    }
    
    void updateModel(){}
    
    void setParentId(int parentId)
    {
        Q_UNUSED(parentId);
    }
    
    void setFlagsOn(const EntryFlags &onFlags) 
    {
        Q_UNUSED(onFlags);
    }
    
    void setFlagsOff(const EntryFlags &offFlags) 
    {
        Q_UNUSED(offFlags);
    }
    
    QSharedPointer<CaEntry> entry(const QModelIndex &index) const 
    {
        Q_UNUSED(index);
        return mEntryResult;
    }
    
    QList<int> getUninstallingEntriesIds(int componentId)
    {
        Q_UNUSED(componentId);
        QList<int> ids;
        ids.append(mEntryResult->id());
        ids.append(4);
        return ids;
    }
    
    void updateProgress(int id, int valueOfProgress)
    {
        if (id == mEntryResult->id()) {
            mEntryResult->setAttribute(QString("uninstall_progress"), 
                    QString().setNum(valueOfProgress));
        }
    }
};

#endif // CASTANDARDITEMMODEL_H
