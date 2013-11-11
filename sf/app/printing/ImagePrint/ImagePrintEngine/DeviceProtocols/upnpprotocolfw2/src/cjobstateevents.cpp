/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CJobStateEvents class
*
*/


#include <upnpcons.h> 

#include "cjobstateevents.h"

// ============================ MEMBER FUNCTIONS ===============================

// Constructors

// -----------------------------------------------------------------------------
// CJobStateEvents::NewL
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewL()
{
	CJobStateEvents *self = NewLC();
	CleanupStack::Pop();	// self
	return self;
}


// -----------------------------------------------------------------------------
// CJobStateEvents::NewLC
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewLC()
{
	CJobStateEvents *self = NewLC(
		KNullDesC8(), KNullDesC8(), KNullDesC8(), KNullDesC8(),
		KNullDesC8(), KNullDesC8(), KNullDesC8());
	return self;
}

// -----------------------------------------------------------------------------
// CJobStateEvents::NewL
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewL(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState)
{
	CJobStateEvents *self = NewLC(
		aPrinterState, aPrinterStateReasons, aJobIdList,aJobEndState,
		aJobMediaSheetsCompleted, aContentCompleteList, aJobAbortState);
	CleanupStack::Pop();	// self
	return self;
}


// -----------------------------------------------------------------------------
// CJobStateEvents::NewLC
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewLC(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState)
{
	CJobStateEvents *self = new (ELeave) CJobStateEvents();
	CleanupStack::PushL(self);
	self->ConstructL(
		aPrinterState, aPrinterStateReasons, aJobIdList,aJobEndState,
		aJobMediaSheetsCompleted, aContentCompleteList, aJobAbortState);
	return self;
}

// -----------------------------------------------------------------------------
// CJobStateEvents::NewL
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewL(CJobStateEvents &a)
{
	CJobStateEvents *self = NewLC(a);
	CleanupStack::Pop();	// self
	return self;
}


// -----------------------------------------------------------------------------
// CJobStateEvents::NewLC
// -----------------------------------------------------------------------------
//
CJobStateEvents* CJobStateEvents::NewLC(CJobStateEvents &a)
{
	CJobStateEvents *self = new (ELeave) CJobStateEvents();
	CleanupStack::PushL(self);
	
	User::LeaveIfNull( a.iPrinterState );
	User::LeaveIfNull( a.iPrinterStateReasons );
	User::LeaveIfNull( a.iJobIdList );
	User::LeaveIfNull( a.iJobEndState );
	User::LeaveIfNull( a.iJobMediaSheetsCompleted );
	User::LeaveIfNull( a.iContentCompleteList );
	User::LeaveIfNull( a.iJobAbortState );
	
	self->ConstructL(
		a.iPrinterState->Des(), a.iPrinterStateReasons->Des(), a.iJobIdList->Des(), a.iJobEndState->Des(),
		a.iJobMediaSheetsCompleted->Des(), a.iContentCompleteList->Des(), a.iJobAbortState->Des());
	return self;
}

// -----------------------------------------------------------------------------
// CJobStateEvents::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CJobStateEvents::ConstructL(
				const TDesC8& aPrinterState, const TDesC8& aPrinterStateReasons, const TDesC8& aJobIdList, 
				const TDesC8& aJobEndState,const TDesC8& aJobMediaSheetsCompleted,
				const TDesC8& aContentCompleteList, const TDesC8& aJobAbortState)
{
	Delete();
	iPrinterState = aPrinterState.AllocL();          
	iPrinterStateReasons = aPrinterStateReasons.AllocL();   
	iJobIdList = aJobIdList.AllocL();             
	iJobEndState = aJobEndState.AllocL();           
	iJobMediaSheetsCompleted = aJobMediaSheetsCompleted.AllocL();
    iContentCompleteList = aContentCompleteList.AllocL();
    iJobAbortState = aJobAbortState.AllocL();
}

// -----------------------------------------------------------------------------
// CJobStateEvents::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CJobStateEvents::ConstructL(CJobStateEvents &a)
{
	ConstructL(
		a.iPrinterState->Des(), a.iPrinterStateReasons->Des(), a.iJobIdList->Des(), a.iJobEndState->Des(),
		a.iJobMediaSheetsCompleted->Des(), a.iContentCompleteList->Des(), a.iJobAbortState->Des());
}

// publics

