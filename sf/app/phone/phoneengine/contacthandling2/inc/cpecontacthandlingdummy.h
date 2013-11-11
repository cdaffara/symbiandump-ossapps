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
*              : CPEContactHandlingDummy class.
*
*/


#ifndef CPECONTACTHANDLINGDUMMY_H
#define CPECONTACTHANDLINGDUMMY_H

//  INCLUDES
#include "mpecontacthandling.h"
#include <e32base.h>
#include <mpephonemodelinternal.h>

// CLASS DECLARATION

/**
*  Dummy interface to substitute real contact handling subject when neccessary
*
*  @lib ContactHandling.lib
*  @since Series60_4.0
*/
class CPEContactHandlingDummy
        : public CBase,
          public MPEContactHandling
    {
    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPEContactHandlingDummy* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPEContactHandlingDummy();

    public: // New functions
        
        /**
        * Synchronous method for searching contact information. This function call's FindContactInfoSyncL()
        * which can leave. This function call is Trapped so API funktion dosn't leave.
        * @param aCallId        call id from phone engine
        *        aFindKey       defines the key to search the contact data with
        *        aSearchFrom    search from key
        * @return Error code indicating the success of the search
        */
        TInt FindContactInfoSync( 
                    TInt aCallId,
                    TPEFindContactKey aFindKey );

        /**
        * Reroutes errorCodes to the Phone Model
        * @param aErrorCode is the errorCode
        * @return None.
        */
        void SendErrorMessage( TInt aErrorCode );

        /**
        * Reroutes messages to the Phone Model
        * @param aMessage is the message id
        * @return None.
        */
        void SendMessage( MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );
        
   protected:  // New functions
        /**
        * C++ default constructor.
        */
        CPEContactHandlingDummy();

    };

#endif      // CPECONTACTHANDLINGDUMMY_H
            
// End of File
