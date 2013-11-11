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

#ifndef MEMSPYECOMIMPLEMENTATIONDETAILVIEW_H_
#define MEMSPYECOMIMPLEMENTATIONDETAILVIEW_H_

#include <QAbstractListModel>

#include "memspylistview.h"
#include "enginewrapper.h"


class MemSpyEComImplementationDetailModel : public QAbstractListModel
{
public:
    MemSpyEComImplementationDetailModel(MemSpyEComImplementation* implementation, QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    
private:
    QVariantList mData;
};

class MemSpyEComImplementationDetailView : public MemSpyListView
{
    
    Q_OBJECT
    
public:
    MemSpyEComImplementationDetailView(EngineWrapper &engine, ViewManager &viewManager) : 
            MemSpyListView(engine, viewManager),
            mModel(0)
        {}
    
protected:
    void initialize(const QVariantMap& params);
    
private:
    MemSpyEComImplementationDetailModel* mModel;
};

#endif /* MEMSPYECOMIMPLEMENTATIONDETAILVIEW_H_ */
