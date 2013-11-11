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

#ifndef LOGSFILTER_H_
#define LOGSFILTER_H_

#include <QObject>
#include "logsmodel.h"

class QModelIndex;


/*!
 * Mock
 */

class LogsFilter: public QObject
{
    Q_OBJECT
public:
    enum FilterType{
        Missed
    };

 

    LogsFilter(FilterType type);
    virtual ~LogsFilter();
     
    void setSourceModel( LogsModel *model );
    void setMaxSize(int maxSize);
     
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column) const;
    
};

#endif /* LOGSFILTER_H_ */
