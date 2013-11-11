/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Property Observer class for handling property change events.  
*
*/



// INCLUDE FILES
#include "PropertyObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPropertyObserver::RunL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CPropertyObserver::RunL(void)
{
    if( iStatus.Int() == KErrNone )
    {
        Subscribe();
        NotifyCurrentValue();
    }
}

// -----------------------------------------------------------------------------
// CPropertyObserver::DoCancel
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CPropertyObserver::DoCancel(void)
{
    iProperty.Cancel();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::Subscribe
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CPropertyObserver::Subscribe(void)
{
    iProperty.Subscribe( iStatus );
    SetActive();
}

// -----------------------------------------------------------------------------
// CPropertyObserver::NotifyCurrentValue
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CPropertyObserver::NotifyCurrentValue(void)
{
    TInt value(0);

    if( iSkipValue || iProperty.Get( iCategory, iKey, value ) == KErrNone )
    {
        iHandler.HandlePropertyChange( iCategory, iKey, value );
    }
}


// End of File
