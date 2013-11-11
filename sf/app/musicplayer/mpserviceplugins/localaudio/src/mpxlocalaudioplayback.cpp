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


// INCLUDE FILES
//<branchInfo originator="alakurik" since="26-NOV-2007"/>
// Including adaptationaudiopreference.h instead of audiopreference.h to
// bring in needed constants for __HIGH_RESOLUTION_VOLUME
//</branchInfo>
//BRANCH 26-NOV-2007
//#include <audiopreference.h>
//BRANCH_END
#if defined(__HIGH_RESOLUTION_VOLUME) && !defined(__LOW_POWER_AUDIO_PLAYBACK)
#include <adaptationaudiopreference.h>
#else
#include <AudioPreference.h>
#endif
#include <badesca.h>
#include <apgcli.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfmeta.h>
#include <mpxmedia.h>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediadrmdefs.h>
#include "mpxaudioeffectengine.h"
#include <mpxprivatecrkeys.h>
#include <mpxcenrepwatcher.h>

#include "mpxlocalaudioplayback.h"
#include "mpxlog.h"

// CONSTANTS
const TUid  KLocalPlaybackUid={0x101FFC06};

    
// ============================ LOCAL FUNCTIONS ==============================
LOCAL_C TInt Balance(TInt aMMFBalance)
    {
    return (aMMFBalance-KMMFBalanceCenter) *
           (EPbBalanceMaxRight-EPbBalanceMaxLeft) /
           (KMMFBalanceMaxRight-KMMFBalanceMaxLeft);
    }

LOCAL_C TInt MMFBalance(TInt aBalance)
    {
    return KMMFBalanceCenter+(KMMFBalanceMaxRight-KMMFBalanceMaxLeft)/
           (EPbBalanceMaxRight-EPbBalanceMaxLeft)*aBalance;
    }


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXLocalAudioPlayback* CMPXLocalAudioPlayback::NewL(TAny* /*aInitParams*/)
    {
    CMPXLocalAudioPlayback* p=new(ELeave)CMPXLocalAudioPlayback();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::ConstructL()
    {
    MPX_FUNC_EX("CMPXLocalAudioPlayback::ConstructL()");
    iVolumeWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                             KMPXPlaybackVolume,
                                             this);

    iMuteWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                           KMPXPlaybackMute,
                                           this);

    iPlayer = CMdaAudioPlayerUtility::NewL(*this,
#ifdef __LOW_POWER_AUDIO_PLAYBACK
                        KAudioPriorityMusicPlayer,
                        TMdaPriorityPreference(KAudioPrefMusicLocalPlayback));
#else
#ifdef __HIGH_RESOLUTION_VOLUME
                        KAudioPriorityTwentyStepsVolumeMusicPlayer,
                        TMdaPriorityPreference(KAudioPrefTwentyStepsVolumeMusicPlayerPlayback));
#else
                        KAudioPriorityRealOnePlayer,
                        TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));
#endif // __HIGH_RESOLUTION_VOLUME
#endif // __LOW_POWER_AUDIO_PLAYBACK

    User::LeaveIfError(iFs.Connect());
    iFs.ShareProtected();
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();
    iAudioEffects = CMPXAudioEffectEngine::NewL( iPlayer );
    }

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CMPXLocalAudioPlayback::CMPXLocalAudioPlayback()
	: iAudioEffectsOn(ETrue), iIsPlaying(EFalse)
    {}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXLocalAudioPlayback::~CMPXLocalAudioPlayback()
    {
    MPX_FUNC_EX("CMPXLocalAudioPlayback::~CMPXLocalAudioPlayback");
    delete iMuteWatcher;
    delete iVolumeWatcher;
    if ( iDrmMediaUtility )
        {
        TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ) );
        iDrmMediaUtility->Close();
        delete iDrmMediaUtility;
        }
    if( iAudioEffects )
        {
        iAudioEffects->DestroyAudioEffect();
        delete iAudioEffects;
        }
    if (iPlayer)
        {
        iPlayer->Close();
        delete iPlayer;
        }
    iFile.Close();
    iFs.Close();
    delete iSong;
    }

