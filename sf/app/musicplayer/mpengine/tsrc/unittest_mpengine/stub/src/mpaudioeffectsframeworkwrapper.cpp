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
* Description: stub for MpAudioEffectsFrameworkWrapper
*
*/

#include "mpaudioeffectsframeworkwrapper.h"

/*!
 Constructor.
 */
MpAudioEffectsFrameworkWrapper::MpAudioEffectsFrameworkWrapper()
{
}

/*!
 Destructor.
 */
MpAudioEffectsFrameworkWrapper::~MpAudioEffectsFrameworkWrapper()
{

}

/*!
 Returns the current persistent balance.
 */
int MpAudioEffectsFrameworkWrapper::balance()
{
    return mBalance;
}

/*!
 Returns the current persistent loudness.
 */
bool MpAudioEffectsFrameworkWrapper::loudness()
{
    return mLoudness;
}

/*!
 Set the \a balance.
 */
void MpAudioEffectsFrameworkWrapper::setBalance( int balance )
{
    Q_UNUSED(balance);
}

/*!
 Set the loudness \a mode.
 */
void MpAudioEffectsFrameworkWrapper::setLoudness( bool mode )
{
    Q_UNUSED(mode);
}

