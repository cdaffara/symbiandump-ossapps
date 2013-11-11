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
#ifndef IRQMMFADAPTER_H_
#define IRQMMFADAPTER_H_

#include <videoplayer2.h>
#include <MMFMetadataEventConfigConstants.h>
#include "irqplayeradapterinterface.h"

class IRQMetaData;
class CStereoWidening;

/**
 * This class wraps the MMF interface CVideoPlayerUtility
 */
class IRQMMFAdapter : public IRQPlayerAdapterInterface,
                      public MVideoPlayerUtilityObserver,
                      public MVideoLoadingObserver
{
    Q_OBJECT

public:
    IRQMMFAdapter();
    ~IRQMMFAdapter();

    // IRQPlayerAdapterInterface methods
    void  playStation(const QString &aUrl, int aApId); // Play specific url via certain access point id
    void  stop();                                      // Stop Playback
    void  setVolume(int aVolume);                      // Set volume to player
    int   getVolume();                                 // Get current volume from player
    void* getPlayerInstance();                         // Get audio player instance for stereo effect
    void  enableStereoEffect();                        //Turns on the stereo effect
    void  disableStereoEffect();                       //Turns off the stereo effect

    // Callback functions, MVideoPlayerUtilityObserver methods
    void MvpuoOpenComplete(TInt aError);                  // Opening url completed
    void MvpuoPlayComplete(TInt aError);                  // Playback completed, never called
    void MvpuoEvent(const TMMFEvent& aEvent);             // Events from player
    void MvpuoPrepareComplete(TInt aError);               // Preparation for playback completed.
    void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError); // For video playback only.

    // Callback functions, MVideoLoadingObserver methods
    void MvloLoadingStarted();                      // Buffering started
    void MvloLoadingComplete();                     // Buffering completed.

private:
    void playL(const QString &aUrl, int aApId);     // Play a station
    void getRefreshedMetaDataL(TInt aIndex);        // Get refreshed data from player
    void createPlayerL();                           // Created player
    void destroyPlayer();                           // Destroy player
    void setMetadataEventConfig();                  // Enable meta data event
    static TInt isPrepareCompleted(TAny* aPointer); // Periodic called function
    void checkPrepare();                            // Check if preparation completed
    void enableStereoEffectL();
    
private:
    CVideoPlayerUtility2*  iVideoPlayer;
    IRQMetaData*          iQMetaData;       // Saving meta data
    /*
     * Sometimes the station server is on without encoder. In this case,
     * MvpuoPrepareComplete is not called after MvpuoOpenComplete. Need to stop playback
     * in this case. So start a timer to check if the preparation is complete.
     */
    CPeriodic*            iPrepareTimer;
    QString               iLastArtistSongName;
    
    /**
     *  Used for handling Stereo Mode
     */
    CStereoWidening* iStereoEffect;
};

#endif /* IRQMMFADAPTER_H_ */
