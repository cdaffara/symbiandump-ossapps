/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: Framework wraper for Audio Effects - Private.
*
*/

#include "mpaudioeffectsframeworkwrapper_p.h"
#include "mpxaudioeffectproperties.h"
#include "mpcommondefs.h"
#include "mptrace.h"

/*!
    \class MpAudioEffectsFrameworkWrapperPrivate
    \brief Wrapper for audio effects framework utilities - private implementation.

    This is a private implementation of the audio effects framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpAudioEffectsFrameworkWrapperPrivate::MpAudioEffectsFrameworkWrapperPrivate()
    :mAudioEffectProperties( new CMPXAudioEffectProperties() )
{
}

/*!
 \internal
 */
MpAudioEffectsFrameworkWrapperPrivate::~MpAudioEffectsFrameworkWrapperPrivate()
{
    delete mAudioEffectProperties;
}

/*!
 \internal
 */
void MpAudioEffectsFrameworkWrapperPrivate::init()
{
    TRAPD( err, doInitL() );
    if ( err != KErrNone ) {
        mAudioEffectProperties->Reset();
    }
}

/*!
 \internal
 */
void MpAudioEffectsFrameworkWrapperPrivate::setBalance( int balance )
{
    mAudioEffectProperties->SetBalance( balance );
    saveToFile();
}

/*!
 \internal
 */
void MpAudioEffectsFrameworkWrapperPrivate::setLoudness( bool mode )
{
    mAudioEffectProperties->SetLoudness( mode );
    saveToFile();
}

/*!
 \internal
 */
int MpAudioEffectsFrameworkWrapperPrivate::balance()
{
    return mAudioEffectProperties->Balance();
}

/*!
 \internal
 */
bool MpAudioEffectsFrameworkWrapperPrivate::loudness()
{
    return mAudioEffectProperties->Loudness();
}

/*!
 \internal
 */
void MpAudioEffectsFrameworkWrapperPrivate::saveToFile()
{
    TRAPD( err, mAudioEffectProperties->SaveToFileL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpAudioEffectsFrameworkWrapperPrivate::doInitL()
{
    mAudioEffectProperties->LoadFromFileL();
}

