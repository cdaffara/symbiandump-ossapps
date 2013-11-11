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

// INCLUDE FILES
#include "logsduplicatelookup.h"
#include "logsevent.h"
#include "logscommondata.h"

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsDuplicateLookup::LogsDuplicateLookup()
{
    for ( int i = 0; i <= LogsEvent::DirMissed; i++ ){
        LogsLookupHash* directionLookup = new LogsLookupHash;
        mDirectionLookupTables.append( directionLookup );
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsDuplicateLookup::~LogsDuplicateLookup()
{
    qDeleteAll(mDirectionLookupTables);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsDuplicateLookup::invalidate()
{
    for( int k = 0; k < mDirectionLookupTables.count(); k++ ){
        LogsLookupHash& table = *mDirectionLookupTables.at(k);
        LogsLookupHash::iterator it;
        for (it = table.begin(); it != table.end(); ++it){
            it.value() = 0;
        }
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsDuplicateLookup::cleanup()
{
    for( int k = 0; k < mDirectionLookupTables.count(); k++ ){
        LogsLookupHash& table = *mDirectionLookupTables.at(k);
        LogsLookupHash::iterator it = table.begin();
        while ( it != table.end() ){
            if ( it.value() == 0 ){
                it = table.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
int LogsDuplicateLookup::addLookupEntry( LogsEvent& event )
{
    if ( !validLookupEvent(event) ){
        return -1;
    }
    int dir = event.direction();
    QString key = constructLookupKey(event);
    LogsLookupHash::iterator it = mDirectionLookupTables.at(dir)->find(key);
    if ( it != mDirectionLookupTables.at(dir)->end() ){
        if ( it.value() == 0 ){
            it.value() = &event;
        }
    } else {
        mDirectionLookupTables.at(dir)->insert(key, &event);    
    }
    return 0;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsEvent* LogsDuplicateLookup::findDuplicate( const LogsEvent& event ) const
{
    if ( !validLookupEvent(event) ){
        return 0;
    }
    int dir = event.direction();
    QString key = constructLookupKey(event);
    LogsLookupHash::const_iterator it = 
        mDirectionLookupTables.at(dir)->find(key);
    if ( it != mDirectionLookupTables.at(dir)->constEnd() ){
        return it.value();
    }
    return 0;
}
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
QString LogsDuplicateLookup::constructLookupKey( const LogsEvent& event ) const
{
    return ( event.remoteParty() + "/t" + 
        event.number().right(LogsCommonData::getInstance().telNumMatchLen() ) );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsDuplicateLookup::validLookupEvent( const LogsEvent& event ) const
{
    int dir = event.direction();
    return ( !event.remoteParty().isEmpty() && 
             dir >= 0 && dir < mDirectionLookupTables.count() );
}