// -----------------------------------------------------------------------------
// CJobStateEvents::ChangedEventL
// -----------------------------------------------------------------------------
//
TBool CJobStateEvents::ChangedEventL(const TDesC8 &aEvent, TJobStateEvent aEnum)
{
	TBool changed(EFalse);
	
	switch( aEnum )
	{
		case EPrinterState:
			if(iPrinterState)
			{
				if(0 != aEvent.Compare(iPrinterState->Des()))
				{
					delete iPrinterState;
					iPrinterState = NULL;
				}
				else break;
			}
			iPrinterState = aEvent.AllocL();
			changed = ETrue;
			break;
		case EPrinterStateReasons:
			if(iPrinterStateReasons)
			{
				if(0 != aEvent.Compare(iPrinterStateReasons->Des()))
				{
					delete iPrinterStateReasons;
					iPrinterStateReasons = NULL;
				}
				else break;
			}
			iPrinterStateReasons = aEvent.AllocL();
			changed = ETrue;
			break;
		case EJobIdList:
			if(iJobIdList)
			{
				if(0 != aEvent.Compare(iJobIdList->Des()))
				{	
					delete iJobIdList;
					iJobIdList = NULL;
				}
				else break;
			}
			iJobIdList = aEvent.AllocL();
			changed = ETrue;
			break;
		case EJobEndState:
			if(iJobEndState)
			{
				if(0 != aEvent.Compare(iJobEndState->Des()))
				{
					delete iJobEndState;
					iJobEndState = NULL;
				}
				else break;
			}
			iJobEndState = aEvent.AllocL();
			changed = ETrue;
			break;
		case EJobMediaSheetsCompleted:
			if(iJobMediaSheetsCompleted)
			{
			if(0 != aEvent.Compare(iJobMediaSheetsCompleted->Des()))
				{
					delete iJobMediaSheetsCompleted;
					iJobMediaSheetsCompleted = NULL;
				}
				else break;
			}
			iJobMediaSheetsCompleted = aEvent.AllocL();
			changed = ETrue;
			break;
		case EContentCompleteList:
			ContentCompleteL( aEvent, changed );
			break;
		case EJobAbortState:
			JobAbortStateL( aEvent, changed );
			break;
		default:
			break;
	}
	return changed;
}
void CJobStateEvents::ContentCompleteL ( const TDesC8 &aEvent, TBool& aChanged )
	{
	if(iContentCompleteList)
	{
		if(0 != aEvent.Compare(iContentCompleteList->Des()))
		{
			delete iContentCompleteList;
			iContentCompleteList = NULL;
		}
		else return;
	}
	iContentCompleteList = aEvent.AllocL();
	aChanged = ETrue;

	}

void CJobStateEvents::JobAbortStateL ( const TDesC8 &aEvent, TBool& aChanged )
	{
	if(iJobAbortState)
		{
			if(0 != aEvent.Compare(iJobAbortState->Des()))
			{
				delete iJobAbortState;
				iJobAbortState = NULL;
			}
			else return;
		}
		iJobAbortState = aEvent.AllocL();
		aChanged = ETrue;
	}

// -----------------------------------------------------------------------------
// CJobStateEvents::~CJobStateEvents
// C++ destructor
// -----------------------------------------------------------------------------
//
CJobStateEvents::~CJobStateEvents()
{
	Delete(); 
}	

// -----------------------------------------------------------------------------
// CJobStateEvents::Delete
// -----------------------------------------------------------------------------
//
void CJobStateEvents::Delete()
{
	if(iPrinterState)
		delete iPrinterState;
	iPrinterState = NULL;
	
	if(iPrinterStateReasons)
		delete iPrinterStateReasons; 
	iPrinterStateReasons = NULL;
	
	if(iJobIdList)
		delete iJobIdList;
	iJobIdList = NULL;
	
	if(iJobEndState)
		delete iJobEndState; 
	iJobEndState = NULL;
	
	if(iJobMediaSheetsCompleted)
		delete iJobMediaSheetsCompleted;
	iJobMediaSheetsCompleted = NULL;
	
	if(iContentCompleteList)
		delete iContentCompleteList; 
	iContentCompleteList = NULL;
	
	if(iJobAbortState)
		delete iJobAbortState;
	iJobAbortState = NULL;
}	

// -----------------------------------------------------------------------------
// CJobStateEvents::InitL
// -----------------------------------------------------------------------------
//
void CJobStateEvents::InitL()
{
	Delete();
	iPrinterState = KNullDesC8().AllocL();          
	iPrinterStateReasons = KNullDesC8().AllocL();   
	iJobIdList = KNullDesC8().AllocL();             
	iJobEndState = KNullDesC8().AllocL();           
	iJobMediaSheetsCompleted = KNullDesC8().AllocL();
    iContentCompleteList = KNullDesC8().AllocL();
    iJobAbortState = KNullDesC8().AllocL();
}

// operators

// -----------------------------------------------------------------------------
// CJobStateEvents::operator==
// -----------------------------------------------------------------------------
//
TBool CJobStateEvents::operator==(const CJobStateEvents& a)
{
	if ( iPrinterState && a.iPrinterState )
	{
		if(0 != a.iPrinterState->Compare(iPrinterState->Des()))
			return EFalse;
	}else return EFalse;
	if( iPrinterStateReasons && a.iPrinterStateReasons )
	{
		if(0 != a.iPrinterStateReasons->Compare(iPrinterStateReasons->Des()))
			return EFalse;
	}
	if( iJobIdList && a.iJobIdList )
	{
		if(0 != a.iJobIdList->Compare(iJobIdList->Des()))
			return EFalse;
	}else return EFalse;
	if( iJobEndState && a.iJobEndState )
	{
		if(0 != a.iJobEndState->Compare(iJobEndState->Des()))
			return EFalse;
	}else return EFalse;
	if( iJobMediaSheetsCompleted && a.iJobMediaSheetsCompleted )
	{
		if(0 != a.iJobMediaSheetsCompleted->Compare(iJobMediaSheetsCompleted->Des()))
			return EFalse;
	}else return EFalse;
	if( iContentCompleteList && a.iContentCompleteList )
	{
		if(0 != a.iContentCompleteList->Compare(iContentCompleteList->Des()))
			return EFalse;
	}else return EFalse;
	if( iJobAbortState && a.iJobAbortState )
	{
		if(0 != a.iJobAbortState->Compare(iJobAbortState->Des()))
			return EFalse;
	}else return EFalse;
	
	return ETrue;	
}

// -----------------------------------------------------------------------------
// CJobStateEvents::operator!=
// -----------------------------------------------------------------------------
//
TBool CJobStateEvents::operator!=(const CJobStateEvents& a)
{
	return *this == a ? EFalse : ETrue;
}

// protected

// -----------------------------------------------------------------------------
// CJobStateEvents::CJobStateEvents
// C++ default constructor
// -----------------------------------------------------------------------------
//
CJobStateEvents::CJobStateEvents()
{
}	

// End Of File
