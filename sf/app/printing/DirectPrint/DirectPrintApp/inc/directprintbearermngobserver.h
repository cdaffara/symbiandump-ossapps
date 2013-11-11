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
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTBEARERMNGOBSERVER_H__
#define __DIRECTPRINTBEARERMNGOBSERVER_H__

// System includes
#include <e32base.h>

// Class declaration
/**
 *  Bearer manager observer mix-in class
 *  more_complete_description
 */
class MDirectPrintBearerMngObserver
	{
public:
	/** The ID of bearer event. */
	enum TDirectPrintBearerMngEvents
		{
		EDirectPrintBearerMngErrorConnectBearer,
		EDirectPrintBearerMngConnectedBearer,
		// add any events, if needed

		// end of event
		EDirectPrintBearerMngEventEnd
		};

public:
	/**
	 * Handles user events.
	 * @param aEvent The ID of the event.
	 * @param aError Error code.
	 * @param aParam Parameter.
	 */
	virtual void HandleBearerMngEventL(
									TDirectPrintBearerMngEvents aEvent,
									TInt aError,
									const TDesC& aParam) = 0;

	/**
	 * Handles user events.
	 * @param aStatus The ID of the status.
	 * @param aPercentCompletion Percentage of progress.
	 * @param aJobStateCode The ID of job states.
	 */
	virtual void PrintJobProgressEvent( TInt aStatus, 
									TInt aPercentCompletion, 
									TInt aJobStateCode ) = 0;
									
	/**
	 * Handles user events.
	 * @param aError The ID of the error.
	 * @param aErrorStringCode The ID of error strings.
	 */
	virtual void PrintJobErrorEvent( TInt aError, 
									TInt aErrorStringCode ) = 0;
									
	/**
	 * Handles user events.
	 * @param aError The ID of the error.
	 * @param aErrorStringCode The ID of error strings.
	 */
	virtual void PrinterStatusEvent( TInt aError, 
									TInt aErrorStringCode ) = 0;
									
	/**
	 * Handles user events.
	 * @param aFsBitmapHandle The Handle of the bitmap.
	 */
	virtual void PreviewImageEvent( TInt aFsBitmapHandle ) = 0;
	
	/**
	 * Show messages.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 */
	virtual void ShowMessageL( TInt aMsgLine1Code, 
									TInt aMsgLine2Code ) = 0;
									
	/**
	 * Show Yes/No question.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 * @return ETrue is Yes. EFalse is No.
	 */
	virtual TBool AskYesNoQuestionL( TInt aMsgLine1Code, 
									TInt aMsgLine2Code ) = 0;
									
	/**
	 * input.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 * @return Inputed strings.
	 */
	virtual const TDesC& AskForInputL( TInt aMsgLine1Code, 
									TInt aMsgLine2Code ) = 0;
	};

#endif // __DIRECTPRINTBEARERMNGOBSERVER_H__
// End of file
