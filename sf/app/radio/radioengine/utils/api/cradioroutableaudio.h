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

#ifndef C_RADIOROUTABLEAUDIO_H
#define C_RADIOROUTABLEAUDIO_H

// System includes
#include <e32base.h>

// User includes
#include "radioenginedef.h"

// Forward declarations
class CAudioOutput;
class CRadioAudioRouter;

/**
 * CRadioRoutableAudio
 *
 * Defines common functionality for routable audio sources by defining common
 * skeleton method for routing.
 *
 */
class CRadioRoutableAudio : public CBase
    {

public:

    IMPORT_C CRadioRoutableAudio( CRadioAudioRouter* aAudioRouter );

    IMPORT_C ~CRadioRoutableAudio();

    /**
     * Sets audio routing. Please note that subclasses must define
     * <code>RoutedAudio</code> method otherwise this will not do anything.
     * @param aDestination 'Not documented'? old documentation: if <code>true</code> audio routing will be set to
     * speaker otherwise to headset.
     */
    IMPORT_C void SetAudioRouteL( RadioEngine::TRadioAudioRoute aRoute );

    /**
     * Tells if audio routing is supported
     * @return EFalse if routing is not supported, otherwise true
     */
    IMPORT_C TBool AudioRoutingSupported() const;

protected:

    /**
     * Sets audio output. Must be called from subclass.
     * Ownership is moved.
     * @param aAudioOutput for setting routing.
     */
    IMPORT_C void SetAudioOutput( CAudioOutput* aAudioOutput );

    /**
     * Deletes audio output.
     * Must be called from subclass before
     * the actual audio utility class is deleted.
     */
    IMPORT_C void DeleteAudioOutput();

protected: //data

    /**
     * Routes audio. Used to register/unregister this routable audio for routing.
     * Own.
     */
    CRadioAudioRouter* iAudioRouter;

private: // data

    /**
     * AudioOutput for setting routing.
     * Own.
     */
    CAudioOutput*       iAudioOutput;

    };

#endif // C_RADIOROUTABLEAUDIO_H
