/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This header file contains constants definitions and type
*                definition
*
*/


#ifndef MPXCOLLECTIONDBSTD_H
#define MPXCOLLECTIONDBSTD_H

// CONSTANTS
enum TMPXAutoPlaylistType
    {
    EMPXNoAutoPlaylist = -1,
    EMPXRecentlyPlayedPlaylist,
    EMPXMostPlayedPlaylist,
    EMPXRecentlyAddedPlaylist,
    EMPXAutoPlaylistCount
    };

const TInt KDBPluginUid = 0x101FFC3A;

const TInt KMPXMaxRecentlyPlayedSongs = 20;
const TInt KMPXMaxMostPlayedSongs = 20;
const TInt KMPXRecentlyAddedInterval = 7;

const TInt KMCMaxTextLen          = 255; // Maximum field size for categories is 255

#endif // MPXCOLLECTIONDBSTD_H

// End Of File
