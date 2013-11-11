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

#ifndef LOGSREADERSTATECONTEXT_H
#define LOGSREADERSTATECONTEXT_H

//  INCLUDES
#include <QList>
#include <QHash>

// FORWARD DECLARATION
class LogsReaderStateBase;
class LogsEvent;
class LogsEventStrings;
class LogsReaderObserver;

// CLASS DECLARATION

class ContactCacheEntry 
{
public:
    inline ContactCacheEntry() : mRemoteParty(QString()), mContactLocalId(0) {}
    inline ContactCacheEntry( const ContactCacheEntry& entry );
    inline ContactCacheEntry(QString remoteParty, unsigned int contactId);
    
    QString mRemoteParty;
    unsigned int mContactLocalId;
};

/**
 * Context for reader state machine
 */
class LogsReaderStateContext {
    public:
        
        /**
         * Get event container
         * @return events
         */
        virtual QList<LogsEvent*>& events() = 0;
        
        /**
         * Get commonly used strings
         * @return strings
         */
        virtual LogsEventStrings& strings() = 0;

        /**
         * Get observer
         * @return observer
         */
        virtual LogsReaderObserver& observer() = 0;
        
        /**
         * Get contact cache
         * @return cache (key:telnum, value:contactname)
         */
        virtual QHash<QString, ContactCacheEntry>& contactCache() = 0;
        
        /**
         * Get event container for duplicated events
         * @return duplicated events
         */
        virtual QList<LogsEvent*>& duplicatedEvents() = 0;
};



inline ContactCacheEntry::ContactCacheEntry( const ContactCacheEntry& entry ) 
{
    mRemoteParty = entry.mRemoteParty;
    mContactLocalId = entry.mContactLocalId;
}

inline ContactCacheEntry::ContactCacheEntry(QString remoteParty, unsigned int contactId)
{
    mRemoteParty = remoteParty;
    mContactLocalId = contactId;    
}

#endif      // LOGSREADERSTATECONTEXT_H


// End of File
      

        
       
