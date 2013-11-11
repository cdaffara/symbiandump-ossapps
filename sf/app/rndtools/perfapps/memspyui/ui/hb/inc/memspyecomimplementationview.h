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

#ifndef MEMSPYECOMIMPLEMENTATIONVIEW_H_
#define MEMSPYECOMIMPLEMENTATIONVIEW_H_

#include <QAbstractListModel>

#include "memspylistview.h"
#include "enginewrapper.h"


class MemSpyEComImplementationModel : public QAbstractListModel
{
public:
    MemSpyEComImplementationModel(EngineWrapper &engine, int interfaceId, QObject *parent = 0);
    
    ~MemSpyEComImplementationModel();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    
    void refresh();
    
private:
    int mInterfaceId;
    
    EngineWrapper& mEngine;
    
    QList<MemSpyEComImplementation*> mData;
};

class MemSpyEComImplementationView : public MemSpyListView
{
    
    Q_OBJECT
    
public:
    MemSpyEComImplementationView(EngineWrapper &engine, ViewManager &viewManager) : 
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
    MemSpyEComImplementationModel* mModel;
    
    int mParentId;
    
    QString mParentName;
};

#endif /* MEMSPYECOMIMPLEMENTATIONVIEW_H_ */
