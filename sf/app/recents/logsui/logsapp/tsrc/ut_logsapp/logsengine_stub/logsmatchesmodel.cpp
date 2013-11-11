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
#include "logsmatchesmodel.h"
#include "logslogger.h"
#include "logsengdefs.h"
#include "logsdbconnector.h"
#include "logscall.h"
#include "logsdetailsmodel.h"
#include "logscontact.h"

Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsDetailsModel*)

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMatchesModel::LogsMatchesModel( LogsDbConnector& dbConnector ) 
    : LogsAbstractModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::LogsMatchesModel()" )    
    mDbConnector = &dbConnector;
    mLastCall = QString("constructor");
    mTextData.append( "firstitem" );
    
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::LogsMatchesModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMatchesModel::~LogsMatchesModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::~LogsMatchesModel()" )
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::~LogsMatchesModel()" )
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
int LogsMatchesModel::rowCount(const QModelIndex & /* parent */) const
{
    return mTextData.count();
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsMatchesModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    if ( role == RoleCall && mTextData.count() > 0 ) {
        LogsCall* logsCall = new LogsCall();
        QVariant var = qVariantFromValue(logsCall);
        return var;
    } else if (role == RoleDetailsModel) {
        if (mTextData.count() > 0){
           LogsDetailsModel* logsDetails = new LogsDetailsModel();
           QVariant var = qVariantFromValue(logsDetails);
           return var;     
        }
   }
    return QVariant();
}

void LogsMatchesModel::logsMatches( const QString& pattern )
{
    Q_UNUSED(pattern);
    mLastCall = QString("logsMatches");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsMatchesModel::lastCall()
{
    return mLastCall;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::resetLastCall()
{
    mLastCall = QString();
}
