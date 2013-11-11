/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

// INCLUDE FILES
#include "CVcxTestActiveWait.h"
#include "VCXTestLog.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxTestActiveWait* CVcxTestActiveWait::NewL()
    {
    VCXLOGLO1(">>>CVcxTestActiveWait::NewL");
    CVcxTestActiveWait* self = new (ELeave) CVcxTestActiveWait();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    VCXLOGLO1("<<<CVcxTestActiveWait::NewL");
    return self;
    }
// -----------------------------------------------------------------------------
// CVcxTestActiveWait::~CVcxTestActiveWait
// destructor
// -----------------------------------------------------------------------------
//
CVcxTestActiveWait::~CVcxTestActiveWait()
    {
    VCXLOGLO1(">>>CVcxTestActiveWait::~CVcxTestActiveWait");

    ActiveWait(EActiveWaitStop);
    delete iActiveSchedulerWait;
    iActiveSchedulerWait = NULL;

    VCXLOGLO1("<<<CVcxTestActiveWait::~CVcxTestActiveWait");
    }

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::~CVcxTestActiveWait
// destructor
// -----------------------------------------------------------------------------
//
CVcxTestActiveWait::CVcxTestActiveWait()
    {
    VCXLOGLO1(">>>CVcxTestActiveWait::CVcxTestActiveWait");


    VCXLOGLO1("<<<CVcxTestActiveWait::CVcxTestActiveWait");
    }


// -----------------------------------------------------------------------------
// CVcxTestActiveWait::ConstructL
//
// -----------------------------------------------------------------------------
//
void CVcxTestActiveWait::ConstructL()
    {
    VCXLOGLO1(">>>CVcxTestActiveWait::ConstructL");

    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;

    VCXLOGLO1("<<<CVcxTestActiveWait::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::ActiveWait
// -----------------------------------------------------------------------------
//
void CVcxTestActiveWait::ActiveWait( TVcxTestActiveWaitCmd aActiveWaitCmd )
    {
    VCXLOGLO1(">>>CVcxTestActiveWait::ActiveWait");

  	VCXLOGLO2("aActiveWaitCmd=%d", aActiveWaitCmd );

	switch( aActiveWaitCmd )
		{
		case EActiveWaitStart:
		    {
		    if( !iActiveSchedulerWait->IsStarted() )
		        {
		        iActiveSchedulerWait->Start();
		        }
		    else
		        {
		        VCXLOGLO1("ActiveSchedulerWait already started");
		        }
		    }
			break;
		case EActiveWaitStop:
		    {
			if( iActiveSchedulerWait->IsStarted() )
				{
				iActiveSchedulerWait->AsyncStop();
				}
			else
		        {
		        VCXLOGLO1("ActiveSchedulerWait already stopped");
		        }
			}
			break;
		default:
			break;
		}

    VCXLOGLO1("<<<CVcxTestActiveWait::ActiveWait");
    }

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::IsWaiting
// -----------------------------------------------------------------------------
//
TBool CVcxTestActiveWait::IsWaiting()
    {
    return iActiveSchedulerWait->IsStarted();
    }

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::Start
// -----------------------------------------------------------------------------
//
TInt CVcxTestActiveWait::Start( )
    {
    TInt err = KErrNotReady;
    if( !iActiveSchedulerWait->IsStarted() )
        {
        iActiveSchedulerWait->Start();
        err = KErrNone;
        }
    else
        {
        VCXLOGLO1("ActiveSchedulerWait already started");
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CVcxTestActiveWait::Stop
// -----------------------------------------------------------------------------
//
TInt CVcxTestActiveWait::Stop( )
    {
    TInt err = KErrNotReady;
    if( iActiveSchedulerWait->IsStarted() )
        {
        iActiveSchedulerWait->AsyncStop();
        err = KErrNone;
        }
    else
        {
        VCXLOGLO1("ActiveSchedulerWait already stopped");
        }
    return err;
    }

//  End of File
