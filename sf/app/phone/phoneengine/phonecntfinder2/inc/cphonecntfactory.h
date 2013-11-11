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

#ifndef CPHCNTFACTORY_H
#define CPHCNTFACTORY_H

//  INCLUDES
#include    <e32base.h>
#include    <cntdef.h>

// FORWARD DECLARATIONS

class CPhCntMatcher;
class MProfileChangeObserver;
class CPhCntContactId;

// CLASS DECLARATION

/**
*  Factory class for creating phonebook related classes.
*
*  @lib PhoneCntFinder
*  @since 1.0
*/
class CPhCntFactory : public CBase
    {
    public:


        /** 
        * Creates an instance of Contact Matcher
        *
        * @return Contact Matcher object. Ownership is transferred.
        */
        virtual CPhCntMatcher* CreateContactMatcherL() = 0;

        /**
        * Creates an instance of profile engine notify handler.
        * @param aObserver observer to be notified of profile changes.
        * @return new instance.
        */
        virtual CBase* CreateProfileEngineNotifyHandlerL( 
            MProfileChangeObserver* aObserver ) = 0;
        

            
        /**
         * Creates contact id instance.
         *
         * @since S60 v3.1
         * @return New instance.
         */
        //virtual CPhCntContactId* CreateContactIdL() = 0;
            
        /**
         * Creates an contact id instance.
         *
         * @since S60 v3.1
         * @return New instance of CPhCntContactId
         */
        virtual CPhCntContactId* CreateContactIdL( TContactItemId aContactId ) = 0;

        /**
         * Create and contact id instance from contact link.
         *
         * @since S60 v3.2
         */
        virtual CPhCntContactId* CreateContactIdL( const TDesC8& aContactId ) = 0;    

 
    };

/**
* Creates CPhCntFactory. Position 1 in DLL's lookup table.
* @return Reference to CPhCntFactory object. Ownership is transferred.
*/
IMPORT_C CPhCntFactory* CreateCntFactoryL();

#endif      // CPHCNTFACTORY_H
            
// End of File
