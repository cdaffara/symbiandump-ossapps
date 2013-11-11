/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network info listener.
*
*/


#ifndef CNETWORKLISTENER_H
#define CNETWORKLISTENER_H

// System includes
#include <e32base.h>
#include <nwhandlingengine.h>
#include <qsysteminfo.h>
#include <MSSSettingsRefreshObserver.h>
#include <MPsetNetworkInfoObs.h>

// User includes

// Forward declarations
class CNWSession;
class MNetworkListenerObserver;
class CPsetNetwork;
class CPsetContainer;
class CPSetRefreshHandler;

using namespace QtMobility;

/**
 *  Network info listener.
 */
NONSHARABLE_CLASS( CNetworkListener ) : public CBase, 
    public MNWMessageObserver,
    public MSSSettingsRefreshObserver,
    public MPsetNetworkInfoObserver
    {
public:

    /**
     * Get pointer to network info listener.
     * @return pointer to network info listener.
     */
    static CNetworkListener* NewL( MNetworkListenerObserver& aObserver );
    
    ~CNetworkListener();

    // from base class MNWMessageObserver

    /**
     * From MNWMessageObserver.
     * Called by network handling engine when network info changes.
     * @param aMessage is type of the change.
     */
    void HandleNetworkMessage( const TNWMessages aMessage );

    /**
     * From MNWMessageObserver.
     * Called fi network handling engine fails.
     * @param aOperation is failed operation.
     * @param aErrorCode is fail reason.
     */
    void HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode );
    
    static TInt NWLostDelayCallBack(TAny* aParam);
    
    static TInt NWSimRefreshCallBack(TAny* aParam);
    
    /**
     * From MSSSettingsRefreshObserver.
     * Refresh query. Client should determine whether it allow the
     * refresh to happen.
     * 
     * @param aType  Refresh type.
     * @param aFiles Elementary files which are to be changed. 
     *               May be zero length. 
     * @return ETrue to allow refresh, EFalse to decline refresh.
     */
    TBool AllowRefresh(
        const TSatRefreshType aType,
        const TSatElementaryFiles aFiles );

    /**
     * From MSSSettingsRefreshObserver.
     * Notification of refresh.
     * 
     * @param aType Type of refresh which has happened.
     * @param aFiles List of elementary files which have been changed.
     *               May be zero length.
      */
    void Refresh(
        const TSatRefreshType aType,
        const TSatElementaryFiles aFiles );
   
    /**
    * Shows notes when network information received.
    *
    * @param aInfoArray Network information. Receiver owns the array.
    * @param aResult Result of action.
    */
    void HandleNetworkInfoReceivedL( 
        const CNetworkInfoArray* aInfoArray, const TInt aResult );
    
    /**
    * Shows notes when requested for current network information.
    *
    * @param aCurrentInfo Current network information.
    * @param aResult Result of action.
    */
    void HandleCurrentNetworkInfoL( 
        const MPsetNetworkSelect::TCurrentNetworkInfo& aCurrentInfo, 
        const TInt aResult );
    
    /**
    *  DEPRECATED.
    */
    void HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo& aCurrentInfo,
        const MPsetNetworkSelect::TCurrentNetworkStatus aStatus, 
        const TInt aResult );    

    /**
    * Shows notes when network change has been approved.
    *
    * @param aCurrentInfo Current network information.
    * @param aStatus Current network status.
    * @param aResult Result of action.
    */
    void HandleNetworkChangedL( 
        const MPsetNetworkSelect::TNetworkInfo& aCurrentInfo,
        const RMobilePhone::TMobilePhoneRegistrationStatus& aStatus, 
        const TInt aResult );    

    /**
    * Searches for networks that have coverage.
    *
    * @param aRequest Type of request that is being handled.
    */
    void HandleSearchingNetworksL( TServiceRequest aRequest );

    /**
    * Handles user request to register to specific network.
    *
    * @param aOngoing Is there an ongoing request.
    */
    void HandleRequestingSelectedNetworkL( TBool aOngoing );

    /**
    * Handles notes when call is active.
    */
    void HandleCallActivatedL();

    /**
    * Handles errors.
    * 
    * @param aRequest current request.
    * @param aError error code.
    */
    void HandleNetworkErrorL( const MPsetNetworkInfoObserver::TServiceRequest aRequest,
        const TInt aError );

private:
    
    CNetworkListener( MNetworkListenerObserver& aObserver );
    
    void ConstructL();
    
    void HandleNetworkFound();
     
    void HandleNetworkLost();
    
    /**
     * Tests if network connections are allowed.
     * @return true if network connections are allowed
     *    otherwise eg. in offline mode false
     */
    TBool IsNetworkConnectionAllowed() const;
    
    /**
     * Tests if Bluetooth SAP is in connected mode.
     * @return ETrue if Bluetooth SAP is active.
     */
    TBool IsBluetoothSAPConnected() const;
    
    /**
     * Tests if SIM is in OK state.
     * @return ETrue if SIM is in OK state.
     */
    TBool IsSimOk() const;
    
private: // data

    /**
     * Session to network handling engine.
     * Own.
     */
    CNWSession* iSession;

    /**
     * Cached network info structure.
     */
    TNWInfo iInfo;
    
    /**
     * Timer to delay the showing of the notification if needed
     * Own
     */
    CPeriodic *iTimer;
    
    /**
     * A true value if registered to network.
     */
    TBool iRegistered;
    
    /**
     * Observer.
     */
    MNetworkListenerObserver& iObserver;

    /**
     * System device info API.
     * Own.
     */
    QSystemDeviceInfo* iDeviceInfo;
    
    /**
     * PhoneSettings container.
     * Own.
     */
    CPsetContainer* iContainer;
    
    /**
     * PhoneSettings engine object.
     * Own.
     */
    CPsetNetwork* iSettingsEngine;
    
    /**
     * PhoneSettings refresh handler.
     * Own.
     */
    CPSetRefreshHandler* iRefreshHandler;
    
    };
    
#endif // CNETWORKLISTENER_H
