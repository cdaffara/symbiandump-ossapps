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


//debug
#include "caleninstanceid.h"

#include <e32def.h>
#include <agendaentry.h>

const ulong KNullLocalUid = 0;
const AgendaEntry::Type KNullEntryType = AgendaEntry::TypeAppoinment;


// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::create( const AgendaEntry& entry )
    {
    TCalenInstanceId id;
    id.mEntryLocalUid = entry.id();
    id.mInstanceTime = entry.startTime();
    id.mType = entry.type();
    return id;
    }


inline TCalenInstanceId TCalenInstanceId::create( const AgendaEntry& entry,const QDateTime& instanceTime )
    {
    TCalenInstanceId id;
    id.mEntryLocalUid = entry.id();
    id.mInstanceTime = instanceTime;
    id.mType = entry.type();
    return id;

    }

// -----------------------------------------------------------------------------
// TCalenInstanceId::NullInstanceIdL
// Returns a null instance id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::nullInstanceId()
    {
    return TCalenInstanceId();
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TCalenInstanceId::TCalenInstanceId()
    : mEntryLocalUid( KNullLocalUid ),
      mInstanceTime( QDateTime() ),
      mType( KNullEntryType )
    {
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*inline TCalenInstanceId TCalenInstanceId::TCalenInstanceIdWithType(
    const ulong luid,
    const QDateTime& instanceTime,
    AgendaEntry::Type type )
    {
    
    }*/

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline bool TCalenInstanceId::operator==( const TCalenInstanceId& aX ) const
    {
    return aX.mEntryLocalUid == mEntryLocalUid
        && aX.mInstanceTime == mInstanceTime;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::create(const ulong& luid, 
                                                const QDateTime& instanceTime,
                                                bool instanceViewed)
    {
    return TCalenInstanceId( luid ,instanceTime, instanceViewed );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TCalenInstanceId::TCalenInstanceId(
        const ulong& luid, 
        const QDateTime& instanceTime,
        bool instanceViewed)
    : mEntryLocalUid( luid ),
      mInstanceTime( instanceTime ),
      mInstanceViewed( instanceViewed )
    {
    }
