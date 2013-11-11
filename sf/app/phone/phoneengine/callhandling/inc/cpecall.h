/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPECall object
*
*/


#ifndef CPECALL_H
#define CPECALL_H

//INCLUDES
#include <pevirtualengine.h>
#include <mccecall.h>
#include <mccecallobserver.h>
#include <mpecall.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None. 

// FORWARD DECLARATIONS
class MPEMessageSender;
class MCCECallObserver;

// CLASS DECLARATION

/**
*  Create call handling requests to Call Command object
*  @lib CallHandling.lib
*  @since Series60_4.0
*/
class CPECall 
        : public CBase,
          public MPECall
    {
    protected: //Constructors and descructor
        /**
        * C++ default constructor.
        */
        IMPORT_C CPECall( MPEMessageSender& aOwner );

        /**
        * Destructor.
        */
        virtual IMPORT_C ~CPECall();
              
    public:    //New functions
        
        /**
        * Reroutes call messages from other objects to call handling object
        * @param aMessage The message code of the message
        */    
        virtual void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );
        
        /**
        * Reroutes error messages from other objects to call handling object
        * @param aErrorCode The error code of the failed operation
        */        
        IMPORT_C virtual void SendErrorMessage( const TInt aErrorCode );
        
        /**
        * Returns call error code and cause.
        * @param aErrorCode, the error code stored after previous unsuccessful request
        */
        virtual void GetErrorCode( TInt& aErrorCode ) const;
        
        /**
        * Sets call status information
        * @param aCallState the status of the current call
        */    
        virtual void SetCallState( const TPEState aCallState );

        /**
        * Returns call status information
        * @return the status of the current call
        */    
        virtual TPEState GetCallState() const;

        /**
        * Sets call ID number
        * @param aCallId, the call id number
        */    
        virtual void SetCallId( const TInt aCallId );
        
        /**
        * Returns call id number
        * @return the call id number
        */    
        virtual TInt GetCallId() const;

    protected:

        // Reference to owner of this class
        MPEMessageSender& iOwner;
        // The id number of the call
        TInt iCallId;
        // The current state of the call
        TInt iCallState;
        // Includes possible error code of the call.
        TInt iErrorCode;
    };

#endif // CPECALLDATA_H

//    End of File
