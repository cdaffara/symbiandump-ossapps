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

#ifndef MEMSPYECOMCATEGORYVIEW_H_
#define MEMSPYECOMCATEGORYVIEW_H_

#include <QAbstractListModel>

#include "memspylistview.h"
#include "enginewrapper.h"


class MemSpyEComCategoryModel : public QAbstractListModel
{
public:
    MemSpyEComCategoryModel(EngineWrapper &engine, QObject *parent = 0);
    
    ~MemSpyEComCategoryModel();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    
    void refresh();
    
private:
    EngineWrapper& mEngine;
    
    QList<MemSpyEComCategory*> mData;
};

class MemSpyEComCategoryView : public MemSpyListView
{
    
    Q_OBJECT
    
public:
    MemSpyEComCategoryView(EngineWrapper &engine, ViewManager &viewManager) : 
            MemSpyListView(engine, viewManager),
            mModel(0)
        {}
    
public slots:
    virtual void refresh();
    
protected:
    void initialize(const QVariantMap& params);
    
protected:
    virtual bool isRefreshable() const { return true; }
    
private slots:
    void itemClicked(const QModelIndex& index);
    
private:
    MemSpyEComCategoryModel* mModel;
};

#endif /* MEMSPYECOMCATEGORYVIEW_H_ */