// ----------------------------------------------------------------------------
// Set observer
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SetObserver(MMPXPlaybackPluginObserver& aObs)
    {
    MPX_FUNC_EX("CMPXLocalAudioPlayback::SetObserver(MMPXPlaybackPluginObserver& aObs)");
    iObs = &aObs;
    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::InitialiseL(const TDesC& aSong)
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::InitialiseL 0x%08x, (%S)", this, &aSong);

    iDrmMediaUtility->Close();
    delete iSong;
    iSong = NULL;
    iSong = aSong.AllocL();
    iFile.Close();
    TInt err( iFile.Open( iFs, aSong, EFileRead | EFileShareReadersOrWriters ));
    // Remap KErrNotReady to KErrNotFound, because it is referencing a drive
    // that is not existent
    if ( KErrNotReady == err )
        {
        err = KErrNotFound;
        }
    User::LeaveIfError( err );
    
#if defined(__HIGH_RESOLUTION_VOLUME)
    TParsePtrC parser(aSong);
    
    // Merlin twentysteps hack start
    if (parser.Ext().CompareF(KWmaExtension) == 0 || parser.Ext().CompareF(KRaExtension) == 0)
        {
        // This is a wma song, need to delete iPlayer and reset the volume resolution
        // this is because the volume level 20 is not supported for wma with headphone
        MPX_DEBUG1("CMPXLocalAudioPlayback::InitialiseL it is a wma file, so set to 10 steps");
        delete iAudioEffects;
        iAudioEffects = NULL;
        delete iPlayer;
        iPlayer = NULL;
        iPlayer = CMdaAudioPlayerUtility::NewL(*this,
                        KAudioPriorityRealOnePlayer,
                        TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));
        // Also regenerate audio effects
        iAudioEffects = CMPXAudioEffectEngine::NewL( iPlayer );
        }                        
    // Merlin twentysteps hack end
#endif // __HIGH_RESOLUTION_VOLUME
    TMMFileHandleSource source(iFile, KDefaultContentObject, EPlay);
    iPlayer->OpenFileL(source);
    iDrmMediaUtility->InitL( iFile );
    iState = EStateInitialising;
    iClosedByAudioPolicy = EFalse;
    iConsumeStarted = EFalse;

    MPX_DEBUG3("<--CMPXLocalAudioPlayback::InitialiseL 0x%08x, (%S)", this, &aSong);
    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::InitialiseL(RFile& aSong)
    {
    MPX_DEBUG2("-->CMPXLocalAudioPlayback::InitialiseL(RFile) 0x%08x", this);

    iDrmMediaUtility->Close();
    delete iSong;
    iSong = NULL;
    iSong = HBufC::NewL(KMaxFileName);
    TPtr ptr = iSong->Des();
    aSong.FullName(ptr);
    
#if defined(__HIGH_RESOLUTION_VOLUME)
    TParsePtrC parser(ptr);
    
    // Merlin twentysteps hack start
    if (parser.Ext().CompareF(KWmaExtension) == 0 || parser.Ext().CompareF(KRaExtension) == 0)
        {
        // This is a wma song, need to delete iPlayer and reset the volume resolution
        // this is because the volume level 20 is not supported for wma with headphone
        delete iAudioEffects;
        iAudioEffects = NULL;
        delete iPlayer;
        iPlayer = NULL;
        iPlayer = CMdaAudioPlayerUtility::NewL(*this,
                        KAudioPriorityRealOnePlayer,
                        TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));
        // Also regenerate audio effects
        iAudioEffects = CMPXAudioEffectEngine::NewL( iPlayer );
        }    
    // Merlin twentysteps hack end
