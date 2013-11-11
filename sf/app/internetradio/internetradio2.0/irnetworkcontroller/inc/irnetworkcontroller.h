/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Controls the network interface(s) for Internet Radio.
*
*/


#ifndef C_IRNETWORKCONTROLLER_H
#define C_IRNETWORKCONTROLLER_H

#include <badesca.h>
#include <es_sock.h>
#include <rhttpsession.h>
//Added for ALR/SNAP
#include <comms-infras/cs_mobility_apiext.h>

#include "iractivenetworkobserver.h"
#include "irnetworkcontrollerobserver.h"
#include "irdatatransfertracker.h"

class CIRNetworkObserver;
class CIRSettings;
class MIRActiveNetworkObserver;

/**
 * This class provides the interface to IRNetworkController component
 *
 * @code
 * // CIRNetworkController follows a singleton pattern
 * // hence the destructor is private and to destroy it we need to use an API
 * // For creating a network controller which has provision for network monitoring
 * // we need to pass the reference of the observer which should derive from
 * // MIRNetworkControllerObserver class the network even observed will be
 * // indicated to the observer with the call back IRNetworkEvent() function
 * CIRNetworkController* iNetworkController = CIRNetworkController::NewL(iObserver)
 *
 * // For creating a network controller which does not have provision for network monitoring
 * // we use the overloaded NewL() of CIRNetworkController
 * CIRNetworkController* iNetworkController = CIRNetworkController::NewL()
 *
 * // This API is used to destroy the network controller handle
 * iNetworkController->DestroyNetworkController();
 *
 * // This API returns the instance of RConnection
 * // The same RConnection object is used across the entire iRAPP components to
 * // provide central arbiter and monitoring of Network Connection
 * iNetworkController->GetIRConnection();
 *
 * // This API returns the instance of RSocketServ
 * // The same RSocketServ object is used across the entire iRAPP components to
 * // provide central arbiter and monitoring of Network Connection
 * iNetworkController->GetIRSocketServer();
 *
 * // This API returns a pointer to the x-wap-profile string.
 * // The ownership of the object is passed onto the caller
 * iNetworkController->GetWapProfString();
 *
 * // This API returns a pointer to the UAProf string.
 * // The ownership of the object is passed onto the caller
 * iNetworkController->GetUAProfString();
 *
 * // This API returns ETrue of network connection is up and running EFalse
 * // otherwise
 * iNetworkController->GetNetworkStatus();
 *
 * // This API returns the IAP id of the connected connection
 * // this is used by download manager for choosing the access point
 * iNetworkController->GetIAPId();
 *
 * // This API is called to initiate access point selection
 * iNetworkController->ChooseAccessPointL();
 *
 * // This API returns ETrue if the phone is in offline mode, EFalse
 * // otherwise
 * iNetworkController->IsOfflineMode();
 *
 * // This API returns ETrue if WLan is supported on the device, EFalse
 * // otherwise
 * iNetworkController->IsWlanSupported();
 *
 * // This API allows the caller to reset the network connection status
 * iNetworkController->ResetConnectionStatus();
 *
 * // This API returns the type of connection open
 * iNetworkController->IdentifyConnectionType();
 *
 * @endcode
 *
 */
