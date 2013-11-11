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
#ifndef LOGSCUSTOMFILTER_H
#define LOGSCUSTOMFILTER_H

#include <QSortFilterProxyModel>
#include <logsexport.h>
#include <logsmodel.h>

/**
 * LogsCustomFilter can be used to filter events 
 * from logs model based on more detailed search terms.
 */
class LogsCustomFilter : public QSortFilterProxyModel 
{
    Q_OBJECT

public: // The exported API

    LOGSENGINE_EXPORT LogsCustomFilter();
    LOGSENGINE_EXPORT ~LogsCustomFilter();
    
    /**
     * Filter events based on contact id of event.
     * @param contactId
     */
    LOGSENGINE_EXPORT void setContactId(quint32 contactId);
    
    /**
     * Starts removing events. Clearing can be sync or async. 
     * In case of async, completion is indicated by clearingCompleted signal.
     * @return true if async clearing started
     */
    LOGSENGINE_EXPORT bool clearEvents();
    
    /**
     * Mark events as seen. Completion is indicated by
     * markingCompleted signal.
     * @param eventIds, ids of the events to be marked
     * @return true if marking started
     */
    LOGSENGINE_EXPORT bool markEventsSeen();
    
protected: // From QSortFilterProxyModel
    
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

signals:
    /**
     * Signaled once asycn event clearing has completed.
     * @param err, 0 if marking completed succesfully
     */
    void clearingCompleted(int err);
    
    /**
     * Signaled once asycn event marking has completed.
     * @param err, 0 if marking completed succesfully
     */
    void markingCompleted(int err);
    
private:
    
    QList<LogsEvent*> getEvents(bool onlyUnseen = false) const;
    
private: //data 

    quint32 mContactId;

private:  // Testing related friend definitions
        
    friend class UT_LogsCustomFilter;
    friend class UT_LogsModel;
};
                  
#endif // LOGSCUSTOMFILTER_H
