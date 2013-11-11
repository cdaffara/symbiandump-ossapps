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
* Description:  definition of enum value for each view
*
*/

// Version : %version: 6 %

#ifndef MPXHBVIDEOCOMMONDEFS_H
#define MPXHBVIDEOCOMMONDEFS_H

namespace MpxHbVideoCommon
{

    const long int KMpxVideoPluginTypePlaybackUid   = {0x20024338};
    const long int KMpxVideoPluginTypeCollectionUid = {0x200211FC};
    const long int KMpxVideoPluginTypeFileDetailsUid = {0x20021200};
    
    const long int KMpxVideoPluginDllPlaybackUid   = {0x20024335};
    const long int KMpxVideoPluginDllCollectionUid = {0x200211FB};
    const long int KMpxVideoPluginDllFileDetailsUid = {0x200211FF};

    enum MpxHbVideoCommand 
    {
        ActivateCollectionView,
        ActivatePlaybackView,
        ActivateVideoDetailsView,
        DoDelayedLoad
    };

    enum MpxHbVideoViewType 
    {
        CollectionView, 
        PlaybackView,
        VideoDetailsView
    };
}

#endif	// MPXHBVIDEOCOMMONDEFS_H
