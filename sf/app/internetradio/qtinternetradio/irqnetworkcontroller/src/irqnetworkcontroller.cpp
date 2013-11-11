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

#include "irqnetworkcontroller.h"
#include "irqnetworkcontroller_p.h" 


IRQNetworkController * IRQNetworkController::mInstance = NULL;
QMutex IRQNetworkController::mMutex;

// ---------------------------------------------------------------------------
// IRQNetworkController::openInstance()
// Static function to get a singleton instance of IRQNetworkController
// @return IRQNetworkController *
// ---------------------------------------------------------------------------
//
IRQNetworkController* IRQNetworkController::openInstance()
{
    mMutex.lock();
    
    if (NULL == mInstance) 
    {
        mInstance = new IRQNetworkController();

        if (!mInstance->mInitPrivateSuccess) 
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
    else
    {
        mInstance->mRefCount++;
    }

    mMutex.unlock();

    return mInstance;
}

// ---------------------------------------------------------------------------
// IRQNetworkController::closeInstance()
// Close a singleton instance of IRQNetworkController
// ---------------------------------------------------------------------------
//
void IRQNetworkController::closeInstance()
{
    mMutex.lock();
    if ((--mRefCount) == 0)
    {
        if(this == mInstance)
        {
            mInstance = NULL;
        }
        delete this;
    }
    mMutex.unlock();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::GetNetworkStatus()
// Returns the variable which indicates if connection is active or not
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkController::getNetworkStatus() const
{
    return d_ptr->getNetworkStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::getIAPId()
// Gets the IAP Id of the chosen IAP
// @return IRQError
// ---------------------------------------------------------------------------
//
IRQError IRQNetworkController::getIAPId(unsigned long& aIapId) const
{
    return d_ptr->getIAPId(aIapId);
}

// ---------------------------------------------------------------------------
// IRQNetworkController::chooseAccessPoint()
// Configures the Access Point which is used by all the components for network
// connectivity
// ---------------------------------------------------------------------------
//
void IRQNetworkController::chooseAccessPoint()
{
    d_ptr->chooseAccessPoint();
}

/*
 * Cancel configuring access point
 */
void IRQNetworkController::cancelConnecting()
{
    d_ptr->cancelConnecting();
    d_ptr->resetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isOfflineMode()
// Indicates if the phone is in offline mode
// @return True if the phone is in offline mode else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkController::isOfflineMode()
{
    return d_ptr->isOfflineMode();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isWlanSupported()
// Indicates if the phone supports WLan usage
// @return True if the phone supports else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkController::isWlanSupported() const
{
    return d_ptr->isWlanSupported();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::resetConnectionStatus()
// Resets the connection status to Disconnected state
// ---------------------------------------------------------------------------
//
void IRQNetworkController::resetConnectionStatus()
{
    d_ptr->resetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::identifyConnectionType()
// Indicates the type of connection
// @return enum describing the type of connection ( GPRS/3G/WiFi )
// ---------------------------------------------------------------------------
//
IRQConnectionType IRQNetworkController::identifyConnectionType() const
{
    return d_ptr->identifyConnectionType();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::notifyActiveNetworkObservers()
// Notifies all observers whose network request is active to reissue the request
// ---------------------------------------------------------------------------
//
void IRQNetworkController::notifyActiveNetworkObservers(IRQNetworkEvent aEvent)
{
    d_ptr->notifyActiveNetworkObservers(aEvent);
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isHandlingOverConnection()
// Indicates if the hand over of network connection has happened
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkController::isHandlingOverConnection()
{
    return d_ptr->isHandlingOverConnection();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isConnectRequestIssued()
// Indicates if chooseAccessPoint is called
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkController::isConnectRequestIssued() const
{
    return d_ptr->isConnectRequestIssued();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::IRQNetworkController()
// Default constructor
// ---------------------------------------------------------------------------
//
IRQNetworkController::IRQNetworkController() :
    mRefCount(1), d_ptr(new IRQNetworkControllerPrivate(this)), mInitPrivateSuccess(false)
{
    Q_ASSERT(d_ptr);
    if (d_ptr) 
    {
        mInitPrivateSuccess = d_ptr->init();
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkController::~IRQNetworkController()
// Destructor
//
// ---------------------------------------------------------------------------
//
IRQNetworkController::~IRQNetworkController()
{
    delete d_ptr;
}

