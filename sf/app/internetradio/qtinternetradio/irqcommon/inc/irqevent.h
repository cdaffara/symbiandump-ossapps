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
#ifndef IRQEVENT_H_
#define IRQEVENT_H_

/**
* Identifies the type of connection
*/
enum IRQConnectionType
    {
    EIRQGprs,
    EIRQWcdma,
    EIRQCdma2000,
    EIRQWiFi,
    EIRQEdge
    };

/**
* Indicates the various Network events
*/
enum IRQNetworkEvent
{
    EIRQNetworkConnectionConnecting,
    EIRQNetworkConnectionEstablished,
    EIRQNetworkConnectionDisconnected,
    EIRQDisplayNetworkMessageNoConnectivity,
    EIRQDisplayOfflineMode,
    EIRQDisplayNoAccessPointsDefined,
    EIRQAccessPointSelectionCancelled,
    EIRQConnectingCancelled
};


#endif /* IRQEVENT_H_ */
