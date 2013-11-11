/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Externalizable container for DSA region
*
*/

// Version : %version: 3 %



#ifndef MPXHELIXPLAYBACKPLUGINDEFS_H_
#define MPXHELIXPLAYBACKPLUGINDEFS_H_

enum TMPXVideoPlaybackState
{
    EMPXVideoNotInitialized,
    EMPXVideoInitializing,
    EMPXVideoInitialized,
    EMPXVideoBuffering,
    EMPXVideoPlaying,
    EMPXVideoPaused,
    EMPXVideoStopped,
    EMPXVideoSeeking,
    EMPXNumberOfStates
};


enum TMPXVideoMode
{
    EMPXVideoLocal,
    EMPXVideoStreaming,
    EMPXVideoLiveStreaming,
    EMPXVideoProgressiveDownload,
    EMPXNumberOfModes
};



#endif /*MPXHELIXPLAYBACKPLUGINDEFS_H_*/
