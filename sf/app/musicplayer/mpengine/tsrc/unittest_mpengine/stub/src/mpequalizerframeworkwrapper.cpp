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
* Description: stub for equalizer framework.
*
*/

// System includes
#include <QStringList>
#include <QDebug>

// User includes
#include "mpequalizerframeworkwrapper.h"

/*!
    \class MpEqualizerFrameworkWrapper
    \ingroup musicplayer
    \brief Wrapper for equalizer framework.
    \since 10.1
    
    Equalizer wrapper provides Qt style interface to the audio equalizer
    utilities. Its implementation is hidden using private class data pattern.
    
    This class defines several APIs that are needed from \a MpPlaybackView
    and other components in \a mpplaybackviewplugin.

    \sa MpEqualizerFrameworkWrapperPrivate
*/

/*!
    \fn MpEqualizerFrameworkWrapper::equalizerReady()

    This signal will be emitted when Audio Eqalizer is initialized. This is
    when all APIs, like \a applyPreset, are ready to be used.
    
    \sa MpEqualizerFrameworkWrapperPrivate::MapcInitComplete
*/

/*!
    Constructs a new MpEqualizerFrameworkWrapper with \a parent and initializes
    private imaplementation.

*/
MpEqualizerFrameworkWrapper::MpEqualizerFrameworkWrapper( QObject *parent )
    : QObject(parent)
{

}

/*!
    Destructs the class and its private imaplementation.
    
 */
MpEqualizerFrameworkWrapper::~MpEqualizerFrameworkWrapper()
{

}

/*!
    Get the preset name key by giving \a presetIndex. The command then 
    relays to its private implementation.

 */
int MpEqualizerFrameworkWrapper::getPresetNameKey( int presetIndex )
{
    Q_UNUSED(presetIndex);
	return 0;
}

/*!
    Get the preset index by giving \a presetNameKey. The command then 
    relays to its private implementation.

 */
int MpEqualizerFrameworkWrapper::getPresetIndex( int presetNameKey )
{
    Q_UNUSED(presetNameKey);
    return 0;
}

/*!
    Returning the list of availale preset names. The command then relays to its 
    private implementation. 

 */
 QStringList MpEqualizerFrameworkWrapper::presetNames()
{
    return mList;
}

 //End of File
