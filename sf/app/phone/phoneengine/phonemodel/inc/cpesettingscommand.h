/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPESettingsCommand.h 
*                object.
*
*/



#ifndef CPESETTINGSCOMMAND_H
#define CPESETTINGSCOMMAND_H

//  INCLUDES
#include <e32base.h>
#include <nwdefs.h>
#include "pepanic.pan"
#include "pevirtualengine.h"
#include <msssettingsobserver.h>


// FORWARD DECLARATIONS
class MPEPhoneModelInternal;
class CPESettingsObserver;

// CLASS DECLARATION

/**
*  CPESettingsCommand observer SSSettings changes
*
*  @lib phoneenginegsm
*  @since Series_60 4.0
*/
NONSHARABLE_CLASS( CPESettingsCommand ) : public CBase,
                           public MSSSettingsObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPESettingsCommand* NewL( 
            MPEPhoneModelInternal& aModel );
        
        /**
        * Destructor.
        */
        virtual ~CPESettingsCommand( );

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
        void GetCugL( TInt aCugIndex );

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
        CPESettingsCommand( MPEPhoneModelInternal& aModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data

        //MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        //Pointer to instance of Settings
        RSSSettings iSSSettings;

    };

#endif      // CPESETTINGSCOMMAND_H
            
// End of File
