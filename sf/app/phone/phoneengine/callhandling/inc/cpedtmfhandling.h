/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPEDtmfHandling class.
*
*/



#ifndef CPEDTMFHANDLING_H
#define CPEDTMFHANDLING_H

//INCLUDES
#include <pevirtualengine.h>
#include <mccedtmfobserver.h>

//CONSTANTS
//None.

// MACROS
//None.

// DATA TYPES
//None.

// FUNCTION PROTOTYPES
//None.

// FORWARD DECLARATIONS
class CPECallHandling;
class MCCEDtmfInterface;
class CCCE;

// CLASS DECLARATION

/**
*  Handles dtmf commands.
*
*  @lib callhandling.lib
*  @since Series60_4.0
*/
class CPEDtmfHandling : public CBase,
                        public MCCEDtmfObserver
    {
    public: //Constructors and descructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPEDtmfHandling* NewL( 
            CPECallHandling& aOwner, 
            MCCEDtmfInterface& aDtmfInterface );

        /**
        * Destructor.
        */
        virtual IMPORT_C ~CPEDtmfHandling();

    protected:  //Constructors and descructor
        /**
        * C++ default constructor.
        */
        IMPORT_C CPEDtmfHandling( 
            CPECallHandling& aOwner, 
            MCCEDtmfInterface& aDtmfInterface );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        virtual IMPORT_C void ConstructL();

    public: // From MCCEDtmfObserver
                                       
        /**
        * All the occurred DTMF events are notified through this interface
        *
        * @since S60 3.2
        * @param aEvent Event code
        * @param aError Error code
        * @param aTone tone 
        * @return none
        */
        IMPORT_C void HandleDTMFEvent( const MCCEDtmfObserver::TCCEDtmfEvent aEvent, 
                                       const TInt aError, 
                                       const TChar aTone );
                   
    public: // New functions

        /**
        * Cancels Dtmf String sending
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        void CancelDtmfString();

        /**
        * Allows a client to continue the sending of a dtmf string when it was
        * stopped by use of 'w' char in the string
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        void ContinueDtmfSending();

        /**
        * Send dtmf string to the etel 
        * @since Series60_4.0
        * @param  aTones dtmf string
        * @return None.
        */
        TInt SendDtmfString( const TDesC& aTones );

        /**
        * allows a client initiate the sending of a DTMF tone
        * @since Series60_4.0
        * @param  aTone dtmf tone
        * @return None.
        */
        TInt StartDtmfTone( const TChar& aTone ) const;

        /**
        * Allows a client to cancel the sending of a dtmf string when it was 
        * stopped by use of 'w' char in the string
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        void StopDtmfSending();

        /**
        * allows a client terminate the sending of a DTMF tone
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        TInt StopDtmfTone() const;

    protected:
    
        // CPECallHandling object which owns this Sender
        CPECallHandling& iOwner;
        // MCCEDtmfInterface& object which make dtmf requests to the mmetel.
        MCCEDtmfInterface& iDtmfInterface;
};

#endif // CPEDTMFHANDLING_H

// End of File