#endif // __HIGH_RESOLUTION_VOLUME
    TMMFileHandleSource source(aSong, KDefaultContentObject, EPlay);
    iPlayer->OpenFileL(source);
    User::LeaveIfError( iFile.Duplicate( aSong ));
    iDrmMediaUtility->InitL( iFile );
    iState = EStateInitialising;
    iClosedByAudioPolicy = EFalse;
    iConsumeStarted = EFalse;

    MPX_DEBUG2("<--CMPXLocalAudioPlayback::InitialiseL(RFile) 0x%08x", this);
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected song
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::CommandL(TMPXPlaybackCommand aCmd, TInt aData)
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::CommandL 0x%08x cmd %d", this, aCmd);
    switch(aCmd)
        {
        case EPbCmdPlay:
            {
            // If closed by audio policy, then play command will need to
            // re-initialise the plugin first.
            if ( iClosedByAudioPolicy )
                {
                HBufC* song( iSong->AllocLC() );
                InitialiseL( *song );
                iConsumeStarted = ETrue;
                CleanupStack::PopAndDestroy( song );
                iClosedByAudioPolicy = EFalse;
                }
            else
                {
                // Treat song as play complete if try to play at end of song
                TTimeIntervalMicroSeconds dur( iPlayer->Duration() );
                TTimeIntervalMicroSeconds pos( 0 );
                TInt err( iPlayer->GetPosition( pos ));
                MPX_DEBUG4("CMPXLocalAudioPlayback::CommandL(): dur=%d, pos=%d, err=%d", I64INT(dur.Int64()), I64INT(pos.Int64()), err);
                if ( !err &&
                     Abs( dur.Int64() - pos.Int64() ) < KPbMilliMultiplier &&
                     dur.Int64() > KPbMilliMultiplier )
                    {
                    MapcPlayComplete( KErrNone );
                    }
                else
                    {
                    if (iConsumeStarted)
                        {
                        TRAPD( drmErr, ConsumeRightsL( ContentAccess::EContinue ) );
                        if ( drmErr == KErrCANoRights )
                            {
                            iDrmMediaUtility->Close();
                            iPlayer->Stop();
                            iIsPlaying = EFalse;
                            iAudioEffects->DestroyAudioEffect();
                            iPlayer->Close();
                            iObs->HandlePluginEvent(
                                    MMPXPlaybackPluginObserver::EPStopped, 0,
                                    drmErr);
                            iFile.Close();
                            iState = EStateNotInitialised;
                            iObs->HandlePluginEvent(
                                    MMPXPlaybackPluginObserver::EPClosed,
                                    EPbCmdStop, drmErr);
                            iClosedByAudioPolicy = EFalse;
                            break;
                            }
                        }
                    else
                        {
                        MPX_TRAPD( AEErr,  ConsumeRightsL( ContentAccess::EPlay ) );
                        if (AEErr == KErrDiskFull)
                        	{
                        	iDrmMediaUtility->Close();
				            iPlayer->Stop();
				            iIsPlaying = EFalse;
				            iAudioEffects->DestroyAudioEffect();
				            iPlayer->Close();
				            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPStopped,
				                    0, KErrDiskFull);
				            iFile.Close();
				            iState = EStateNotInitialised;
				            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
				                                    EPbCmdStop, KErrDiskFull);
				            iClosedByAudioPolicy = EFalse;
				            break;
                        	}
                        iConsumeStarted = ETrue;
                        }

					if ( iAudioEffectsOn )
						{
                    	MPX_TRAP( err,  iAudioEffects->CreateAudioEffectsL() );
						}

                    iPlayer->Play();
                    iIsPlaying = ETrue;

                    if( iAudioEffectsOn && err != KErrNone )
                        {
                        MPX_TRAP( err,  iAudioEffects->CreateAudioEffectsL() );
                        }

                    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlaying,
                                             0, KErrNone);
                    }
                }
            break;
            }
        case EPbCmdPause:
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EPause ) );

            TInt err( iPlayer->Pause() );
            iIsPlaying = EFalse;
            iAudioEffects->DestroyAudioEffect();

            MPX_DEBUG2("CMPXLocalAudioPlayback::CommandL(): Pause err = %d", err);
            // If pause is not supported, resend the play command so plugin
            // state is correct.
            // This is for cases like playing .RNG files
            if ( KErrNotSupported == err )
                {
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                         0,
                                         KErrNone);
                iPlayer->Play();
                iIsPlaying = ETrue;
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlaying,
                                         0,
                                         KErrNone);
                }
            else
                {
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                         0,
                                         err);
                }
            iClosedByAudioPolicy = EFalse;
            break;
            }
        case EPbCmdStop:
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ));
            iConsumeStarted = EFalse;
            iDrmMediaUtility->Close();
            iPlayer->Stop();
            iIsPlaying = EFalse;
            iAudioEffects->DestroyAudioEffect();
            iPlayer->Close();

            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPStopped,
                    0, KErrNone);
            iFile.Close();
            iState = EStateNotInitialised;
            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
                                    EPbCmdStop, KErrNone);
            iClosedByAudioPolicy = EFalse;
            break;
            }
        case EPbCmdClose:
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ));
            iConsumeStarted = EFalse;
            iDrmMediaUtility->Close();
            iAudioEffects->DestroyAudioEffect();
            iPlayer->Close();
            iIsPlaying = EFalse;
            iFile.Close();
            iState = EStateNotInitialised;
            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
                                    EPbCmdClose, KErrNone);
            iClosedByAudioPolicy = EFalse;
            break;
            }
        case EPbApplyEffect:
            {
            // Re-init audio effects
            MPX_DEBUG1("CMPXLocalAudioPlayback::CommandL EPbApplyEffect");
            if( ( aData == KAudioEffectsID || aData == KEqualizerID ) &&
                ( EStateInitialised == iState ) )
                {
                // Here we rely on CreateAudioEffectsL() to be safe to be called 
                // in order to just set values again     
                TRAPD(err, iAudioEffects->CreateAudioEffectsL() );
                if(err)
                    {
                    MPX_DEBUG2("EPbApplyEffect err = %d", err);
                    // Sometimes we get KErrAccessDenied, then destroying effects
                    // and setting them again should help. This is a work-around, 
                    // and would be good to be fixed more properly.   
                    iAudioEffects->DestroyAudioEffect(); 
                    TRAPD(err, iAudioEffects->CreateAudioEffectsL() );
                    if(err)
                        {
                        MPX_DEBUG2("EPbApplyEffect again err = %d", err);
                        }
					else
						{
                        MPX_DEBUG1("EPbApplyEffect again set");
						}
                    }
				else
					{
                    MPX_DEBUG1("EPbApplyEffect set");
					}
                iAudioEffectsOn = ETrue;
                }
            break;
            }
        case EPbCmdCloseItem:
            {
            iDrmMediaUtility->Close();
            iPlayer->Close();
            iIsPlaying = EFalse;
            iFile.Close();
            iState = EStateNotInitialised;
            iClosedByAudioPolicy = EFalse;
            break;
            }
        case EPbCmdDisableEffect:
        	{
			iAudioEffectsOn = EFalse;
			break;
			}
		default:
			break;
        }
    MPX_DEBUG3("<--CMPXLocalAudioPlayback::CommandL 0x%08x cmd %d", this, aCmd);
    }

