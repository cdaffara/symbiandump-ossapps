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
* Description:
*
*/


#ifndef C_IRNETWORKOBSERVER_H
#define C_IRNETWORKOBSERVER_H

#include <rconnmon.h>

#include "irnetworkcontrollerobserver.h"

class MIRNetworkController;
class CIRNetworkController;


NONSHARABLE_CLASS( CIRNetworkObserver ): public CActive , 
                                         public MConnectionMonitorObserver
    {
public: 

    /**
     *Creates an Instance of CIRNetworkObserver
     *@return CIRNetworkObserver*
     */    
    IMPORT_C static CIRNetworkObserver* NewL( CIRNetworkController* aNetworkController );

    /** 
     *  Default Destructor
     */
    ~CIRNetworkObserver();

    /** 
     *  Initializes the Connection monitor
     */    
    void InitializeNetworkObserver();

    /** 
     *  Set the observer used to communicate with the IRNetworkController
     */    
    void SetObserver( MIRNetworkController* aObserver );
    
    /**
     * Sets network monitoring observer to decide whether network monitoring is 
     * required.
     */
    void SetNetworkMonitoring( TBool aValue );

private: 

    /** 
     *  Default Constructor
     */
    CIRNetworkObserver(CIRNetworkController *aNetworkController);

    /** 
     *  Creates an Instance of CIRNetworkObserver
     *  @return CIRNetworkObserver*
     */    
    static CIRNetworkObserver* NewLC( CIRNetworkController* aNetworkController );

    /** 
     *  Second Phase construction.
     */
    void ConstructL();

//  Derived from CActive

    /** 
     * Derived from CActive
     * The function is called by the active scheduler when a request 
     * completion event occurs,
     */
    virtual void RunL();

    /** 
     *  Cancels the pending requests on the CIRNetworkObserver Active object
     */
    virtual void DoCancel();

    /** 
     *  Handles a leave occurring in the request completion event handler RunL()
     */
    virtual TInt RunError( int aError );

    /**
     *    Derived from MConnectionMonitorObserver
     */

    /** 
     *  Derived from MConnectionMonitorObserver
     *  Implements the EventL method which is called when there is a network event
     *  @param aConnMonEvent Indicates the type of event occurred
     */     
    void  EventL( const CConnMonEventBase &aConnMonEvent );

    /** 
     *  Identifies the type of connection we have used to get connected to network
     */
    void IdentifyConnection();

    /**
     * Utility function used just to keep RunL() small
     */
    void IRNetworkObserverRunL();

    /**
     * Extracts the IAP Id of the access point
     */
    void GetAPId();


public: // Public Data Members

    /**
     * Stores the type of connection that is currently open
     */
    TIRConnectionType iIRConnectionType;

    /**
     * Stores the IAP ID of the connection
     */
    TUint iIAPId;

    TBool iIsIAPIdAvailable;

private:

    /** 
     *IR Connection monitor states ( indicates the different 
     *possible states the state machine can be in )
     */
    enum TIRObserverState
    {
    EInitializing,
    EGettingConnectionInfo,
    EGettingIAPId
    };

    /**
     * Used to monitor the network
     */
    RConnectionMonitor iIRConnectionMonitor;

    /**
     * Stores the currently open connection Id
     */
    TUint iConnectionId;

    /**
     * Stores the currently open Sub connection Id
     */
    TUint iSubConnectionCount;

    /**
     * Stores the count of currently open sub connections
     */
    TUint iConnectionCount;

    /**
     * Stores the state of the observer
     */
    TIRObserverState iObserverState;

    /**
     * Stores the type of connection open
     */
    TInt iConnectionType;

    /**
     * Pointer to the network controller object
     */
    MIRNetworkController* iMonitorObserver;

    /**
     * Boolean indicates if monitoring option is requested by the creator
     * of the CIRNetworkController instance
     */
    TBool iMonitoringRequired;

    CIRNetworkController *iNetworkController;
    };

#endif // C_IRNETWORKOBSERVER_H
