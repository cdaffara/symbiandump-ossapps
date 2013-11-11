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

#ifndef LOGSMATCHESMODEL_H
#define LOGSMATCHESMODEL_H

#include <logsexport.h>
#include <logsabstractmodel.h>
#include <QStringList>
#include "logsengdefs.h"

class LogsEvent;
class LogsDbConnector;
class HbIcon;
class LogsContact;

/**
 * Model for log event details.
 *
 */
class LogsMatchesModel : public LogsAbstractModel 
{
    //Q_OBJECT
    friend class UT_LogsMatchesView;
    
public:
    
    explicit LogsMatchesModel( LogsDbConnector& dbConnector );
    
public: // The exported API
  
    ~LogsMatchesModel();
    void logsMatches( const QString& pattern );

        
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

public://test functions
    
    QString lastCall();
    void resetLastCall();
    
public: //test data
    
    QString mLastCall;
    QStringList mTextData;
  
};

#endif //LOGSMATCHESMODEL_H