// ----------------------------------------------------------------------------
// Sets a property of the plugin
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SetL(TMPXPlaybackProperty aProperty,TInt aValue)
    {
    MPX_DEBUG4("-->CMPXLocalAudioPlayback::SetL 0x%08x, (prop %d, val %d)",
               this, aProperty, aValue);
    TBool isSupported=ETrue;
    switch(aProperty)
        {
        case EPbPropertyVolume:
            {
            SetVolume( aValue, EFalse ); // don't notify MPX because playback engine converts EPSetComplete to EPropertyChanged
            break;
            }
        case EPbPropertyVolumeRamp:
            iPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(TInt64(aValue)));
            break;
        case EPbPropertyMute:
            SetMute( aValue, EFalse ); // don't notify MPX because playback engine converts EPSetComplete to EPropertyChanged
            break;
        case EPbPropertyBalance:
            iPlayer->SetBalance(MMFBalance(aValue));
            break;
        case EPbPropertyPosition:
            {
            TInt64 pos(aValue);
            pos *= KPbMilliMultiplier;
            
            if (iIsPlaying)
                {
                iPlayer->Pause();
                iPlayer->SetPosition(pos);
                //Handle error of license expired when tapping the progress playing bar 
                TRAPD( drmErr, ConsumeRightsL( ContentAccess::EContinue ) );
                if ( drmErr == KErrCANoRights )
                    {
                    iDrmMediaUtility->Close();
                    iPlayer->Stop();
                    iIsPlaying = EFalse;
                    iAudioEffects->DestroyAudioEffect();
                    iPlayer->Close();
                    iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPStopped, 0, drmErr);
                    iFile.Close();
                    iState = EStateNotInitialised;
                    iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPClosed, EPbCmdStop,
                            drmErr);
                    iClosedByAudioPolicy = EFalse;
                    return;
                    }
                iPlayer->Play();
                }
            else
                {
                iPlayer->SetPosition(pos);
                }
            }
            break;
        default:
            isSupported=EFalse;
        }

    if (!isSupported)
        {
        User::Leave(KErrNotSupported);
        }
    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPSetComplete,
                             aProperty, KErrNone);
    MPX_DEBUG4("<--CMPXLocalAudioPlayback::SetL 0x%08x, (prop %d, val %d)",
               this, aProperty, aValue);
    }

// ----------------------------------------------------------------------------
// Gets a property of the plugin (async)
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::PropertyL(TMPXPlaybackProperty aProperty) const
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::PropertyL 0x%08x (prop %d)",
               this, aProperty);
    TBool isSupported=ETrue;
    TInt value=KErrNotFound;
    TInt err(KErrNone);
    switch(aProperty)
        {
        case EPbPropertyVolume:
            {
            value = iVolumeWatcher->CurrentValueL();
            break;
            }
        case EPbPropertyMaxVolume:
            value=iPlayer->MaxVolume();
            break;
        case EPbPropertyMute:
            value = iMuteWatcher->CurrentValueL();
            break;
        case EPbPropertyBalance:
            err = iPlayer->GetBalance(value);
            value=Balance(value);
            break;
        case EPbPropertyDuration:
            {
            TTimeIntervalMicroSeconds duration = iPlayer->Duration();
            value = duration.Int64() / KPbMilliMultiplier;
            }
            break;
        case EPbPropertyPosition:
            {
            TTimeIntervalMicroSeconds pos;
            iPlayer->GetPosition(pos);
            MPX_DEBUG2("CMPXLocalAudioPlayback::PropertyL position %ld", pos.Int64());
            value = pos.Int64() / KPbMilliMultiplier;
            }
            break;
        case EPbPropertySupportedFeatures:
            value = EPbFeatureBalance | EPbFeatureVolumeRamp;
            break;
        default:
            isSupported=EFalse;
        }
    if (!isSupported)
        {
        User::Leave(KErrNotSupported);
        }
    iObs->HandleProperty(aProperty,value,err);
    MPX_DEBUG3("<--CMPXLocalAudioPlayback::PropertyL 0x%08x (prop %d)",
               this, aProperty);
    }

