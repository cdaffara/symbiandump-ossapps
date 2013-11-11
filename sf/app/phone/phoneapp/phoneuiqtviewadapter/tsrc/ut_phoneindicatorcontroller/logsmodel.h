/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef LOGSMODEL_H_
#define LOGSMODEL_H_

#include<QObject>
 /*!
 * Mock
  */
class LogsModel: public QObject
{
public:
    enum LogsModelType
        {
        LogsFullModel
        };
Q_OBJECT
public:
    LogsModel( LogsModel::LogsModelType type );
    virtual ~LogsModel();
   
};

#endif /* LOGSMODEL_H_ */
