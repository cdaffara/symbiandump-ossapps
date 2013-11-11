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


#ifndef CJOBGUARD_H
#define CJOBGUARD_H

#include <e32base.h>

class RImagePrintClient;
class MPrintEventObserver;
class TJobGuardData;

class CJobGuard : public CActive
	{
	public:

		/**
         *	2-phase constructor
         *	
		 *	@param aClient client for communication with server process 
         *  @return new object
         */
		IMPORT_C static CJobGuard* NewL( RImagePrintClient& aClient );
		
		/**
         *	Destructor         
         */
		IMPORT_C ~CJobGuard();	
		
	public:
	
		/**
         *	Create print job and start watching for print job events
         *	
         *	@param aPrinterID printer id
         *	@param aImages array of image file names to be printed
		 *	@param aObserver observer for notifications about print job events
         *  @return error code
         */
		IMPORT_C TInt Guard( TInt aPrinterID, RPointerArray<TDesC>& aImages, MPrintEventObserver& aObserver );

	private:

		CJobGuard( RImagePrintClient& aClient );
		void ConstructL();
		
	private: // from CActive
	
		void RunL();
		void DoCancel();
		
	private: // data
	
		RImagePrintClient& iClient;
		MPrintEventObserver* iObserver;
		TJobGuardData* iData;
	};


#endif // CJOBGUARD_H

//  End of File
