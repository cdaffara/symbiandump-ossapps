/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for handling MMS log entries
*     (sent messages and delivery reports)
*
*/



#ifndef MMSLOG_H
#define MMSLOG_H

//  INCLUDES
#include <msvstd.h>
#include <mentact.h>
#include <logwraplimits.h>
#include <logengdurations.h>
// MACROS

// DATA TYPES

enum TMmsLoggingState
    {
    EMmsLogIdle,
    EMmsLogFiltering,
    EMmsLogMatchingEntry,
    EMmsLogUpdatingEntry,
    EMmsLogCreatingEntry,
    EMmsLogAddingEventType,
    EMmsLogFinal
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogClient;
class CLogEvent;
class CLogViewEvent;
class CLogFilter;
class CLogFilterList;

// CLASS DECLARATION

/**
*  State machine that handles log database for MMS.
*/
class CMmsLog :public CMsgActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aLogClient instance of log client. Caller creates for reusability
        * @param aLogViewEvent common settings for the event. Only remote party will vary
        */
        static CMmsLog* NewL( CLogClient& aLogClient, CLogViewEvent& aLogViewEvent, RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CMmsLog();

    public: // New functions

        /**
        * Start the state machine
        * @param aLogEvent common settings for an event. <br>
        *     aLog Event will contain the filter keys to be used.<br>
        *     (event type and message id (delivery report) or
        *     message entry id (sent messages)
        * @param aRemoteParties list of remote parties (pure addresses)
        * @param aStatus status of the calling active object
        */
        void StartL( CLogEvent& aLogEvent, CDesCArray& aRemoteParties, TRequestStatus& aStatus );
        
        TLogLink GetLink();
        
    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CMmsLog( CLogClient& aLogClient, CLogViewEvent& aLogViewEvent );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( RFs& aFs );

        // By default, prohibit copy constructor
        CMmsLog( const CMmsLog& );
        // Prohibit assignment operator
        CMmsLog& operator= ( const CMmsLog& );

        /**
        * Active object completion
        */
        void RunL();

        /**
        * Active object completion
        */
        void DoRunL();

        /**
        * Sweep the floor after everything has crashed
        */
        void DoComplete( TInt& aStatus );

        /**
        * Select next state for state machine
        */
        void SelectNextState();

        /**
        * Switch states
        */
        void ChangeStateL();

        /**
        * Find matching entries
        */
        void FilterL();

        /**
        * Check if found entry matches
        */
        void MatchEntryL();
        
        /**
        * Matching entry in log - update it
        */
        void UpdateEntryL();

        /**
        * Create a new entry into log
        */
        void CreateEntryL();

        /**
        * Add MMS Event Type
        */
        void AddEventTypeL();


    public:     // Data
    
    protected:  // Data

    private:    // Data
        RFs             iFs; // file system
        // State machine state
        TInt            iState;
        TInt            iError;
        // The log classes are instantiated by the caller
        // because the classes may be reused to add several
        // entries to logs.
        CLogClient&     iLogClient;     // caller opens this
        CLogViewEvent&  iLogViewEvent;  // caller provides this
        CLogEvent*      iLogEvent;      // caller provides this
        CLogFilterList* iFilterList;    // this is ours
        CDesCArray*     iRemoteParties; // caller provides this
        CLogEvent*      iLogUpdatedEvent; // We need this to update entries
        CLogEventType*  iLogEventType;  // to add mms event type      

        // sent entries are filtered by entry id
        TMsvId          iEntryId;
        // delivery reports are filtered by message id
        TPtrC8          iMessageId;
        TInt            iCurrentRemoteParty;
        TBool           iEvents; // filter results
        TBool           iEventMatched; // matching event found from the log
        TBool           iEventTypeAdded; // try adding only once
        
        TLogLink iLastMatchedLink;  

        TBuf<KLogMaxRemotePartyLength> iAlias; // alias for phone number

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSLOG_H   
            
// End of File
