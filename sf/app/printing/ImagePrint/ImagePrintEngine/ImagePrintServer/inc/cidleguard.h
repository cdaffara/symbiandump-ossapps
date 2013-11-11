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


#ifndef CIDLEGUARD_H
#define CIDLEGUARD_H

#include <e32base.h>

class RImagePrintClient;
class MIdleObserver;
class TIdleGuardData;

class CIdleGuard : public CActive
	{
	public:

		/**
         *	2-phase constructor
         *	
		 *	@param aClient client for communication with server process 
         *  @return new object
         */
		IMPORT_C static CIdleGuard* NewL( RImagePrintClient& aClient );
		
		/**
         *	Destructor         
         */
		IMPORT_C ~CIdleGuard();	
		
	public:
	
		/**
         *	Start watching for idle events
         *	
		 *	@param aObserver observer for notifications about idle events
         */
		IMPORT_C void Guard( MIdleObserver* aObserver );

	private:

		CIdleGuard( RImagePrintClient& aClient );
		void ConstructL();
		
	private: // from CActive
	
		void RunL();
		void DoCancel();
		
	private: // data
	
		RImagePrintClient& iClient;
		MIdleObserver* iObserver;
		TIdleGuardData* iData;
	};


#endif // CIDLEGUARD_H

//  End of File
