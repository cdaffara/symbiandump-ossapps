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

#ifndef MEMSPYWINDOWGROUPSDETAILVIEW_H_
#define MEMSPYWINDOWGROUPSDETAILVIEW_H_

#include <QAbstractListModel>

#include "memspylistview.h"
#include "enginewrapper.h"


class MemSpyWindowGroupsDetailModel : public QAbstractListModel
{
public:
    MemSpyWindowGroupsDetailModel(MemSpyWindowGroup* group, QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    
private:
    QVariantList mData;
};

class MemSpyWindowGroupsDetailView : public MemSpyListView
{
    
    Q_OBJECT
    
public:
    MemSpyWindowGroupsDetailView(EngineWrapper &engine, ViewManager &viewManager) : 
            MemSpyListView(engine, viewManager),
            mModel(0)
        {}
    
protected:
    void initialize(const QVariantMap& params);
    
private:
    MemSpyWindowGroupsDetailModel* mModel;
};

#endif /* MEMSPYWINDOWGROUPSDETAILVIEW_H_ */
