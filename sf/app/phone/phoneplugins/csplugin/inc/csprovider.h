/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main class for CS Call Plug-in
*
*/



#ifndef CSPROVIDER_H
#define CSPROVIDER_H

#include <e32base.h>
#include <cconvergedcallprovider.h>
#include <mccpcsobserver.h>
#include <mccpdtmfprovider.h>
#include <mccpcscall.h>
#include <mccecall.h>

#include "mcspincomingcallobserver.h"
#include "mcspsecuritysettingobserver.h"
#include "mcspcommoninfo.h"
#include "mcsppubsubobserver.h"
#include "mcspremotealertingtoneobserver.h"
#include "mcspconferencestatusobserver.h"
#include "mcsptestcallprovider.h"


// FORWARD DECLARATIONS
class CSPEtelIncomingCallMonitor;
class CSPDTMFProvider;
class CSPServiceSettingsHandler;
class CSPEtelCallAddedMonitor;
class CSPConferenceCall;
class CSPCallArray;
class CSPEtelCallWaitingRequester;
class CSPSupplementaryServicesMonitor;
class MCCPSsObserver;
class CSPCipheringStatusMonitor;
class CSPSsSettingsHandler;
class CSPCallAddedHandler;
class CSPPubSubListener;
class CSPCallCommandHandler;
class CSPRemoteAlertingToneListener;
class CSPEtelConferenceStatusMonitor; 
class CSPAudioHandlerBase;


