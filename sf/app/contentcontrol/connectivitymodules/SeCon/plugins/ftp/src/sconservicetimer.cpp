/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Profile Plug-in Service Timer implementation
*
*/


// INCLUDE FILES

#include "sconservicetimer.h"
#include "sconftpplugin.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSConServiceTimer::CSConServiceTimer( CSConFTPplugin* aEngine, TInt aValue )
// Constuctor
// -----------------------------------------------------------------------------
//
CSConServiceTimer::CSConServiceTimer( CSConFTPplugin* aEngine, TInt aValue )
	: CActive( EPriorityStandard ), iEngine( aEngine ), iValue( aValue ) {}

// -----------------------------------------------------------------------------
// CSConServiceTimer::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConServiceTimer::ConstructL()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::ConstructL()" );
	// create a thread-relative timer
	User::LeaveIfError( iTimer.CreateLocal() );
	LOGGER_LEAVEFN( "CSConServiceTimer::ConstructL()" );
	}

// -----------------------------------------------------------------------------
// CSConServiceTimer::~CSConServiceTimer()
// Destructor
// -----------------------------------------------------------------------------
//
CSConServiceTimer::~CSConServiceTimer()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::~CSConServiceTimer()" );
	iTimer.Close();
	LOGGER_LEAVEFN( "CSConServiceTimer::~CSConServiceTimer()" );	
	}

// -----------------------------------------------------------------------------
// CSConServiceTimer::~CSConServiceTimer()
// Starts the timer
// -----------------------------------------------------------------------------
//
void CSConServiceTimer::StartTimer()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::StartTimer()" );
	if( !IsActive() )
		{
		LOGGER_WRITE( "CSConServiceTimer::StartTimer() : !IsActive()" );
		iTimer.After( iStatus, TTimeIntervalMicroSeconds32( iValue ) );
		iEngine->iSessionActive = ETrue;
		SetActive();
		LOGGER_WRITE( "CSConServiceTimer::StartTimer() : SetActive()" );
		}
	LOGGER_LEAVEFN( "CSConServiceTimer::StartTimer()" );
	}

// -----------------------------------------------------------------------------
// CSConServiceTimer::StopTimer()
// StopTimer()
// -----------------------------------------------------------------------------
//
void CSConServiceTimer::StopTimer()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::StopTimer()" );
	iEngine->iSessionActive = ETrue;	
	LOGGER_WRITE( "CSConServiceTimer::StopTimer() : iSessionActive = ETrue" );
	LOGGER_LEAVEFN( "CSConServiceTimer::StopTimer()" );	
	}

// -----------------------------------------------------------------------------
// CSConServiceTimer::DoCancel()
// Cancels the timer
// -----------------------------------------------------------------------------
//
void CSConServiceTimer::DoCancel()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::DoCancel()" );
	iTimer.Cancel();
	LOGGER_LEAVEFN( "CSConServiceTimer::DoCancel()" );
	}

// -----------------------------------------------------------------------------
// CSConServiceTimer::RunL()
// Executed when timer is triggered
// -----------------------------------------------------------------------------
//
void CSConServiceTimer::RunL()
	{
	LOGGER_ENTERFN( "CSConServiceTimer::RunL()" );
	iEngine->iSessionActive = EFalse;
	LOGGER_WRITE( "CSConServiceTimer::RunL() : iSessionActive = EFalse" );
	iEngine->iStartTimer = EFalse;
	LOGGER_WRITE( "CSConServiceTimer::RunL() : iStartTimer = EFalse " );
	LOGGER_LEAVEFN( "CSConServiceTimer::RunL()" );
	}
	
// End of file
