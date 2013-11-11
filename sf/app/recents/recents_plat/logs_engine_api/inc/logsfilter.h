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
#include <logsexport.h>
#include <logsmodel.h>

/**
 * LogsFilter can be used to filter certain events 
 * from logs model.
 */
class LogsFilter : public QSortFilterProxyModel 
{
    Q_OBJECT
    
public:
    
    enum FilterType{
        All,
        Received,
        Called,
        Missed
    };
    
public: // The exported API

    /**
     * Constructor
     * @param type, filter type defining which events are shown
     */
    LOGSENGINE_EXPORT LogsFilter( FilterType type = All );
    
    /**
     * Destructor
     */
    LOGSENGINE_EXPORT ~LogsFilter();
    
    /**
     * Get current filter type
     * @return filtertype
     */
    LOGSENGINE_EXPORT FilterType filterType() const;
    
    /**
    * Get current clear type
    * @return cleartype
    */ 
    LOGSENGINE_EXPORT LogsModel::ClearType clearType() const;
    
    /**
     * Set maximum number of events.
     * @param max size 
     */    
    LOGSENGINE_EXPORT void setMaxSize(int maxSize);
    
protected: // From QSortFilterProxyModel
    
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    
private: //data 
    
    FilterType mFilterType;

private:  // Testing related friend definitions
        
    friend class UT_LogsFilter;
    friend class UT_LogsModel;
};
                  
#endif // LOGSFILTER_H
