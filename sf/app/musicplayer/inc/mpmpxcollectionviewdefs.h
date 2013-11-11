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
* Description: Music Player collection view common definitions.
*
*/

#ifndef MPMPXCOLLECTIONVIEWDEFS_H
#define MPMPXCOLLECTIONVIEWDEFS_H

#include <QMetaType>

// Context of collection view.
// It's the exact location within collection navigation.
enum TCollectionContext {
    ECollectionContextUnknown = 0,
    ECollectionContextAllSongs,
    ECollectionContextArtists,
    ECollectionContextArtistAlbums,
    ECollectionContextArtistAlbumsTBone,
    ECollectionContextArtistAllSongs,
    ECollectionContextAlbums,
    ECollectionContextAlbumsTBone,
    ECollectionContextPlaylists,
    ECollectionContextPlaylistSongs,
    ECollectionContextAlbumsMediaWall
};
// Register so type can be used in signal and slot connection
Q_DECLARE_METATYPE(TCollectionContext)

// Layout where the collection is being shown.
enum TCollectionLayout {
    ECollectionListView = 0,
    ECollectionSelectionDialog,
    ECollectionArrangeSongsDialog
};

#endif  // MPMPXCOLLECTIONVIEWDEFS_H

