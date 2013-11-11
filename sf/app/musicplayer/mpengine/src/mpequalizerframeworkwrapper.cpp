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
* Description: Wrapper for equalizer framework.
*
*/

// System includes
#include <QStringList>
#include <QDebug>

// User includes
#include "mpequalizerframeworkwrapper.h"
#include "mpequalizerframeworkwrapper_p.h"
#include "mptrace.h"

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
    TX_ENTRY

    d_ptr = new MpEqualizerFrameworkWrapperPrivate(this);
    d_ptr->init();

    TX_EXIT
}

/*!
    Destructs the class and its private imaplementation.
    
 */
MpEqualizerFrameworkWrapper::~MpEqualizerFrameworkWrapper()
{
    TX_LOG

    delete d_ptr;
}

/*!
    Get the preset name key by giving \a presetIndex. The command then 
    relays to its private implementation.

 */
int MpEqualizerFrameworkWrapper::getPresetNameKey( int presetIndex )
{
    TX_LOG_ARGS( "Preset index: " << presetIndex );

	return d_ptr->getPresetNameKey( presetIndex );
}

/*!
    Get the preset index by giving \a presetNameKey. The command then 
    relays to its private implementation.

 */
int MpEqualizerFrameworkWrapper::getPresetIndex( int presetNameKey )
{
    TX_LOG_ARGS( "Preset name key: " << presetNameKey );
    
    return d_ptr->getPresetIndex( presetNameKey );
}

/*!
    Returning the list of availale preset names. The command then relays to its 
    private implementation. 

 */
 QStringList MpEqualizerFrameworkWrapper::presetNames()
{
    return d_ptr->presetNames();
}

 //End of File
