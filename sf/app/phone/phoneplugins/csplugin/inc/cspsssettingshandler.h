/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class 
*                CSPSsSettingsHandler.
*
*/



#ifndef CSPSSSETTINGSHANDLER_H
#define CSPSSSETTINGSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <msssettingsobserver.h>


// FORWARD DECLARATIONS
class MCCPSsObserver;

// CLASS DECLARATION

/**
*  CSPSsSettingsHandler observer SSSettings changes
*
*  @lib phoneenginegsm
*  @since Series_60 4.0
*/
class CSPSsSettingsHandler : public CBase,
                           public MSSSettingsObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSPSsSettingsHandler* NewL( const MCCPSsObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CSPSsSettingsHandler( );

    public: // New functions
        
        /**
        * GetALSLineL
        * @param aALSLine, Als line information to be updated
        */
        void GetALSLineL( RMobilePhone::TMobilePhoneALSLine& aALSLine );

        /**
        * SetALSLineL
        * @param aALSLine, Als line information to set
        */
        void SetALSLineL( RMobilePhone::TMobilePhoneALSLine& aALSLine );
        
        /**
        * GetClirL
        * @param aClir, Clir information to be updated
        */
        void GetClirL( RMobileCall::TMobileCallIdRestriction& aClir );
        
        /**
        * GetCugL
        * @param aCugIndex, CUG index
        */
        void GetCugL( TInt& aCugIndex );
        
        /**
         * GetDefaultCugL
         * Get CUG default value.
         */
        void GetDefaultCugL(TInt& aDefCug );
        
        /**
         * IsValueValidCugIndex
         * Is CUG index in allowed range. 
         */
        TBool IsValueValidCugIndex( const TInt& aValue ) const;

    public: // Functions from MSSSettingsObserver

        /*
        * Observer functoin 
        * @param aSetting Current setting
        * @param aNewValue New value for the setting
        */
        virtual void PhoneSettingChanged( 
            TSSSettingsSetting aSetting, 
            TInt aNewValue );

    private:

        /**
        * Constructor.
        */
        CSPSsSettingsHandler( const MCCPSsObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Call register Als notify after idle callback
        */                 
        void RegisterAfterIdle();               

        /**
        * CIdle CallBack method
        */  
        static TInt CIdleCallBack( TAny* aAny );

        /**
        * Register to notify ALS line changed
        */  
        void Register();

    private: // Data

        /**
         * MCCPSsObserver is used to send message to the CCE/phone.
         */
        MCCPSsObserver& iObserver;
        //Pointer to instance of Settings
        RSSSettings iSSSettings;
        // Idle object to register ALS notify.
        CIdle* iRegisterAlsNofify;

    };

#endif      // CSPSSSETTINGSHANDLER_H
