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
* Description:  Common definitios.
*
*/



#ifndef MPCOMMONDEFS_H
#define MPCOMMONDEFS_H

#include <QMetaType>

namespace MpCommon
{
    // UID definition
    const quint32 KMusicPlayerUid      = {0x10207C62};
    const quint32 KCollectionViewUid   = {0x10207C63};
    const quint32 KPlaybackViewUid     = {0x10207C64};
    const quint32 KSettingsViewUid     = {0x10207C66};
    const quint32 KDetailsViewUid      = {0x2002D0AA};
    const quint32 KMediaWallViewUid    = {0x10207C94};

    /*!
     Command code sent from the views to the application main window
     through command signal.
     */
    enum MpCommandCode {
        Exit,
        SendToBackground,
        ActivateCollectionView,
        ActivatePlaybackView,
        ActivateSettingsView,
        ActivateDetailsView,
        ActivatePreviousView
    };

    /*!
     View's mode.
     DefaultView - Default mode; normal Music Player operation.
     FetchView - Music fetcher mode; launched through QtHighway interface.
     */
    enum MpViewMode {
        DefaultView,
        FetchView,
        EmbeddedView
    };
    
    /*!
    Thumbnail type, used in collection model.
    */
    enum MpThumbType{
        ListThumb,
        TBoneThumb,
        MediaWallThumb,
    };
    
    /*!
    Volume property, used in MpMpxPlaybackFramework and MpVolumeSlider.
    */
    enum MpVolumeProperty{
        MaxVolume,
        Volume,
        MuteState
    };
}

    // Register MpVolumeProperty so can be used in signal and slot connection
    Q_DECLARE_METATYPE( MpCommon::MpVolumeProperty )

#endif	// MPCOMMONDEFS_H
