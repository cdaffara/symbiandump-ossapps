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
public:
    enum LogsDetailsRole {
        RoleDuplicatesSeparator = RoleBaseLast + 1 //used to check whether item is separator
    };
   
    explicit LogsDetailsModel();
    
public: // The exported API
  
    ~LogsDetailsModel();
    void clearEvent();
    void getNumberToClipboard();
    LogsEvent getLogsEvent() const;
    
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, 
                                int role = Qt::EditRole);
    
   
private:
    
private: //testing purpose data 

    LogsEvent* mEvent;
    int mDetailItemsCount;
    QMap<int, HbIcon*> mDetailIcons;
    QMap<int, QStringList> mDetailTexts;
    static QString mLastCallName; //last called exported function name
 
    int mSeparatorIndex;
    bool mSeparatorCollapsed;
    
    friend class UT_LogsDetailsView;
    friend class UT_LogsBaseView;
};

#endif //LOGSDETAILSMODEL_H
