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


#ifndef MDISCOVERYOBSERVER_H
#define MDISCOVERYOBSERVER_H

#include <e32def.h>
#include <e32std.h>

/**
 *
 * Interface used to notify UI about the found devices and error
 *
 */
class MDiscoveryObserver
    {
    public:     // Enumerations
        
        enum TPrinterType
            {
            EBPP,
            EMMC,
            EUSB,
            EOPP,
            EWLAN,
            ENOPRINTER
            };

	public:		// Abstract methods

        /**
         *  Notification of new print device
         *
         *  @param aText    Textual description of the new print device.
         *                  Ownership is not transferred
         *  @param aUid     Uid that is associated with the print device
         *  @param aCached  Is the printer cached or not?
         *  @param aType    Type of the printer
         *  @param aVendor  Printer vendor
         */
        virtual void NotifyNewPrintDeviceL( HBufC* aText, TInt aUid, 
                                           TBool aCached, TPrinterType aType,
                                           TUint aVendor ) = 0;

        /**
         *  Notification of error / cancellation of the sequence.
         *  Device discovery process is cancelled
         *
         *  @param aErrCode Error code  
         */
        virtual void DiscoveryError( TInt aErrCode ) = 0;

        /**
         *  Notifies completion of the device discovery.
         */
        virtual void DeviceDiscoveryCompleted() = 0;

        /**
         *  Removes cached printer as it is not found
         *
         *  @param aUid     Uid that is associated with the print device
         */
        virtual void RemoveCachedPrinterL( TInt aUid ) = 0;
    };

#endif //  MDISCOVERYOBSERVER_H

//  End of File
