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
* Description:  Audio effect engine
*
*/


#ifndef _MPXAUDIOEFFECTENGINE_H_
#define _MPXAUDIOEFFECTENGINE_H_
#define _LOUDNESS
//#undef _LOUDNESS
//#define _BASSBOOST
#undef _BASSBOOST
#define _REVERB
//#undef _REVERB
#define _STEREO


// INCLUDES
const TInt KAudioEffectsID = 0x101FFC02;
const TInt KEqualizerID = 0x101FFC76;

// System Includes
#include <e32base.h>
#include <mdaaudiosampleplayer.h>

#include <mpxplaybackobserver.h>
#include "mpxaudioeffectproperties.h"

// FORWARD DECLARATIONS
class CMdaAudioPlayerUtility;
class CEnvironmentalReverbUtility;
class CStereoWidening;
class CBassBoost;
class CLoudness;
class CAudioEqualizerUtility;

class MMPXPlaybackUtility;
class TMPXPlaybackMessage;

/**
*  Audio effect engine Class
*
*  @lib mpxaudioeffectengine.dll
*  @since Series 60 3.0
*/
class CMPXAudioEffectEngine : public CBase
{
public: 

    /**
    * Two-phased constructor
    *
    * @param aPlayer audio player utility
    * @return object of constructed
    */
	IMPORT_C static CMPXAudioEffectEngine* NewL(
	                                    CMdaAudioPlayerUtility* aPlayer);
	
    /**
    * Destructor
    */
	IMPORT_C ~CMPXAudioEffectEngine();
	
    /**
    * Set balance
    */
 	IMPORT_C void SetBalanceL();

    /**
    * Set bass boost
    */
	IMPORT_C void SetBassBoostL();

    /**
    * Set stero widening
    */
	IMPORT_C void SetStereoWideningL();

    /**
    * Set reverberation
    */
 	IMPORT_C void SetReverberationL();

    /**
    * Set loundness
    */
    IMPORT_C void SetLoudnessL();

    /**
    * Sets the equalizer preset
    * This function will check MPX cenrep for preset id
    */
    IMPORT_C void SetEqualizerL();
    
    /**
     * Creates the audio effects
     */
    IMPORT_C void CreateAudioEffectsL();

    /**
    * Destroy audio effect objects
    */
    IMPORT_C void DestroyAudioEffect();
   
private:
    /**
    * C++ constructor
    *
    * @param aPlayer audio player utility
    */
	CMPXAudioEffectEngine(CMdaAudioPlayerUtility* aPlayer);

    /**
    * Second phase constructor
    */
    void ConstructL();
 
    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState );

private: //Data
	
    CMPXAudioEffectProperties* iProp;
    
private:
	CMdaAudioPlayerUtility* iMdaPlayer;  // Not owned
	
	CEnvironmentalReverbUtility* iReverbEffect;
	CStereoWidening* iStereoEffect;
	CBassBoost* iBassBoostEffect;
	CLoudness*  iLoudnessEffect;
	CAudioEqualizerUtility* iEqualizerEffect;
	};

#endif // _MPXAUDIOEFFECTENGINE_H_

// End of File
