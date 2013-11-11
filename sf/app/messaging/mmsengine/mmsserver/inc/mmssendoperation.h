/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for message sending
*
*/



#ifndef MMSSENDOPERATION_H
#define MMSSENDOPERATION_H

//  INCLUDES
#include <badesca.h>

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
class MMmsCodecDataSupplier;

// CLASS DECLARATION

/**
*  State machine for message sending
*/
class CMmsSendOperation :public CMmsBaseOperation
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor is private.
        */

        /**
        * Two-phased constructor.
        * @param aFs file system session
        */
        static CMmsSendOperation* NewL( RFs& aFs, CMmsSettings* aMmsSettings );
        
        /**
        * Destructor.
        */
        virtual ~CMmsSendOperation();

    public: // New functions
        
        /**
        * Start the state machine.
        * @param aSelection list of message ids to be sent
        * @param aServerEntry CMsvServerEntry pointer from CMmsServer
        * @param aService current MMS service id
        * @param aStatus TRequestStatus of the calling active object
        */
        void StartL(
            CMsvEntrySelection& aSelection,
            CMsvServerEntry& aServerEntry,
            TMsvId aService,
            TRequestStatus& aStatus );

        /**
        * Get list of successfully sent messages
        */
        inline CMsvEntrySelection& Sent() const;

        /**
        * Get list of failed messages
        */
        inline CMsvEntrySelection& Failed() const;

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * Default constructor
        */
        CMmsSendOperation( RFs& aFs );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CMmsSettings* aMmsSettings );

        // We don't need to prohibit copy & assignment constructors here,
        // as they are in CBase already

        /**
        * Encode one message
        */
        void EncodePDUL();

        /**
        * Send one message to MMSC
        */
        void SubmitTransactionL();

        /**
        * Check response to send request (extract message id)
        */
        void StoreResponseL();

        /**
        * Log the send attempt, if we got so far that we really tried
        * to send something.
        */
        void LogL();

        /**
        * Move message from Outbox to Sent folder,
        * or move it to message heaven (delete it completely)
        * Depends on settings.
        */
        void MoveEntryL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
    
        TParse              iParse; // For file name parsing
        CMmsHeaders*        iMmsHeaders;
        CMsvEntrySelection* iSent;           // successfully sent messages
        RFs                 iShareProtectedFileSession;
        TBool               iShareProtectedFileSessionOpened;
        // data supplier for chunked encoding
        MMmsCodecDataSupplier* iDataSupplier;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#include "mmssendoperation.inl"

#endif // MMSSENDOPERATION_H   
            
// End of File
