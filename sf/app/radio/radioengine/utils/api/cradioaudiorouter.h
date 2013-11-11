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

#ifndef C_RADIOAUDIOROUTER_H
#define C_RADIOAUDIOROUTER_H

#include <e32base.h>

#include "radioenginedef.h"

// Forward declarations
class CRadioRoutableAudio;
class MRadioAudioRoutingObserver;

/**
 * CRadioAudioRouter
 *
 */
NONSHARABLE_CLASS( CRadioAudioRouter ) : public CBase
    {
public:

    IMPORT_C static CRadioAudioRouter* NewL( MRadioAudioRoutingObserver& aAudioRoutingObserver );

    IMPORT_C ~CRadioAudioRouter();

    /**
     * Sets audio route
     *
     * @param aAudioRoute new audio route
     */
    IMPORT_C void SetAudioRouteL( RadioEngine::TRadioAudioRoute aAudioRoute );

    /**
     * Register routable audio which routing will be changed via this class.
     *
     * @param aRoutableAudio audio which routing will be added to routable
     * audios. Ownership is not moved.
     */
    IMPORT_C void RegisterRoutableAudio( CRadioRoutableAudio* aRoutableAudio );

    /**
     * Removes registration of routable audio.
     *
     * @param aRoutableAudio audio which routing registration will be
     * removed. Ownership is not moved.
     */
    IMPORT_C void UnRegisterRoutableAudio( CRadioRoutableAudio* aRoutableAudio );

private:

    CRadioAudioRouter( MRadioAudioRoutingObserver& aAudioRoutingObserver );

    void ConstructL();

private: // data

    /**
     * Reference to audio routing observer. Gets informed when audio routing
     * is changed.
     */
    MRadioAudioRoutingObserver& iAudioRoutingObserver;

    /**
     * Contains routable audios.
     */
    RArray<CRadioRoutableAudio*> iRoutableAudios;

    };

#endif // C_RADIOAUDIOROUTER_H
