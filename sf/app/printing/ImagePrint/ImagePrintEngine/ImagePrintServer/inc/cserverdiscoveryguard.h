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


#ifndef CSERVERDISCOVERYGUARD_H
#define CSERVERDISCOVERYGUARD_H

#include <e32base.h>

#include "imageprint.h"
#include "tdiscoveryguarddata.h"

class CImagePrintBody;
class TMessageWrp2;

class CServerDiscoveryGuard : public CBase, public MPrinterDiscoveryObserver
	{
	public:

		/**
		 *	2-phase constructor
		 *
		 *	@param aEngine printing engine
		 *	@return new object
		 */
		static CServerDiscoveryGuard* NewL( CImagePrintBody& aEngine );
		
		/**
		 *	Destructor		 
		 */
		~CServerDiscoveryGuard();

	private: // from MPrinterDiscoveryObserver
		
		void FoundPrinterL(const TPrinter& aPrinterInfo);			
		void DiscoveryStatusL(TInt aStatus, TInt aErrorCode, TInt aErrorStringCode);			
		void RemovePrinterL(const TPrinter& aPrinterInfo);
		
	public:
	
		/**
		 *	Start discovery and set this object as discovery events observer
		 *
		 *	@param aMessage message contains OR:ed protocol ids
		 */
		TInt StartL( TMessageWrp2& aMessage );
		
		/**
		 *	Continue waiting for discovery events
		 *
		 *	@param aMessage message completed upon event arrival
		 */
		void ContinueL( TMessageWrp2& aMessage );
		
		/**
		 *	Stop waiting for discovery events. Cancel waiting message
		 */
		void Stop();

	private:

		CServerDiscoveryGuard( CImagePrintBody& aEngine );
		void ConstructL();
		void ProcessL();
		
	private: // data
	
		CImagePrintBody& iEngine;
		TMessageWrp2* iMessage;
		RArray<TDiscoveryGuardData> iBuffer;
		TBool iRequestActive;
	};


#endif // CSERVERDISCOVERYGUARD_H

//  End of File
