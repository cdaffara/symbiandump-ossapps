/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility methods for Video Telephone application.
*
*/



#ifndef LCVTUTILITY_H
#define LCVTUTILITY_H

//  INCLUDES
#include    <e32base.h>
#include    <mvtengaudio.h>
#include    <mvtengmedia.h>
#include    <w32std.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Utility methods for Video Telephone application.
*
*  @since Series 60 2.6
*/
class LcVtUtility
    {
    public: // New functions

        /**
        * Enumerates audio routing masks.
        */
        enum TAudioRoutingMask
            {
            // Deactivate loudspeaker.
            EDeactivateLoudspeaker = (1<<0),
            // Activate loudspeaker.
            EActivateLoudspeaker = (1<<1),
            // Deactivate BT handsfree.
            EDeactivateBtHandsfree = (1<<2),
            // Activate BT handsfree.
            EActivateBtHandsfree = (1<<3)
            };

        /**
        * Gets audio routing availabilities.
        * @param aAudio audio instance.
        * @param aAvailable availability bitmap.
        */
        static void GetAudioRoutingAvailability( 
            MVtEngAudio& aAudio,
            TInt& aAvailable );

        /**
        * Gets outgoing media state.
        * @param aMedia media instance.
        * @param aAvailable availability bitmap.
        */
        static void GetOutgoingMediaState(
            MVtEngMedia& aMedia,
            TInt& aAvailable );

        /**
        * Gets incoming media state.
        * @param aMedia media instance.
        * @param aAvailable availability bitmap.
        */
        static void GetIncomingMediaState(
            MVtEngMedia& aMedia,
            TInt& aAvailable );

        /**
        * Checks if media has still image.
        * @param aMedia media instance.
        * @return ETrue if media has still image.
        */
        static TBool HasStillImage(
            MVtEngMedia& aMedia );

        /**
        * Gets freeze's current status.
        * @param aMedia media instance.
        * @return  ETrue if active provider is frozen, EFalse otherwise.
        */
        static TBool GetFreezeState( 
            MVtEngMedia& aMedia );  
                
        /**
        * Checks if freeze is supported.
        * @param aMedia media instance.
        * @return ETrue if freeze is supported.
        */
        static TBool IsFreezeSupported(
            MVtEngMedia& aMedia );
    
        /**
        * Gets current local video quality setting.
        * @param aMedia media instance. 
        * @param aVideoQuality Current local video quality settings.
        */
        static void GetVideoQuality( 
            MVtEngMedia& aMedia,
            MVtEngMedia::TVideoQuality& aVideoQuality );
        /**
        * Gets current object sharing state.
        * @param aMedia media instance.
        * @param aShareObjectState contains current object sharing state on
        * return.
        */
        static void GetObjectSharingState( MVtEngMedia& aMedia,
            MVtEngMedia::TShareObjectState& aShareObjectState );
        
        /**
        * Checks if zooming is allowed.
        * @param aMedia media instance.
        * @return ETrue if allowed, EFalse otherwise.
        */
        static TBool IsZoomAllowed(
            MVtEngMedia& aMedia );

        /**
        * Checks if the device has cameras currently attached.
        * @param aMedia media instance.
        * @return ETrue if there are cameras available.
        */
        static TBool HasCameras( MVtEngMedia& aMedia );

        /**
        * Checks if the character is DTMF enabled.
        * @param aChar The character.
        * @return ETrue if this character is DTMF character.
        */
        static TBool IsDTMFCharacter( const TChar aChar );

    private:

        /**
        * Checks if audio routing is available.
        * @param aAudio audio instance.
        * @param aCurrent current speaker.
        * @param aSource source speaker.
        * @param aTarget target speaker.
        */
        static TBool IsAudioRoutingAvailable(
            MVtEngAudio& aAudio,
            const MVtEngAudio::TAudioRoutingState aCurrent,
            const MVtEngAudio::TAudioRoutingState aSource,
            const MVtEngAudio::TAudioRoutingState aTarget );

    };

#endif      // LCVTUTILITY_H
            
// End of File
