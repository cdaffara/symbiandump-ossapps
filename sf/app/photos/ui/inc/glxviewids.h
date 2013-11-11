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
* Description:   ?Description
*
*/



#ifndef GLXVIEWIDS_H
#define GLXVIEWIDS_H

#define GLX_GRIDVIEW_ID          0x12121212
#define GLX_LISTVIEW_ID          0x12121213
#define GLX_FULLSCREENVIEW_ID    0x12121214
#define GLX_SLIDESHOWVIEW_ID     0x12121215
#define GLX_DETAILSVIEW_ID       0x12121216
#define GLX_SLIDESHOWSETTINGSVIEW_ID 0x12121217


/* 
 * For the non ui component, only sub state is expose in the model 
 * so it is required to have unique substrate of states 
 * Add the new sub state in between NO and Max 
 * and take care uniqueness of all these substrate
 */
typedef enum 
{
    NO_GRID_S = 0,
    ALL_ITEM_S,
    ALBUM_ITEM_S,
	FETCHER_ITEM_S,  // all grid fetcher
	FETCHER_ALBUM_ITEM_S,  //album grid getcher
    MAX_GRID_S
} GridState;

typedef enum 
{
    NO_FULLSCREEN_S = MAX_GRID_S + 1,
	ALL_FULLSCREEN_S,
	ALBUM_FULLSCREEN_S,
    EXTERNAL_S,
    IMAGEVIEWER_S,
	FETCHER_S,
	BROWSE_S,	
	MAX_FULLSCREEN_S
} FullScreenState;

typedef enum 
{
    NO_SLIDESHOW_S = MAX_FULLSCREEN_S + 1,
    SLIDESHOW_GRID_ITEM_S,
    SLIDESHOW_ALBUM_ITEM_S,
    MAX_SLIDESHOW_S 
} SlideShowState;

typedef enum
{
    NO_DETAIL_S = MAX_SLIDESHOW_S + 1,
    IMAGEVIEWER_DETAIL_S,
    MAX_DETAIL_S
} DetailState;
typedef enum
{
    NO_LIST_S  = MAX_DETAIL_S+1,
    FETCHER_ALBUM_S,  // album list fetcher
    MAX_LIST_S
} ListState;
#endif /* GLXVIEWIDS_H */
