/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The interface for observing the application level roaming events
*
*/


#ifndef M_IRCONNECTIONROAMINGOBSERVER_H
#define M_IRCONNECTIONROAMINGOBSERVER_H

#include <e32def.h>

/**
 * The interface for observing the application level roaming events
 */
NONSHARABLE_CLASS( MIRConnectionRoamingObserver )
    {
public:

    /**
     * The connection roaming event type
     */
    enum TIRRoamingEvent
        {
        /** 
         * The new connection is being established 
         */
        EIRRoamingEventHandingOver,

        /**
         * The new connection is established and the old one will be closed
         */
        EIRRoamingEventConnected,

        /**
         * The establishment of the new connection has failed and the old connection will be used
         */
        EIRRoamingEventFailed
        };

    /**
     * Notifies of the application level roaming event
     * 
     * @param aEvent The event that occured
     */
    virtual void HandleRoamingEventL( TIRRoamingEvent aEvent ) = 0;
    };

#endif // M_IRCONNECTIONROAMINGOBSERVER_H
