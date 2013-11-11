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
* Description:  Meeting Request calendar info mailbox extension
*
*/

#ifndef CMRCALENDARINFO_H
#define CMRCALENDARINFO_H

#include <calcommon.h>    // TCalFileId
#include "cemailextensionbase.h"

// interface UID value passed to CFSMailBox::ExtensionL
const TUid KMailboxExtMrCalInfo        = { 0x1020696a };

/**
 * Calendar info extension for CFSMailBox
 * 
 * @code
 * CEmailExtension* extension = mailbox->ExtensionL( KMailboxExtMrCalInfo );
 * CMRCalendarInfo* calInfo = reinterpret_cast<CMRCalendarInfo*>( extension );
 * TCalFileId id;
 * calInfo->GetCalendarDatabaseIdL( id );
 * @endcode
 * 
 */
class CMRCalendarInfo : public CEmailExtension
{
public:
    
    /**
    * Return id of calendar database used with sync.
    * @param aId database id is returned in this argument
    */
    virtual void GetCalendarDatabaseIdL( TCalFileId& aId ) const = 0;

    /**
    * Sets calendar database id.
    * @param aId database id
    */    
    virtual void SetCalendarDatabaseIdL( const TCalFileId& aId ) = 0;

protected:    
    CMRCalendarInfo( const TUid& aUid );
};

#endif // CMRCALENDARINFO_H
