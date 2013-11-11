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
#include <logsabstractmodel.h>

class LogsEvent;
class LogsDbConnector;
class LogsMatchesModel;
class LogsConfigurationParams;
class LogsDetailsModel;

/**
 * Model for log events.
 *
 */
class LogsModel : public LogsAbstractModel
{
    Q_OBJECT
    
public:
    
    enum ClearType {
        TypeLogsClearAll = 0,
        TypeLogsClearReceived,
        TypeLogsClearCalled,
        TypeLogsClearMissed
    };
    
    enum LogsModelType {
        LogsRecentModel, // Model handles recent events
        LogsFullModel    // Model handles all events
    };
public: // The exported API

    /**
     * Constructor
     * @param modelType
     * @param resourceControl, true will start model in compressed data mode
     *  where memory usage is minimized and refreshData call is required
     *  to get all available data
     */
    LOGSENGINE_EXPORT explicit LogsModel(
        LogsModelType modelType = LogsRecentModel, bool resourceControl = false);
    
    /**
     * Destructor
     */
    LOGSENGINE_EXPORT ~LogsModel();

    /**
     * Clear events. Clearing is async operation and completion is indicated
     * by clearingCompleted signal.
     * @param cleartype, type of events to be cleared
     * @return true if async clearing started
     */
    LOGSENGINE_EXPORT bool clearList(LogsModel::ClearType cleartype);
        
    /**
     * Get matches model.
     * @return matches model
     */
    LOGSENGINE_EXPORT LogsMatchesModel* logsMatchesModel();
    
    /**
     * Mark events as seen. Operation is asycn and completion is indicated
     * by markingCompleted signal.
     * @param cleartype, type of events to be marked as seen
     * @return true if async marking started, false if marking did not start
     */
    LOGSENGINE_EXPORT bool markEventsSeen(LogsModel::ClearType cleartype);
    
    /**
     * Clear missed calls counter
     * @return 0 if clearing was success
     */    
    LOGSENGINE_EXPORT int clearMissedCallsCounter();
    
    /**
     * Refresh data if it was compressed, can be used only if resourceControl
     * is enabled.
     * @return 0 if refreshed
     */
    LOGSENGINE_EXPORT int refreshData();
    
    /**
     * Compress data, minimizes memory usage, can be used only if resourceControl
     * is enabled.
     * @return 0 if compressed
     */
    LOGSENGINE_EXPORT int compressData();
 
    /**
     * Configuration.
     * @param configuration parameters
     * @return 0 if configured succesfully
     */
    LOGSENGINE_EXPORT virtual int updateConfiguration(LogsConfigurationParams& params);
    
    /**
     * Get details model.
     * @return details model
     */
    LOGSENGINE_EXPORT LogsDetailsModel* logsDetailsModel(LogsEvent& event);

public: // From QAbstractItemModel
    
    /**
     * Get number of events currently in the model.
     * @return number of events
     */
    virtual int rowCount(const QModelIndex &parent) const;
    
    /**
     * Get various data from the model. Fetched data type is defined
     * by role input parameter. Besides standard Qt::DisplayRole and
     * Qt::DecorationRole, LogsAbstractModel::LogsModelRole defines
     * additional data types.
     */
    virtual QVariant data(const QModelIndex &index, int role) const;

signals:
    
    /**
     * Signaled once clearing has completed.
     * @param err, 0 if clearing was success
     */
    void clearingCompleted(int err);
    
    /**
     * Signaled once marking has completed.
     * @param err, 0 if marking was success
     */
    void markingCompleted(int err);
    
       
public slots:

    void dataAdded(QList<int> addedIndexes);   
    void dataUpdated(QList<int> updatedIndexes);
    void dataRemoved(QList<int> removedIndexes);
    void resetModel();

private:
    
    /**
     * Find sequential indexes and place each sequence to own list.
     * @param indexes, index list
     * @return list of index sequence lists
     */
    QList< QList<int> > findSequentialIndexes(const QList<int>& indexes); 
    QString getCallerId(const LogsEvent& event) const;
    QString SqueezedString(QString basestring, QString secondarystring, qreal maxwidth) const;
    void initIcons();
    bool matchEventWithClearType(const LogsEvent& event, LogsModel::ClearType clearType);
    
private: //data 
    
    LogsModelType mModelType;
    QList<LogsEvent*> mEvents;
    
private:  // Testing related friend definitions
    
    friend class UT_LogsModel;
    friend class UT_LogsFilter;
    friend class UT_LogsCustomFilter;
    friend class UT_LogsMatchesModel;
    
};

#endif //LOGSMODEL_H
