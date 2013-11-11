/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   The header file of the CClockEComListener class.
*
*/

#ifndef __CLOCK_ECOM_LISTENER_H__ 
#define __CLOCK_ECOM_LISTENER_H__ 

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class MClockEComObserver;
class REComSession;

// Class declaration
/** 
* @class CClockEComListener
* @brief An instance of class CClockEComListener which listens for changes in the ECOM registry.
* @dll clockecomlistener.dll
*/
class CClockEComListener : public CActive
    {
    public:             // New functions
    
        /**
        * @brief Creates a CClockEComListener object.
        * @param aEComObserver Observer of type MClkEComObserver
        * @return CClockEComListener*
        */
        IMPORT_C static CClockEComListener* NewL( MClockEComObserver& aClockEComObserver );                                         
        
        /**
        * @brief Destroy the object and release all memory objects.
        */
        IMPORT_C ~CClockEComListener();
        
        /**
        * @brief Notifies the observer whenever the ECOM registry information changes
        * @param aImplInfo The implementation information of the plugin which was changed.
        * @param aInstalled ETrue if a plugin has been installed, EFalse otherwise.
        */
        void NotifyEComObserverL( CImplementationInformation* aImplInfo, TBool aInstalled );

   protected:           // Functions from base classes
        
        /**
        * @brief From CActive. Called when the ECOM registry changes.
        */
        void RunL();

        /**
        * @brief From CActive. Make NITZ inactive.
        */
        void DoCancel();
        
    private:            // New functions
        
        /** 
        * @brief Default C++ constructor.
        */
        CClockEComListener( MClockEComObserver& aClockEComObserver );
        
        /** 
        * @brief Symbian OS 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * @brief Starts the active object to listen to ECOM registry change notifications
        */
        void Start();

        /**
        * @brief Stops the active object
        */
        void Stop();
        
        /** 
        * @brief Checks if a plugin is installed or removed.
        */
        void CheckForEcomPluginInstallUninstallL();
        
        /**
        * @brief Checks if a new implementation has been added in the ECOM registry.
        * @param aUid The plugin UID which is to be checked.
        */
        TBool IsPluginInstalled( const TUid aUid );
        
        /**
        * @brief Checks if a new implementation has been removed in the ECOM registry.
        * @param aUid The plugin UID which is to be checked.
        * @param aPluginArray The array of plugins which have been implemented.
        */
        TBool IsPluginUninstalled( const TUid aUid, RImplInfoPtrArray aPlugInArray );
        
        /**
        * @brief Checks if an upgrade or downgrade of the existing plugins happened.
        * @param aPluginArray The array of plugins which have been implemented.
        */
        void CheckForPluginUpgradeDowngradeL( const RImplInfoPtrArray& aPlugInArray );
        
    private:            // Data
    
        /**
        * @var iClockEComObserver 
        * @brief An object of type MClockEComObserver.
        */
        MClockEComObserver&             iClockEComObserver;
        
        /**
        * @var iEComSession 
        * @brief An object of type REComSession.
        */
        REComSession*                   iEComSession;
        
        /**
        * @var iImplArray
        * @brief Is an array of type CImplementationInformation
        */
        RImplInfoPtrArray               iImplArray;
    };

#endif  // __CLOCK_ECOM_LISTENER_H__

// End of file
 