/**
* Main class for using CS Call Plug-in. Implements the CCP API.
*
*  @lib csplugin.dll
*  @since S60 v3.2
*/
class CSProvider : public CConvergedCallProvider,
                   public MCSPIncomingCallObserver,
                   public MCSPCommonInfo,
                   public MCSPSecuritySettingObserver,
                   public MCSPPubSubObserver,
                   public MCSPRemoteAlertingToneObserver, 
                   public MCSPConferenceStatusObserver,
                   public MCSPTestCallProvider
{
    public:
    
        /**
        * Two-phased constructor.
        * @return new instance of the class
        */    
        static CSProvider* NewL();
        
        /**
        * C++ default destructor
        */        
        virtual ~CSProvider();
        
        /**
        * Notification of SS event.
        * @param aSsTypeAndMode SS type and SS mode
        * @param aSsInfo SS info
        */ 
        void NotifySsEvent( RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode,
                            RMmCustomAPI::TSsInfo& aSsInfo );
        
        
// from base class MCSPTestCallProvider
        
        /**
         * Initializes call provider for testing purposes. 
         * @since S60 10.1         
         * @param aObserver general observer
         * @param aSsObserver observer for events related to supplementary 
         *        services.
         * @param aAudioHandler AudioHandler implementation for testing purposes.
         */
        void InitializeL( const MCCPObserver& aObserver,
                          const MCCPSsObserver& aSsObserver,
                          CSPAudioHandlerBase* aAudioHandler );


// from base class CConvergedCallProvider    

        /**
        * Initializes plug-in. All required parameters must be passed
        * with this function in order to use services.
        * Simplifies pluging construct removing need to pass parameters 
        * to ECom interface
        * @since S60 3.2
        * @param aAppUID Application UID of Telephony application
        * @param aObserver general observer
        * @param aSsObserver observer for events related to supplementary 
        *        services.
        */
        void InitializeL( const MCCPObserver& aObserver,
                          const MCCPSsObserver& aSsObserver );

        /**
        * Creates a new call and checks secure status from profile.
        * @since S60 3.2
        * @param aServiceId service id
        * @param aRemoteParty Recipients address/number
        * @param aObserver Observer
        * @return MCCPMoCall
        */
        MCCPCall* NewCallL( const CCCPCallParameters& aCallParameters,
                            const TDesC& aRemoteParty, 
                            const MCCPCallObserver& aObserver );


        /**
        * From CConvergedCallProvider
        * Releases all bindings to call and deletes it. 
        * This will be called by CCE to free all the resources reserved for the call.
        * After this call CCE will not access the call object anymore and observer 
        * for the class must not be used in plug-in.
        * This operation must always succeed. If plug-in decides to pre-allocate the 
        * call object it can choose to do so, but plug-in has to make sure call is 
        * returned to the state it was before calling CConvergedCallProvider::NewCallL.
        * @param aCalltoRemove Call to remove.
        * @return KErrNone or KErrNotFound if removed call is not found.
        * @pre None, can always be called.
        * @post Plug-in must quarantee all resources for the call will be cleaned up. 
        * and plug-in must not call observer after this call!
        * @since S60 3.2
        */
        virtual TInt ReleaseCall( MCCPCall& aCalltoRemove );
        
        /**
        * From CConvergedCallProvider
        * Creates a new Emergency call and add user-agent header.
        * @since S60 3.2
        * @param aServiceId service id
        * @param aAddress address of emergency call
        * @param aObserver Observer
        * @return MCCPEmergencyCall
        */
        MCCPEmergencyCall* NewEmergencyCallL( 
                                          const TUint32 aServiceId,
                                          const TDesC& aAddress,
                                          const MCCPCallObserver& aObserver );

        /**
        * From CConvergedCallProvider.
        * Releases all bindings to call and deletes it. 
        * This must be called to free all the resources reserved for the call.
        * After this call CCE must not be access the call object anymore and observer 
        * for the class must not be used in plug-in.
        * This operation mustways succeed. If plug-in decides to pre-allocate the 
        * call object it can choose to do so, but client has to be sure call is 
        * cleared from the plug-in and all resuorces are returned to the state before calling 
        * CConvergedCallProvider::NewEmergencyCallL.
        * @param aCalltoRemove Call to remove.
        * @return KErrNone or KErrNotFound if removed call is not found.
        * @pre None, can always be called.
        * @post Plug-in must quarantee all resources for the call will be cleaned up. Call must not 
        * be referenced anymore from CCE and plug-in must not call observer after this call!
        * @since S60 3.2
        */
        TInt ReleaseEmergencyCall( MCCPEmergencyCall& aCalltoRemove );
        
        /**
        * From CConvergedCallProvider
        * Creates a new conference call.
        * @since S60 3.2
        * @param aServiceId service id
        * @param aObserver Observer
        * @return MCCPConferenceCall
        */
        MCCPConferenceCall* NewConferenceL( const TUint32 aServiceId, 
                                  const MCCPConferenceCallObserver& aObserver );

        /**
        * From CConvergedCallProvider
        * Releases all bindings to conference call. CCE will call this after receiving 
        * MCCPConferenceCallObserver::ECCPConferenceIdle from plug-in or when it itself 
        * wants to release the call. In normal sequence called from CCE after HangUp is completed.
        * @param aCalltoRemove Call to remove.
        * @return KErrNone or KErrNotFound if removed call is not found.
        * @pre None, can always be called.
        * @post Plug-in must quarantee all resources for the call will be cleaned up. Call must not 
        * be referenced anymore from CCE and plug-in must not call observer after this call!
        * @since S60 3.2
        */
        TInt ReleaseConferenceCall( MCCPConferenceCall& aCalltoRemove );

        /**
        * Returns Uid of plug-in which is on use.
        * @since S60 3.2
        * @return Uid of plug-in
        */   
        const TUid& Uid() const;

        /**
        * Get Plug-In capabilities
        * @since S60 3.2
        * @return caps
        */
        TUint32 Caps( ) const;

        /**
        * Get DTMF provider
        * @since S60 3.2
        * @return Pointer to MCCPDTMFProvider if succesfull,
        *         NULL if not available
        */
        MCCPDTMFProvider* DTMFProviderL( const MCCPDTMFObserver& aObserver );

        /**
        * Get extension provider
        * @since S60 3.2
        * @return Pointer to MCCPExtensionProvider if succesfull,
        *         NULL if not available
        */
        MCCPExtensionProvider* ExtensionProviderL( 
                            const MCCPExtensionObserver& aObserver );
                            
        /**
        * This method gets the lifetime of the MS. The lifetime information 
        * includes the manufacturing date of the MS and the total amount of airtime use, 
        * from the manufacturing date until the call to this method. Calling this method 
        * does not reset any data. This is used only CS, other plug-ins may ignore this method.
        * @since S60 3.2
        * @param aLifeTimeInfo Life time information
        * @return none 
        */
        TBool GetLifeTime( TDes8& aLifeTimeInfo );
       
        /**
        * This method gets serial number from MS. The CSInfo includes
        * the serial number.
        * @since S60 5.0
        * @param aCSInfo CS specific information from MS.
        * @return Get succeeded or not.
        */
        TBool GetCSInfo( CSInfo& aCSInfo );

// from base class MCSPIncomingCallObserver

        /**
        * From MCSPIncomingCallObserver.
        * Informs incoming call.
        * 
        * @since S60 3.2
        * @param aLine the line that corresponds to the call
        * @param aCallName the name of call
        * @param aLineId line identifier for the call
        */    
        void IncomingCallArrived( RMobileLine& aLine, const TName& aCallName,
                                  RCSPLineContainer::TCSPLineId aLineId );
        
// from base class MCSPCommonInfo

        /**
        * From MCSPCommonInfo
        * Makes the request. Note that ERequestTypeDial is made with a 
        * separate MakeDialRequest function.
        *
        * @since S60 3.2
        * @param aRequest type of request
        * @return ETrue if active, EFalse if not
        */
        void GetCallWaitingL( const CCCECallParameters& iParams,
                              TBool& aCallWaitingStatus );
                              
        /**
        * From MCSPCommonInfo
        * Diagnostic error fetching.
        * @param aCallName name of call related to error
        * @return diagnostic error code
        */
        TInt GetDiagnosticError( TName& aCallName );

        /**
        * From MCSPCommonInfo
        *
        * Network security status
        * 
        * @since Series60_4.0
        * @return true if security enabled
        */
        TBool NetworkSecurityStatus() const;

        /**
        * From MCSPCommonInfo         
        * Secure specified status. 
        * @since Series60_5.0
        * @return true if secure specified
        */
        TBool SecureSpecified() const;
        
        /**
        * From MCSPCommonInfo
        * Remote alerting tone status.
        * @return tone status for remote alerting tone playing
        */
        RMmCustomAPI::TRemoteAlertingToneStatus GetRemoteAlertingToneStatus();
        
        /**
         * From MCSPCommonInfo
         * Initializes with current settings.
         * @param aParams call params 
         */
        void InitializeCallParameters( RMobileCall::TMobileCallParamsV7& aParams );

        /**
         * From MCSPCommonInfo
         * Initializes with current data call settings.
         * @param aParams call params 
         */
        void InitializeDataCallParameters( RMobileCall::TMobileHscsdCallParamsV1& aParams );
        
        /**
        * From MCPSCommonInfo
        * @param aCall client call to be indicated
        */
        void IndicateClientCall( MCCPCSCall* aCall );
        
        /**
        * Indicates incoming call for CCE.
        * @param aCall incoming call
        */
        TInt IndicateIncomingCall( MCCPCSCall* aCall );
        
        /**
        * Notify data port name about a loaned data port.
        * @param aDataPortName port name
        */
        void NotifyDataPortName( TName& aDataPortName );

        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateActiveHangup( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates hangup command complete.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateHangupComplete( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateDialRequest( MCCPCallCommandHandling& aCall );
        
        /**
        * Indicates active hangup command.
        * @param aCall MT call
        * @return system wide error code
        */
        TInt IndicateAnswerRequest( MCCPCallCommandHandling& aCall );
        
        /**
        * Don't report termination error
        * @since Series60_5.0
        * @return system wide error code
        */
        TInt DontReportTerminationError();
        
// From base class MCSPSecuritySettingObserver

        /**
        * From MCSPSecuritySettingObserver
        * Notifies about changed security status.
        * @param aValue 
        */
        void SecuritySettingChanged( TInt aValue );
        
// From base class MCSPPubSubObserver

        /**
        * Handler for changed event.
        * @param aUid uid of setting 
        * @param aKey id of setting
        * @param aStatus status of completed AO operation
        */
        void HandleNotifyPSL( const TUid aUid, const TInt& aKey, 
            const TRequestStatus& aStatus );
        
// From base class MCSPRemoteAlertingToneObserver
        
        /**
         * From MCSPRemoteAlertingToneObserver
         * @see MCSPRemoteAlertingToneObserver
         */
        void RemoteAlertingToneStatusChanged(
            RMmCustomAPI::TRemoteAlertingToneStatus aNewStatus );

// From base class MCSPConferenceStatusObserver
        
        /**
        * From MCSPConferenceStatusObserver.
        * Notifies event to observers.
        * @param aStatus new status of the conference call
        */        
        void NotifyStateChange( 
            MCSPConferenceStatusObserver::TCSPConferenceState aStatus );
                        
    private:
    
        /**
        * C++ default constructor
        */
        CSProvider();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates connections to ETel.
        */
        void CreateEtelConnectionsL();
        
        /**
        * Handles diverting and barring notifications.
        * @param aAddr address
        * @param aSsTypeAndMode ss type and mode struct
        */
        void HandleDivertOrBarring(
                const TDesC& addr, 
                RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode );
        
        /**
        * Handles SIM status
        */
        void HandleSIMStatusL();
        
    // data
    private:
        
        /**
        * Implementation UID
        */
        TUid iImplementationUid;
        
        /**
        * ETel connection
        */
        RTelServer iServer;
        
        /**
        * Mobile phone handle.
        */
        RMobilePhone iMobilePhone;

        /**
        * Mobile conference call handle.
        */
        RMobileConferenceCall iMobileConferenceCall;
        
        /**
        * Observer for provider level functionality.
        * Not own.
        */        
        MCCPCSObserver* iCCPObserver;

        /**
        * Observer for SS events.
        * Not own.
        */
        MCCPSsObserver* iSsObserver;
        
        /**
        * DTMF provider.
        * Own.
        */
        CSPDTMFProvider* iDTMFProvider;
        
        /**
        * Monitors line for incoming calls.
        * Own.
        */        
        CSPEtelIncomingCallMonitor* iIncomingVoiceCallMonitor;
        
        /**
        * Monitors line for incoming data/video calls.
        * Own.
        */        
        CSPEtelIncomingCallMonitor* iIncomingDataCallMonitor;
        
        /**
        * Monitors line for incoming calls from aux line
        * Own.
        */        
        CSPEtelIncomingCallMonitor* iIncomingAuxCallMonitor;
        
        /**
        * Call array.
        * Own.
        */
        CSPCallArray* iCallArray;
                
        /**
        * Custom API handle for custom functionality.
        */        
        RMmCustomAPI iMmCustom;
        
        /**
        * Line container.
        */
        RCSPLineContainer iLineContainer;
        
        /**
        * Service settings handler for reading 
        * service id and name information.
        * Own.
        */
        CSPServiceSettingsHandler* iServiceHandler;
        
        /**
        * Initialisation status.
        */
        TBool iInitialized;
                
        /**
        * Call added monitor.
        * Own.
        */
        CSPCallAddedHandler* iCallAddedHandler;
        
        /**
        * Service ID.
        */
        TUint32 iServiceId;
        
        /**
        * Audio interface.
        * Own.
        */
        CSPAudioHandlerBase* iAudioHandler;
        
        /**
        * Call waiting requester.
        * Own.
        */
        CSPEtelCallWaitingRequester* iCwRequester;
        
        /**
        * Supplementary service related event monitor.
        * Own.
        */ 
        CSPSupplementaryServicesMonitor* iSsMonitor;
        
        /**
         * Ciphering (network security level On/Off) setting monitor.
         */
        CSPCipheringStatusMonitor* iCipheringStatusMonitor;
        
        /**
         * SS Settings handler for reading ALS line and ALS supported status.
         */
        CSPSsSettingsHandler* iSsSettingsHandler;
        
        /**
        * Sim status listener.
        * Own.
        */ 
        CSPPubSubListener* iSimStatusListener;
        
        /**
        * CallCommandHandler.
        * Own.
        */ 
        CSPCallCommandHandler* iCallCommandHandler;
        
        /** 
         * Remote Alerting Tone listener.
         */
        CSPRemoteAlertingToneListener* iRemoteAlertingToneListener;
        
        /** 
         * Monitor for conference status changes.
         * Own.
         */
        CSPEtelConferenceStatusMonitor* iConferenceStatusMonitor;

        /** 
         * Keep track of conference existence.
         */
        CSPConferenceCall* iConferenceCall; 
};

#endif // CSPROVIDER_H
