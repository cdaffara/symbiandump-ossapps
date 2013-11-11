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
* Description:  This class plays local audio file
*
*/



#ifndef CLOCALAUDIOPLAYBACK_H
#define CLOCALAUDIOPLAYBACK_H

// INCLUDES
#include <mpxplaybackplugin.h>
#include <mdaaudiosampleplayer.h>
#include <mpxcenrepobserver.h>

// FORWARD DECLARATIONS
class CMPXCenRepWatcher;
class CMPXDrmMediaUtility;
class CMPXAudioEffectEngine;

// CONSTANTS

// CLASS DECLARATION

/**
*  CMPXLocalAudioPlayback class
*
*  Plug-in basically handles audio via MMF's audio player utility.
*  @lib LocalAudioPlayback.lib
*/
NONSHARABLE_CLASS(CMPXLocalAudioPlayback) : public CMPXPlaybackPlugin,
                                            public MMdaAudioPlayerCallback,
                                            public MMPXCenRepObserver
    {
    private:
        enum TMPXLocalAudioPlaybackState
            {
            EStateNotInitialised,
            EStateInitialising,
            EStateInitialised
            };
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aInitParams, initialization parameter
        * @return a pointer to the created instance
        */
        static CMPXLocalAudioPlayback* NewL(TAny* aInitParams);

        /**
        * Destructor
        * Destroy the object and release all memory objects
        */
        ~CMPXLocalAudioPlayback();

    private: // Functions from base classes
        /**
        *  from CMPXPlaybackPlugin
        */

        /**
        * Set observer
        *
        * @param aObs observer
        */
        void SetObserver(MMPXPlaybackPluginObserver& aObs);

        /**
        * Initializes a song for playback
        * @param aSong the song path
        */
        void InitialiseL(const TDesC& aSong);

        /**
        * Initializes a song for playback
        * @param aFile file handle of a song
        */
        void InitialiseL(RFile& aFile);

        /**
        * Executes a command on the selected song
        * @param aCmd a command
        * @param aData, data
        */
        void CommandL(TMPXPlaybackCommand aCmd, TInt aData=0);

        /**
        * Sets a property of the plugin
        * @param aProperty a property
        * @param aValue the value of the setting
        */
        void SetL(TMPXPlaybackProperty aProperty,TInt aValue);

        /**
        * Gets a property of the plugin (async)
        * @param aProperty a property
        */
        void PropertyL(TMPXPlaybackProperty aProperty) const;

        /**
        * Gets a list of sub players
        * @return a list of names of sub players
        */
        void SubPlayerNamesL();

        /**
        * Select a sub player
        * @param aIndex index to the sub player
        */
    	void SelectSubPlayerL(TInt aIndex);

        /**
        * Returns current sub player name
        * @return friendly name of the current the sub player
        */
        const TDesC& SubPlayerName();

        /**
        * Current sub player index
        * @return index to the sub player
        */
        TInt SubPlayerIndex() const;

        /**
        * Media properties of the current file (async)
        * @param aAttrs attributes requested
        */
        void MediaL(const TArray<TMPXAttribute>& aAttrs);

        /**
        * Cancel async request
        */
        void CancelRequest();

        /**
        *  From MMdaAudioPlayerCallback
        */
        void MapcInitComplete(TInt aError,
                              const TTimeIntervalMicroSeconds& aDuration);

        /**
        *  From MMdaAudioPlayerCallback
        */
        void MapcPlayComplete(TInt aError);

        /**
        * From MMPXCenRepObserver
        * Handle a change in a setting value.
        *
        * @param aRepositoryUid Central repository UID containing the setting
        * @param aSettingId Id of the setting
        * @since 3.0
        */
        void HandleSettingChange( const TUid& aRepositoryUid,
                                  TUint32 aSettingId);
    private:
       /**
        * C++ default constructor
        */
        CMPXLocalAudioPlayback();

        /**
        * the second phase constructor ConstructL to safely construct things
        * that can leave
        */
        void ConstructL();

        /**
        * Sets the volume level in audio controller
        * @param aVolume Volume level, ranges from 0 - KPbPlaybackVolumeLevelMax
        * @param aNotifyChange ETrue if EPVolumeChanged must be sent to observer
        */
        void SetVolume( TInt aVolume, TBool aNotifyChange = ETrue );

        /**
        * Sets the volume level in audio controller to mute.
        * @param aMute ETrue to indicated mute
        * @param aNotifyChange ETrue if EPMuteChanged must be sent to observer 
        */
        void SetMute( TBool aMute, TBool aNotifyChange = ETrue );

        /**
        *  Consumes the rights for the current media according
        *  to the specified consume type
        *
        *  @param aType Type of consumption to execute
        */
        void ConsumeRightsL(ContentAccess::TIntent aIntent);


    private: // Data
        CMPXCenRepWatcher* iVolumeWatcher;
        CMPXCenRepWatcher* iMuteWatcher;
        CMdaAudioPlayerUtility* iPlayer;
        HBufC* iSong;
        TInt iVolume;
        RFs iFs;
        RFile iFile;
        CMPXDrmMediaUtility* iDrmMediaUtility;
        CMPXAudioEffectEngine* iAudioEffects;
        TMPXLocalAudioPlaybackState iState;
        TBool iClosedByAudioPolicy;
        TBool iConsumeStarted;
        TBool iAudioEffectsOn;
        TBool iIsPlaying;
    };

#endif      // CLOCALAUDIOPLAYBACK_H


