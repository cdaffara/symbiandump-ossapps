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
* Description: Wrapper for equalizer framework - private implementation.
*
*/

// System includes
#include <QDebug>
#include <QStringList>
#include <hbglobal.h>
#include <AudioEqualizerUtility.h>
#include <EqualizerConstants.h>
#include <mpxuser.h>

// User includes
#include "mpequalizerframeworkwrapper.h"
#include "mpequalizerframeworkwrapper_p.h"
#include "mptrace.h"

// CONSTANTS
_LIT( KMPXNullSoundPath, "\\system\\data\\nullsound.mp3" );

/*!
    \class MpEqualizerFrameworkWrapperPrivate
    \ingroup musicplayer
    \brief Wrapper for equalizer framework - private implementation.
    
    This is a private implementation of the audio eqalizer wrapper 
    utilties interface.

*/

/*!
    Constructs a new MpEqualizerFrameworkWrapper with \a parent and initializes its
    private imaplementation.
*/
MpEqualizerFrameworkWrapperPrivate::MpEqualizerFrameworkWrapperPrivate( MpEqualizerFrameworkWrapper *wrapper ): 
    q_ptr( wrapper ),
    mEqualizerUtility (NULL)
{
    TX_LOG
}

/*!
    Destructs the class.    
 */
MpEqualizerFrameworkWrapperPrivate::~MpEqualizerFrameworkWrapperPrivate()
{
    TX_ENTRY

    delete mEqualizerUtility;
    
    if ( mMdaPlayUtility ) {
        mMdaPlayUtility->Close();
    }
    delete mMdaPlayUtility;

    TX_EXIT
}

/*!
    Calls the DoInit to initialize Symbian based components.
    
    \sa DoInitL()
 */
void MpEqualizerFrameworkWrapperPrivate::init()
{
    TX_ENTRY

    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("MpEqualizerFrameworkWrapperPrivate::init err = " << err);
    }
    
    TX_EXIT
}

/*!
    Get the preset name key by giving preset index.

 */
int MpEqualizerFrameworkWrapperPrivate::getPresetNameKey( int presetIndex )
{
    TX_ENTRY_ARGS( "preset index=" << presetIndex );

    int presetKey( KEqualizerPresetNone );
    if ( mEqualizerUtility ) {
        TRAPD( err, presetKey = DoGetPresetNameKeyL( presetIndex ) );
        if ( err != KErrNone ) {
            TX_LOG_ARGS("DoGetPresetNameKeyL Error  = " << err);
            presetKey = KEqualizerPresetNone;
        }
        TX_LOG_ARGS( "Preset key in engine =" << presetKey );
    } 
    else {
        TX_LOG_ARGS( "Equalizer not ready." );
    }

    TX_EXIT_ARGS( "Preset Name Key = " << presetKey );
    
    return presetKey;
}

/*!
    Get the preset index by giving preset name key.
    
 */
int MpEqualizerFrameworkWrapperPrivate::getPresetIndex( int presetNameKey )
{
    TX_ENTRY_ARGS( "Preset Name Key = " << presetNameKey);

    int presetIndex = 0;
    if ( mEqualizerUtility && presetNameKey != KEqualizerPresetNone ) {
        TArray<TEfAudioEqualizerUtilityPreset> presetArray = mEqualizerUtility->Presets();
        for ( TInt i = 0; i < presetArray.Count(); i++ ) {
            TX_LOG_ARGS("presetArray[" << i << "].iPresetNameKey (" 
                << presetArray[i].iPresetNameKey << ") vs presetId (" <<presetNameKey << ")");
            if ( presetArray[i].iPresetNameKey == static_cast<TUint32>( presetNameKey ) ) {
				//Set the current preset index. 1 is added to index because index 0 represent "Off" at UI level.
                presetIndex = i+1;
                break;
            }
        }
    }

    TX_EXIT_ARGS( "Preset Index = " << presetIndex );

    return presetIndex;
}


/*!
    Returning the list of availale preset names.  

    \sa CAudioEqualizerUtility
 */
QStringList MpEqualizerFrameworkWrapperPrivate::presetNames()
{
    TX_ENTRY

    QStringList presetList;

    if ( mEqualizerUtility == NULL ) {
        TX_LOG_ARGS( "Equalizer not ready." );
        return presetList;
    }

    TArray<TEfAudioEqualizerUtilityPreset> presetArray = mEqualizerUtility->Presets();
    for (TInt i=0; i<presetArray.Count(); i++) {
        TPresetName selectedItem = presetArray[i].iPresetName;
        presetList << QString::fromUtf16( selectedItem.Ptr(), selectedItem.Length() );
    }

    TX_EXIT_ARGS( "PresetNames=" << presetList);

    return presetList;
}

/*!
 \internal
 */
void MpEqualizerFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY

    if (mMdaPlayUtility) {
        mMdaPlayUtility->Close();
        delete mMdaPlayUtility;
        mMdaPlayUtility = NULL;
    }
    
    mMdaPlayUtility = ( CMdaAudioPlayerUtility* )User::LeaveIfNull(
        CMdaAudioPlayerUtility::NewL(
            *this,
            EMdaPriorityMin,
            EMdaPriorityPreferenceNone ));

    TX_LOG_ARGS("Loading null sound");
    TFileName nullSoundPath( KMPXNullSoundPath );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( nullSoundPath ) );
    TX_LOG_ARGS("Loading null sound done!");
    mMdaPlayUtility->OpenFileL( nullSoundPath );
    
    TX_EXIT
}

/*!
 \internal
 */
TInt MpEqualizerFrameworkWrapperPrivate::DoGetPresetNameKeyL( const TInt presetIndex ) const 
{
    TX_ENTRY_ARGS( "preset index=" << presetIndex );

    TInt presetNameKey = KErrNotFound;
    TPresetName presetName = mEqualizerUtility->GetPresetL( presetIndex );

    TArray<TEfAudioEqualizerUtilityPreset> presets = mEqualizerUtility->Presets();
    for ( TInt index = 0; index < presets.Count(); index++ ) {
        TPresetName selectedItem = presets[index].iPresetName;
        TX_LOG_ARGS(" Comparing..." << presets[index].iPresetNameKey );
        if ( selectedItem == presetName ) {
            presetNameKey = presets[index].iPresetNameKey;
            TX_LOG_ARGS(" Comparing mached presetNameKey = " << presetNameKey );
            break;
            }
    }

    TX_EXIT

    return presetNameKey;
}

/*!
 \internal
 */
void MpEqualizerFrameworkWrapperPrivate::MapcInitComplete(
    TInt aError,
    const TTimeIntervalMicroSeconds& /* aDuration */ )
{
    TX_ENTRY_ARGS( "aError=" << aError );

    if ( !aError ) {
        TRAPD(err, mEqualizerUtility = CAudioEqualizerUtility::NewL( *mMdaPlayUtility ) );
        if ( err == KErrNone ) {
            emit q_ptr->equalizerReady();
        }
        else {
            TX_ENTRY_ARGS( "CAudioEquqlizerUtility error=" << err );
        }
    }
    else {
        TX_ENTRY_ARGS( "MapcInitComplete error=" << aError );
    }

    TX_EXIT
}

/*!
 \internal
 */
void MpEqualizerFrameworkWrapperPrivate::MapcPlayComplete(
    TInt /* aError */ )
{
    TX_LOG
    // Do nothing
}

//End of File
