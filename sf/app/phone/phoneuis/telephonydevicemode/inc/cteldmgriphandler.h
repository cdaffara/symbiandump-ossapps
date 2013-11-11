/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Concrete session class for Telephony Device Mode
*
*/



#ifndef CTELDMGRIPHANDLER_H
#define CTELDMGRIPHANDLER_H

//  INCLUDES

#include    <e32property.h>
#include    <e32base.h>


// FORWARD DECLARATIONS

class MPhoneDeviceModeObserver;
class MTelDMAccessory;
class CTelDMCommandHandler;

// CLASS DECLARATION

/**
*  Concrete session class for Telephony Device Mode
*
*  @since 3.0
*/
 
NONSHARABLE_CLASS( CTelDMGripHandler ) : public CActive
    {
    public: // Constructors and destructors.

        /**
        * Two-phased constructor.
        */ 
        static CTelDMGripHandler* NewL( CTelDMCommandHandler& aCommandHandler );
        
        /**
        * Destructor.
        */
        virtual ~CTelDMGripHandler();

    public: // New functions
          
    public: // Functions from base classes
   
    private: // Functions from base classes  
   
        /**
        * @see CActive::RunL.
        */
        void RunL();

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();
     
    private:
        
        /**
        * Handle notification request
        */      
        void HandleChange();
        
        /**
        * Start notification.
        */
        void IssueRequest();
        
        /**
        * Constructor.
        */
        CTelDMGripHandler( CTelDMCommandHandler& aCommandHandler );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();
        
        /**
        * Check phone settings
        * @return Status of settings value
        */
        TBool IsSliderCallAnswer() const;
        
        /**
        * Check phone settings
        * @return Status of settings value
        */        
        TBool IsSliderCallEnd()const;
        
        /**
        * Get integer value of setting
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        * @param aValue the value associated with the central repository ID
        * @return
        */
        TInt GetInt( 
            const TUid& aUid, 
            const TUint aId,
            TInt& aValue ) const;
            
    private:
        // Owned Publish & Subscribe instance.
        RProperty iProperty;
      
        // Reference to commander
        CTelDMCommandHandler& iCommandHandler;       
        

    };
 
#endif      // CTELDMGRIPHANDLER_H  

// End of File
