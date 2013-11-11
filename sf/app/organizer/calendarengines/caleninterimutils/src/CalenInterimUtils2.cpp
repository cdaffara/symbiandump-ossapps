/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains utility methods related to usage of
*                  Calendar Interim API.
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include <CalenInterimUtils2.h>
#include "CalenInterimUtils2Impl.h"

//debug
#include "calendarengines_debug.h"

// -----------------------------------------------------------------------------
// CCalenInterimUtils22::NewL()
// Creates and returns a new CCalenInterimUtils2 object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenInterimUtils2* CCalenInterimUtils2::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenInterimUtils2* self = new(ELeave) CCalenInterimUtils2();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

CCalenInterimUtils2::CCalenInterimUtils2()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2::ConstructL()
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iImpl = CCalenInterimUtils2Impl::NewL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2::~CCalenInterimUtils2()
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenInterimUtils2::~CCalenInterimUtils2()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    delete iImpl;
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2::PopulateChildFromParentL
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenInterimUtils2::PopulateChildFromParentL(
    CCalEntry& aChild,
    const CCalEntry& aParent )
    {
    TRACE_ENTRY_POINT;

    CCalenInterimUtils2Impl::PopulateChildFromParentL(aChild, aParent);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenInterimUtils2::GlobalUidL
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CCalenInterimUtils2::GlobalUidL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iImpl->GlobalUidL();
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2::MRViewersEnabledL()
// Checks to see if Meeting Request Viewer functionality
// is enabled and an implementation is available to use
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenInterimUtils2::MRViewersEnabledL(TBool aForceCheck)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iImpl->MRViewersEnabledL(aForceCheck);
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2::StoreL()
// Public exported function. Checks to see if the given entry should be entered
// using StoreL() or UpdateL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenInterimUtils2::StoreL( CCalEntryView& aEntryView,
                                CCalEntry& aEntry,
                                TBool aCopyToChildren )
    {
    TRACE_ENTRY_POINT;
    
    CCalenInterimUtils2Impl::StoreL(aEntryView, aEntry, aCopyToChildren);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2::IsMeetingRequestL()
// Check to see if the given entry is a meeting request.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenInterimUtils2::IsMeetingRequestL( CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenInterimUtils2Impl::IsMeetingRequestL(aEntry);
    }

// End of file
