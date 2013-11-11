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
* Description:  Manager number entry for state
*
*/


#ifndef CPONENUMBERENTRYMANAGER_H
#define CPONENUMBERENTRYMANAGER_H

//INCLUDES
#include <e32base.h>
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamkeyevent.h"

class CPhoneUiCommandManager;
class MPhoneViewCommandHandle;
class MPhoneStateMachine;
class MPhoneCustomization;
class CPhoneState;
class CEikonEnv;

/**
 * Class contains helper functions for NE content
 *
 * @lib phoneuicontrol
 * @since S60 v9.1
 */
NONSHARABLE_CLASS( CPhoneNumberEntryManager ) :  public CBase
    {
    public:
        
        /**
        * Creates new instance of CPhoneNumberEntryManager
        * @param aState: Handle to cphonestate.
        * @param aStateMachine: a state machine
        * @param aViewCommandHandle: Handle to the PhoneUIView.
        * @param aCustomization: Handle to customization.
        * @param aUiCommandManager: Handle to cphoneuicommandmanager.
        * @return CPhoneNumberEntryManager* object  
        */
        static CPhoneNumberEntryManager* NewL(
                    CPhoneState* aState,
                    MPhoneViewCommandHandle& aViewCommandHandle,
                    MPhoneStateMachine& aStateMachine,
                    MPhoneCustomization* aCustomization,
                    CPhoneUiCommandManager& aUiCommandManager );
        /**
        * Destructor.
        */
        virtual ~CPhoneNumberEntryManager();    
        
    public:

        /**
        * Stores the number entry content to the cache
        */
        IMPORT_C void StoreNumberEntryContentL();
        

        /**
        * Check if number entry content is stored
        * @param None
        * @return boolean value indicating that number entry content is stored
        */
        IMPORT_C TBool IsNumberEntryContentStored();
        
        /**
        * Clears the number entry content cache
        */        
        IMPORT_C void ClearNumberEntryContentCache();
        
        /**
        * Set Number Entry visibility.
        * @param aVisible ETrue if numberentry is wanted to be shown
        *                 (Note ETrue will set NE CBA's)
        *                 EFalse if numberentry isnt wanted to be shown
        *                 (Note EFalse doesnt affect to CBA's)
        */
        IMPORT_C void SetNumberEntryVisibilityL( TBool aVisible );
        
        /**
        * Check if number entry is used
        * @return boolean value indicating that number entry is used
        */
        IMPORT_C TBool IsNumberEntryUsedL() const;
        
        /**
        * Check if number entry is visible
        * @return boolean value indicating that number entry is visible
        */
        IMPORT_C TBool IsNumberEntryVisibleL() const;
        
        /**
         * Returns phone number from the phone number entry.
         * @return  Phone number
         */
        IMPORT_C HBufC* PhoneNumberFromEntryLC() const;
        
        /**
         * Internal number entry handling methods.
         */
        IMPORT_C void NumberEntryClearL() const;
        
        /**
         * Returns request status, if return value is true then number
         * entry visibility was set according to aVisibility if false
         * then nothing was done.
         */
        IMPORT_C TBool SetVisibilityIfNumberEntryUsedL( TBool aVisibility );
        
        /**
         * Request causes number entry removal only if visibility is false.
         */
        IMPORT_C void RemoveNumberEntryIfVisibilityIsFalseL();
        
        /**
         * Cache is cleared if there is some content in store.
         */
        IMPORT_C void ClearNumberEntryContentCacheIfContentStored();
        
        /**
        * Create call if in numberentry more that 2 number and Send key
        * Send manual control sequence
        * if 1-2 number in numberentry and Send key
        */
        IMPORT_C void CallFromNumberEntryL();
        
        /**
        * Checks whether customized dialer view is active,
        * @return ETrue if customized dialer is active
        */
        IMPORT_C TBool IsCustomizedDialerVisibleL() const;
    
    private:
    
        /**
        * C++ default constructor.
        */
        CPhoneNumberEntryManager(
                CPhoneState* aState,
                MPhoneViewCommandHandle& aViewCommandHandle,
                MPhoneStateMachine& aStateMachine,
                MPhoneCustomization* aCustomization,
                CPhoneUiCommandManager& aUiCommandManager );
        
        /**
        * Symbian constructor
        */
        void ConstructL();
        
    private:
        
        CPhoneState* iState;
        MPhoneViewCommandHandle& iViewCommandHandle;
        MPhoneStateMachine& iStateMachine;
        MPhoneCustomization* iCustomization;
        CPhoneUiCommandManager& iUiCommandManager;
                
        /**
        * Cache for the number entry content
        */
        HBufC* iNumberEntryContent;
        
        /**
        * Internal variable for EikonEnv to avoid
        * use of static system calls
        */
        CEikonEnv& iEnv;

    };


#endif // CPONENUMBERENTRYMANAGER_H
