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

//USER
#include "logsfilter.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logscommondata.h"

Q_DECLARE_METATYPE(LogsEvent *)

// -----------------------------------------------------------------------------
// LogsFilter::LogsFilter
// -----------------------------------------------------------------------------
//
LogsFilter::LogsFilter( FilterType type ) 
: QSortFilterProxyModel(), mFilterType( type )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsFilter::LogsFilter(), type:", mFilterType )
    
    // Enable filter check for dataChanged phase
    setDynamicSortFilter( true );
    
    LOGS_QDEBUG( "logs [ENG] <- LogsFilter::LogsFilter()" )
}


// -----------------------------------------------------------------------------
// LogsFilter::~LogsFilter
// -----------------------------------------------------------------------------
//
LogsFilter::~LogsFilter()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsFilter::~LogsFilter()" )
}

// -----------------------------------------------------------------------------
// LogsFilter::filterType
// -----------------------------------------------------------------------------
//
LogsFilter::FilterType LogsFilter::filterType() const
{
    return mFilterType;
}

// -----------------------------------------------------------------------------
// LogsFilter::clearType
// -----------------------------------------------------------------------------
//
LogsModel::ClearType LogsFilter::clearType() const
{
    LogsModel::ClearType cleartype(LogsModel::TypeLogsClearAll);
    switch (mFilterType){
            case All:
                cleartype = LogsModel::TypeLogsClearAll;
               break;
            case Received:
                cleartype = LogsModel::TypeLogsClearReceived;
               break;
            case Called:
                cleartype = LogsModel::TypeLogsClearCalled;
                break;
            case Missed:
                cleartype = LogsModel::TypeLogsClearMissed;
                break;
            default:
               break;
    }
    return cleartype;
}

// -----------------------------------------------------------------------------
// LogsFilter::setMaxSize
// -----------------------------------------------------------------------------
//
void LogsFilter::setMaxSize(int maxSize)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsFilter::LogsFilter(), maxSize", maxSize )
    
    LogsEvent::LogsDirection dir( LogsEvent::DirUndefined );
    if ( mFilterType == Missed ){
        dir = LogsEvent::DirMissed;
    } else if ( mFilterType == Received ){
        dir = LogsEvent::DirIn;
    } else if ( mFilterType == Called ){
        dir = LogsEvent::DirOut;
    }
    LogsCommonData::getInstance().configureReadSize(maxSize, dir);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsFilter::setMaxSize()" )
}
    
// -----------------------------------------------------------------------------
// LogsFilter::filterAcceptsRow
// From QSortFilterProxyModel
// -----------------------------------------------------------------------------
//
bool LogsFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if ( !sourceModel() ){
        return false;
    }
    bool accept = false;
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    const LogsEvent *event = qVariantValue<LogsEvent *>( sourceModel()->data(
            index0, LogsModel::RoleFullEvent) );
    
    if ( event ){
        switch (mFilterType){
            case All:
               accept = true;
               break;
            case Received:
               accept = ( event->direction() == LogsEvent::DirIn );
               break;
            case Called:
                accept = ( event->direction() == LogsEvent::DirOut );
                break;
            case Missed:
                accept = ( event->direction() == LogsEvent::DirMissed );
                break;
            default:
               break;
       }
    }
    return accept;
}

// End of file

