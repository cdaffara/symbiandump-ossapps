/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the
*              : CPEContactHandlingProxy class.
*
*/



#ifndef CPECONTACTHANDLINGPROXY_H
#define CPECONTACTHANDLINGPROXY_H

//  INCLUDES
#include "mpecontacthandling.h"
#include <e32base.h>
#include <mpephonemodelinternal.h>

// CLASS DECLARATION

/**
*  This proxy class provides for the access to contact information services.
*  The construction of contact handling component is two-fold:
*  1) CreateFirstPhaseL() creates the proxy object along with cnt handling dummy
*  2) CreateSecondPhaseL() creates the real subject with capability to utilize
*     contact database.
*
*  @lib ContactHandling.lib
*  @since Series60_4.0
*/
class CPEContactHandlingProxy
        : public CBase,
          public MPEContactHandling
    {
    public:
        /**
        * Create contact handling proxy and dummy implementation
        * @return pointer to log handling proxy object, i.e. the handle to
        * access log handling services
        */
        IMPORT_C static MPEContactHandling* CreateFirstPhaseL();

        /**
        * Create contact handling sub system
        * This method creates the actual log handling implementation which
        * replaces the dummy component in use. The phone model should call this
        * function as all the critical sub systems already enable emergency call
        * capabilities.
        * @param aModel, handle to the owner of this object and phone engine
        * @param aFsSession, handle to the shared file server session
        */
        IMPORT_C void CreateSecondPhaseL( 
            MPEPhoneModelInternal& aModel, 
            RFs& aFsSession );
      
        /**
        * Destructor.
        */
        virtual ~CPEContactHandlingProxy();

    public:
        /**
        * Synchronous method for searching contact information.
        * @param aCallId        call id from phone engine
        *        aFindKey       defines the key to search the contact data with
        *        aSearchFrom    search from key
        * @return possible Symbian OS error code
        */
        IMPORT_C TInt FindContactInfoSync( 
            const TInt aCallId,
            const TPEFindContactKey aFindKey );

    protected:
        /**
        * C++ default constructor.
        */
        CPEContactHandlingProxy();

        /**
        * Constructor, second phase. 
        */
        void ConstructL();


    private:    // Data
        // Handle to contact handling implementation
        MPEContactHandling* iContactHandling;
        // Handle to contact handling dummy implementation
        MPEContactHandling* iContactHandlingDummy;
    };

#endif      // CPECONTACTHANDLINGPROXY_H
            
// End of File
