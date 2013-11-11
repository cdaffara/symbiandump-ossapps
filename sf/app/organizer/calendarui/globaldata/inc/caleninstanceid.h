/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CALENINSTANCEID_H
#define CALENINSTANCEID_H

#include <qdatetime.h>
#include <agendaentry.h>

/**
 * TCalenInstanceId references to a possible instance of (repeating) entry.
 * Reference is very weak:
 * There might not be such instance, and case by case has to be decided
 * what should be done, if such instance doesn't exists.
 *
 * This class is meant to be used, when we want to store what instance was
 * used/focused/modified/displayed, but we don't want to store actual instance.
 * Actual instance might become unusable, if underlying entry is modified
 * or removed.
 *
 * Thus it's safer to store weak reference to it and fetch instance when
 * needed.
 *
 * Entry type is cached to this class only for optimization.
 * Entry type is needed e.g. when opening Options-menus to decide
 * what items should be there and fetching real instances to get entry
 * type would be too heavy operation.
 *
 * @since S60 v3.1
 */
class TCalenInstanceId
    {
public:
    // Factory method, use this normally
    static TCalenInstanceId create( const AgendaEntry& entry );

    // Factory method, use this normally
    static TCalenInstanceId create( const AgendaEntry& entry,
                                    const QDateTime& instanceTime );
    
    static TCalenInstanceId nullInstanceId();
    
    /**
     * For handling missed alarms through multiple contexts
     */
    static TCalenInstanceId create(const ulong& luid, 
                                    const QDateTime& instanceTime,
                                    bool instanceViewed);
    
    // C++ constructors, do not use these normally
   /* inline TCalenInstanceId TCalenInstanceIdWithType( const ulong& luid,
                               const QDateTime& instanceTime,
                               AgendaEntry::Type type );*/
    inline TCalenInstanceId();

    /**
     * For handling missed alarms through multiple contexts
     * C++ constructors
     */
    inline TCalenInstanceId( const ulong& luid,
                                const QDateTime& instanceTime,
                                bool instanceViewed );
    
    bool operator==( const TCalenInstanceId& aX) const;
    
public: // data members
    ulong mEntryLocalUid;
    QDateTime mInstanceTime;
    AgendaEntry::Type mType;
    bool mInstanceViewed;
    };
    
#include "CalenInstanceId.inl"


#endif // CALENINSTANCEID_H

