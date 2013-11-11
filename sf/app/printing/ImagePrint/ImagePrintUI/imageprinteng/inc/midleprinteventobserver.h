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


#ifndef MIDLEPRINTEVENTOBSERVER_H
#define MIDLEPRINTEVENTOBSERVER_H

#include <e32def.h>
#include <e32std.h>

enum TImgPrintUSBState
	{
	EUSBUndefined = 0,
	EUSBConnected,
    ENOUSBEvent
	};

//  CLASS DEFINITION
/**
 *
 * Interface to notify UI about idle state events
 *
 */
class MIdlePrintEventObserver
    {	
	public:		// Abstract methods

       /**
        *  Called when usb printer is disconnected
        */
        virtual void UsbPrinterDisconnected() = 0;           

	   /**
        *  Called when usb printer is connected
        */
        virtual void UsbPrinterConnected() = 0;

		/**
		* Notifies about USB state changes. Method is also used for other events
        * @param aState information about usb state
		*/ 
		virtual void UsbStateChanged( TImgPrintUSBState aState, TInt aError ) = 0;

		virtual void ReadyToContinue() = 0;
		
		/**
		 * Notifies that there's happened an asynchronous leave in server process,
		 * which in practice means that any ongoing printing should be cancelled
		 *
		 * @param aError leave code
		 */
		virtual void AsynchronousServerLeave( TInt aError ) = 0;
    };

#endif // MIDLEPRINTEVENTOBSERVER_H

//  End of File
