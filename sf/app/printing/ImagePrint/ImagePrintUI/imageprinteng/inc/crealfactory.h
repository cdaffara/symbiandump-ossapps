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


#ifndef CREALFACTORY_H
#define CREALFACTORY_H

#include <e32def.h>

#include "ciffactory.h"
#include "rimageprintclient.h"

class CImagePrint;
class CDiscoveryManager;
class CPrintEventCatcher;
class CPrintJobManager;
class CSettingsManager;
class MIdlePrintEventObserver;
class CIdlePrintEventCatcher;

//  CLASS DEFINITION
/**
 *
 * Factory class for creating concrete interface classes for the UI
 *
 */
class CRealFactory
    : public CIFFactory
    {
    public:     // Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aEngine  Pointer to Image Printing Engine class instanace
         *
         *  @return Initialized instance of this factory
         */
        static CRealFactory* NewL( CImagePrintEngine* aEngine );

        /**
         *  Destructor
         */
        virtual ~CRealFactory();

    protected:      // Constructors and destructors

        /**
         *  Constructor
         */
        CRealFactory( CImagePrintEngine* aEngine );

        /**
         *  2nd phase constructor
         */
        void ConstructL( );
        
    public:         // Methods derived from CIFFactory
                
        // The factory methods that return each of the interfaces
        virtual MDiscovery* DiscoveryIF();
        virtual MPrintSettings* SettingsIF();
        virtual MPrintJob* PrintJobIF();
        virtual MIdlePrintEventRegisterIF* IdlePrintEventRegisterIF();
        virtual RImagePrintClient ImagePrintClient();
        virtual TBool IsApplicationAlreadyInUse();
		virtual MPrintEventObserver* PrintEventObserverIF();        
		virtual MPrintEventRegisterIF* PrintEventRegisterIF();
        // Method for restarting Image Print Engine
        virtual  void RestartEngine();

    public:         // New methods

        /**
         *  Returns Engine
         *
         *  @return Engine
         */
        CImagePrint* Engine();

        /**
         *  Returns if the printing has been activated
         *  
         *  @return state of printing
         */
        TBool IsPrinting() const;

    protected:      // Data

        // The actual printin engine
        CImagePrint* iImageServer;

        // Discovery manager
        CDiscoveryManager* iDiscoveryMgr;

        // Print job manager
        CPrintJobManager* iPrintJobMgr;

        // Settings manager
        CSettingsManager* iSettingsMgr;

        // Print event catcher - listens to events coming from the server
        CPrintEventCatcher* iEventCatcher;

        // Idle event catcher for idle events
        CIdlePrintEventCatcher* iIdleEventCatcher;
       
        RImagePrintClient iImagePrintClient;
    };

#endif //  CREALFACTORY_H

//  End of File
