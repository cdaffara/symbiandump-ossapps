/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#ifndef DIRECTPRINTSERVERJOBGUARD_H
#define DIRECTPRINTSERVERJOBGUARD_H

#include <e32base.h>

#include "imageprint.h"
#include "directprintjobguarddata.h"

class CDirectPrintBody;
class TDpMessage;

class CDirectPrintServerJobGuard : public CBase, public MPrintEventObserver
	{
	public:

		/**
		 *	2-phase constructor
		 *
		 *	@param aEngine printing engine
		 *	@return new object
		 */
		static CDirectPrintServerJobGuard* NewL( CDirectPrintBody& aEngine );
		
		/**
		 *	Destructor
		 */
		~CDirectPrintServerJobGuard();

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
		void PrepareL( TDpMessage& aMessage );
		
		/**
		 *	Create print job and set this object as print job events observer
		 *
		 *	@param aMessage message contains printer id
		 */
		TInt CreateL( TDpMessage& aMessage );
		
		/**
		 *	Continue waiting for print job events
		 *
		 *	@param aMessage message completed upon event arrival
		 */
		void ContinueCreateL( TDpMessage& aMessage );
		
		/**
		 *	Stop waiting for print job events. Cancel waiting message
		 */
		void Stop();

	private:

		CDirectPrintServerJobGuard( CDirectPrintBody& aEngine );
		void ConstructL();
		void Process( TInt aErr = KErrNone );
		void DoProcessL();
		void DoPreProcessL( const TDirectPrintJobGuardData& aData );
		
	private: // data
	
		CDirectPrintBody& iEngine;
		TDpMessage* iMessage;
		RArray<TDirectPrintJobGuardData> iBuffer;
		RPointerArray<TDesC> iImages;
		TBool iRequestActive;
	};


#endif // DIRECTPRINTSERVERJOBGUARD_H

//  End of File
