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

#include "logsfilter.h"
#include "logsevent.h"
#include <QVariant>
#include <QModelIndex>
#include <logsmodel.h>

int m_modelMaxSize = 0;

LogsFilter::LogsFilter( FilterType /*type*/ )
{
}

/**
 * Destructor
 */
LogsFilter::~LogsFilter()
{
}

void LogsFilter::setMaxSize(int maxSize)
{
    m_modelMaxSize = maxSize;
}

QVariant LogsFilter::data(const QModelIndex &proxyIndex, int role) const
{   
    return QVariant();
}

QModelIndex LogsFilter::index(int row, int column) const
{
return QModelIndex();
}

void LogsFilter::setSourceModel(LogsModel *sourceModel)
{
}


