/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony engine TLS container.
*
*/



#ifndef VTENGUTILS_H
#define VTENGUTILS_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CVtEngInitializer;
class CVtEngHandlerContainer;
class CVtEngDebug;
class CVtEngStateManager;
class CVtEngSettings;
class CVtEngEventManager;
class CVtEngCRProxy;
class CVtEngAccessoryHandler;
class CVtEngSATClient;
class CVtEngDeviceLockMonitor;
class CVtEngExtensions;
class CVtEngMdtrMessageListener;
class CVtEngMdtrCommandSender;

// CLASS DECLARATION

/**
*  TLS container for pointers to essential objects.
*
*  @lib videoteleng.lib
*  @since Series 60 2.6
*/
NONSHARABLE_CLASS( CVtEngUtility ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVtEngUtility* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVtEngUtility();

    public: // New functions
    
        /**
        * Creates SAT listener for updates.
        */
        void StartSATListenL();
        
        /**
        * Returns engine initializer.
        * @return initializer
        */
        inline CVtEngInitializer& Initializer() const;

        /**
        * Returns engine utility.
        * @return utility
        */
        static CVtEngUtility* EngineUtils();

        /**
        * Returns state manager.
        * @return state manager
        */
        static CVtEngStateManager* StateManager();

        /**
        * Returns event manager.
        * @return event manager
        */
        static CVtEngEventManager& EventManager();

        /**
        * Returns Central repository proxy.
        * @return ref to CR proxy
        */
        static CVtEngCRProxy& CRProxy();

        /**
        * Returns accessory listener.
        * @return accessory handler
        */
        static CVtEngAccessoryHandler& AccessoryHandler();

        /**
        * Returns settings.
        * @return ref to settings
        */
        static CVtEngSettings& Settings();
        
        /**
        * Returns mediator command sender.
        * @return ref to command sender
        */
        static CVtEngMdtrCommandSender& MediatorCommands();

        /**
        * Prepares the utility
        * @param aHandlers handler container
        * @param aStateManagerPtr state manager pointer ref
        * @param aEventManagerPtr event manager pointer ref
        * @param aExtensionPtr extension container pointer
        */
        void PrepareL( 
            CVtEngHandlerContainer& aHandlers,
            CVtEngStateManager*& aStateManagerPtr,
            CVtEngEventManager*& aEventManagerPtr,
            CVtEngExtensions* aExtensionPtr );

        /**
        * Notifies device lock to UI.
        */
        void NotifyLockState();
        
        /**
        * Returns Extension container.
        * @since S60 v3.2
        * @return pointer to extensions
        */
        static CVtEngExtensions* Extensions();
        /**
        * Starts listening mediator messages.
        */
        void StartMediatorListenerL();
        
    private:

        /**
        * C++ default constructor.
        */
        CVtEngUtility();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data

        // Initializater for the engine
        CVtEngInitializer*   iEngineInit;

        // State manager pointer
        CVtEngStateManager** iStateManager;

        // Settigs
        CVtEngSettings*      iSettings;

        // Event manager pointer
        CVtEngEventManager** iEventManager;

        // Central repository interface
        CVtEngCRProxy*       iCRProxy;

        // Accessory handler
        CVtEngAccessoryHandler* iAccessoryHandler;
        
        // SAT refresh listener
        CVtEngSATClient* iSATClient;
        
        // Monitor for device lock state
        CVtEngDeviceLockMonitor* iLockMonitor;
        
        // Extension pointer
        CVtEngExtensions* iExtensions;
        // For receiving Mediator commands and events        
        CVtEngMdtrMessageListener* iMdtrMessageListener;

        // For sending Mediator commands
        CVtEngMdtrCommandSender*   iMdtrCommandSender;
        
    };

#include "VtEngUtils.inl"

#endif      // VTENGUTILS_H
            
// End of File
