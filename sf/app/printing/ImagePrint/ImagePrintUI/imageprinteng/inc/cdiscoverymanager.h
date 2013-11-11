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


#ifndef CDISCOVERYMANAGER_H
#define CDISCOVERYMANAGER_H

#include <e32def.h>
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "mdiscovery.h"
#include "mdiscoveryobserver.h"
#include "imageprint.h"

class CRealFactory;
class CIFFactory;
class CImagePrint;

/**
 *
 *  Manages the device discovery part.
 *
 */
class CDiscoveryManager
    : public CBase,
      public MDiscovery,
      public MPrinterDiscoveryObserver
    {
    public:     // Construction and destruction

        /**
         *  Two-phase construction
         *
         *  @param aFactory Pointer to factory
         *  @return Initialized instance of the class
         */
        static CDiscoveryManager* NewL( CIFFactory* aFactory );
        static CDiscoveryManager* NewLC( CIFFactory* aFactory );

        /**
         *  Destructor
         */
        virtual ~CDiscoveryManager();

    private:    // Construction and destruction

        /**
         *  Constructor
         *
         *  @param aFactory Pointer to factory
         */
        CDiscoveryManager( CIFFactory* aFactory );

        /**
         *  Second-phase constructor
         */
        void ConstructL();

    private:    // Methods derived from MDiscovery

        virtual void StartDeviceDiscoveryL(
            MDiscoveryObserver* aNotifier, TUint aProtocol );
        virtual void CancelDeviceDiscoveryL();
        virtual TInt ChoosePrinterL( TInt aPrinterUid );
        virtual TUint SupportedProtocols();
        virtual TInt IsPictBridgeMode();
        virtual void RemoveDiscoveryObserver();

    private:    // Methods derived from MPrinterDiscoveryObserver

        virtual void FoundPrinterL( const TPrinter& aPrinterInfo );
	    virtual void DiscoveryStatusL( TInt aStatus, TInt aErrorCode,
            TInt aErrorStringCode );
        virtual void RemovePrinterL( const TPrinter& aPrinterInfo );

    private:    // Enumerations

        // Different states of device discovery
        enum TDiscoveryState
            {
            EDiscovering,
            EDoneDiscovery,
            ECancellingDiscovery
            };

    private:    // Data

        // Pointer to factory object
        CIFFactory* iFactory;

        // Pointer to the observer
        MDiscoveryObserver* iDiscoveryObserver;

        // Flag to indicate if the discovery or cancelling of discovery is on-going
        TBool iOnGoingDiscovery;
        TBool iCancelling;

        // Chosen printer UID
        TInt iPrinterUid;

        // Synchronous wait system for cancellation
        CActiveSchedulerWait iWait;

        // UID for MMC printer (if available)
        TInt iMMCUid;
    };

#endif  //  CDISCOVERYMANAGER_H

//  End of File
