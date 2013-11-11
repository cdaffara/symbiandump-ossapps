/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides methods for CNSmlDMDbNotifier class. 
*
*/


#include "nsmldmdbnotifier.h"
#include "nsmldmsyncdebug.h"
#include "nsmldmdef.h"

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::CNSmlDMDbNotifier
// -----------------------------------------------------------------------------
//
CNSmlDMDbNotifier::CNSmlDMDbNotifier( RSyncMLSession* aSyncSession,
                                      MNSmlDMDbEventHandler* aHandler)
                                     : iSyncSession(aSyncSession),
                                       iHandler( aHandler )
 	{
 	}


// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::NewL
// -----------------------------------------------------------------------------
//
CNSmlDMDbNotifier* CNSmlDMDbNotifier::NewL( RSyncMLSession* aSyncSession,
                                            MNSmlDMDbEventHandler* aHandler)
	{
	CNSmlDMDbNotifier* self = new (ELeave) CNSmlDMDbNotifier( aSyncSession,
	                                                          aHandler );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::ConstructL()
	{
	iActiveCaller = CNSmlDMActiveCaller::NewL(this);
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CNSmlDMDbNotifier::~CNSmlDMDbNotifier()
	{		
	delete iActiveCaller;
	iList.Close();
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::RequestL
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::RequestL()
	{
	Session().RequestEventL( *this );  // request MSyncMLEventObserver events
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::OnSyncMLSessionEvent
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::OnSyncMLSessionEvent( TEvent aEvent,
                                              TInt aIdentifier,
                                              TInt aError,
                                              TInt /*aAdditionalData*/ )
	{
	TNSmlDMDbEvent event;

	event.iType = aEvent;
	event.iError = aError;
		
	if (aEvent == MSyncMLEventObserver::EProfileCreated || 
	    aEvent == MSyncMLEventObserver::EProfileChanged ||
	    aEvent == MSyncMLEventObserver::EProfileDeleted )
		{
		event.iProfileId = aIdentifier;
		}
	else
		{
		event.iProfileId = KErrNotFound;
		}
	
	TRAP_IGNORE( iList.AppendL (event) );
	
	CallObserverWithDelay();
	TRAP_IGNORE( RequestL() );
    }

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::SetDisabled
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::SetDisabled( TBool aDisable )
	{
    iDisabled = aDisable;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::Reset
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::Reset()
	{
    iDisabled = EFalse;
	iList.Reset();
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::EventCount
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDbNotifier::EventCount()
	{
	return iList.Count();
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::Event
// -----------------------------------------------------------------------------
//
TNSmlDMDbEvent CNSmlDMDbNotifier::Event( TInt aIndex )
	{
	__ASSERT_DEBUG( aIndex >= 0 && aIndex < iList.Count(),
	                TUtil::Panic(KErrGeneral) );

	return iList[aIndex];
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::Session
// -----------------------------------------------------------------------------
//
RSyncMLSession& CNSmlDMDbNotifier::Session()
	{
	__ASSERT_DEBUG( iSyncSession, TUtil::Panic(KErrGeneral) );
	
	return *iSyncSession;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::CallObserverWithDelay
//
// This is needed to avoid unnecessary observer calls (eg. in case 10 events are
// reported in short time only last is reported to observer). 
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::CallObserverWithDelay()
	{
    iActiveCaller->Cancel();
	iActiveCaller->Start( KErrNone, KListBoxUpdateDelay );
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::CallObserver
//
// this function investigates received events from last observer call
// and calls observer to inform UI what to do. 
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::CallObserver()
	{
	TNSmlDMDbEvent event;
	event.iProfileId = 0;
	event.iType = 0;
	event.iError = 0;
	//
	// check for critical error
	//
	if ( FindCloseEvent() != KErrNotFound )
		{
		event.iType = EClose;
		TRAP_IGNORE( iHandler->HandleDbEventL( event ) );
		return; // some database problem - UI should close
		}
	
	//
	// check if only one profile has changed
	//
	TInt index = FindSingleProfileEvent();
	if ( index != KErrNotFound )
		{
		TNSmlDMDbEvent e = iList[index];
        if (e.iType == MSyncMLEventObserver::EProfileDeleted)
			{
			event.iType = EDelete;
			event.iProfileId = e.iProfileId; 
			}
		else
			{
			event.iType = EUpdate;
			event.iProfileId = e.iProfileId; 
			}
	
		TRAP_IGNORE( iHandler->HandleDbEventL( event ) );
        return;
		}
		
	//
	// check if 2 or more profiles have changed
	//
	if (FindProfileEvent() != KErrNotFound)
		{
	    event.iType = EUpdateAll;
    	TRAP_IGNORE( iHandler->HandleDbEventL( event ) );
        return;
    	}
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::FindCloseEvent
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDbNotifier::FindCloseEvent()
	{
	TInt count = iList.Count();

	for ( TInt index = 0; index < count; index++ )
		{
		TNSmlDMDbEvent event = iList[index];
		if ( event.iType == MSyncMLEventObserver::EServerTerminated )
			{
			return index;
			}
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::FindProfileEvent
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDbNotifier::FindProfileEvent()
	{
	TInt count = iList.Count();

    for ( TInt index = 0; index < count; index++ )
		{
		TNSmlDMDbEvent event = iList[index];
		if ( event.iProfileId != KErrNotFound )
			{
			return index;
			}
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::FindSingleProfileEvent
// -----------------------------------------------------------------------------
//
TInt CNSmlDMDbNotifier::FindSingleProfileEvent()
	{
	TInt count = iList.Count();

	// find out whether list contains update events for one profile only
	for ( TInt index = 0; index < count; index++ )
		{
		TNSmlDMDbEvent event = iList[index];
		if ( event.iProfileId != KErrNotFound &&
		     IsUniqueProfileId(event.iProfileId) )
			{
			return index;
        	}
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::IsUniqueProfileId
// -----------------------------------------------------------------------------
//
TBool CNSmlDMDbNotifier::IsUniqueProfileId( TInt aId )
	{
	TInt count = iList.Count();

    for ( TInt index = 0; index < count; index++ )
		{
		TNSmlDMDbEvent event = iList[index];
		if ( event.iProfileId != KErrNotFound && event.iProfileId != aId )
			{
			return EFalse;
			}
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CNSmlDMDbNotifier::HandleActiveCallL
// -----------------------------------------------------------------------------
//
void CNSmlDMDbNotifier::HandleActiveCallL()
	{
	if ( iDisabled )
		{
		return;  // UI has disabled notifications
		}

	if ( iSyncRunning )
		{
		return; // no UI updates while syncing
		}

	CallObserver();
	Reset();
	}

// End of File
