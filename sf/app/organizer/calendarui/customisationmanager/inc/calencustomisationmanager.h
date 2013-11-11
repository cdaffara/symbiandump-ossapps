/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Customisation manager.
*
*/


#ifndef CALENCUSTOMISATIONMANAGER_H
#define CALENCUSTOMISATIONMANAGER_H

#include <qglobal.h>					// Q_DECL_EXPORT macro
#include <e32base.h>                    // CBase
#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include <ecom/ecom.h>                  // RImplInfoPtrArray


#ifdef  CALENCUSTOMISATIONMANAGER_DLL
#define CUSTOMISATIONMANAGER_EXPORT Q_DECL_EXPORT
#else
#define CUSTOMISATIONMANAGER_EXPORT Q_DECL_IMPORT
#endif

// forward declarations


class CCalenCustomisation;
class MCalenCommandHandler;
class MCalenServicesFactory;
class MCalenServices;
class HbWidget;
class HbMenu;

/**
* CCalenCustomisationManager manages customisations for calendar.
*
*/
class CUSTOMISATIONMANAGER_EXPORT CCalenCustomisationManager : public CBase,
                                   public MCalenNotificationHandler
    {
public:  // Types
    /**
     * Struct showing the uid and availaibility of known plugins.
     */
    struct TCalenPluginAvailability
        {
        public:
            TUid    iUid;
            TBool   iEnabled;
        };

    // Array of plugin availabilities.
    typedef RArray<CCalenCustomisationManager::TCalenPluginAvailability> CCalenPluginStatusArray;

public:  // Construction and destruction
    /**
     * Returns a new CCalenCustomisationManager object.
     */
    static CCalenCustomisationManager* NewL(
                                       MCalenServicesFactory& aFactory,                                       
									   									 MCalenServices& aServices );

    static TBool PluginInfoFinder( const TUid* aUid  , 
								const CImplementationInformation&  aArrayItem);
    
    /**
     * Destructor.
     */
    ~CCalenCustomisationManager();

public:  // New functions
 

    /**
     * Gets a info bar from the appropriate customisation.
     */
    HbWidget* Infobar();
    
    /**        
     * Returns info bar text
     */
    QString* InfobarTextL();
    
    /**
      * Offer the menu to Plugins to customisation
      * @param aMenu Where the customisation apply to
      */
    void OfferMenu(HbMenu* aHbMenu);
    
    /**
     * Get a command handler for a specific command
     * @param aCommand Command to find a command handler for
     */
    MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );
    
    /**
     * Returns a reference to an array of active plugins uids
     */
    const RArray<TUid>& ActivePlugins() const;

    /**
     * Disables the given plugin. Does nothing if the plugin can't be found.
     */
    void SetPluginAvailabilityL( TUid aPluginUid, TBool aEnabled );

    /**
     * Returns a const reference to the plugin information array.
     */
    const RImplInfoPtrArray& PluginInfoArray() const;

    /**
     * When a plugin is disabled it can not be unloaded until
     * any views etc it has provided have been deleted. This function
     * is called when it is safe to unload the plugin.
     */
    void UnloadPluginsL( const RArray<TUid>& aPlugins);    
   
     
public:  // from MCalenNotificationHandler
    void HandleNotification(const TCalenNotification aNotification );

private: // Constructors
    CCalenCustomisationManager( MCalenServicesFactory& aFactory,
                                MCalenServices& aServices );

    void ConstructL();

private: // New functions
    void LoadPluginL( TUid aPluginUid );

    void DoImmediatePluginLoadingL();    

    CCalenCustomisation* FindPluginL( TUid aUid );

    static TBool PluginAvailabilityFinder( const TUid* aUid, 
                                 const TCalenPluginAvailability& aArrayItem );
                                     
    void AddPluginL( CCalenCustomisation* aPlugin, TUid aUid );     

    void CreateActivePluginListL();
    
    void LoadPluginsBasedOnVariantL();

    void EnablePluginL( TUid aPluginUid );

    void DisablePluginL( TUid aPluginUid );    
    
    void HandleNotificationL(TCalenNotification aNotification);

private:
    /**
     * Internal class used as a wrapper around a plugin
     * and its uid.
     */
    class CPluginInfo : public CBase
        {
        public: // Constructor & Destructor
            CPluginInfo( CCalenCustomisation* aPlugin, TUid aUid );
            ~CPluginInfo();

        public: // New functions
            static TBool Identifier( const TUid* aUid,
                                     const CPluginInfo& aArray );
            CCalenCustomisation& Plugin();
            TUid Uid() const;
            void Disable( TBool aDisabled );
            TBool IsDisabled();

        private: // Data
            CCalenCustomisation* iPlugin;
            TUid iUid;
            TBool iDisabled;
        };

private: // Data
    
    RImplInfoPtrArray iPluginInfo;
    MCalenServicesFactory& iServicesFactory;
    // takes ownership of the plugins
    RPointerArray<CPluginInfo> iPlugins;
    RArray<TUid> iActivePlugins;
    RArray<TUid> iRomBasedPlugins;   
    MCalenServices& iServices;
    TUid iInfoBarProviderUid;    
    TBool iPluginsEnabledDisabled;
    RArray<TUid> iDefferedUnloadPluginList; 
    
    };

#endif    // CALENCUSTOMISATIONMANAGER_H

// End of File
