/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: An observer and I/F class to observe Publish and Subscribe key changes.
*
*/



// INCLUDE FILES
#include "pim_trace.h"
#include "PropertyObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPropertyObserver::CPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPropertyObserver::CPropertyObserver(MPropertyChangeHandler& aHandler, TUid aCategory, TUint aKey)
: CActive( CActive::EPriorityStandard ),
  iCategory( aCategory ),
  iKey( aKey ),
  iHandler( aHandler )
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPropertyObserver::ConstructL(void)
{
    TRACE_ENTRY_POINT;
    CActiveScheduler::Add( this );

    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );

    NotifyCurrentValue(); // for initial value

    // Start observing...
    Subscribe();
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPropertyObserver* CPropertyObserver::NewL(MPropertyChangeHandler& aHandler, TUid aCategory, TUint aKey)
{
    TRACE_ENTRY_POINT;
    CPropertyObserver* self = new( ELeave )CPropertyObserver( aHandler, aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
}

// Destructor
CPropertyObserver::~CPropertyObserver()
{
    TRACE_ENTRY_POINT;
    Deque();
    iProperty.Cancel();
    iProperty.Close();
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::RunL(void)
{
    TRACE_ENTRY_POINT;
    ASSERT( !(iStatus.Int()) );
    if( iStatus.Int() == KErrNone )
    {
        Subscribe();
        NotifyCurrentValue();
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::DoCancel(void)
{
    TRACE_ENTRY_POINT;
    iProperty.Cancel();
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::Subscribe
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::Subscribe(void)
{
    TRACE_ENTRY_POINT;
    iProperty.Subscribe( iStatus );
    SetActive();
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// CPropertyObserver::NotifyCurrentValue
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPropertyObserver::NotifyCurrentValue(void)
{
    TRACE_ENTRY_POINT;
    TInt value;

    if( iProperty.Get( iCategory, iKey, value ) == KErrNone )
    {
        iHandler.HandlePropertyChange( iCategory, iKey, value );
    }
    TRACE_EXIT_POINT;
}


// End of File
