/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine class that uses the Effects API and saves changes.
*
*/


// INCLUDE FILES
#include <centralrepository.h>
#include <CustomCommandUtility.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>

#include <EnvironmentalReverbUtility.h>
#include <EnvironmentalReverbUtilityData.h>
#include <StereoWideningBase.h>
#include <StereoWideningData.h>
#include <BassBoostBase.h>
#include <BassBoostData.h>
#include <LoudnessBase.h>
#include <LoudnessData.h>
#include <AudioEqualizerUtility.h>
#include <EqualizerConstants.h>
#include <mpxlog.h>

#include "mpxaudioeffectengine.h"

// Music setting
const TUid KMPCenRepSettingsFeature = {0x10207C92};
const TUint32 KMPCenRepSettingPresetIdKey = 0x00000004;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::NewL()
// Standard 2-Phased Constructor
// -----------------------------------------------------------------------------
// 
EXPORT_C CMPXAudioEffectEngine* CMPXAudioEffectEngine::NewL(
    CMdaAudioPlayerUtility* aPlayer )
    {
    CMPXAudioEffectEngine* self = new( ELeave ) CMPXAudioEffectEngine( aPlayer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::CMPXAudioEffectEngine()
// Constructor
// -----------------------------------------------------------------------------
// 
CMPXAudioEffectEngine::CMPXAudioEffectEngine(CMdaAudioPlayerUtility* aPlayer)
                                    :iMdaPlayer(aPlayer)   
    {
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::~CMPXAudioEffectEngine()
// Virtual destructor
// -----------------------------------------------------------------------------
// 
EXPORT_C CMPXAudioEffectEngine::~CMPXAudioEffectEngine()
    {    
    if (iProp)
        {
        delete iProp;
        }
    DestroyAudioEffect();
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::ConstructL()
// Standard Second Phased Constructor
// -----------------------------------------------------------------------------
// 
void CMPXAudioEffectEngine::ConstructL()
    {
    iProp = new(ELeave) CMPXAudioEffectProperties();   
    MPX_TRAPD( err, iProp->LoadFromFileL() ); //Loads saved settings if available
    if( err != KErrNone )
        {
        MPX_DEBUG1("CMPXAudioEffectEngine::ConstructL -- reset save to file");
        iProp->Reset();
        iProp->SaveToFileL();        
        }
    }


// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::SetBalanceL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetBalanceL()
    {
    iProp->LoadFromFileL();
    if(iMdaPlayer)
        {
        TInt currentBalance;    
        TInt err = iMdaPlayer->GetBalance(currentBalance);
        if( err !=KErrNone )
            {   
            User::Leave(KErrNotSupported);    
            }        
        // don't set non-changed values:   
        if( iProp->Balance() != currentBalance )
            {
            err = iMdaPlayer->SetBalance(iProp->Balance());
            if( err !=KErrNone )
                {
                User::Leave(KErrNotSupported);    
                }            
            }
        } 
    }
  
  
// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::SetReverberationL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetReverberationL()
    {
#ifdef _REVERB
    if(!iReverbEffect)
        {
         //Effect enabled immediately, cmdUtil ownership passed into new object
        MPX_TRAPD(error, iReverbEffect = 
                            CEnvironmentalReverbUtility::NewL(*iMdaPlayer));
        if(!error)
            {
            SetReverberationL();
            }
        }

    iProp->LoadFromFileL();

    TBuf16<32> reverbation;
    TBuf16<32> reverbation2;
  
    switch( iProp->Reverb() ) //aIndex ranges always from 0 to 7
        {
        case 1:   _LIT( KRevName, "Alley" );
              reverbation.Append( KRevName );
              break;
        case 2:   _LIT( KRevName2, "Bathroom" );
              reverbation.Append( KRevName2 );
              break;
        case 3:   _LIT( KRevName3, "Underwater" );
              reverbation.Append( KRevName3 );
              break;
        case 4:   _LIT( KRevName4, "Small room" );
              reverbation.Append( KRevName4 );
              break;
        case 5:   _LIT( KRevName5, "Medium room" );
              reverbation.Append( KRevName5 );
              break;
        case 6:   _LIT( KRevName6, "Large room" );
              reverbation.Append( KRevName6 );
              break;
        case 7:   _LIT( KRevName7, "Large hall" );
              reverbation.Append( KRevName7 );
              break;
        default:  break;
        }
    TUint32 count;
    count = iReverbEffect->NumberOfPreDefinedPresets();
    TArray<TEfEnvironmentalReverbUtilityPreset> array = iReverbEffect->Presets();
    
    if( 0 != iProp->Reverb() )
        {
        TInt i = 0;
        
        do    //compare descriptors and apply preset if descriptors match
          {
          reverbation2.Copy( array[i].iPresetName );
          if( reverbation2.Compare(reverbation) == 0 )
            {
            iReverbEffect->ApplyPresetL( i );
            i = count;
            }
          i++;
          }while( i < count );
        }
    else
        {
        iReverbEffect->DisableEnvironmentalReverbL();
        }
#endif
    }
  

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::SetStereoWideningL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetStereoWideningL()
    {
#ifdef _STEREO
    iProp->LoadFromFileL();
    
    if(iProp->Stereo())
        {
        if(!iStereoEffect)  // If stereo widening is ON and not constructed
            {
            TUint stereoLevel = 30;
            // cmdUtil ownership passed into new object           
            MPX_TRAPD(error, 
                  iStereoEffect = CStereoWidening::NewL(*iMdaPlayer, 
                                                        EFalse, stereoLevel));
            if(error)
                {
                iStereoEffect = NULL;
                User::Leave(KErrNotSupported);
                }
            }

        if ( !iStereoEffect->IsEnabled() )
            {
            iStereoEffect->EnableL();
            TUint8 level = 30;
            iStereoEffect->SetStereoWideningLevelL( level );
            iStereoEffect->ApplyL();                
            }

        }
    else
        {
        if (iStereoEffect && iStereoEffect->IsEnabled() )   // If audio effects was not on, then no need to disable
            { 
            iStereoEffect->DisableL();
            }
        }
#endif
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::SetBassBoostL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetBassBoostL()
  {
#ifdef _BASSBOOST
    iProp->LoadFromFileL();
    if(iProp->BassBoost())
        {
        if (!iBassBoostEffect)
            {
            // cmdUtil ownership passed into new object           
            MPX_TRAPD(error, 
                  iBassBoostEffect = CBassBoost::NewL(*iMdaPlayer, 
                                                      iProp->BassBoost()));
            if (error)
                {
                User::Leave(KErrNotSupported);
                }
            }
        iBassBoostEffect->EnableL();
        }
    else
        {
        if (iBassBoostEffect)
            {
            iBassBoostEffect->DisableL();    
            }
        }
#endif
  }

// -----------------------------------------------------------------------------
// CMPXAudioEffectEngine::SetLoudnessL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetLoudnessL()
    {
#ifdef _LOUDNESS
    iProp->LoadFromFileL();
    if(iProp->Loudness())
        {
        if(!iLoudnessEffect)
            {
            // cmdUtil ownership passed into new object           
            MPX_TRAPD(error, 
                  iLoudnessEffect = CLoudness::NewL(*iMdaPlayer, 
                                                    iProp->Loudness()));
            if(error)
                {
                iLoudnessEffect = NULL;
                User::Leave(KErrNotSupported);
                }
            else
                {
                }
            } 
            
        if( !iLoudnessEffect->IsEnabled() )
            {   
            iLoudnessEffect->EnableL();    
            }
        }
    else
        { 
        if( iLoudnessEffect && iLoudnessEffect->IsEnabled() )
            {       
            iLoudnessEffect->DisableL();
            }
        }
#endif
    }

// -----------------------------------------------------------------------------
// Sets the equalizer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::SetEqualizerL()
    {
    MPX_DEBUG1("CMPXAudioEffectEngine::SetEqualizerL <--");
    
    // Create the effect
    //
    if( !iEqualizerEffect )
        {
        iEqualizerEffect = CAudioEqualizerUtility::NewL(*iMdaPlayer);
        }
        
    // Get equalizer preset id from cenrep
    //
    TInt presetId( KEqualizerPresetNone );
    TRAP_IGNORE(
        {
        CRepository* repository = CRepository::NewL( KMPCenRepSettingsFeature  );
        repository->Get( KMPCenRepSettingPresetIdKey, presetId );
        delete repository;
        repository = NULL;
        } );

    // Translate preset ID to index
    //
    MPX_DEBUG1("CMPXAudioEffectEngine::SetEqualizerL = finding preset index");
    
    TInt index( KErrNotFound );
    if ( iEqualizerEffect && presetId != KEqualizerPresetNone )
        {
        TArray<TEfAudioEqualizerUtilityPreset> presetArray = 
            iEqualizerEffect->Presets();

        TBool found( EFalse );
        for ( TInt i = 0; i < presetArray.Count() && found == EFalse; i++ )
            {
            if ( presetArray[i].iPresetNameKey == 
                static_cast<TUint32>( presetId ) )
                {
                found = ETrue;
                index = i;
                }
            }
        }
    MPX_DEBUG2("CMPXAudioEffectEngine::SetEqualizerL effect index: %i", index);
    
    // Apply preset index index or disable if not found
    //
    if ( KErrNotFound != index )
        {
        iEqualizerEffect->ApplyPresetL( index );
        }
    else
        {
        iEqualizerEffect->DisableEqualizerL();    
        }
        
    MPX_DEBUG1("CMPXAudioEffectEngine::SetEqualizerL -->");
    }
    
// ---------------------------------------------------------------------------
// CreateAudioEffectsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXAudioEffectEngine::CreateAudioEffectsL()
    {
    SetEqualizerL();
    SetStereoWideningL();
    SetBalanceL();
    SetLoudnessL();
    }
    
// -----------------------------------------------------------------------------
// Destroy audio effect
// -----------------------------------------------------------------------------
// 
EXPORT_C void CMPXAudioEffectEngine::DestroyAudioEffect()
    {
    if (iReverbEffect)
        {
        delete iReverbEffect;     //effect automatically disabled
        iReverbEffect = NULL;
        }
        
    if (iStereoEffect)
        {
        delete iStereoEffect;     //effect automatically disabled
        iStereoEffect = NULL;
        }
    if (iBassBoostEffect)
        {
        delete iBassBoostEffect;  //effect automatically disabled
        iBassBoostEffect = NULL;
        }
    if (iLoudnessEffect)
        {
        delete iLoudnessEffect;   //effect automatically disabled
        iLoudnessEffect = NULL;
        }
    if (iEqualizerEffect)
        {
        delete iEqualizerEffect;  //effect automatically disabled
        iEqualizerEffect = NULL;
        }
    }

//End of File
