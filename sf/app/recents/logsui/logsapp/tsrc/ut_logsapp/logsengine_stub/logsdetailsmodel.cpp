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

#include "logsdetailsmodel.h"
#include "logsevent.h"
#include "logsmodel.h"
#include "logslogger.h"
#include "logsengdefs.h"
#include "logscall.h"
#include "logscontact.h"
#include "logsmessage.h"
#include <hbicon.h>
#include <hblineedit.h>
#include <QStringList>

Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsContact*)
Q_DECLARE_METATYPE(LogsMessage*)

const int LogsDetailsRemotePartyRow = 0;
const int LogsDetailsDateAndTimeRow = 1;
const int LogsDetailsCallDirectionRow = 2;
const int LogsDetailsCallTypeRow = 3;
const int LogsDetailsCallDurationRow = 4;

QString LogsDetailsModel::mLastCallName = QString();

// -----------------------------------------------------------------------------
// LogsDetailsModel::LogsDetailsModel
// -----------------------------------------------------------------------------
//
LogsDetailsModel::LogsDetailsModel() 
    : LogsAbstractModel(),
      mEvent( 0 ),
      mDetailItemsCount( 0 )
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::LogsDetailsModel()" )
    mLastCallName = QString();
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::LogsDetailsModel()" )
}

// -----------------------------------------------------------------------------
// LogsDetailsModel::~LogsDetailsModel
// -----------------------------------------------------------------------------
//
LogsDetailsModel::~LogsDetailsModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::~LogsDetailsModel()" )
    mLastCallName = QString();
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::~LogsDetailsModel()" )
}

// -----------------------------------------------------------------------------
// LogsDetailsModel::clearEvent
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::clearEvent()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::clearEvent()" )
    mLastCallName = QString("clearEvent");
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::clearEvent()" )
}

// -----------------------------------------------------------------------------
// LogsDetailsModel::getNumberToClipboard()
// -----------------------------------------------------------------------------
//
void LogsDetailsModel::getNumberToClipboard()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDetailsModel::getNumberToClipboard()" )
    mLastCallName = QString("getNumberToClipboard");
    LOGS_QDEBUG( "logs [ENG] <- LogsDetailsModel::getNumberToClipboard()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEvent LogsDetailsModel::getLogsEvent() const
{
    return *mEvent;
}


// -----------------------------------------------------------------------------
// LogsDetailsModel::rowCount
// From QAbstractListModel
// -----------------------------------------------------------------------------
//
int LogsDetailsModel::rowCount(const QModelIndex & /* parent */) const
{
    return mDetailItemsCount;
}

// -----------------------------------------------------------------------------
// LogsDetailsModel::data
// From QAbstractListModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    
    if ( role == RoleCall && mEvent->eventType() != LogsEvent::TypeUndefined ) {
        LogsCall* logsCall = new LogsCall();
        QVariant var = qVariantFromValue(logsCall);
        return var;
    } else if (role == RoleContact ) {
        LogsContact* logsContact = new LogsContact();
        QVariant var = qVariantFromValue(logsContact);
        return var;
    } else if (role == RoleMessage && mEvent->eventType() != LogsEvent::TypeUndefined  ) {
        LogsMessage* logsMessage = new LogsMessage();
        QVariant var = qVariantFromValue(logsMessage);
        return var;
    } else if (role == RoleDuplicatesSeparator) {
        bool separator  = index.row() == mSeparatorIndex;
        return QVariant(separator);
    }
         
    return QVariant();
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsDetailsModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    
    if (role == Qt::DisplayRole && mEvent){
        QString testName("testname");
        return QVariant(testName);
    }
    
    return QVariant();
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
bool LogsDetailsModel::setData(const QModelIndex &index, const QVariant &value, 
                               int role)
{
    Q_UNUSED(role);
    Q_UNUSED(index);
    if (value.isValid() && value.toBool() != mSeparatorCollapsed) {
        mSeparatorCollapsed = !mSeparatorCollapsed;
    }
}
