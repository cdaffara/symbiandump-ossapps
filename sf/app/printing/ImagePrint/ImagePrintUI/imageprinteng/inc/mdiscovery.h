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


#ifndef MDISCOVERY_H
#define MDISCOVERY_H

#include <e32def.h>

class MDiscoveryObserver;

/**
 *
 * Interface to use the device discovery functionality of Image Print Engine
 *
 */
class MDiscovery
    {
	public:		// Abstract methods

        /**
         *  Initializes and starts the device discovery. Engine notifies
         *  about the found devices through device observer interface.
         *  Asynchronous service.
         *
         *  @param aNotifier    an object that implements the observer IF
         *  @param aProtocol    Which protocol should be used in discovery,
         *                      zero if all of them
         */
        virtual void StartDeviceDiscoveryL( 
            MDiscoveryObserver* aNotifier, TUint aProtocol ) = 0;

        /**
         *  Returns supported protocols
         *  
         *  @return supported protocols
         */
        virtual TUint SupportedProtocols() = 0;

        /**
         *  Cancels the device discovery process. The discovery is cancelled
         *  and the observer receives error code KErrCancel.
         *  Asynchronous service.
         */
        virtual void CancelDeviceDiscoveryL() = 0;

        /**
         *  Chooses the printer based on its UID and locks it for the 
         *  application.
         *  Synchronous service.
         *
         *  @param aPrinterUid  Uid of the chosen printer 
         *  @return Boolean value. If ETrue, the search will continue (user
         *          has selected 'Wireless Printers' or such)
         */
        virtual TInt ChoosePrinterL( TInt aPrinterUid ) = 0;
        
        /**
         *  PictBridge USB mode checker
		 *	@return TInt value more than 0, if PictBridge USB mode is on; 
		 *          0 if off; negative value if error.
         */
        virtual TInt IsPictBridgeMode() = 0;
        
        /**
         * Removes the current discover observer.
         * Used for special cases where the observer object 
         * is destroyed while still being the current observer.
         */
        virtual void RemoveDiscoveryObserver() = 0;
    };

#endif  //  MDISCOVERY_H

//  End of File
