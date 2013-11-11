/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for sending a forward request
*
*/



#ifndef MMSFORWARDOPERATION_H
#define MMSFORWARDOPERATION_H

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
//class CLogClient;
//class CLogViewEvent;
//class CLogEvent;
//class CMmsLog;

// CLASS DECLARATION

/**
*  State machine for sending forward request
*/
class CMmsForwardOperation :public CMmsBaseOperation
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aFs file system session
        * @param aLogClient log client for writing into log
        * @param aLogEvent log event for writing entries into log
        */
        static CMmsForwardOperation* NewL( RFs& aFs, CMmsSettings* aMmsSettings );
        
        /**
        * Destructor.
        */
        virtual ~CMmsForwardOperation();

        /**
        * Failed
        * @return selection of failed entries
        */
        CMsvEntrySelection& Failed() const;

    public: // New functions
        
    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * Default constructor
        */
        CMmsForwardOperation( RFs& aFs );

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
        * Update the trigger entry status.
        */
        void UpdateEntryStatusL();

        /**
        * Move message from Outbox to Sent folder,
        * or move it to message heaven (delete it completely)
        * Depends on settings.
        */
        void MoveEntryL();

        /**
        * Adds log entry after sending if necessary (depends on settings).
        */
        void LogL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        CMmsHeaders*        iMmsRequestHeaders;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
    };

#endif      // MMSFORWARDOPERATION_H   
            
// End of File