// ----------------------------------------------------------------------------
// Gets a list of sub players, UPnP only
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SubPlayerNamesL()
    {
    iObs->HandleSubPlayerNames(KLocalPlaybackUid, NULL, ETrue, KErrNone);
    }

// ----------------------------------------------------------------------------
// Select a sub player
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SelectSubPlayerL(TInt /*aIndex*/)
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Returns current sub player name
// ----------------------------------------------------------------------------
//
const TDesC& CMPXLocalAudioPlayback::SubPlayerName()
    {
    return KNullDesC; //No subplayer name for local playback
    }

// ----------------------------------------------------------------------------
// Current sub player index
// ----------------------------------------------------------------------------
//
TInt CMPXLocalAudioPlayback::SubPlayerIndex() const
   {
   return KErrNotFound;
   }

// ----------------------------------------------------------------------------
// Gets media properties
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::MediaL(const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_DEBUG2("-->CMPXLocalAudioPlayback::MediaL 0x%08x", this);
    RArray<TInt> suppIds;
    CleanupClosePushL(suppIds);
    suppIds.AppendL(KMPXMediaIdMusic);
    suppIds.AppendL(KMPXMediaIdGeneral);
    suppIds.AppendL(KMPXMediaIdAudio);
    suppIds.AppendL(KMPXMediaIdDrm);
    CMPXMedia* media=CMPXMedia::NewL(suppIds.Array());
    CleanupStack::PopAndDestroy(&suppIds);
    CleanupStack::PushL(media);

    if ( EStateInitialised == iState )
        {
        TUint attrG(0); // General attributes
        TUint attrA(0); // Audio attributes
        TUint attrM(0); // Music attributes
        TUint attrD(0); // DRM attributes

        for (TInt i=aAttrs.Count(); --i>=0;)
            {
            TMPXAttribute attr(aAttrs[i]);
            if (attr.ContentId() == KMPXMediaIdGeneral)
                {
                attrG |= attr.AttributeId();
                }
            else if (attr.ContentId() == KMPXMediaIdMusic)
                {
                attrM |= attr.AttributeId();
                }
            else if (attr.ContentId() == KMPXMediaIdAudio)
                {
                attrA |= attr.AttributeId();
                }
            else if ( attr.ContentId() == KMPXMediaIdDrm )
                {
                attrD |= attr.AttributeId();
                }
            }

        TInt metaCount = 0;

        // Get metadata from MMF
        TInt error = iPlayer->GetNumberOfMetaDataEntries(metaCount);
        CMMFMetaDataEntry* metaData = NULL;

        if (!error)
            {
            for (TInt i = 0; i < metaCount; ++i)
                {
                metaData = iPlayer->GetMetaDataEntryL(i);
                CleanupStack::PushL(metaData);

                if (metaData->Name().CompareF(KMMFMetaEntrySongTitle()) == 0 &&
                    attrG & EMPXMediaGeneralTitle)
                    { // TODO to check request
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryArtist()) == 0 &&
                    attrM & EMPXMediaMusicArtist)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicArtist),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryAlbum()) == 0 &&
                    attrM & EMPXMediaMusicAlbum)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbum),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryYear()) == 0 &&
                    attrM & EMPXMediaMusicYear)
                    {
                    TInt year;
                    TLex lex( metaData->Value() );
                    lex.Val( year );

                    TDateTime dt;
                    dt.SetYear( year );
                    TTime time( dt );

                    media->SetTObjectValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicYear),
                        time.Int64());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryComment()) == 0 &&
                    attrG & EMPXMediaGeneralComment)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralComment),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryComposer()) == 0 &&
                    attrM & EMPXMediaMusicComposer)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicComposer),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryAlbumTrack()) == 0 &&
                    attrM & EMPXMediaMusicAlbumTrack)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumTrack),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryGenre()) == 0 &&
                    attrM & EMPXMediaMusicGenre)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicGenre),
                        metaData->Value());
                    }
                //else if (metaData->Name().CompareF(KMMFMetaEntryWOAF()) == 0 &&
                //    attrM & EMPXMediaMusicGenre)
                //    {
                    //media->SetTextValueL(
                    //    TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicGenre),
                    //    metaData->Value());
                    //}
                else if (metaData->Name().CompareF(KMMFMetaEntryAPIC()) == 0 &&
                    attrM & EMPXMediaMusicAlbumArtFileName)
                    {
                    // TODO check collection if user defined album art available
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumArtFileName),
                        *iSong);
                    }

                CleanupStack::PopAndDestroy(metaData);
                metaData = NULL;
                }
            if ( attrG & EMPXMediaGeneralTitle )
                {
                if ( !media->IsSupported(
                    TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle ) ) )
                    {
                    TParsePtrC ptr( *iSong );
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                        ptr.Name() );
                    }
                }
            }
        if (attrG & EMPXMediaGeneralUri)
            {
            media->SetTextValueL(
                TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralUri),
                *iSong);
            }

        if (attrG & EMPXMediaGeneralDuration)
            {
            TTimeIntervalMicroSeconds duration = iPlayer->Duration();
            media->SetTObjectValueL<TInt>(
                   TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralDuration),
                   duration.Int64() / KPbMilliMultiplier);
            }


        // Set bitrate TODO
        TPckgBuf<TMMFAudioConfig> data;

        const TMMFMessageDestinationPckg
            destinationPckg(KUidInterfaceMMFAudioController);

        if (attrA & EMPXMediaAudioBitrate)
            {
            error = iPlayer->CustomCommandSync(destinationPckg,
                EMMFAudioControllerGetSourceBitRate, KNullDesC8, KNullDesC8, data);
            if (!error)
                {
                media->SetTObjectValueL<TInt>(
                       TMPXAttribute(KMPXMediaIdAudio, EMPXMediaAudioBitrate),
                       data().iSampleRate);
                }
            }

        if (attrA & EMPXMediaAudioSamplerate)
            {

            // Set sampling rate
            error = iPlayer->CustomCommandSync(destinationPckg,
                EMMFAudioControllerGetSourceSampleRate, KNullDesC8, KNullDesC8, data);
            if (!error)
                {
                media->SetTObjectValueL<TInt>(
                       KMPXMediaAudioSamplerate,
                       data().iSampleRate);
                }
            }
       if (attrG & EMPXMediaGeneralSize)
            {
            TEntry entry;
            iFs.Entry(iSong->Des(), entry);
            media->SetTObjectValueL<TInt>(
                   TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralSize),
                   entry.iSize);
            }

       if (attrG & EMPXMediaGeneralMimeType)
            {
            RApaLsSession aps;
            error  = aps.Connect(); // always fail in console test
            if (KErrNone == error)
                {
                CleanupClosePushL(aps);
                TDataType dataType;
                TUid ignore;
                if(aps.AppForDocument(iSong->Des(),ignore,dataType)==KErrNone)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralMimeType),
                        dataType.Des());
                    }
                CleanupStack::PopAndDestroy(&aps);
                } // Notes, STIF console test always fail
            }

        // Set DRM info
        const CMPXMedia* drmMedia( iDrmMediaUtility->GetMediaL( attrD ));
        if ( drmMedia )
            {
            TInt count( drmMedia->Count() );
            for ( TInt i = 0; i < count; i++ )
                {
                TUint attrId( drmMedia->Attribute(i).AttributeId() );
                if ( attrD & attrId )
                    {
                    TMPXAttribute mpxAtt( KMPXMediaIdDrm, attrId );
                    switch ( attrId )
                        {
                        case EMPXMediaDrmType:
                        case EMPXMediaDrmRightsStatus:
                        case EMPXMediaDrmRightsType:
                        case EMPXMediaDrmCount:
                            {
                            TInt val(
                                drmMedia->ValueTObjectL<TInt>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmProtected:
                        case EMPXMediaDrmSendingAllowed:
                        case EMPXMediaDrmCanSetAutomated:
                        case EMPXMediaDrmHasInfoUrl:
                        case EMPXMediaDrmHasPreviewUrl:
                        case EMPXMediaDrmAboutToExpire:
                            {
                            TBool val(
                                drmMedia->ValueTObjectL<TBool>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmStartTime:
                        case EMPXMediaDrmEndTime:
                        case EMPXMediaDrmIntervalStartTime:
                        case EMPXMediaDrmAccumulatedTime:
                            {
                            TInt64 val(
                                drmMedia->ValueTObjectL<TInt64>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmInterval:
                            {
                            TTimeIntervalSeconds val(
                                drmMedia->ValueTObjectL<TTimeIntervalSeconds>(mpxAtt));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        default:
                            {
                            break;
                            }
                        }   // end switch (attriId)
                    }   // end if ( attrD & attrId )
                }
            }
        }

    iObs->HandleMedia(*media, KErrNone);
    CleanupStack::PopAndDestroy(media);
    MPX_DEBUG2("<--CMPXLocalAudioPlayback::MediaL 0x%08x", this);
    }

// ----------------------------------------------------------------------------
// Cancel request
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::CancelRequest()
    {
    }

// ----------------------------------------------------------------------------
// File open complete event
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::MapcInitComplete(TInt aError,
                                const TTimeIntervalMicroSeconds& aDuration)
    {
    MPX_DEBUG4("-->CMPXLocalAudioPlayback::MapcInitComplete 0x%08x err (%d) duration (%Ld)",
               this, aError, aDuration.Int64());
    iState = EStateInitialised;

    // Restore volume level
    if ( KErrNone == aError )
        {
        TInt currentVol( 0 );
        MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
        if ( volError == KErrNone )
            {
            SetVolume( currentVol );
            TBool mute( EFalse);
            MPX_TRAPD( muteError, mute = iMuteWatcher->CurrentValueL() );
            if ( muteError == KErrNone && mute )
                {
                SetMute(mute);
                }
            }
        }

    // Disable automatic DRM consumption
    if ( iPlayer )
        {
        MMMFDRMCustomCommand* drmCustom = iPlayer->GetDRMCustomCommand();
        if ( drmCustom )
            {
            drmCustom->DisableAutomaticIntent( ETrue );
            }
        }

    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPInitialised,
                            aDuration.Int64()/KPbMilliMultiplier,aError);

    MPX_DEBUG4("<--CMPXLocalAudioPlayback::MapcInitComplete 0x%08x err (%d) duration (%Ld)",
               this, aError, aDuration.Int64());
    }

// ----------------------------------------------------------------------------
// File play complete event
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::MapcPlayComplete(TInt aError)
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::MapcPlayComplete 0x%08x (err %d)",
               this, aError);
    iState = EStateNotInitialised;
    iIsPlaying = EFalse;
    if ( KErrNone != aError )
        {
        TRAP_IGNORE( ConsumeRightsL( ContentAccess::EPause ) );
        }
    else
        {
        TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ) );
        if ( iConsumeStarted )
            {
            iConsumeStarted = EFalse;
            }
        }

    TRAP_IGNORE( iAudioEffects->DestroyAudioEffect() );

    // If killed by audio policy, mimic a paused state
    if ( KErrDied == aError || KErrAccessDenied == aError || KErrInUse == aError )
        {
        iClosedByAudioPolicy = ETrue;
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                 0,
                                 aError );
        }
     // If disk removed
     else if( KErrNotReady == aError )
        {
        MPX_DEBUG1("CMPXLocalAudioPlayback::MapcPlayComplete - KErrNotReady");

        iClosedByAudioPolicy = EFalse;
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                 0,
                                 KErrNone );
        }
    else
        {
        iClosedByAudioPolicy = EFalse;
        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlayComplete,0,aError);
        }
    MPX_DEBUG3("<--CMPXLocalAudioPlayback::MapcPlayComplete 0x%08x (err %d)",
               this, aError);
    }

