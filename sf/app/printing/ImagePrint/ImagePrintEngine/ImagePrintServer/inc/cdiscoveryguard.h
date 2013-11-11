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


#ifndef CDISCOVERYGUARD_H
#define CDISCOVERYGUARD_H

#include <e32base.h>

class RImagePrintClient;
class MPrinterDiscoveryObserver;
class TDiscoveryGuardData;

class CDiscoveryGuard : public CActive
	{
	public:

		/**
         *	2-phase constructor
         *	
		 *	@param aClient client for communication with server process 
         *  @return new object
         */
		IMPORT_C static CDiscoveryGuard* NewL( RImagePrintClient& aClient );
		
		/**
         *	Destructor         
         */
		IMPORT_C ~CDiscoveryGuard();	
		
	public:
	
		/**
         *	Start printer discovery (and watching for discovery events)
         *	for one or several protocols
         *	
		 *	@param aObserver observer for notifications about discovery events
		 *	@param aProtocols OR:ed protocol ids
         *  @return error code
         */
		IMPORT_C TInt Guard( MPrinterDiscoveryObserver& aObserver, TUint aProtocols );

	private:

		CDiscoveryGuard( RImagePrintClient& aClient );
		void ConstructL();
		
	private: // from CActive
	
		void RunL();
		void DoCancel();
		
	private: // data
	
		RImagePrintClient& iClient;
		MPrinterDiscoveryObserver* iObserver;
		TDiscoveryGuardData* iData;
	};


#endif // CDISCOVERYGUARD_H

//  End of File
