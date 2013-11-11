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
* Description:  Custom active scheduler for the preset server
*
*/


#include "psscheduler.h"
#include "psserver.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//

namespace
    {

#ifdef _DEBUG
    
    void Panic( TInt aReason )
        {
        _LIT( schedulerPanic, "PSScheduler" );
        User::Panic( schedulerPanic, aReason );
        }

#endif // _DEBUG
    
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPSScheduler::CPSScheduler()
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPSScheduler::~CPSScheduler()
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPSScheduler::SetServer( CPSServer* aServer )
    {
    iServer = aServer;
    }

// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CPSScheduler::Error( TInt aError ) const
    {
    __ASSERT_DEBUG( iServer, Panic( KErrArgument ) );
    
    if ( iServer->IsActive() )
        {
        __DEBUGGER(); // Severe preset server error; activate a breakpoint if debugging.
        }
    else
        {
        if ( aError == KErrBadDescriptor )
            {
            iServer->PanicClient( aError );
            }

        iServer->Message().Complete( aError );
        iServer->ReStart();
        }
    }
	 
// ======== GLOBAL FUNCTIONS ========