// ----------------------------------------------------------------------------
// Handle a change in a setting value.
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::HandleSettingChange(
    const TUid& aRepositoryUid,
    TUint32 aSettingId )
    {
    MPX_DEBUG2("-->CMPXLocalAudioPlayback::HandleSettingChange 0x%08x", this);

    if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackVolume == aSettingId )
        {
        MPX_DEBUG1("CMPXLocalAudioPlayback::HandleSettingChange() Volume setting changed");
        TInt vol( 0 );
        MPX_TRAPD( error, vol = iVolumeWatcher->CurrentValueL() );
        if ( EStateInitialised == iState && error == KErrNone )
            {
            SetVolume( vol );
            }
        else if ( error == KErrNone )
            {
            // Do not need to set volume if not initialised,
            // just notify observers
            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                     vol,
                                     KErrNone);
            }
        }
    else if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackMute == aSettingId )
        {
        MPX_DEBUG1("CMPXLocalAudioPlayback::HandleSettingChange() Mute setting changed");
        TBool mute( EFalse );
        MPX_TRAPD( error, mute = static_cast<TBool>(iMuteWatcher->CurrentValueL()) );
        if ( EStateInitialised == iState && error == KErrNone )
            {
            TInt oldVolume( 0 );
            iPlayer->GetVolume( oldVolume );
            if ( (mute && oldVolume != 0) || (!mute && oldVolume == 0) )
                {
                SetMute( mute );
                }
            }
        else if ( error == KErrNone )
            {
            // Do not need to set volume if not initialised,
            // just notify observers
            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPMuteChanged,
                                     mute,
                                     KErrNone);
            }
        }
    MPX_DEBUG2("<--CMPXLocalAudioPlayback::HandleSettingChange 0x%08x", this);
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SetVolume( TInt aVolume, TBool aNotifyChange )
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::SetVolume 0x%08x vol (%d)", this, aVolume);

    // Ensure that level is within min and max values
    if ( aVolume > KPbPlaybackVolumeLevelMax )
        {
        aVolume = KPbPlaybackVolumeLevelMax;
        }
    if ( aVolume < KPbPlaybackVolumeLevelMin )
        {
        aVolume = KPbPlaybackVolumeLevelMin;
        }

    TBool changed( EFalse );
    // Change MMF Audio player's volume
    if ( EStateInitialised == iState )
        {
        TInt newVolume( aVolume * iPlayer->MaxVolume() / 100 );
        MPX_DEBUG2("CMPXLocalAudioPlayback::SetVolume(): Setting volume = %d", newVolume);

        // First check if MMF Audio player's volume is changed by new value
        TInt oldVolume( 0 );
        iPlayer->GetVolume( oldVolume );
        if ( newVolume != oldVolume )
            {
            iPlayer->SetVolume( newVolume );
            changed = ETrue;
            }
        }

    // Change setting in cenrep
    TInt currentVol( 0 );
    MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
    if ( volError == KErrNone && aVolume != currentVol )
        {
        MPX_TRAP( volError, iVolumeWatcher->SetValueL( aVolume ) );
        if( aVolume == 0 )
            {
            MPX_TRAP( volError, iMuteWatcher->SetValueL( ETrue ) );
            }
        else if( aVolume > 0 )
            {
            TBool currentMute( EFalse );
            
            MPX_TRAP( volError, currentMute = iMuteWatcher->CurrentValueL() );
            if( volError == KErrNone && currentMute )
                {
                MPX_TRAP( volError, iMuteWatcher->SetValueL( EFalse ) );
                }
            }
        }
    else if ( volError == KErrNone && aVolume == currentVol && changed )
        {
        // volume changed only to player after it has been initialised, 
        // no need to reflect this to upper layers as the stored setting haven't changed
        changed = EFalse;
        }

    // Notify observer if value changed
    if ( changed && aNotifyChange )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                 aVolume,
                                 KErrNone);
        }

    MPX_DEBUG3("<--CMPXLocalAudioPlayback::SetVolume 0x%08x vol (%d)", this, aVolume);
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::SetMute( TBool aMute, TBool aNotifyChange )
    {
    MPX_DEBUG3("-->CMPXLocalAudioPlayback::SetMute 0x%08x vol (%d)", this, aMute);

    TBool changed( EFalse );
    // Change MMF Audio player's volume
    if ( EStateInitialised == iState )
        {
        TInt currentVolume(0);
        iPlayer->GetVolume(currentVolume);
		if ( aMute && currentVolume != 0 )
			{
			iVolume = currentVolume;
			iPlayer->SetVolume(0);
			changed = ETrue;
			}
		else if ( !aMute && currentVolume == 0 )      // UnMute
			{
			iPlayer->SetVolume(iVolume);
			changed = ETrue;
			}
        }

    // Change setting in cenrep
    TBool currentMute( EFalse );
    MPX_TRAPD( muteError, currentMute = iMuteWatcher->CurrentValueL() );
    if ( muteError == KErrNone )
        {
        if ( aMute && !currentMute )
            {
            MPX_TRAP( muteError, iMuteWatcher->SetValueL( aMute ) );
            }
        else if ( !aMute && currentMute )
            {
            MPX_TRAP( muteError, iMuteWatcher->SetValueL( aMute ) );
            }
        else if ( changed ) // Cenrep setting hasn't changed, no need to propagate to MPX
            {
            changed = EFalse;
            }
        }

    // Notify observer if value changed
    if ( changed  && aNotifyChange )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPMuteChanged,
                                 aMute,
                                 KErrNone);
        }

    MPX_DEBUG3("<--CMPXLocalAudioPlayback::SetMute 0x%08x vol (%d)", this, aMute);
    }

