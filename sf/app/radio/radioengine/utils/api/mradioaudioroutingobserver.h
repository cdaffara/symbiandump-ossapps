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

#ifndef MRADIOAUDIOROUTINGOBSERVER_H
#define MRADIOAUDIOROUTINGOBSERVER_H

#include "radioenginedef.h"

/**
 *  Interface for observing audio routing events.
 *
 *  Implementer will be notified when audio routing changes
 *  between IHF and headset.
 *
 */
NONSHARABLE_CLASS( MRadioAudioRoutingObserver )
    {
public:

    /**
     * Notifies of a requested audio routing change. Changes requested
     * by other applications do not generate a notification.
     *
     * @param aRoute Current audio route
     */
    virtual void AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute ) = 0;

    };

#endif // MRADIOAUDIOROUTINGOBSERVER_H
