/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPELogEvent class.
*
*/




#ifndef    CPELOGEVENT_H
#define    CPELOGEVENT_H

//  INCLUDES
#include "pevirtualengine.h"
#include "cpeloghandling.h"
#include <logwrap.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logwraplimits.h>
#include <logengdurations.h>
#endif

// CONSTANTS
const TInt KPEESDWFormattedTimeLength( 50 ); // Formatted time length
_LIT( KPEESDWTimeFormat, "%F%D/%M/%Y %H:%T:%S" ); //Time format

//Defined in \s60\app\contacts\logsui\EngineIncLogsApiConsts.h
const TInt KTUintCharLength( 10 );

// MACROS
//None.

// DATA TYPES
//None.

// FUNCTION PROTOTYPES
//None.

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CPELogEvent is the class that interfaced with Symbian Log Engine over the
* log client interface.
*
*  @lib loghandling.lib
*  @since Series60_2.7
*/
class CPELogEvent : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CPELogEvent* NewL( CPELogHandling& aOwner,
                                  CPELogHandlingCommand& aLogHandlingCommand );
     
        /**
        * Destructor.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual ~CPELogEvent(); 
         
    protected:
        
        /**
        * C++ default constructor.
        */
        CPELogEvent( CPELogHandling& aOwner, CPELogHandlingCommand& aLogHandlingCommand );

        /**
        * By default EPOC constructor is private.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual void ConstructL();


    public: // New functions
        /**
        * Add call entry to database.
        * @param None
        * @return None.
        */
        void SaveL( );

        /**
        * Update log info for the log event
        * @param TPELogInfo& alogInfo, log information
        * @return None.
        */
        void UpdateLogInfoL( const CPELogInfo& aLogInfo );
        
        /**
        * Reset log info 
        */
        void ResetLogInfo();

        /**
        * Reset event.
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        void ResetEvent();      
       
        /* Get call id of the event
        * @param: None
        * @return TInt aCallId, call idendification.
        */  
        TInt CallId( );

        /* Check is the event has been completed; no new updates expected
        * @param: None.
        * @return TBool ETrue or False.
        */  
        TBool IsCompleted();

        /* Check is the event has been completed; no new updates expected
        * @param: None.
        * @return None.
        */  
        void SetAdded();
        
        /* Get call id of the event
        * @param: None
        * @return TBool save leave flag. ETrue if SaveL leaves.
        */  
        TBool SaveLeave( );        

    private:
        /**
        * Set phonenumber type to event subject from phonenumber id.
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        void SetPhoneNumberId();
    
        /**
        * Set Event type
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        void SetEventTypeL();

        /**
        * Set log event information.
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        void SetLogEvent();

        /**
        * Set log event duration.
        * @since N/A
        * @param None.
        * @return None.
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                        
        void SetDuration();

        /**
        * Sets callers service id to log event.
        * @since N/A
        * @param None.
        * @return None.
        */
        void SetServiceIdL();
        
        /**
        * Sets callers contact link to log event.
        * 
        * @since v5.0
        */
        void SetContactLinkL();

        /**
        * Sets a data field like VoIP address or my address.
        * @since    S60 v5.1
        * @param    aTag        Log tag identifying a field.
        * @param    aFieldData  Field data.
        */
        void SetDataFieldL( const TDesC8& aTag, const TDesC& aFieldData );

        /**
         * Sets remote party to the given log event.
         * @since    S60 v5.1
         * @param    aEvent        Log event.
         * @param    aLogInfo      Log info containing remote party.
         */
        void SetRemoteParty( CLogEvent& aEvent, const CPELogInfo& aLogInfo );
        
        /**
         * Sets remote contact to the given log event.
         * @since    S60 v5.1
         * @param    aEvent        Log event.
         * @param    aLogInfo      Log info containing remote contact.
         */
        void SetRemoteContact( CLogEvent& aEvent, const CPELogInfo& aLogInfo );
        
    private:    // Data
        // Owner of the object.
        CPELogHandling& iOwner;
        // Reference to log handling command object
        CPELogHandlingCommand& iLogHandlingCommand; 
        // Log database event.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        CLogEvent* iEvent;       
        // Call information.
        CPELogInfo* iLogInfo;
        // Completed indicator
        TBool iCompleted;
        // Added indicator.
        TBool iAdded;
        // Save Leave indicator
        TBool iSaveLeave;
    };

#endif      // CPELOGSENTRY_H  
            
// End of File