NONSHARABLE_CLASS( CIRNetworkController ): public CActive, 
                                           public MIRDataTransferObserver,
                                           public MMobilityProtocolResp
    {
public:

    /**
     *  Creates an Instance of CIRNetworkController
     *  @return CIRNetworkController*
     */
    IMPORT_C static CIRNetworkController* OpenL( MIRNetworkController* aObserver = NULL );

    /**
     *  Conditionally destroys the IRNetworkController object
     */
    IMPORT_C void Close();

    /**
     *  Default C++ Destructor
     */
    ~CIRNetworkController();

    /**
     *  Returns the instance of RConnection
     *  The same RConnection object is used across the entire iRAPP components to
     *  provide central arbiter and monitoring of Network Connection
     *  @return RConnection&
     */
    IMPORT_C RConnection& GetIRConnection();

    /**
     *  Returns the instance of RSocketServ
     *  The same RSocketServ object is used across the entire iRAPP components to
     *  provide central arbiter and monitoring of Network Connection
     *  @return RSocketServ&
     */
    IMPORT_C RSocketServ& GetIRSocketServer();

    /**
     *  Returns a HBufC pointer to the x-wap-profile string
     *  Transfers the ownership of the returned HBufC8 object
     *  The caller must takecare of deleting the returned object
     *  @return HBufC8*
     */
    IMPORT_C HBufC8* GetWapProfString();

    /**
     *  Returns a HBufC pointer to the UAProf string
     *  Transfers the ownership of the returned HBufC8 object
     *  The caller must takecare of deleting the returned object
     *  @return HBufC8*
     */
    IMPORT_C HBufC8* GetUAProfString();

    /**
     *  Returns the variable which indicates if connection is active or not
     *  @return TBool iIsConnectedToNetwork
     */
    IMPORT_C TBool GetNetworkStatus() const;

    /**
     *  Gets the IAP Id of the chosen IAP
     *  @return TInt Error code.
     */
    IMPORT_C TInt GetIAPId(TUint32& aIapId) const;

  /**
     *  Configures the Access Point which is used by all the components for network connectivity
     */
    IMPORT_C void ChooseAccessPointL(TBool aDefaultConnection = ETrue);

    /*
     * cancel configuring access point
     */
    IMPORT_C void CancelConnecting();
    
    /**
     *  This api is used to determine if the phone is in offline mode
     *  @return ETrue if the phone is in offline mode else EFalse
     */
    IMPORT_C TBool IsOfflineMode();

    /**
     *  This api is used to determine if the phone supports WLan usage
     *  @return ETrue if the phone supports else EFalse
     */
    IMPORT_C TBool IsWlanSupported() const;

    /**
     *  Reset the connection status to Disconnected state
     */
    IMPORT_C  void ResetConnectionStatus();

    /**
     *  Used to determine the type of connection
     *  @return enum describing the type of connection ( GPRS/3G/WiFi )
     */
    IMPORT_C TIRConnectionType IdentifyConnectionType() const;

    /**
     *  Binds DataTranseferTracker instance to HTTPSession (Byte Counter Impl)
     */
    IMPORT_C  void InitializeHttpSessionL( const RHTTPSession& aHTTPSession, 
                        MIRDataTransferTracker::TIRTransferCategory aCategory );

    /**
     *  Binds LogoDataTranseferTracker instance to Logo HTTPSession (Byte Counter Impl)
     */
    IMPORT_C  void RegisterLogoDataTransferTrackerL( RHTTPSession& aHTTPSession );

    /**
     *  DataTranseferTracker instance for observers and Raw sockets (Byte Counter Impl)
     */
    IMPORT_C  MIRDataTransferTracker& DataTransferTracker();

    /**
     *  LogoDataTranseferTracker instance for observers and Raw sockets (Byte Counter Impl)
     */
    IMPORT_C  MIRDataTransferTracker& LogoDataTransferTracker();

    /**
     *  Method to receive notification when logo is downloaded
     */
    void HandleDataTransferEventL( const MIRDataTransferTracker::TIRDataTransferPckg& aData );

    /*
     * Registers the observer that will be notified for 
     * a network connection.
     * Observer requires notification to reissue pending request
     */ 
    IMPORT_C  void RegisterActiveNetworkObserverL( MIRActiveNetworkObserver& aActiveNetworkObserver );

    /**
     * DeleteRoamingObserver()
     * Used to remove an observer for roaming events from the observer array
     */
    IMPORT_C void DeleteActiveNetworkObserver( MIRActiveNetworkObserver& aActiveNetworkObserver );
        
    /**
     * Notifies all observers whose network request is active
     * to reissue the request  
     * NotifyActiveNetworkObserversL()
     */    
    IMPORT_C  void NotifyActiveNetworkObserversL( TIRNetworkEvent aEvent );
     
     /**
     * Notifies all observers whose network request is active
     * to reset the pending request status  
     * ResetPendingRequests()
     */    
     void ResetPendingRequests( TBool aValue );

protected:

    /**
     *  Derived from CActive
     */

    /**
     * The function is called by the active scheduler when a request completion event occurs,
     */
    virtual void RunL();

    /**
     *  Cancels the pending requests on the CIRNetworkController Active object
     */
    virtual void DoCancel();
    
    
public: // From MMobilityProtocolResp        //to be changed to private after demo

            //Added for ALR/SNAP
        void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                        TAccessPointInfo aNewAPInfo,
                                        TBool aIsUpgrade,
                                        TBool aIsSeamless );
        void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless );
        void Error( TInt aError );    
        
public:
        /**
         * IsHandingOverConnection()
         * Indicates if the Hand over of Network connection has happened
         */
        IMPORT_C TBool IsHandingOverConnection();
        
        void ResetHandingOverConnection();
        
