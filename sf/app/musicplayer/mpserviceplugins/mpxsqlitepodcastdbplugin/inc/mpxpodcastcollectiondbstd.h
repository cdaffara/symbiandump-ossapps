/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MPXPODCASTCOLLECTIONDBSTD_H
#define MPXPODCASTCOLLECTIONDBSTD_H

// CONSTANTS

const TInt KDBPluginUid           = 0x101FFC3C;
const TInt KMCMaxQueryLen         = 500; // 255 max uri length + 245 for query
const TInt KMCMaxTextLen          = 255; // Maximum field size for categories is 255

const TInt KSystemTableID = 99;
const TInt KRecentlyPlayedPlaylistUID = 1 | (KSystemTableID << 24);
const TInt KMostPlayedPlaylistUID     = 2 | (KSystemTableID << 24);
const TInt KRecentlyAddedPlaylistUID  = 3 | (KSystemTableID << 24);
const TInt KNotPlayedPlaylistUID      = 4 | (KSystemTableID << 24);

const TInt KPublishAllPlaylistUID       = 11 | (KSystemTableID << 24);
const TInt KPublishTodayPlaylistUID     = 12 | (KSystemTableID << 24);
const TInt KPublishYesterdayPlaylistUID = 13 | (KSystemTableID << 24);
const TInt KPublishThisWeekPlaylistUID  = 14 | (KSystemTableID << 24);
const TInt KPublishLastWeekPlaylistUID  = 15 | (KSystemTableID << 24);
const TInt KPublish2WeeksAgoPlaylistUID = 16 | (KSystemTableID << 24);
const TInt KPublish3WeeksAgoPlaylistUID = 17 | (KSystemTableID << 24);
const TInt KPublishLastMonthPlaylistUID = 18 | (KSystemTableID << 24);
const TInt KPublishEarlierPlaylistUID   = 19 | (KSystemTableID << 24);
const TInt KPublishUnknownPlaylistUID   = 20 | (KSystemTableID << 24);

#endif // MPXPODCASTCOLLECTIONDBSTD_H

// End Of File
