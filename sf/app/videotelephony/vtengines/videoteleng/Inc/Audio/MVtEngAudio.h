/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Audio volume and routing interface
*
*/



#ifndef MVTENGAUDIO_H
#define MVTENGAUDIO_H

//  INCLUDES
#include <e32def.h>

/**
*  Interface for audio related information.
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngAudio
    {
    public:

        /**
        * Current routing state
        */
        enum TAudioRoutingState 
            {
            EAudioHandset,
            EAudioLoudspeaker,
            EAudioBT,
            EAudioWiredHeadset
            };

        /**
        * Wired headset type
        */
        enum TWiredHeadsetType
            {
            EWiredHeadset,
            EWiredLoopset,
            EWiredTty,
            EWiredNone
            };

        /**
        * Audio routing setting to be used when changing
        * audio routing from client.
        */
        enum TVtEngRoutingSetting
            {
            EActivateHandsfree,
            EDeactivateHansfree,
            EActivateBT,
            EDeactivateBT
            };

        /**
        * Speaker volume levels for Handset and Handsfree.
        */
        struct TVtEngOutputVolume
            {
            TInt iHandsetVolume;
            TInt iHandsfreeVolume;
            };

        /**
        * Returns target where audio is currently routed to.
        * @param aAudioRoutingState routing target
        * @return Symbian OS error code
        */
        virtual TInt GetRoutingState(
            TAudioRoutingState& aAudioRoutingState ) = 0;

        /**
        * Returns if routing to specified target can be
        * done.
        * @param aAudioRoutingState routing target
        * @param aAvailable ETrue if possible to do
        * @return Symbian OS error code
        */
        virtual TInt GetRoutingAvailability(
            const TAudioRoutingState aAudioRoutingState,
            TBool& aAvailable ) = 0;

        /**
        * Returns type of wired headset
        * @return Symbian OS error code.
        */
        virtual TInt GetHeadsetType(
            TWiredHeadsetType& aHeadsetType ) = 0;

        /**
        * Returns speaker volume
        * @param aHandsetVolume if ETrue returns handset volume,
        * otherwise loudspeaker volume.
        * @return output volume ranging from 1 to 10 or Symbian OS error code.
        */
        virtual TInt OutputVolume( 
            const TBool aHandsetVolume ) const = 0;


    };

#endif      // MVTENGAUDIO_H
