/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPELogHandling class.
*
*/


#ifndef    CPELOGHANDLING_H
#define    CPELOGHANDLING_H

//INCLUDES
#include <e32base.h>
#include <mccecall.h>
#include <f32file.h>
#include <logwrap.h>
#include "mpeloghandling.h"

// CONSTANTS
// Maximum number of log events that may be needed at any given moment.
// Due to log system limitations there is a chance more log event are needed
// and this will be handled as it happens
const TInt KPEMaximumNumberOfLogEvents = KPEMaximumNumberOfCalls + 2;

// MACROS
// None.

// DATA TYPES
typedef TBuf8<KCntMaxTextFieldLength> TPELogsDataFieldType;


// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPELogHandlingCommand;
class CPELogExternalData;
class CPELogEvent;
class CLogClient;
class MPEPhoneModelInternal;
class MPEDataStore;
class CPELogInfo;
class CTelLoggingExtension;
class CSPSettings;
class CPELogExtensionWrapper;

// CLASS DECLARATION

class CPELogHandling : public CBase,
                       public MPELogHandling
    {
    public:

        /**
        * Two-phased constructor
        */
        static MPELogHandling* NewL( MPEPhoneModelInternal& aModel, RFs& aFsSession );
             
        /**
         * Destructor.
         */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        virtual IMPORT_C ~CPELogHandling();
        
    protected:

        /**
        * C++ default constructor.
        */
        CPELogHandling( MPEPhoneModelInternal& aModel, RFs& aFsSession );

        /**
        * Constructor, second phase. 
        */
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        void ConstructL();

    public: // From MPELogHandling
        /**
        * Save call entry
        * @since N/A
        * @param: TInt, call id
        * @return: possible error code.
        */
        IMPORT_C TInt SaveCallEntry( const TInt aCallId );
        
    public:      
        /**
        * Reroutes messages to the Phone Model.
        * @since Series60_2.7
        * @param aMessage Message id.
        * @param aErrorCode Error code.
        * @return None.
        */
        void SendMessage( MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, TInt aErrorCode );

        /* Get log string
        * @param: None
        * @return TLogString, string for incoming call.
        */  
        TLogString& LogStringIn( );


        /* Get log string
        * @param: None
        * @return TLogString, string for outgoing call.
        */  
        TLogString& LogStringOut( );

        /* Get log string
        * @param: None
        * @return TLogString, string for missed call.
        */  
        TLogString& LogStringMissed( );

        /* Get log string
        * @param: None
        * @return TLogString, string for delivery.
        */  
        TLogString& LogStringDelivery( );

        /* Get log string
        * @param: None
        * @return TLogString, string for incoming auxiliary call.
        */  
        TLogString& LogStringInAlt( );
        
        /* Get log string
        * @param: None
        * @return TLogString, string for outgoing auxiliary call.
        */  
        TLogString& LogStringOutAlt( );
        
        /* Get log string
        * @param: None
        * @return TLogString, string for unknown call.
        */  
        TLogString& LogStringUnknown( );
        
        /**
        * Reset missed call status.
        */
        void ResetMissedCalls( const TInt aCallId );
        
    private:

        /**
         * Set remote contact information to log info.
         * @since   S60 v9.1
         * @param   aCallId     Call identifier.
         * @param   aLogInfo    Log info.
         */
        void SetRemoteContact( TInt aCallId, CPELogInfo& aLogInfo );
        
        /**
        * Update call info to log database.
        * @since N/A
        * @param  CPELogInfo aCallInfo, call information.
        * @return None.
        */
        void SaveCallInfoL( const CPELogInfo& aLogInfo );

        /**
        * Save log event
        * @since N/A
        * @param: const CPELogInfo&, log information
        * @return: none.
        */
        void SaveEventL( const CPELogInfo& aLogInfo );        
    
        /**
        * Find Log database entry index by given call id.
        * @since Series60_2.7
        * @param: TInt aCallId , call identification number.
        * @return: Error code.
        */
        TInt FindEventIndexById( const TInt aCallId );
        
        /**
        * Delete event in Id array
        * @since Series60_2.7
        * @param: CPELogsEntry* aLogEntry pointer
        * @return: None.
        */
        void DeleteArrayEntry( CPELogEvent* aLogEvent );                

        /**
        * Log Handling cleanup if log handling of an event leaves.
        * @since Series60_2.7
        */
        void DoCleanup();
        
        /**
         * Updates log info with a data queried from logging extension plugin.
         * Log info is not changed if error happens while using extension.
         * @since   S60 v9.1
         * @param   aCallId     Call identifier.
         * @param   aLogInfo    Log info to be updated.
         */
        void UpdateLogInfoWithExtensionDataL( TInt aCallId, 
            CPELogInfo& aLogInfo );

        /**
         * Returns logging extension plugin for the given service. A new 
         * plugin is created if extension is not yet created for the service.
         * @since   S60 v5.1
         * @param   aServiceId  Service identifier.
         */
        CTelLoggingExtension& LoggingExtensionL( TUint aServiceId );
        
        /**
         * Returns logging extension plugin identifier from service
         * provider settings.
         * @since   S60 v9.1
         * @param   aServiceId  Service identifier.
         */
        TUid LoggingPluginIdentifierL( TUint aServiceId ) const;
        
        /**
         * Sets extension data to log info.
         * @since   S60 v9.1
         * @param   aLogInfo        Log info to be updated.
         * @param   aPhoneNumber    Phone number of remote party.
         * @param   aVoipAddress    Voip address of remote party.
         * @param   aMyAddress      Local user's registered address.
         * @param   aRemotePartyName Name of the remote party.
         */
        void SetExtensionData( CPELogInfo& aLogInfo, 
            const TDesC& aPhoneNumber, const TDesC& aVoipAddress, 
            const TDesC& aMyAddress, const TDesC& aRemotePartyName );
        
        /**
         * Creates an logging extension wrapper instance.
         * @since   S60 v5.1
         * @param   aPluginUid      An ECom plugin implementation identifier.
         * @return  Extension wrapper instance.
         */
        CPELogExtensionWrapper* CreateExtensionWrapperLC( 
            const TUid& aPluginUid ) const;
      
        /**
         * Sets emergency call text to the log info's
         * name field.
         * @param   aLogInfo      Log info.
         * @return  None.
         */
        void SetEmergencyCallName( CPELogInfo& aLogInfo );
        
    private: // data 
        // Reference to MPEPhoneModelInternal, also object owner
        MPEPhoneModelInternal&  iModel;
        // Reference to data store
        MPEDataStore& iDataStore;
        // Log database connection.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        CLogClient* iLogClient;           
        // Array of active log events
        RPointerArray< class CPELogEvent > iActiveLogEventArray;
        // Array of queued log events
        RPointerArray< class CPELogEvent > iQueuedLogEventArray;
        // Array of free log event objects
        RPointerArray< class CPELogEvent > iFreeLogEventArray;
        // Logs handling command pointer   
        CPELogHandlingCommand* iLogHandlingCommand;  
        // Log external data pointer
        CPELogExternalData* iLogExternalData;
        // File session connection
        RFs& iFsSession;
        // Identifiers of active calls.
        RArray<TInt> iActiveCallIds;
        // pointer to the log event under processing
        CPELogEvent* iLogEventUnderProcessing;
        
        // Enumeration for incoming call, get from database  
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringIn;          
        // Enumeration for outgoing call, get from database
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringOut;       
        // Enumeration for missed call, get from database
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringMissed;  
        // Enumeration for delivery, ger from database.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringDelivery;  
       // Enumeration for incoming auxiliary line call, get from database.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringInAlt;
        // Enumeration for outgoing auxiliary line call, get from database.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringOutAlt;
        // Enumeration for Unknown name, get from database.
        /*****************************************************
        *    Series 60 Customer / LOGENG
        *    Series 60  LOGENG API
        *****************************************************/                
        TLogString iLogStringUnknown;        
        
        /**
         * Logging extension plugins.
         * Own.
         */
        RPointerArray<CPELogExtensionWrapper> iPlugins;

    };

#endif // CPELOGSHANDLER_H

// End of File

