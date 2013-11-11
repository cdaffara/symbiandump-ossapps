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
* Description:  This file contains the header file of the CPEPhoneModel class.
*
*/


#ifndef CPEPHONEMODEL_H
#define CPEPHONEMODEL_H

//INCLUDES
#include "mpeactivestarter.h"
#include "mpephonemodelinternal.h"
#include <apgtask.h>
#include <coemain.h>
#include <cpephonemodelif.h>
#include <DosSvrServices.h>
#include <e32base.h>
#include <talogger.h>

// FORWARD DECLARATIONS
class CPEActiveStarter;
class CCCE;
class CPEEngineInfo;
class CPEGsmAudioData;
class CPEExternalDataHandler;
class CPEMessageHandler;
class CPESimStateMonitor;
class MEngineMonitor;
class MPEContactHandling;
class MPECallHandling;
class MPELogHandling;
class MCCEDtmfInterface;
class CPEAudioFactory;
class MPEServiceHandling;
class CPERemotePartyInfoMediator;
class PhoneServices;

// CLASS DECLARATION

// DESCRIPTION
// Handles phone related events from the phone application.
//
NONSHARABLE_CLASS( CPEPhoneModel ) 
    : public CPEPhoneModelIF,
      public MPEPhoneModelInternal,
      public MPEActiveStarter
    {
    public:  //Constructors and destructor

        // Starting modules of active starter (StepL method)
        enum TActiveStarterSteps
            {
            EPEInitializeCceConnection = 0,
            EPEExternalData,
            EPECallHandling,
            EPEAudioHandling,
            EPELogHandlingPhaseOne,
            EPEContactHandlingPhaseOne,
            EPESimStateMonitor,
            EPEServiceHandling,
            EPEMessageHandler,
            EPESimChangedMonitor,
            EPESimStatusMonitorStart,
            EPELogHandlingPhaseTwo,
            EPEContactHandlingPhaseTwo,
            EPEMediatorCommandHandler,
            EPEPhoneServices,
            EPENetworkHandling
            };

        /**
        * Two-phased constructor.
        * 
        * @param aEngineMonitor, for phone application communication
        */
        static CPEPhoneModel* NewL( MEngineMonitor& aEngineMonitor );
        
        /**
        * Two-phased constructor for module tests.
        * 
        * @param aEngineMonitor, for phone application communication
        * @param aAudioFactory for creating audio related classes.
        */
        static CPEPhoneModel* NewL( 
            MEngineMonitor& aEngineMonitor,
            CPEAudioFactory& aAudioFactory );       

        /**
        * Destructor.
        */
        virtual ~CPEPhoneModel( );

    public:    //New functions 

       /**
        * Recalls starting of modules after leave occured in the middle of StepL sequence.
        * 
        */
        void RecallSteps( );

       /**
        * Starts modules of PhoneEngine.
        * 
        * @return True or False depending on wether to continue stepping. 
        */
        TBool StepL( );
        
       /**
        * Constructs Contact Handling Phase Two
        */           
        void ConstructContactHandlingPhaseTwoL();

// Functions from CPEPhonemodelIF

       /**
        * From CPEPhonemodelIF
        * Returns the name of the specified message to PhoneEngine
        *
        * @since S60 v.4.0
        * @param aMessage is the message code
        * @return String containing the name of the message
        */
        TPtrC NameByMessageToPhoneEngine( const TInt aMessage ) const; 
        
       /**
        * From CPEPhonemodelIF
        * Returns the name of the specified message from PhoneEngine
        
        * @since S60 v.4.0
        * @param aMessage is the message code
        * @return String containing the name of the message
        */
        TPtrC NameByMessageFromPhoneEngine( const TInt aMessage ) const;    
       
// Functions from MPEPhoneModel base class
       
       /**
        * From MPEPhoneModel
        * Returns pointer to information object (CPEEngineInfo) 
        * 
        * @return Return pointer to CPEEngineInfo object as MPEEngineInfo.
        */
        MPEEngineInfo* EngineInfo();
        
       /**
        * From MPEPhoneModel
        * Reroutes messages from the phone application to the message handler
        * 
        * @param aMessage is the message code.
        * @return Return possible error code.
        */
        void HandleMessage( const TInt aMessage );
        
// Functions from MPEPhoneModelInternal base class
        
       /**
        * From MPEPhoneModelInternal
        * Returns pointer to information object (CPEEngineInfo) 
        *
        * @return pointer to CPEEngineInfo object as MPEDataStore
        */
        MPEDataStore* DataStore();

       /**
        * From MPEPhoneModelInternal
        * Returns pointer to external data handler
        *
        * @return pointer to CPEExternalDataHandler as MPEExternalDataHandler
        */
        MPEExternalDataHandler* DataStoreExt(); 
        
        /**
         * From MPEPhoneModelInternal
         * Returns pointer to Mediator Command handler
         *
         * @return pointer to CPERemotePartyInfoMediator object
         */
        CPERemotePartyInfoMediator* MediatorCommunicationHandler();
        
       /**
        * From MPEPhoneModelInternal
        * Handle internal message
        *
        * @since S60 v4.0
        * @param aMessage specifies the event that has taken place
        * @param aCallId is the call id number.
        */
        void HandleInternalMessage( const TInt aMessage );

     
       /**
        * From MPEPhoneModelInternal
        * Reroutes messages from the subsystems to the message handler
        * 
        * @param aMessage is the message id.
        * @return None.
        */
        void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );
        
       /**
        * From MPEPhoneModelInternal
        * Reroutes messages from the subsystems to the message handler.
        * 
        * @param aMessage is the message id.
        * @param aCallId is the call id number.
        * @return None.
        */
        void SendMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
                          const TInt aCallId );
        

    private: 
        
        /**
         * C++ default constructor.
         * @param aAudioFactory Makes copy of aAudioFactory
         */
        CPEPhoneModel( 
            MEngineMonitor& aEngineMonitor );

        /**
        * C++ default constructor.
        * @param aAudioFactory Makes copy of aAudioFactory
        */
        CPEPhoneModel( 
            MEngineMonitor& aEngineMonitor,
            CPEAudioFactory& aAudioFactory );

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
        * Static callback function to send message
        * @param aSelf - pointer to itself
        * @return KErrNone
        */
        static TInt CallBackMessageSend( TAny* aSelf );
        
        /**
        * Process messages from the subsystems.
        * 
        * @param aMessage is the message id.
        * @param aCallId is the call id number.
        * @return error from message handler.        
        */
        TInt ProcessMessage( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
                             const TInt aCallId );
        
        /**
         * Determines which messages needs to be delayed and delays
         * sending them to Engine monitor.
         * 
         * @param aMessage is the message id.  
         * @param aCallId is the call id number.       
         * @return info indicating wheather message sending needs to be delayed.         
         */
        TBool DelayMessageSending( const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
                                   const TInt aCallId ); 
        
       /**
         * Gets call error from callinfo
         */ 
        void SetCallError( TInt aCallId );


    private:    //Data

        // Starts Phone Engine's modules asynchronically.
        CPEActiveStarter* iActiveStarter;
        // MEngineMonitor is the message api to phone application
        MEngineMonitor& iEngineMonitor;
        // CPEEngineInfo object which handles engine information
        CPEEngineInfo* iEngineInfo;    
        // CPEMessageHandler object which handles messages
        CPEMessageHandler* iMessageHandler;    
        // MPECallHandling object which handles call related commands
        MPECallHandling* iCallHandling;
        // CPEAudioHandling object which handles audio related commands
        CPEGsmAudioData* iAudioData;
        // Handle to log handling
        MPELogHandling* iLogHandling;
        // Handle to contact handling
        MPEContactHandling* iContactHandling;
        MPEServiceHandling* iServiceHandling;
        // handle to file server session
        RFs iFsSession;
        // External data handler
        CPEExternalDataHandler* iExternalDataHandler;
        // SIM changed monitor
        CPESimStateMonitor* iSimChangedMonitor;
        // SIM state monitor
        CPESimStateMonitor* iSimStatusMonitor;
        // DosServer connect error code.
        TInt iErrorCode;
        // Stepper counter
        TInt iStep;
        // CCE interface
        CCCE* iConvergedCallEngine;
        // CCE dtmf interface
        MCCEDtmfInterface* iDtmfInterface;
        /*
        * Audio factory
        * Not own.
        */      
        CPEAudioFactory* iAudioFactory;
        // Callback
        CIdle* iCallStackCutter;
        // Message Id for callback       
        MEngineMonitor::TPEMessagesFromPhoneEngine iCallBackMessage; 
        // Call Id for callback
        TInt iCallBackCallId ;
        
        // Mediator command handler
        CPERemotePartyInfoMediator* iMediatorCommunicationHandler;
        
        // Phone Services offered via Qt Highway
        PhoneServices* iPhoneServices;
        
        // Mobile call info.
        RMobileCall::TMobileCallInfoV3* iCallInfo;
    };

#endif // CPEPHONEMODEL_H  
            
// End of File
