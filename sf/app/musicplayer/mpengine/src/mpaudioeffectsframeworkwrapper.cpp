/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: Framework wraper for Audio Effects.
*
*/

#include "mpaudioeffectsframeworkwrapper.h"
#include "mpaudioeffectsframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpAudioEffectsFrameworkWrapper
    \brief Wrapper for Audio Effects framework.

    Audio effects framework wrapper provides Qt style interface to the audio effects framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
 Constructor.
 */
MpAudioEffectsFrameworkWrapper::MpAudioEffectsFrameworkWrapper()
    :d_ptr( new MpAudioEffectsFrameworkWrapperPrivate() )
{
    d_ptr->init();
}

/*!
 Destructor.
 */
MpAudioEffectsFrameworkWrapper::~MpAudioEffectsFrameworkWrapper()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

/*!
 Returns the current persistent balance.
 */
int MpAudioEffectsFrameworkWrapper::balance()
{
    TX_LOG
    return d_ptr->balance();
}

/*!
 Returns the current persistent loudness.
 */
bool MpAudioEffectsFrameworkWrapper::loudness()
{
    TX_LOG
    return d_ptr->loudness();
}

/*!
 Set the \a balance.
 */
void MpAudioEffectsFrameworkWrapper::setBalance( int balance )
{
    TX_ENTRY
    d_ptr->setBalance( balance );
    TX_EXIT
}

/*!
 Set the loudness \a mode.
 */
void MpAudioEffectsFrameworkWrapper::setLoudness( bool mode )
{
    TX_ENTRY
    d_ptr->setLoudness( mode );
    TX_EXIT
}

