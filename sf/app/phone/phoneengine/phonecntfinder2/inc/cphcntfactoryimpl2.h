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
* Description:
*
*/

#ifndef CPHCNTFACTORYIMPL2_H
#define CPHCNTFACTORYIMPL2_H

//  INCLUDES
#include    <e32base.h>
#include    <cphonecntfactory.h>



// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Factory class's implementation. Not for external use.
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
NONSHARABLE_CLASS( CPhCntFactoryImpl2 ) : public CPhCntFactory
    {
    public: // Constructors and destructors.

        /**
        * Two-phased constructor.
        *
        * @return new instance of the class.
        */
        static CPhCntFactoryImpl2* NewL();

        /**
        * Destructor.
        */
        ~CPhCntFactoryImpl2();

    public: // From base classes.



        /**
        * From CPhCntFactory
        */
        CPhCntMatcher* CreateContactMatcherL();


        /**
        * From CPhCntFactory
        */
        CBase* CreateProfileEngineNotifyHandlerL( 
            MProfileChangeObserver* aObserver );
 
            
        /**
         * Creates contact id instance.
         *
         * @since S60 v3.1
         * @return New instance.
         */
        //CPhCntContactId* CreateContactIdL();
        
        /**
         * From CPhCntFactory
         * Creates an contact id instance.
         *
         * @since S60 v3.1
         * @return New instance of CPhCntContactId
         */
        CPhCntContactId* CreateContactIdL( TContactItemId aContactId );
        
        /**
         * From CPhCntFactory
         *
         * @since S60 v3.2
         * @see CPhCntFactory
         */
        CPhCntContactId* CreateContactIdL( const TDesC8& aContactLink );

        
    private:

        /**
        * C++ constructor.
        */
        CPhCntFactoryImpl2();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();
        
  
    };


#endif      // CPHCNTFACTORYIMPL_H   

// End of File
