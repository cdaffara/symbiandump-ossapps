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


#ifndef CSERVERJOBGUARD_H
#define CSERVERJOBGUARD_H

#include <e32base.h>

#include "imageprint.h"
#include "tjobguarddata.h"

class CImagePrintBody;
class TMessageWrp2;

class CServerJobGuard : public CBase, public MPrintEventObserver
	{
	public:

		/**
		 *	2-phase constructor
		 *
		 *	@param aEngine printing engine
		 *	@return new object
		 */
		static CServerJobGuard* NewL( CImagePrintBody& aEngine );
		
		/**
		 *	Destructor		 
		 */
		~CServerJobGuard();

	private: // from MPrintEventObserver
		
		void PrintJobProgressEvent(TInt aStatus, TInt aPercentCompletion, TInt aJobStateCode);
		void PrintJobErrorEvent(TInt aError, TInt aErrorStringCode);
		void PrinterStatusEvent(TInt aError, TInt aErrorStringCode);
		void PreviewImageEvent(TInt aFsBitmapHandle);
		void ShowMessageL(TInt aMsgLine1Code, TInt aMsgLine2Code);
		TBool AskYesNoQuestionL(TInt aMsgLine1Code, TInt aMsgLine2Code);
		const TDesC& AskForInputL(TInt aMsgLine1Code, TInt aMsgLine2Code);
		
	public:
	
		/**
		 *	Prepare print job by copying image file name into internal buffer
		 *
		 *	@param aMessage message contains file name of one image. 
		 */
		void PrepareL( TMessageWrp2& aMessage );
		
		/**
		 *	Create print job and set this object as print job events observer
		 *
		 *	@param aMessage message contains printer id
		 */
		TInt CreateL( TMessageWrp2& aMessage );
		
		/**
		 *	Continue waiting for print job events
		 *
		 *	@param aMessage message completed upon event arrival
		 */
		void ContinueCreateL( TMessageWrp2& aMessage );
		
		/**
		 *	Stop waiting for print job events. Cancel waiting message
		 */
		void Stop();

	private:

		CServerJobGuard( CImagePrintBody& aEngine );
		void ConstructL();
		void Process( TInt aErr = KErrNone );
		void DoProcessL();
		void DoPreProcessL( const TJobGuardData& aData );
		
	private: // data
	
		CImagePrintBody& iEngine;
		TMessageWrp2* iMessage;
		RArray<TJobGuardData> iBuffer;
		RPointerArray<TDesC> iImages;
		TBool iRequestActive;
	};


#endif // CSERVERJOBGUARD_H

//  End of File
