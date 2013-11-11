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
* Description:  Telephony Internal Singleton UIDs.
*
*/


#ifndef TELINTERNALSINGLETONUIDS_H
#define TELINTERNALSINGLETONUIDS_H

#include <e32std.h>

// CONSTANTS

/*****************************************************************************
* Telephony Internal Singleton UIDs 
*****************************************************************************/

// Phone Application's State Machine Singleton UID
static const TUid KUidStateHandleSingleton = { 0x01 };

// Phone Application's View Controller Singleton UID
static const TUid KUidPhoneViewControllerSingleton = { 0x02 };

// Phone Application's Status Pane Handler Singleton UID
static const TUid KUidPhoneStatusPaneHandlerSingleton = { 0x03 };

// Phone Application's Resource Resolver Singleton UID
static const TUid KUidMainResourceResolverSingleton = { 0x04 };

// Phone Application's Logger Singleton UID
static const TUid KUidPhoneUILoggerSingleton = { 0x05 };

// Phone Application's Central Repository Proxy Singleton UID
static const TUid KUidCenRepProxySingleton = { 0x06 };

// Phone Application's Publish and Subscribe Proxy Singleton UID
static const TUid KUidPubSubProxySingleton = { 0x07 };

// Phone Application's Recovery System Singleton UID
static const TUid KUidRecoverySystemSingleton = { 0x08 };

// Phone Application's Reconnect Query Singleton UID
static const TUid KUidReconnectQuerySingleton = { 0x09 };

// Phone Application's Call Header Animation Singleton UID
static const TUid KUidCallHeaderTextAnimationSingleton = { 0x0A };

// Phone Application's Call Black List Clearing Singleton UID
static const TUid KUidClearBlackListSingleton = { 0x0B };

// Phone Application's Error message Handler Singleton UID
static const TUid KUidMainErrorMessagesHandlerSingleton = { 0x0D };

// Phone Application's bluetooth accessory disconnect handler singleton UID.
static const TUid KUidBtaaDisconnectHandlerSingleton = { 0x0E };

// Phone Mediator Command Listener Singleton UID
static const TUid KUidMediatorCommandListenerSingleton = { 0x0F };

// Phone Mediator Sender Singleton UID
static const TUid KUidMediatorSenderSingleton = { 0x10 };

// Phone Mediator Factory Singleton UID
static const TUid KUidMediatorFactorySingleton = { 0x11 };

#endif // TELINTERNALSINGLETONUIDS_H

// End of File
