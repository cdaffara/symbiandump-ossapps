// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// A reusable timer class together with an observer interface
// 
//

#ifndef __CIMAPOBSERVABLETIMER_H__
#define __CIMAPOBSERVABLETIMER_H__

#include <e32base.h>

// Forward declaration
class CImapObservableTimer;

/**
To use CImapObservableTimer, an object must implement this interface, which informs the user
of the timer when a requested timer event has occurred.
@internalTechnology
@prototype
*/
class MImapTimerObserver
	{
public:
	/**
	The implementaton of this method will be called when a requested timer event occurs.
	Use the CTimer base class methods to request a timer event.
	@param aSourceTimer	A reference to the timer object that called this method.  This makes it helps an implementation 
						of MImapTimerObserver to use more than one CObserveableTimer at once, as the implementaiton will know 
						which of its CImapObservableTimer objects called this methiod.
	*/
	virtual void OnTimerL(const CImapObservableTimer& aSourceTimer) =0;
	};
	
/**
A timer object that invokes the supplied observer's OnTimer() method whenever a requested timer event occurs.
Use the CTimer base class methods to request and cancel timer events.
@internalTechnology
@prototype
*/
class CImapObservableTimer : public CTimer
	{
public:
	IMPORT_C static CImapObservableTimer* NewL(MImapTimerObserver& aObserver);

private:
	CImapObservableTimer(MImapTimerObserver& aObserver);

private:	
	// CTimer implementation
	void RunL();

private:
	MImapTimerObserver& iObserver;
	};

#endif // __CIMAPOBSERVABLETIMER_H__
