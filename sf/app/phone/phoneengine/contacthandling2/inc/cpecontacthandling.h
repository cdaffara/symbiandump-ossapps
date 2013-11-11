/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*              : CPEContactHandling class.
*
*/


#ifndef CPECONTACTHANDLING_H
#define CPECONTACTHANDLING_H

//  INCLUDES
#include "mpecontacthandling.h"
#include <e32base.h>
#include <cntdef.h> // TContactItemId
#include <pevirtualengine.h>

// CONSTANTS
//None

// MACROS
//None

// DATA TYPES
//None.

// FUNCTION PROTOTYPES
//None

// FORWARD DECLARATIONS
class CContactGroup;
class CContactItemFieldDef;
class MPEPhoneModelInternal;
class CPEContactMatch;

// CLASS DECLARATION

/**
*  Interface class for phone engine to fetch remote info from contact database or SIM
*
*  @lib ContactHandling.lib
*  @since Series60_4.0
*/
class CPEContactHandling
        : public CBase,
          public MPEContactHandling
    {
    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        */
        static CPEContactHandling* NewL( MPEPhoneModelInternal& aModel, 
                                              RFs& aFsSession );
        
        /**
        * Destructor.
        */
        virtual ~CPEContactHandling();

    public: // New functions
        
        /**
        * Synchronous method for searching contact information. This function call's FindContactInfoSyncL()
        * which can leave. This function call is Trapped so API function does not leave.
        * @param aCallId        call id from phone engine
        * @param aFindKey       defines the key to search the contact data with
        * @return Error code indicating the success of the search
        */
        TInt FindContactInfoSync( 
            const TInt aCallId,
            const TPEFindContactKey aFindKey );

        /**
        * Reroutes errorCodes to the Phone Model
        * @param aErrorCode is the errorCode
        * @return None.
        */
        void SendErrorMessage( TInt aErrorCode );

        /**
        * Reroutes messages to the Phone Model
        * @param aMessage is the message id
        * @param aCallId identifies the call this event is associated to
        */
        void SendMessage( MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
                          const TInt aCallId = KPECallIdNotUsed );
        
   protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        CPEContactHandling( MPEPhoneModelInternal& aModel, 
                        RFs& aFsSession );

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


        /**
        * Synchronous method for searching contact information
        * @param aCallId        call id from phone engine
        * @param aFindKey       defines the key to search the contact data with
        */

        void FindContactInfoSyncL(
            const TInt aCallId, 
            const TPEFindContactKey aFindKey );

        /**
        * Synchronous method for searching contact information from Contactdb
        * @param aFindKey       defines the key to search the contact data with
        * @return None
        */
        void FindContactInfoSyncFromContactDbL(
            const TInt aCallId, 
            const TPEFindContactKey aFindKey ) const;

    private:    // Data
        // Reference to MPEPhoneModelInternal , also object owner
        MPEPhoneModelInternal&          iModel;
        // ContactMatcher
        CPEContactMatch*                iContactMatcher;
        // handle to a file server session
        RFs& iFsSession;
        
    };

#endif      // CPECONTACTHANDLING_H
            
// End of File
