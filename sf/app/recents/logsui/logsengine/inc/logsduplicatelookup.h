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

#ifndef LOGSDUPLICATELOOKUP_H
#define LOGSDUPLICATELOOKUP_H

//  INCLUDES
#include <QList>
#include <QHash>

// FORWARD DECLARATION
class LogsEvent;

typedef QHash<QString, LogsEvent*> LogsLookupHash;

// CLASS DECLARATION
class LogsDuplicateLookup {
public:
    explicit LogsDuplicateLookup();
    virtual ~LogsDuplicateLookup();
    
    void invalidate();
    void cleanup();
    int addLookupEntry( LogsEvent& event );
    LogsEvent* findDuplicate( const LogsEvent& event ) const;
private:
    QString constructLookupKey( const LogsEvent& event ) const;
    bool validLookupEvent( const LogsEvent& event ) const;
    
private:
    QList< LogsLookupHash* > mDirectionLookupTables;
    
private: // For testing
    
    friend class UT_LogsDuplicateLookup;
    friend class UT_LogsReaderStates;
};
                

#endif      // LOGSDUPLICATELOOKUP_H


// End of File
      

        
       
