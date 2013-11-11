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

#include "logsabstractmodel.h"
#include "logsdbconnector.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logscall.h"
#include "logscontact.h"
#include "logsengdefs.h"
#include "logsmessage.h"


Q_DECLARE_METATYPE(LogsEvent *)
Q_DECLARE_METATYPE(LogsCall *)
Q_DECLARE_METATYPE(LogsMessage *)
Q_DECLARE_METATYPE(LogsContact *)

bool LogsAbstractModel::mParamUpdated = false;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAbstractModel::LogsAbstractModel() : 
    QAbstractListModel(), mDbConnector(0),mPredectiveSearchStatus(1)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsAbstractModel::LogsAbstractModel()" )
   
    LOGS_QDEBUG( "logs [ENG] <- LogsAbstractModel::LogsAbstractModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsAbstractModel::~LogsAbstractModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsAbstractModel::~LogsAbstractModel()" )

    LOGS_QDEBUG( "logs [ENG] <- LogsAbstractModel::~LogsAbstractModel()" )
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact* LogsAbstractModel::createContact(const QString& number)
{
    return new LogsContact(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsAbstractModel::predictiveSearchStatus()
{
    return mPredectiveSearchStatus;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsAbstractModel::setPredictiveSearch(bool enabled)
{    
		int result = 0;
		if ( mPredectiveSearchStatus == 0 ){
			result = -5;
		}
		else {
			if ( enabled ){
			mPredectiveSearchStatus = 1;
			}
			else {
			mPredectiveSearchStatus = 2;
			}
	  }
    
    return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsAbstractModel::updateConfiguration(LogsConfigurationParams& /*params*/)
{
    mParamUpdated = true;
    return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsAbstractModel::doGetData(int role, LogsModelItemContainer& item) const
{
    Q_UNUSED(role);
    Q_UNUSED(item);
    return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsAbstractModel::createCall(LogsModelItemContainer& item) const
{
    Q_UNUSED(item);
    return QVariant();        
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsAbstractModel::createMessage(LogsModelItemContainer& item) const
{
    Q_UNUSED(item);
    return QVariant();      
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsAbstractModel::createContact(LogsModelItemContainer& item) const
{
    Q_UNUSED(item);
    return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsAbstractModel::directionIconName(const LogsEvent& event)
{
    Q_UNUSED(event);
    QString direction;
    return direction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsAbstractModel::typeIconName(const LogsEvent& event)
{
    Q_UNUSED(event);
    QString brand;
    return brand;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsModelItemContainer::LogsModelItemContainer(LogsEvent* event) : mEvent(event)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsModelItemContainer::~LogsModelItemContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsEvent* LogsModelItemContainer::event()
{
    return mEvent;
}

