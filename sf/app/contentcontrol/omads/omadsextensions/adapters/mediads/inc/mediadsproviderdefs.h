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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#ifndef __MEDIADSPROVIDERDEFS_H__ 
#define __MEDIADSPROVIDERDEFS_H__ 

#include <e32def.h>
#include <e32cmn.h>

// Database name

_LIT( KMediaDsDbName, "mediads" ); // Default db
_LIT( KMediaDsRefreshDbName, "mediadsrefresh" ); // Used to refresh db, before opening

// Resource file for CSmlDataStoreFormat in SIS installation
_LIT(KMediaDsStoreFormatRsc,"mediadsstore.rsc");

// max foldername
const TInt KMaxFolderNameLength = 50;

// Implementation uid
const TUint KMediaDataProviderImplUid = 0x2001A9A1;

// Folders
_LIT( KAllSongs, "All songs" );
_LIT( KPlaylists, "Playlists" );
_LIT( KAlbums, "Albums" );
const TInt KAllSongsFolderId( 1 );
const TInt KPlaylistsFolderId( 2 );
const TInt KAlbumsFolderId( 3 );

const TInt KLastFolderId( KAlbumsFolderId );

#endif // __MEDIADSPROVIDERDEFS_H__ 
