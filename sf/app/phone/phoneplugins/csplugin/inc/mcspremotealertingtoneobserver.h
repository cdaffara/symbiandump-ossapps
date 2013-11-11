/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer for remote alerting tone change.
*
*/


#ifndef MCSPREMOTEALERTINGTONEOBSERVER_H
#define MCSPREMOTEALERTINGTONEOBSERVER_H

#include <e32std.h>
#include <rmmcustomapi.h>

/**
 *  Observer for remote alerting tone status changes.
 *
 *  @lib CSPlugin
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( MCSPRemoteAlertingToneObserver )
    {

public:

    /**
     * Notification of remote alerting tone status change.
     *
     * @since S60 v5.0
     * @param aNewStatus New remote alerting tone status. 
     */
    virtual void RemoteAlertingToneStatusChanged( 
        RMmCustomAPI::TRemoteAlertingToneStatus aNewStatus ) = 0;

    };


#endif // MCSPREMOTEALERTINGTONEOBSERVER_H
