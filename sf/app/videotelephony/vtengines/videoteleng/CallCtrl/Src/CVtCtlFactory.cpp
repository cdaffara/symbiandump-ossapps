/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CVtCtlFactory class implementation.
*
*/


#pragma CTC SKIP
// INCLUDE FILES
#include "CVtCtlFactory.h"
#include "CVtCtlCallControl.h"
#include <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MVtCtlCallControl::CreateCallControlL
// Creates call control.
// -----------------------------------------------------------------------------
//
MVtCtlCallControl* CVtCtlFactory::CreateCallControlL(
    MVtCtlEventObserver& aEventObserver )
    {
    __VTPRINTENTER( "CVtCtlCallStatusMonitor.IsLegalNewState" )
    MVtCtlCallControl* control = CVtCtlCallControl::NewL(
        aEventObserver );
    __VTPRINTEXIT( "CVtCtlCallStatusMonitor.IsLegalNewState" )
    return control;
    }

// -----------------------------------------------------------------------------
// MVtCtlCallControl::Delete
// Destructs call control object.
// -----------------------------------------------------------------------------
//
void CVtCtlFactory::Delete( MVtCtlCallControl* aCallControl )
    {
    __VTPRINTENTER( "CVtCtlFactory.Delete" )
    CVtCtlCallControl* callControl =
        static_cast<CVtCtlCallControl*>( aCallControl );
    delete callControl;
    __VTPRINTEXIT( "CVtCtlFactory.Delete" )
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// CreateFactory
// Returns: Factory for creating and destructing call control.
// -----------------------------------------------------------------------------
//
EXPORT_C CVtCtlFactory* CreateFactory()
    {
    CVtCtlFactory* factory = new CVtCtlFactory;
    return factory;
    }
#pragma CTC ENDSKIP
//  End of File

