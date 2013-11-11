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
#include <QStringList>
#include "irqnetworkcontroller_p.h"
#include "irqnetworkcontroller.h"

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::IRQNetworkControllerPrivate()
// Constructor
//
// ---------------------------------------------------------------------------
//
IRQNetworkControllerPrivate::IRQNetworkControllerPrivate(IRQNetworkController* aNetworkController) :
    iNetworkController(NULL), q_ptr(aNetworkController)
{

}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::~IRQNetworkControllerPrivate()
// Destructor
//
// ---------------------------------------------------------------------------
//
IRQNetworkControllerPrivate::~IRQNetworkControllerPrivate()
{
    if (iNetworkController)
    {
        iNetworkController->Close();
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::GetNetworkStatus()
// Returns the variable which indicates if connection is active or not
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::getNetworkStatus() const
{
    return (bool)iNetworkController->GetNetworkStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::getIAPId()
// Gets the IAP Id of the chosen IAP
// @return IRQError
// ---------------------------------------------------------------------------
//
IRQError IRQNetworkControllerPrivate::getIAPId(unsigned long& aIapId) const
{
    IRQError retval = EIRQErrorNone;

    if (KErrNotFound == iNetworkController->GetIAPId(aIapId))
    {
        retval = EIRQErrorNotFound;
    }

    return retval;
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::chooseAccessPoint()
// Configures the Access Point which is used by all the components for network
// connectivity
// ---------------------------------------------------------------------------
//
void  IRQNetworkControllerPrivate::chooseAccessPoint()
{
    TRAPD(error, iNetworkController->ChooseAccessPointL());
    if (KErrNone != error)
    {
        emit q_ptr->errorOccured(EIRQErrorGeneral);
    }
}

/*
 * Cancel configuring access point
 */
void IRQNetworkControllerPrivate::cancelConnecting()
{
    iNetworkController->CancelConnecting();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::isOfflineMode()
// Indicates if the phone is in offline mode
// @return True if the phone is in offline mode else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::isOfflineMode()
{
    return (bool)iNetworkController->IsOfflineMode();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::isWlanSupported()
// Indicates if the phone supports WLan usage
// @return True if the phone supports else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::isWlanSupported() const
{
    return (bool)iNetworkController->IsWlanSupported();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::resetConnectionStatus()
// Resets the connection status to Disconnected state
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerPrivate::resetConnectionStatus()
{
    iNetworkController->ResetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::identifyConnectionType()
// Indicates the type of connection
// @return enum describing the type of connection ( GPRS/3G/WiFi )
// ---------------------------------------------------------------------------
//
IRQConnectionType IRQNetworkControllerPrivate::identifyConnectionType() const
{
    return (IRQConnectionType)iNetworkController->IdentifyConnectionType();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::notifyActiveNetworkObservers()
// Notifies all observers whose network request is active to reissue the request
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerPrivate::notifyActiveNetworkObservers(IRQNetworkEvent aEvent)
{
    TRAPD(error, iNetworkController->NotifyActiveNetworkObserversL((TIRNetworkEvent)aEvent));
    if (KErrNone != error)
    {
        emit q_ptr->errorOccured(EIRQErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::isHandlingOverConnection()
// Indicates if the hand over of network connection has happened
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::isHandlingOverConnection()
{
    return (bool)iNetworkController->IsHandingOverConnection();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::isConnectRequestIssued()
// Indicates if chooseAccessPoint is called
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::isConnectRequestIssued() const
{
    return (bool)iNetworkController->iIsConnectRequestIssued;
}

// ---------------------------------------------------------------------------
// MIRActiveNetworkObserver::NotifyActiveNetworkObserversL()
// Callback which notifies all observers whose network request is active to reissue the request
// @param aEvent Indicates the type of network event that occurred
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerPrivate::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
{
    emit q_ptr->networkRequestNotified((IRQNetworkEvent)aEvent);
}

// ---------------------------------------------------------------------------
// MIRActiveNetworkObserver::ResetPendingRequests()
// Callback which notifies all observers whose network request is active to
// reset the pending request status
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerPrivate::ResetPendingRequests(TBool aValue)
{
    emit q_ptr->pendingRequestsReset((bool)aValue);
}

// ---------------------------------------------------------------------------
// MIRNetworkController::IRNetworkEventL()
// @param aEvent Indicates the type of network event that occurred
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerPrivate::IRNetworkEventL(TIRNetworkEvent aEvent)
{
    emit q_ptr->networkEventNotified((IRQNetworkEvent)aEvent);
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerPrivate::init()
// Create resources
//
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerPrivate::init()
{
    bool br = false;
    TInt leaveCode;
    
    // Get singleton instance
    TRAP(leaveCode, iNetworkController = CIRNetworkController::OpenL(this));
    if(KErrNone == leaveCode)
    {
        TRAP(leaveCode,iNetworkController->RegisterActiveNetworkObserverL(*this));
        if(KErrNone == leaveCode)
        {
            br = true;
        }
    }
   
    return br;
}
