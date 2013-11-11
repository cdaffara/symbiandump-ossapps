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

#include "lcfmodel.h"
#include "logslogger.h"
#include "logscntfinder.h"
#include <QStringList>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LcfModel::LcfModel(LogsCntFinder& finder ) 
    : QAbstractListModel(),
      mFinder( finder ),
      mEnabled( true )
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfModel::LcfModel()" )
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfModel::LcfModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LcfModel::~LcfModel()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfModel::~LcfModel()" )
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfModel::~LcfModel()" )
}


// -----------------------------------------------------------------------------
// From QAbstractListModel
// -----------------------------------------------------------------------------
//
int LcfModel::rowCount(const QModelIndex & /* parent */) const
{
    if (!mEnabled) {
        return 0;
    }
    int count = mFinder.resultsCount();
    LOGS_QDEBUG_2( "logs [LCFPROTO] <-> rowCount()=", count )
    return count;
}


// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LcfModel::data(const QModelIndex &index, int role ) const
{
    LOGS_QDEBUG_2( "logs [LCFPROTO] <-> row()=", index.row() )
    const QChar separ( ' ');
    
    if (!mEnabled) {
        return QVariant();
    }
    
    int resultCount = mFinder.resultsCount();
    if (!index.isValid() || 
         index.row() >= resultCount || 
         index.row() < 0 ) {
        return QVariant();
    }
    if (role == Qt::DisplayRole ){
        const LogsCntEntry& result = mFinder.resultAt( index.row() );
        QStringList name;
        for( int i=0;i<result.firstName().count();i++) {
            name << result.firstName()[i].richText();
        }        
        for( int i=0;i<result.lastName().count();i++) {
            name << result.lastName()[i].richText();
        }
        
        if ( result.phoneNumber().text().isEmpty() ) {
            return QVariant(name.join( separ ) );
        } else {
            QStringList all( name.join( separ ) );
            all.append( result.phoneNumber().richText() );
            return QVariant( all );
        }
        
    } else {
        return QVariant();
    }
    
}

