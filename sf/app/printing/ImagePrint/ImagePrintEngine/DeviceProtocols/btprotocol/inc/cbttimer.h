/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef CBTTIMER_H
#define CBTTIMER_H

#include <e32base.h>

namespace
	{
	// 0,1 second for AOs to finish their RunL methods
	const TUint KPrintingErrorDelay = 100000;

	// 60 seconds for discovery
	const TUint KDiscoveryDelay = 60000000;

	// 5 seconds for data sending percents
	const TUint KSentPercentsDelay = 3000000;

	// 5 seconds for data sending percents
	const TUint KPrinterResponseTimeout = 5000000;

	}

class MBtTimerObserver
	{
	public:
		virtual void HandleTimeoutL() = 0;
	};

class CBtTimer : public CTimer
	{
	public:
		static CBtTimer* NewL( MBtTimerObserver& aObserver );
		~CBtTimer();		
		void Start(TUint aDelay);		

	private:		
		CBtTimer( MBtTimerObserver& aObserver );
		void ConstructL();
		void RunL();
		
	private:
		MBtTimerObserver& iObserver;				
	};


#endif // CBTTIMER_H

//  End of File
