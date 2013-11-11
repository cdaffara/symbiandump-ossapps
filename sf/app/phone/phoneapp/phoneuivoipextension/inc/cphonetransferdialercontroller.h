/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Unattended transfer dialer customization
*     controller
*
*/


#ifndef CPHONETRANSFERDIALERCONTROLLER_H_
#define CPHONETRANSFERDIALERCONTROLLER_H_

// INCLUDES
#include <aknbutton.h>
#include "mphonedialercontroller.h"

// FORWARD DECLARATIONS
class CAknToolbar; 

/**
*  VoIP unattended transfer dialer customization
*/
NONSHARABLE_CLASS( CPhoneTransferDialerController ): public CBase,
                                       public MPhoneDialerController
    {
    public:      

        static CPhoneTransferDialerController* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPhoneTransferDialerController();

    public: // From MDialerController
    
        
        /**
        * Initializes the controller. 
        * @param    aToolbar    CAknToolbar instance.
        * @since    S60 v5.1
        */ 
        void InitializeL( CAknToolbar& aToolbar );
        
        /**
        * Returns Cba resource id
        * @return Resource Id of the softkeys
        * @since S60 v5.1
        */
        TInt CbaResourceId() const;
        
        /**
        * Returns Menu resource id
        * @return Id of the menu resource
        * @since S60 v5.1
        */
        TInt MenuResourceId() const;
        
        /**
        * Returns number entry prompt text
        * @return Reference to text to be shown on number entry
        * @since S60 v5.1
        */
        const TDesC& NumberEntryPromptTextL();
        
        /**
        * Handles the number entry empty event
        * @param aEmpty ETrue if numberentry is empty
        * @since S60 v5.1
        */
        void HandleNumberEntryIsEmpty( TBool aEmpty );
        
        /**
        * Sets visibility of buttons created by the implementation
        * @param aShow ETrue if numberentry is empty
        * @since S60 v5.1
        */
        void ShowButtons( TBool aShow );
        
    private:
    
        /**
         * Creates instance of CAKnButton
         * @param aNormalIconId Button normal icon id
         * @param aNormalMaskId Mask id
         * @param aTooltipText Reference to tooltip text
         * @param aSkinIconId Skin icon id
         * @return Pointer to created button instance
         */
        CAknButton* CreateButtonLC( TInt aNormalIconId,
                                    TInt aNormalMaskId,
                                    const TDesC& aTooltipText, 
                                    const TAknsItemID& aSkinIconId ) const; 
        
        /**
         * Returns tool tip for specific command
         * @param aCommandId Command id which the tooltip text 
         * is needed
         * @return Pointer to descriptor containing tooltip text
         */
        HBufC* GetTooltipTextL( TInt aCommandId ) const; 
        
        TAknsItemID SkinId( TInt aIconIndex ) const;

    protected:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneTransferDialerController();
            
    private: // Data
    
        // Pointer to CAknToolbar, Not own
        CAknToolbar* iToolbar;
        
        // ETrue if number entry input field is empty
        TBool iNumberEntryIsEmpty;
        
        // Number entry prompt text ("Address:")
        HBufC* iNumberEntryPromptText;
        
        // ETrue if initialized 
        TBool iIsInitialized;
      
    };

#endif /*CPHONETRANSFERDIALERCONTROLLER_H_*/
