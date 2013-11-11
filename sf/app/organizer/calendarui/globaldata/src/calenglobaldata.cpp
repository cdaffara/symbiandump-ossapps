/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global Data for Calendar application
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenglobaldata.h"        // CalenGlobalData
#include "calencontext.h"           // Calendar context
#include "calencontextimpl.h"       // Calendar context implementation
#include "hb_calencommands.hrh"
#include <CalSession.h>             // Calendar database session
#include <calinstanceview.h>        // Calendar Instance view
#include <calentryview.h>           // Calendar Entry view

// Default Calendar database path
_LIT( KCalendarDatabaseFilePath, "c:Calendar" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenGlobalData::CCalenGlobalData
// C++ default constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

CCalenGlobalData::CCalenGlobalData( MCalProgressCallBack& aCalCallBack )
    : iCalCallBack( aCalCallBack )
    {
    TRACE_ENTRY_POINT;

    iGlobalDataOwnsCalSession = EFalse;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGlobalData::NewL
// Two phased construction. This will leave if an instance of global data
// already exists.  All access to an instance of the global data should be
// through the InstanceL function, except for the initial construction
// which should be handled by the controller.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenGlobalData* CCalenGlobalData::NewL( MCalProgressCallBack& aCalCallBack,
                                                   MCalenContextChangeObserver* aNotifier )
    {
    TRACE_ENTRY_POINT;
    CCalenGlobalData* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if (!tlsPtr)
        {
        // TLS is NULL, so no CCalenGlobalData has been created yet.
        self = new( ELeave ) CCalenGlobalData( aCalCallBack );
        CleanupStack::PushL( self );
        // Store a self pointer in TLS
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( self ) ) );
        // Increment ref count right away. If we don't do it here, and someone
        // calls CCalenGlobalData::InstanceL in ConstructL and then ConstructL
        // leaves, we will double delete the global data.
        ++self->iRefCount;
        self->ConstructL( aNotifier );
        CleanupStack::Pop( self );
        }
    else
        {
        // An instance of the global data exists already.
        // This function should only have been called once, by CCalenAppUi
        User::Leave( KErrAlreadyExists );
        }

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::InstanceL()
    {    	
    TRACE_ENTRY_POINT;

    CCalenGlobalData* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if (!tlsPtr)
        {
        // The global data has not yet been constructed.
        // FIXME - Revise comment
        //TLS is NULL, so no CCalenGlobalData has been created yet.      
        User::Leave( KErrNotReady );
        }
    else
        {
        self = static_cast<CCalenGlobalData*>( tlsPtr );
        }

    ++self->iRefCount;

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Two-phased constructor.  Creates the CCalenGlobalData with an existing
// CCalSession.  CCalenGlobalData does NOT take ownership of the passed session
// The caller is responsible for ensuring that the session exists until all
// open references to the CCalenGlobalData are released.
// Calls User::Invariant() if CCalenGlobalData already has a valid session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::InstanceL(CCalSession& aSession)
    {
    TRACE_ENTRY_POINT;

    CCalenGlobalData* self = CCalenGlobalData::InstanceL();
    CleanupStack::PushL(self);
    self->SetSessionL(&aSession);
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Non-leaving version of constructor. Returns an instance of
// CCalenGlobalData if one exists or NULL otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::Instance()
    {
    TRACE_ENTRY_POINT;
    TAny* tlsPtr = Dll::Tls();
    CCalenGlobalData* self = static_cast<CCalenGlobalData*>( tlsPtr );

    if ( self )
        {
        ++self->iRefCount;
        }

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::ConstructL( MCalenContextChangeObserver* aNotifier)
    {
    TRACE_ENTRY_POINT;

    iContext = new (ELeave ) CalenContextImpl( aNotifier );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::SetSessionL
// Sets the CCalenGlobalData to use an externally created CCalSession.
// Leaves with KErrAlreadyExists if a session has already been created by the
// CCalenGlobalData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::SetSessionL(CCalSession* aSession)
    {
    TRACE_ENTRY_POINT;

    if (iCalSession)
        {
        __ASSERT_ALWAYS(iCalSession == aSession, User::Leave(KErrAlreadyExists));
        }
    else
        {
        iGlobalDataOwnsCalSession = EFalse;
        iCalSession = aSession;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::Release
// Release handle to CCalenGlobalData.  If no other objects are referencing
// this class it will be deleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CCalenGlobalData::Release()
    {
    TRACE_ENTRY_POINT;

    --iRefCount;
    if (iRefCount == 0)
        {
        delete this;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Destructor
// This is private and should only be called from the Release() function above
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenGlobalData::~CCalenGlobalData()
    {
    TRACE_ENTRY_POINT;
    
    delete iInterimUtils;    
    delete iEntryView;
    delete iInstanceView;
    delete iEntryViewCreation;
    delete iInstanceViewCreation;
    delete iContext;

    if (iGlobalDataOwnsCalSession)
        {
        delete iCalSession;
        }
    
    --iRefCount;
    
    // The controller owns its own instance of the services, therefore the
    // reference count will be one, immediatley before deletion.
    if (iRefCount == 1)
        {
        delete this;
        }

    Dll::SetTls(NULL);

    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenGlobalData::CalSessionL
// Create a Calendar session if necessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalSession& CCalenGlobalData::CalSessionL()
    {
    TRACE_ENTRY_POINT;

    if (!iCalSession)
        {
        iGlobalDataOwnsCalSession = ETrue;
        iCalSession = CCalSession::NewL();

        TRAPD( err, iCalSession->OpenL(KCalendarDatabaseFilePath));
        if ( err == KErrNotFound )
            {
            iCalSession->CreateCalFileL(KCalendarDatabaseFilePath);
            iCalSession->OpenL(KCalendarDatabaseFilePath); // not trapped anymore
            }
        else
            {
            User::LeaveIfError( err );
            }

        }

    TRACE_EXIT_POINT;
    return *iCalSession;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceViewL
// Create an instance view if necessary and returns a reference.  Creating an
// instance view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// instance view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalInstanceView* CCalenGlobalData::InstanceViewL()
    {
    TRACE_ENTRY_POINT;

    if (!iInstanceView && !iInstanceViewCreation)
        {
        if ( iEntryViewCreation )
            {
            /* We need queueing to avoid two immediate instance and entry view creation requests to symbian.
             * Though symbian handle two immediate requests ,this we require because we have only one observer
             * for callback and we would not be able to differentiate whether instance view got created or entry view got created.
             * If we dont queue the second request, we may end up giving incomplete object to the caller function.
             */
            iQueued = &CreateInstanceViewL;
            }
        else
            {
            CreateInstanceViewL();
            }
        }

    TRACE_EXIT_POINT;
    return iInstanceView;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::EntryViewL
// Create an entry view if necessary and returns a reference.  Creating an
// entry view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// entry view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalEntryView* CCalenGlobalData::EntryViewL()
    {
    TRACE_ENTRY_POINT;

    if (!iEntryView && !iEntryViewCreation)
        {
        if ( iInstanceViewCreation )
            {
            /* We need queueing to avoid two immediate instance and entry view creation requests to symbian.
             * Though symbian handle two immediate requests ,this we require because we have only one observer
             * for callback and we would not be able to differentiate whether instance view got created or entry view got created.
             * If we dont queue the second request, we may end up giving incomplete object to the caller function.
             */
            iQueued = &CreateEntryViewL;
            }
        else
            {
            CreateEntryViewL();
            }
        }
    TRACE_EXIT_POINT;
    return iEntryView;
    }

void CCalenGlobalData::CreateEntryViewL()
    {
    TRACE_ENTRY_POINT;
    iEntryViewCreation = CCalEntryView::NewL( CalSessionL(), iCalCallBack );
    TRACE_EXIT_POINT;
    }

void CCalenGlobalData::CreateInstanceViewL()
    {
    TRACE_ENTRY_POINT;
    iInstanceViewCreation = CCalInstanceView::NewL( CalSessionL(), iCalCallBack );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CEntryViewObserver::ViewCreationCompleted
// FIXME -- Update description
// Get call after entryview / instance view being created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::ViewCreationCompleted( TBool aSuccess )
    {
    TRACE_ENTRY_POINT;

    if ( !aSuccess )
        {
        // Something has gone wrong
        delete iEntryViewCreation;
        delete iInstanceViewCreation;
        }
    else
        {
        if ( iEntryViewCreation )
            {
            iEntryView = iEntryViewCreation;
            }
        else if ( iInstanceViewCreation )
            {
            iInstanceView = iInstanceViewCreation;
            }
        }

    iEntryViewCreation = NULL;
    iInstanceViewCreation = NULL;

    if ( iQueued )
        {
        (this->*iQueued)();
        iQueued = NULL;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::RegisterEntryViewObserver
// Add observer into view observers entry observer list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::EntryViewExists()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iEntryView != NULL );
    }


// -----------------------------------------------------------------------------
// CCalenGlobalData::Context
// Return a reference to the CCalenContext object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C MCalenContext& CCalenGlobalData::Context()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iContext;
    }

void CCalenGlobalData::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    if ( aNotification == ECalenNotifyEntryInstanceViewCreated )
        {
        ViewCreationCompleted( ETrue );
        }
    else if (aNotification == ECalenNotifyEntryInstanceViewCreationFailed )
        {
        ViewCreationCompleted( EFalse );
        }

    TRACE_EXIT_POINT;
    }

// Enf of file
