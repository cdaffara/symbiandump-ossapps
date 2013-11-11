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
#ifndef LOGSFILTER_H
#define LOGSFILTER_H

#include <QSortFilterProxyModel>
#include "logsmodel.h"

/**
 * 
 */
class LogsFilter : public QSortFilterProxyModel 
{
    
public:
    
    enum FilterType{
        All,
        Received,
        Called,
        Missed
    };
    
public: // The exported API

    LogsFilter( FilterType type = All )
    {
        mFilterType = type;
    };
    ~LogsFilter(){};
    
    FilterType filterType() const {return mFilterType;}
    LogsModel::ClearType clearType() const  {return LogsModel::TypeLogsClearAll;}
    
protected: // From QSortFilterProxyModel
    
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const 
    {
        Q_UNUSED(sourceRow);
        Q_UNUSED(sourceParent);
        return true;
    }
    
private: //data 
    
    FilterType mFilterType;
    
};
                  
#endif // LOGSFILTER_H
