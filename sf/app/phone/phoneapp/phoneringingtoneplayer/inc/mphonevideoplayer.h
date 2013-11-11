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

#ifndef M_PHONEVIDEOPLAYER_H
#define M_PHONEVIDEOPLAYER_H

// Forward declarations
class MPhoneVideoPlayerObserver;

/**
 *  Abstract video playing interface.
 *
 *
 *  @lib Phone.exe
 *  @since S60 v3.2
 */
class MPhoneVideoPlayer
    {
public:
    /**  Play modes */
    enum TPlayMode
        {
        EPlayInLoop,
        EPlayOnce,
        EPlayAscending    
        }; 

public:
    /**
    * Plays given video ringing tone.
    *
    * @since S60 S60 v3.2
    * @param aFileName File name.
    * @param aPlayMode Playing mode.
    * @param aVolumeLevel Playing volume, range: 0-10.
    * @param aArbitraryScaling Set ETrue if arbitrary scaling supported.
    * @param aObserver Observer for playback events.
    */
    virtual void PlayVideoRingTone( const TDesC& aFileName,
                                    TPlayMode aPlayMode,
                                    TInt aVolumeLevel,
                                    TBool aArbitraryScaling,
                                    MPhoneVideoPlayerObserver* aObserver ) = 0;
    
    /**
    * Stops ringing.
    *
    * @since S60 S60 v3.2
    */
    virtual void StopVideoRingTone() = 0;
    
    /**
    * Mutes ringing tone playing.
    *
    * @since S60 S60 v3.2
    */    
    virtual void MuteVideoRingTone() = 0;
        
    /**
    * Cancels ringing tone playing.
    *
    * @since S60 S60 v3.2
    */
    virtual void CancelVideoRingTone() = 0;
    };


#endif // M_PHONEVIDEOPLAYER_H
