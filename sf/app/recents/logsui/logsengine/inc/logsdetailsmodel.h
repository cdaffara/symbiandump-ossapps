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

#ifndef LOGSDETAILSMODEL_H
#define LOGSDETAILSMODEL_H

#include <logsexport.h>
#include <logsabstractmodel.h>
#include <QList>
#include "logsengdefs.h"

class LogsEvent;
class LogsDbConnector;
class HbIcon;

/**
 * Model for log event details.
 *
 */
class LogsDetailsModel : public LogsAbstractModel
{
    Q_OBJECT

public:
    
    enum LogsDetailsRole {
        RoleDuplicatesSeparator = RoleBaseLast + 1 //used to check whether item is separator
    };
    
    explicit LogsDetailsModel( LogsDbConnector& dbConnector, LogsEvent& event );
    
public: // The exported API
  
    LOGSENGINE_EXPORT ~LogsDetailsModel();

    LOGSENGINE_EXPORT void clearEvent();
    LOGSENGINE_EXPORT void getNumberToClipboard();
    LOGSENGINE_EXPORT LogsEvent getLogsEvent() const;
    
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, 
                         int role = Qt::EditRole);

protected: // From LogsAbstractModel
    
    virtual QVariant createContact(const LogsModelItemContainer& item) const;
    virtual void updateModel();
    
private slots:

    void contactActionCompleted(bool modified);
    void duplicatesRead();
    
private:
    
    void getDecorationData(int row, QList<QVariant>& iconList) const;
    void getDisplayData(int row, QStringList& texts) const;
    QString getCallerId(const LogsEvent& event) const;
    QString getHeaderData(const LogsEvent& event) const;
    QString getRemoteUri(const LogsEvent& event) const;
    bool isAddress(QString value) const;
    bool isOutgoingCall() const;
    QString getHeaderValue(QString value,bool isremote) const;
    void initContent();
    void initTexts();
    void initIcons();
    void addDateAndTimeTextRow(const LogsEvent& event, bool firstOfMultipleDates = false);
    
private: //data 
    
    LogsEvent* mEvent;
    QList<HbIcon*> mDetailIcons;
    QList<QStringList> mDetailTexts;
    QList<LogsEvent*> mDuplicates;
    
    int mSeparatorIndex;
    bool mSeparatorCollapsed;
    
private:
        
    friend class UT_LogsModel;
    friend class UT_LogsDetailsModel;
    
    
};

#endif //LOGSDETAILSMODEL_H
