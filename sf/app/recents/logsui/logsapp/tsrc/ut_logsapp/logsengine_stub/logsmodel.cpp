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
#include "logsmodel.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logsdetailsmodel.h"
#include "logscontact.h"
#include "logsmatchesmodel.h"

Q_DECLARE_METATYPE(LogsCall*)
Q_DECLARE_METATYPE(LogsDetailsModel*)
Q_DECLARE_METATYPE(LogsMessage*)
Q_DECLARE_METATYPE(LogsContact*)


bool mMatchesModelCreated = false;

// -----------------------------------------------------------------------------
// LogsModel::LogsModel
// -----------------------------------------------------------------------------
//
LogsModel::LogsModel(LogsModelType /*modelType*/, bool /*resourceControl*/) : 
    LogsAbstractModel(), mDbConnector(0), mIsCleared(false)
{
    mEmptyModel = false;
    mTestIsMarkingNeeded = false;
    mTestEventsMarked = false;
    mMissedCallsCounterCleared = false;
    mRefreshCalled = false;
    mCompressCalled = false;
}

// -----------------------------------------------------------------------------
// LogsModel::~LogsModel
// -----------------------------------------------------------------------------
//
LogsModel::~LogsModel()
{
    mTextData.clear();
    mIconData.clear();
    //delete mDbConnector; // Not created in stub, no point in deletion 
}

        
// -----------------------------------------------------------------------------
// LogsModel::clearList
// -----------------------------------------------------------------------------
//
bool LogsModel::clearList(LogsModel::ClearType /*cleartype*/)
{
    mIsCleared = true;
    return true;
}

// -----------------------------------------------------------------------------
// LogsModel::clearList
// -----------------------------------------------------------------------------
//
int LogsModel::clearMissedCallsCounter()
{
    mMissedCallsCounterCleared = true;
    return 0;
}
       
// -----------------------------------------------------------------------------
// LogsModel::logsMatchesModel
// -----------------------------------------------------------------------------
//
LogsMatchesModel* LogsModel::logsMatchesModel()
{
    mMatchesModelCreated = true;
    return new LogsMatchesModel(*mDbConnector);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsDetailsModel* LogsModel::logsDetailsModel(LogsEvent& event)
{
    return new LogsDetailsModel();
}

// -----------------------------------------------------------------------------
// LogsModel::markEventsSeen
// -----------------------------------------------------------------------------
//
bool LogsModel::markEventsSeen(LogsModel::ClearType /*cleartype*/)
{
    mTestEventsMarked = true;
    return mTestIsMarkingNeeded;
}

// -----------------------------------------------------------------------------
// LogsModel::refreshData
// -----------------------------------------------------------------------------
//
int LogsModel::refreshData()
{
    mRefreshCalled = true;
    return 0;
}

// -----------------------------------------------------------------------------
// LogsModel::compressData
// -----------------------------------------------------------------------------
//
int LogsModel::compressData()
{
    mCompressCalled = true;
    return 0;
}

// -----------------------------------------------------------------------------
// LogsModel::rowCount
// -----------------------------------------------------------------------------
//
int LogsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if ( mEmptyModel ){
        return 0;
    }
    return 1;//mTextData.count();
};

// -----------------------------------------------------------------------------
// LogsModel::data
// -----------------------------------------------------------------------------
//
QVariant LogsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    if (role == Qt::DisplayRole) {
        return QVariant(mTextData);
    } else if (role == Qt::DecorationRole) {
        return QVariant(mIconData);
    }else if ( role == RoleCall ){ 
        if (mTextData.count() > 0){
            LogsCall* logscall = new LogsCall();
            QVariant var = qVariantFromValue(logscall);
            return var;         
        }
    } else if (role == RoleDetailsModel) {
         if (mTextData.count() > 0){
            LogsDetailsModel* logsDetails = new LogsDetailsModel();
            QVariant var = qVariantFromValue(logsDetails);
            return var;         
        }
    } else if (role == RoleMessage) {
         if (mTextData.count() > 0){
            LogsMessage* logsMessage = new LogsMessage();
            QVariant var = qVariantFromValue(logsMessage);
            return var;         
        }
    } else if (role == RoleContact)
        if (mTextData.count() > 0){
            LogsContact* logsContact = new LogsContact();
            QVariant var = qVariantFromValue(logsContact);
            return var;
        }
    return QVariant();
}

// -----------------------------------------------------------------------------
// LogsModel::addTextData
// -----------------------------------------------------------------------------
//
void LogsModel::addTextData(QString text)
{
    mTextData.append(text);
    reset();
}

// -----------------------------------------------------------------------------
// LogsModel::isMatchesModelCreated
// -----------------------------------------------------------------------------
//
bool LogsModel::isMatchesModelCreated()
{
		return mMatchesModelCreated;
}

// -----------------------------------------------------------------------------
// LogsModel::setMatchesModelCreated
// -----------------------------------------------------------------------------
//
void LogsModel::setMatchesModelCreated(bool isCreated)
{
		mMatchesModelCreated = isCreated;
}


// -----------------------------------------------------------------------------
// LogsModel::updateConfiguration
// -----------------------------------------------------------------------------
//
int LogsModel::updateConfiguration(LogsConfigurationParams& /*params*/)
{
    return 0;
}
