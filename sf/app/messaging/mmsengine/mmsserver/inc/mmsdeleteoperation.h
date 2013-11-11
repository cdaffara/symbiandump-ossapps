/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for sending a delete request to network
*
*/



#ifndef MMSDELETEOPERATION_H
#define MMSDELETEOPERATION_H

//  INCLUDES

#include "mmsbaseoperation.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsHeaders;
class CMmsEncode;
class CMmsDecode;
class CLogClient;
class CLogViewEvent;
class CLogEvent;
class CMmsLog;

// CLASS DECLARATION

/**
*  State machine for sending forward request
*/
class CMmsDeleteOperation :public CMmsBaseOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFs file system session
        * @param aLogClient log client for writing into log
        * @param aLogEvent log event for writing entries into log
        */
        static CMmsDeleteOperation* NewL( RFs& aFs,  CMmsSettings* aMmsSettings );
        
        /**
        * Destructor.
        */
        virtual ~CMmsDeleteOperation();

    public: // New functions
                
        /**
        * Start the state machine.
        * @param aDeleteType specifies the scope of the notification delete
        * @param aSelection list of message entry ids to be handled.
        * @param aServerEntry CMsvServerEntry pointer from CMmsServer
        * @param aSettings MMSC settings (entry points)
        * @param aService current MMS service id
        * @param aStatus TRequestStatus of the calling active object
        */
        void StartL(
            TMmsDeleteOperationType aDeleteType,
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus );

    public: // Functions from base classes

        /**
        * Overriding the StartL of the base operation
        * This should not be called with DeleteOperation.
        * Leaves with Not implemented
        */
        void StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus );

    protected:  // New functions
        
    protected:  // Functions from base classes

        /**
        * DoCancel
        */
        virtual void DoCancel();
        
    private:

        /**
        * Default constructor
        */
        CMmsDeleteOperation( RFs& aFs );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CMmsSettings* aMmsSettings );

        // We don't need to prohibit copy & assignment constructors here,
        // as they are in CBase already

    private: // Methods representing states that are overridden

        /**
        * Encode one message
        */
        void EncodePDUL();

        /**
        * Connect to GPRS AP
        */
        void ConnectToIAPL();

        /**
        * Initialize the session
        */
        void InitializeSessionL();

        /**
        * Submit transaction
        */
        void SubmitTransactionL();

        /**
        * Decode Response
        */
        void DecodeResponseL();

        /**
        * Update the trigger entry status
        */
        void UpdateEntryStatusL();

    private: // Other private methods

        /**
        * DeleteNotificationEntryL deletes the entry
        */
        void DeleteNotificationEntryL();

        /**
        * MarkSelectionAsFailed clears the flags of all the entries in iSelection
        */
        void MarkSelectionAsFailed();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        TMmsDeleteOperationType iDeleteType; // specifies the type of the delete operation
        CMmsHeaders* iMmsRequestHeaders; // request PDU

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // MMSDELETEOPERATION_H   
            
// End of File
