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
#ifndef LOGSMODEL_H
#define LOGSMODEL_H

#include <logsexport.h>
#include "logsabstractmodel.h"
#include "logsengdefs.h"
#include <QStringList>

class LogsMatchesModel;
class LogsConfigurationParams;
class LogsDetailsModel;

/**
 * 
 */
class LogsModel : public LogsAbstractModel
{
public:

    friend class UT_LogsRecentCallsView;
    
    enum ClearType {
                TypeLogsClearAll = 0,
                TypeLogsClearReceived,
                TypeLogsClearCalled,
                TypeLogsClearMissed
                };

    enum LogsModelType {
        LogsRecentModel,
        LogsFullModel
    };

public: // The exported API

    LogsModel(LogsModelType modelType = LogsRecentModel, bool resourceControl = false);
    ~LogsModel();
    
    bool clearList(LogsModel::ClearType cleartype);
    int clearMissedCallsCounter();
    LogsMatchesModel* logsMatchesModel();
    bool markEventsSeen(LogsModel::ClearType cleartype);
    int refreshData();
    int compressData();
	int updateConfiguration(LogsConfigurationParams& params);
	LogsDetailsModel* logsDetailsModel(LogsEvent& event);

public: // From QAbstractItemModel
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
        
signals:
        
    void clearingCompleted(int err);
    void markingCompleted(int err);
            
public: //data 
    
 

    void addTextData(QString text);
    static bool isMatchesModelCreated();
    static void setMatchesModelCreated(bool isCreated);
    
    QStringList mTextData;
    QList<QVariant> mIconData;
    
    LogsDbConnector* mDbConnector;
    bool mEmptyModel;
    bool mIsCleared;
    bool mTestEventsMarked;
    bool mTestIsMarkingNeeded;
    bool mMissedCallsCounterCleared;
    bool mRefreshCalled;
    bool mCompressCalled;
};

#endif //LOGSMODEL_H
