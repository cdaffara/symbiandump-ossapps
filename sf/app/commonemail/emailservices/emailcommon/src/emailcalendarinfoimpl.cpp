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
* Description:  implementation of calendar info extension for mailbox
*
*/

#include "emailtrace.h"

#include "cmrcalendarinfoimpl.h"          

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfo::CMRCalendarInfo( const TUid& aUid ) : CEmailExtension( aUid )
    {
    NM_FUNCTION;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfoImpl::CMRCalendarInfoImpl() : 
    CMRCalendarInfo( KMailboxExtMrCalInfo )
    {
    NM_FUNCTION;
    
    iDatabaseId = MAKE_TINT64(0,0);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMRCalendarInfoImpl::~CMRCalendarInfoImpl()
    {
    NM_FUNCTION;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMRCalendarInfoImpl::GetCalendarDatabaseIdL( TCalFileId& aId ) const
    {
    NM_FUNCTION;
    
    aId = iDatabaseId;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMRCalendarInfoImpl::SetCalendarDatabaseIdL( const TCalFileId& aId )
    {
    NM_FUNCTION;
    
    iDatabaseId = aId;
    }

