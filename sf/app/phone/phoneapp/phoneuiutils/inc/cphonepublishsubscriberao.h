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
* Description:  PhoneUI Publish and Subscriber (Publish & Subscribe).
*
*/


#ifndef __CPHONEPUBLISHSUBSCRIBERAO_H
#define __CPHONEPUBLISHSUBSCRIBERAO_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "mphonepubsubobserver.h"

// CLASS DECLARATION
/**
*  This class observes property variable changes.
*
*  @lib   Services.dll
*/

class CPhonePublishSubscriberAO : public CActive
    {
    public:

        /**
        * Two-phased constructor.
        */         
        static CPhonePublishSubscriberAO* NewL( 
            MPhonePubSubObserver* aPubSubObserver, 
            const TUid& aCategory, 
            const TUint aKey );

        /**
        * Destructor.
        */
        ~CPhonePublishSubscriberAO();

        /**
        * Subscribe to an RProperty
        * @param none
        * @return none
        */
        void Subscribe();

    public: // from CActive

        /**
        * @param none
        * @return none
        */    
        void RunL();
    
        /**
        * @param aError the error returned
        * @return error
        */
        TInt RunError( TInt aError );

        /**
        * @param none
        * @return none
        */    
        void DoCancel();

    public: // new methods

        /**
        * Return property category that is subscribed to
        * @param None
        */
        const TUid Category() const;

        /**
        * Return property key that is subscribed to
        * @param None
        */
        TUint Key() const;
    
    private:

        /**
        * C++ constructor.
        */
        CPhonePublishSubscriberAO( 
            MPhonePubSubObserver* aPubSubObserver, 
            const TUid& aCategory, 
            const TUint aKey );

    private:

        MPhonePubSubObserver*   iPubSubObserver; // NOT OWN
        RProperty               iProperty; 
        TUid                    iCategory;
        TUint                   iKey;
    };

#endif // __CPHONEPUBLISHSUBSCRIBERAO_H

// End of File
