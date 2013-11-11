/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#ifndef IRQNETWORKCONTROLLERPRIVATE_H_
#define IRQNETWORKCONTROLLERPRIVATE_H_

#include <QObject>
#include "irnetworkcontroller.h"
#include "irqenums.h"
#include "irqevent.h"

class IRQNetworkController;
/**
 * This class implements the IR Network Controller component
 */

class IRQNetworkControllerPrivate : public MIRActiveNetworkObserver
                                  , public MIRNetworkController
{    
public:
    IRQNetworkControllerPrivate(IRQNetworkController* aNetworkController);
    
    /**
     *  Create resources
     *  @return bool
     */
    bool init();
    /**
     *  Default C++ Destructor
     */
    ~IRQNetworkControllerPrivate();

    /**
     *  Return the variable which indicates if connection is active or not
     *  @return bool
     */
    bool getNetworkStatus() const;

    /**
     *  Return the IAP Id of the chosen IAP
     *  @return int Error code
     */
    IRQError getIAPId(unsigned long& aIapId) const;
    
    /**
     *  Configure the Access Point which is used by all the components for network connectivity
     */
    void  chooseAccessPoint();

    /*
     * Cancel configuring access point
     */
    void cancelConnecting();
    
    /**
     *  This api is used to determine if the phone is in offline mode
     *  @return True if the phone is in offline mode else False
     */
    bool isOfflineMode();

    /**
     *  This api is used to determine if the phone supports WLan usage
     *  @return True if the phone supports else False
     */
    bool isWlanSupported() const;

    /**
     *  Reset the connection status to Disconnected state
     */
    void resetConnectionStatus();

    /**
     *  Used to determine the type of connection
     *  @return enum describing the type of connection ( GPRS/3G/WiFi )
     */
    IRQConnectionType identifyConnectionType() const;

    /**
     *  Notifies all observers whose network request is active to reissue the request
     */
    void notifyActiveNetworkObservers(IRQNetworkEvent aEvent);

    /**
     *  Indicates if the hand over of network connection has happened
     */
    bool isHandlingOverConnection();

    /**
     *  Indicates if chooseAccessPoint is called
     */
    bool isConnectRequestIssued() const;

    /**
     *  MIRActiveNetworkObserver::NotifyActiveNetworkObserversL()
     *  Callback which notifies all observers whose network request is active to reissue the request
     *  @param aEvent Indicates the type of network event that occurred
     */
    void NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent);

    /**
     *  MIRActiveNetworkObserver::ResetPendingRequests()
     *  Callback which notifies all observers whose network request is active to reset the pending
     *  request status
     */
    void ResetPendingRequests(TBool aValue);

    /**
     *  MIRNetworkController::IRNetworkEventL()
     *  @param aEvent Indicates the type of network event that occurred
     */
    void IRNetworkEventL(TIRNetworkEvent aEvent);

private:

    /**
     *  NetworkController singleton instance
     */
    CIRNetworkController* iNetworkController;
    
    IRQNetworkController* const q_ptr;
};

#endif /* IRQNetworkControllerPrivate_H_ */