// ----------------------------------------------------------------------------
// Consumes the rights for the current media
// ----------------------------------------------------------------------------
//
void CMPXLocalAudioPlayback::ConsumeRightsL(ContentAccess::TIntent aIntent)
    {
    MPX_FUNC("CMPXLocalAudioPlayback::ConsumeRightsL()");
    // Fix for error: PNUI-7Q8GL6
    // Normally,this case does not happen.
    // In EStateInitialising state,consumerights is forbidden.
    if ( iState == EStateInitialising )
        {
        return;
        }
    if ( iPlayer )
        {
        MMMFDRMCustomCommand* drmCustom = iPlayer->GetDRMCustomCommand();
        if ( drmCustom )
            {
            switch ( aIntent )
                {
                case ContentAccess::EPlay:
                case ContentAccess::EStop:
                case ContentAccess::EPause:
                case ContentAccess::EContinue:
                    {
                    break;
                    }
                default:
                    {
                    aIntent = ContentAccess::EUnknown;
                    iConsumeStarted = EFalse;
                    break;
                    }
                }
            TInt returnCode( drmCustom->ExecuteIntent(aIntent) );
            MPX_DEBUG2("CMPXLocalAudioPlayback::ConsumeRightsL() ExecuteIntent return (%d)", returnCode);
            User::LeaveIfError(returnCode);
            }
        }
    }
// End of file