private:

    /**
     *  Used to determine the current profile
     *  @return TInt describing the profile value
     */
    TInt DetermineCurrentProfile() const;

    /**
     *  Pops up the access point list.
     *  @return TBool ETrue feature is supported EFalse otherwise
     */
    TBool CheckFeatureL( TInt aFeatureId ) const;

    /**
     *  Default C++ Constructor
     */
    CIRNetworkController();

    /**
     *  Second Phase construction.
     */
    void ConstructL();

    /**
     *  Second Phase construction.
     *    @param aObserver pointer to the observer class if an observer is needed
     */
    void ConstructL( MIRNetworkController* aObserver );

    /**
     *  Determines the available access points using CommsDat Api
     */
    void QueryCommsForIAPL();

    /**
     *  Queries the system and extracts the UAProf information
     *  Used by IRDataProvider and IRStreamSource
     */
    void BuildUAProfStringL();

    /**
     *  Retrievs the string from Central Repository for the specified key and cen rep uid
     *  Transfers the ownership of the returned HBufC object
     *  The caller must takecare of deleting the returned object*
     *    @param aRepositoryUid specifies the Uid of the repository to be used
     *    @param aKey specifies the key to be used
     *    @return HufC* pointer to the retrieved string
     */
    HBufC* CentralRepositoryStringValueL( const TUid& aRepositoryUid,
        TUint32 aKey ) const;

    /**
     *  Pops up the access point list.
     *  @return TBool ETrue is we need to proceed with access point selection EFalse otherwise
     */
    TBool DisplayAccessPointListL();

    /**
     *  Validates the access point availablity etc.
     */
    TBool ValidateAccessPointsL();

    /**
     *  Handles the success case of Network connection in RunL
     */
    void HandleRunLSuccessL();
    
    /**
     *  Handles the error case of Network connection in RunL
     */
    void HandleRunLErrorL( TInt aStatusCode );
    
public:
    /**
     * Indicates if ChooseAccessPointL is called
     */
    TBool iIsConnectRequestIssued;

private:
    /**
     * This is the physical connection we are putting up
     */
    RConnection* iIRNetworkConnection;

    /**
     * Actual RSocketServ instance
     */
    RSocketServ iIRSocketServer;

    /**
     * Number of objects currently referring to the singleton object CIRNetworkController
     */
    TInt iSingletonInstances;

    /**
     * Used to access Settings object
     */
    CIRSettings* iIRSettings;

    /**
     * IR Network component observer
     */
    MIRNetworkController* iObserver;

    /**
     * Enum declaration which indicate the network status
     */
    enum TIRNetworkConnectionStatus
        {
        EIRNetworkConnectionActive = 1,
        EIRNetworkConnectionInActive
        };

    /**
     * Variable to keep track of the RConnection connection status
     */
    TIRNetworkConnectionStatus iNetworkConnectionState;

    /**
     * Enum declaration which indicate the connection to RSocketServ status
     */
    enum TIRSocketServerConnectionStatus
        {
        EIRSocketServerActive = 1,
        EIRSocketServerInActive
        };

    /**
     * Variable to keep track of the RSocketServ connection status
     */
    TIRSocketServerConnectionStatus iSocketServerConnectionState;

    /**
     * Enum declaration to indicate the NetworkController state
     */
    enum TIRNetworkControllerState
        {
        EInActive = 1,
        EConnectingToNetwork
        };

    /**
     * Specifies the Network controller state
     */
    TIRNetworkControllerState iNetworkControllerState;

    /**
     * Indicates the current network connection status
     */
    TBool iIsConnectedToNetwork;

    /**
     * Network observer
     */
    CIRNetworkObserver* iIRNetworkObserver;

    /**
     * Pointer to UAProf string
     */
    HBufC8* iIRUAProf;

    /**
     * Pointer to x-wap-profile string
     */
    HBufC8* iIRWapProf;

    /**
     * Access point list for settings view
     */
    CDesCArrayFlat* iIapList;

  /**
     * Indicates if WiFi is supported or not
     */
    TBool iIsWlanSupported;

    /**
     * Used to determine if connection is open or not
     */

    TBool iConnectionPresent;

    /**
     * Tracks data transferred over the network for the connection.
     * Owned.
     */

    CIRDataTransferTracker* iDataTransferTracker;

    /**
     * Tracks data transferred over the network for the Logo session.
     * Owned.
     */

    CIRDataTransferTracker* iLogoDataTransferTracker;
    
    //Added for ALR/SNAP
    
    /**
    * Observes the preferred network within defined destination
    */
    CActiveCommsMobilityApiExt* iMobility;

    /**
     * ETrue if the ALR handover is ongoing
     */
    TBool iHandingOver;
    
    /**
     * Array of observers for change in roaming events
     */
    RPointerArray<MIRActiveNetworkObserver> iActiveNetworkObserverArray;
    
    TBool iDefaultConnection;
    };

#endif // C_IRNETWORKCONTROLLER_H
