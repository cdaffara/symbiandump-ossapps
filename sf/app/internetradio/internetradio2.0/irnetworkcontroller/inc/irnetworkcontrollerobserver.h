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


#ifndef M_IRNETWORKCONTROLLEROBSERVER_H
#define M_IRNETWORKCONTROLLEROBSERVER_H

#include <e32def.h>

/** 
* Identifies the type of connection 
*/    
enum TIRConnectionType
    {
    EGprs,
    EWcdma,
    ECdma2000,
    EWiFi,
    EEdge
    };   

/** 
* Indicates the various Network events
*/    
enum TIRNetworkEvent
    {
    ENetworkConnectionConnecting,
    ENetworkConnectionEstablished,
    ENetworkConnectionDisconnected,
    EDisplayNetworkMessageNoConnectivity,
    EDisplayOfflineMode,
    EDisplayNoAccessPointsDefined,
    EAccessPointSelectionCancelled,
    EConnectingCancelled
    };   


/** 
* Observers should derive from this class and pass a pointer of this
* type when creating an instance of CIRNetworkController if they require
* an observer
*/  
class MIRNetworkController
    {
public:

    /** 
    * IRNetworkEventL( TIRNetworkEvent aEvent )    *  @param aEvent Indicates the type of network event that occurred
    */
    virtual void IRNetworkEventL( TIRNetworkEvent aEvent ) = 0;
    };

#endif // M_IRNETWORKCONTROLLEROBSERVER_H
