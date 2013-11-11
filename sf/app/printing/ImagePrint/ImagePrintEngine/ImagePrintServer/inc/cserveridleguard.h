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


#ifndef CSERVERIDLEGUARD_H
#define CSERVERIDLEGUARD_H

#include <e32base.h>

#include "imageprint.h"
#include "tidleguarddata.h"

class TEvent;
class CImagePrintBody;
class TMessageWrp2;

class CServerIdleGuard : public CBase, public MIdleObserver
	{
	public:
		
		/**
		 *	2-phase constructor
		 *
		 *	@param aEngine printing engine
		 *	@return new object
		 */
		static CServerIdleGuard* NewL( CImagePrintBody& aEngine );
		
		/**
		 *	Destructor		 
		 */
		~CServerIdleGuard();

	private: // from MIdleObserver
		
		void StatusEvent(const TEvent &aEvent, TInt aError, TInt aMsgCode);
		
	public:
	
		/**
		 *	Start and continue waiting for idle events
		 *
		 *	@param aMessage message completed upon event arrival
		 */
		void Guard( TMessageWrp2& aMessage );
		
		/**
		 *	Stop waiting for idle events. Cancel waiting message
		 */
		void Stop();

	private:

		CServerIdleGuard( CImagePrintBody& aEngine );
		void ConstructL();
		void DoStatusEventL( const TEvent &aEvent, TInt aError, TInt aMsgCode );
		void Process( TInt aErr = KErrNone );
		void DoProcessL();
		
	private: // data
	
		CImagePrintBody& iEngine;
		TMessageWrp2* iMessage;
		RArray<TIdleGuardData> iBuffer;
		TBool iRequestActive;
	};


#endif // CSERVERIDLEGUARD_H

//  End of File
