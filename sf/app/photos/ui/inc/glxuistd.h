/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Viewer app general definitions
*
*/






#ifndef GLXUISTD_H
#define GLXUISTD_H

#include <e32cmn.h>

enum TGlxHuiCustomCommands 
	{
	EGlxHuiCustomEventBackToZeroSpeed = 200,
	EGlxHuiCustomEventBackToSharpPath
	};

enum GlxFetcherFilterType
    {
    EGlxFetcherFilterNone = 0x00,
    EGlxFetcherFilterJpeg = 0x01,
    EGlxFetcherFilterNonJpeg = 0x02,
    EGlxFetcherFilterJp2 = 0x04,
    EGlxFetcherFilterNonJp2 = 0x08,
    EGlxFetcherFilterLast = 0xFF
    };
enum GlxContextMode
{
    GlxContextInvalid = -1,
	GlxContextLsGrid,
	GlxContextPtGrid,
	GlxContextLsFs,
	GlxContextPtFs,  //TB context check would be applied here . Please do not add any context between GlxContextLsFs and GlxContextPtFs
	GlxContextLsList,
	GlxContextPtList,
    GlxContextSelectionList,
    GlxContextFavorite,
    GlxContextComment   //to fetch the description
};

enum GlxTBContextType
{
	GlxTBContextGrid,
	GlxTBContextLsFs,
	GlxTBContextPtFs,
	GlxTBContextNone
};

typedef enum
{
    NO_EFFECT,
    FLIP_EFFECT,
    SMOOTH_FADE,
    ZOOM_TO_FACE,
    HELIX_EFFECT,
    TRANSITION_EFFECT,
    FADE_EFFECT,
    GRID_TO_FULLSCREEN,
    FULLSCREEN_TO_GRID,
    FULLSCREEN_TO_GRID_PORTRAIT,
    FULLSCREEN_TO_DETAIL,
    DETAIL_TO_FULLSCREEN,
    GRID_TO_ALBUMLIST,
    ALBUMLIST_TO_GRID,
}GlxEffect;

typedef enum
{
 SLOW,
 MEDIUM,
 FAST
}GlxSlideShowDelay;

typedef enum
{
    NO_VIEW,
    CURRENT_VIEW,
    LAUNCH_VIEW,
    BOTH_VIEW    
} GlxViewEffect;

#define DOUBLETAP_TIMEINTERVAL 300

_LIT(KGlxUiUtilitiesResource, "glxuiutilities.rsc");

_LIT(KGlxIconsFilename, "glxicons.mif");

/** View activation parameters */
_LIT8(KGlxActivationFullScreen, "fullscreen");


/** attribute/thumbnail fetch context priority definitions */

const TInt KGlxFetchContextPriorityLow = 0;
const TInt KGlxFetchContextPriorityNormal = 1;

// cloud view fetch context priorities
const TInt KGlxFetchContextPriorityCloudView = 4;
const TInt KGlxFetchContextPriorityCloudViewInfoBubble = 3;

// UMP view fetch context priorities
const TInt KGlxFetchContextPriorityUMPViewAlbumPane = 2;
const TInt KGlxFetchContextPriorityUMPViewTagPane = 3;

// tile view fetch context priorities
const TInt KGlxFetchContextPriorityGridViewVisibleThumbnail = 6;
const TInt KGlxFetchContextPriorityGridViewDetails = 0;
const TInt KGlxFetchContextPriorityGridViewDetailsFirstLast = 0;
const TInt KGlxFetchContextPriorityGridViewNonVisibleThumbnail = 0;
const TInt KGlxFetchContextPriorityGridViewFullscreenPrefetchNonVisibleThumbnail = 0;
const TInt KGlxFetchContextPriorityGridViewFullscreenVisibleThumbnail = 5;

const TInt KGlxFetchContextPriorityCommandHandlerOpening = 7;
const TInt KGlxFetchContextPriorityCommandHandlerPrefetch = 0;

const TInt KGlxFetchContextPriorityListViewTitles = 9;
const TInt KGlxFetchContextPriorityListViewFocusedSubtitle = 8;
const TInt KGlxFetchContextPriorityListViewSubtitles = 0;


// tile fetcher
const TInt KGlxFetchContextPriorityTitleFetcher = 10;

// video playback view UID
const TInt KGlxVideoPlaybackviewPlugin = 0x2000A7B3;

//Constants for ci_photos.confml
const TUint32 KGlxCi_UidGallery = 0x20007193;
const TInt KGlxOperatorLink  = 0x1;
const TInt KGlxOvi  = 0x2;

#endif  // GLXUISTD_H
