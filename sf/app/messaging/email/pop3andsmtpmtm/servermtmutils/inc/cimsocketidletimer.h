// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CIMSOCKETIDLETIMER_H__
#define __CIMSOCKETIDLETIMER_H__

#include <e32base.h>

class CImTextServerSession;


/**
This class provides a timer for inactivity on the socket when either sending or receiving data
When the timer expires, iObserver object is informed and the socket is closed
*/

class CImSocketIdleTimer : public CTimer
/**
@internalComponent
*/

	{
public:

/**
	//cat			Construction
	Intended Usage	:	Static factory constructor. Uses two phase construction and 
						leaves nothing on the CleanupStack.
	@since			7.0s
	@return			A pointer to the newly created CImSocketIdleTimer object. 
	@pre 			None
	@post			A fully constructed and initialised CImSocketIdleTimer object.
	
 */
	static CImSocketIdleTimer* NewL(CImTextServerSession& aObserver);
	
/**
	//cat			Destruction
	@fn				~CImSocketIdleTimer()
	Intended Usage	:	Destructor.
	@since			7.0s	
 */
	virtual ~CImSocketIdleTimer();

private:	// methods from CActive
	virtual void RunL();

private:

/**
	//cat			Construction
	@fn             CImSocketIdleTimer(CImTextServerSession& aObserver);
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			7.0s
	@param			aObserver is the CImTextServerSession object that needs to be alerted when
					the timer expires
*/
	CImSocketIdleTimer(CImTextServerSession& aObserver);

private:
	//the object that reads/writes to the socket
	CImTextServerSession&	iObserver;
	};

#endif	//  __CIMSOCKETIDLETIMER_H__
